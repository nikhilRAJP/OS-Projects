#include <iostream>
#include <fstream>
#include <vector>
#include <pthread.h>
#include <mutex>
#include <condition_variable>
#include <random>
#include <chrono>
#include <iomanip>
#include <atomic>
#include <sstream>
#include <unistd.h>

using namespace std;

// Global parameters
int capacity = 0, np = 0, nc = 0, cntp = 0, cntc = 0;
double up = 0.0, uc = 0.0;

// Pthread condition variables and mutex for buffer synchronization
pthread_cond_t not_full, not_empty;
pthread_mutex_t buffer_mutex;

// Mutex for file writes
pthread_mutex_t file_mutex;

// Tracking metrics (in milliseconds)
vector<double> producer_times;
vector<double> consumer_times;

// Random number generator for exponential distribution
random_device rd;
mt19937 gen(rd());

FILE* outfile;

// Start time of the program
chrono::time_point<chrono::system_clock> ProgramStartTime;

// Return the elapsed time (in microseconds) since the program started
long getCurrentTime() {
    auto now = chrono::system_clock::now();
    return chrono::duration_cast<chrono::microseconds>(now - ProgramStartTime).count();
}

// Shared buffer and counters
vector<int> buffer;
int Count = 0;  // Number of items currently in the buffer
int next_in = 0;
int next_out = 0;

// Producer function
void* producer(void* param) {
    int id = *((int*)param);
    double ProducerDelay = 0.0;
    
    // Delay distribution in seconds
    exponential_distribution<double> exp_dist(1000.0 / up);
    
    for (int i = 0; i < cntp; i++) {
        auto start_time = chrono::high_resolution_clock::now();
        
        pthread_mutex_lock(&buffer_mutex);
        // Wait until the buffer is not full
        while (Count == capacity) {
            pthread_cond_wait(&not_full, &buffer_mutex);
        }
            
        // Produce an item (here using 'i' as the produced value)
        buffer[next_in] = i;
        int location = next_in;
        next_in = (next_in + 1) % capacity;
        Count++;
        
        // Protect file output using file_mutex
        pthread_mutex_lock(&file_mutex);
        fprintf(outfile, "%dth item: %d produced by thread %d at %ld from location %d\n", 
                i + 1, i, id, getCurrentTime(), location);
        pthread_mutex_unlock(&file_mutex);
        
        // Signal that there is an item available for consumption
        pthread_cond_signal(&not_empty);
        pthread_mutex_unlock(&buffer_mutex);
        
        // Simulate production delay
        usleep(static_cast<int>(exp_dist(gen) * 1000000));
        
        auto end_time = chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::milliseconds>(end_time - start_time);
        ProducerDelay += duration.count();
    }
    producer_times[id-1]=(ProducerDelay / cntp);
    return nullptr;
}

// Consumer function
void* consumer(void* param) {
    int id = *((int*)param);
    double ConsumerDelay = 0.0;
    
    // Delay distribution in seconds
    exponential_distribution<double> exp_dist(1000.0 / uc);
    
    for (int i = 0; i < cntc; i++) {
        auto start_time = chrono::high_resolution_clock::now();
        int item;
        
        pthread_mutex_lock(&buffer_mutex);
        // Wait until the buffer is not empty
        while (Count == 0) {
            pthread_cond_wait(&not_empty, &buffer_mutex);
        }
            
        // Consume the item from the buffer (read, then remove)
        item = buffer[next_out];
        int location = next_out;
        next_out = (next_out + 1) % capacity;
        Count--;  // Decrement the count since an item was consumed
        
        // Protect file output using file_mutex
        pthread_mutex_lock(&file_mutex);
        fprintf(outfile, "%dth item: %d consumed by thread %d at %ld from location %d\n", 
                i + 1, item, id, getCurrentTime(), location);
        pthread_mutex_unlock(&file_mutex);
        
        // Signal that there is a free slot available for production
        pthread_cond_signal(&not_full);
        pthread_mutex_unlock(&buffer_mutex);
        
        // Simulate consumption delay
        usleep(static_cast<int>(exp_dist(gen) * 1000000));
        
        auto end_time = chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::milliseconds>(end_time - start_time);
        ConsumerDelay += duration.count();
    }
    consumer_times[id-1]=(ConsumerDelay / cntc);
    return nullptr;
}

int main() {
    ifstream input_file("inp-params.txt");
    if (!input_file.is_open()) {
        cerr << "Failed to open input file" << endl;
        return 1;
    }
    
    // Read input parameters: capacity, number of producers, number of consumers,
    // items per producer, items per consumer, and mean delays for producer and consumer.
    input_file >> capacity >> np >> nc >> cntp >> cntc >> up >> uc;
    input_file.close();
    
    // Resize buffer to the given capacity
    buffer.resize(capacity);
    producer_times.resize(np);
    consumer_times.resize(nc);
    
    // Open output file for writing
    outfile = fopen("output-locks-co23btech11015.txt", "w");
    if (outfile == nullptr) {
        cerr << "Failed to open output file" << endl;
        return 1;
    }
    
    // Check for input parameter conditions that may lead to infinite waiting.
    if (capacity == 0 || ((nc * cntc) > (np * cntp)) || (((np * cntp) - (nc * cntc)) > capacity)) {
        fprintf(outfile, "This selection of input parameters will lead to infinite waiting.\n");
        fprintf(outfile, "Please select a different set of input parameters.\n");
        fclose(outfile);
        return 1;
    }
    
    // Record the start time of the program
    ProgramStartTime = chrono::system_clock::now();

    // Initialize pthread synchronization primitives
    pthread_mutex_init(&buffer_mutex, nullptr);
    pthread_mutex_init(&file_mutex, nullptr);
    pthread_cond_init(&not_full, nullptr);
    pthread_cond_init(&not_empty, nullptr);
    
    // Create producer and consumer threads
    vector<pthread_t> producers(np);
    vector<pthread_t> consumers(nc);
    vector<int> producerIds(np);
    vector<int> consumerIds(nc);
    
    for (int i = 0; i < np; i++) {
        producerIds[i] = i + 1;
    }
    for (int i = 0; i < nc; i++) {
        consumerIds[i] = i + 1;
    }
    
    for (int i = 0; i < np; i++) {
        pthread_create(&producers[i], nullptr, producer, &producerIds[i]);
    }
    for (int i = 0; i < nc; i++) {
        pthread_create(&consumers[i], nullptr, consumer, &consumerIds[i]);
    }
    
    for (int i = 0; i < np; i++) {
        pthread_join(producers[i], nullptr);
    }
    for (int i = 0; i < nc; i++) {
        pthread_join(consumers[i], nullptr);
    }
    
    // Compute average delays (producer_times and consumer_times are in milliseconds)
    double avg_prod_time = 0.0, avg_cons_time = 0.0;
    for (double t : producer_times) {
        avg_prod_time += t;
    }
    for (double t : consumer_times) {
        avg_cons_time += t;
    }
    avg_prod_time /= np;
    avg_cons_time /= nc;
    
    fclose(outfile);
    
    cout << "Average producer delay: " << avg_prod_time << " milliseconds" << endl;
    cout << "Average consumer delay: " << avg_cons_time << " milliseconds" << endl;
    
    // Destroy pthread synchronization primitives
    pthread_mutex_destroy(&buffer_mutex);
    pthread_mutex_destroy(&file_mutex);
    pthread_cond_destroy(&not_full);
    pthread_cond_destroy(&not_empty);
    
    return 0;
}
