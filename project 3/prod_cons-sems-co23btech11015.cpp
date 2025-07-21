#include <iostream>
#include <fstream>
#include <vector>
#include <pthread.h>
#include <mutex>
#include <semaphore>
#include <random>
#include <chrono>
#include <iomanip>
#include <atomic>
#include <sstream>
#include <unistd.h>

using namespace std;

int capacity = 0, np = 0, nc = 0, cntp = 0, cntc = 0;
double up = 0.0, uc = 0.0;

// Use C++20 semaphores
counting_semaphore<INT_MAX> *EmptySemaphore = nullptr;
counting_semaphore<INT_MAX> *FullSemaphore = nullptr;
counting_semaphore<1> *LockingSemaphore = nullptr;

// Shared buffer and buffer index
vector<int> buffer;
int next_in = 0;
int next_out = 0;

// Tracking metrics
vector<double> producer_times;
vector<double> consumer_times;

// Random number generator for exponential distribution
std::random_device rd;
std::mt19937 gen(rd());

FILE* outfile;

// Start time of the program
std::chrono::time_point<std::chrono::system_clock> ProgramStartTime;

// Get current time as microseconds since ProgramStartTime
long getCurrentTime() {
    auto now = std::chrono::system_clock::now();
    return std::chrono::duration_cast<std::chrono::microseconds>(now - ProgramStartTime).count();
}

// Producer function
void* producer(void* param) {
    int id = *((int*)param);
    double ProducerDelay = 0;

    std::exponential_distribution<double> exp_dist(1000.0 / up);

    for (int i = 0; i < cntp; i++) {

        auto start_time = std::chrono::high_resolution_clock::now();

        EmptySemaphore->acquire();
        LockingSemaphore->acquire();

        buffer[next_in] = i;  // Produce item 'i'
        int location = next_in;
        next_in = (next_in + 1) % capacity;

        fprintf(outfile,"%dth item: %d produced by thread %d at %ld from location %d\n", 
               i+1, i, id, getCurrentTime(), location);

        LockingSemaphore->release();
        FullSemaphore->release();

        usleep(static_cast<int>(exp_dist(gen) * 1000000));

        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        ProducerDelay += duration.count();
    }
    producer_times[id-1]=(ProducerDelay / cntp);
    return nullptr;
}

// Consumer function
void* consumer(void* param) {
    int id = *((int*)param);
    double ConsumerDelay = 0;

    std::exponential_distribution<double> exp_dist(1000.0 / uc);

    for (int i = 0; i < cntc; i++) {

        auto start_time = std::chrono::high_resolution_clock::now();

        FullSemaphore->acquire();
        LockingSemaphore->acquire();

        // Read (consume) the item instead of writing into the buffer.
        int item = buffer[next_out];
        int location = next_out;
        next_out = (next_out + 1) % capacity;

        fprintf(outfile,"%dth item: %d consumed by thread %d at %ld from location %d\n", 
               i+1, item, id, getCurrentTime(), location);

        LockingSemaphore->release();
        // Release the EmptySemaphore to indicate a free slot
        EmptySemaphore->release();

        usleep(static_cast<int>(exp_dist(gen) * 1000000));

        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        ConsumerDelay += duration.count();
    }
    consumer_times[id-1]=(ConsumerDelay / cntc);
    return nullptr;
}

int main() {
    std::ifstream input_file("inp-params.txt");
    if (!input_file.is_open()) {
        std::cerr << "Failed to open input file" << std::endl;
        return 1;
    }

    input_file >> capacity >> np >> nc >> cntp >> cntc >> up >> uc;
    input_file.close();

    buffer.resize(capacity);
    producer_times.resize(np);
    consumer_times.resize(nc);

    EmptySemaphore = new std::counting_semaphore<INT_MAX>(capacity);
    FullSemaphore = new std::counting_semaphore<INT_MAX>(0);
    LockingSemaphore = new std::counting_semaphore<1>(1);

    outfile = fopen("output-sems-co23btech11015.txt", "w"); 
    if (outfile == nullptr) {
        cerr << "Failed to open output file" << endl;
        return 1;
    }

    if (capacity == 0 || ((nc * cntc) > (np * cntp) || ((np * cntp) - (nc * cntc) > capacity))) {
        fprintf(outfile, "This selection of input parameters will lead to infinite waiting.\n");
        fprintf(outfile, "Please select a different set of input parameters.\n");
        return 1;
    }

    ProgramStartTime = std::chrono::system_clock::now();
    
    vector<pthread_t> producers(np);
    vector<pthread_t> consumers(nc);
    vector<int> producerIds(np);
    vector<int> consumerIds(nc);

    for (int i = 0; i < np; i++) 
        producerIds[i] = i + 1;
    for (int i = 0; i < nc; i++) 
        consumerIds[i] = i + 1;

    for (int i = 0; i < np; i++) 
        pthread_create(&producers[i], NULL, producer, &producerIds[i]);
    for (int i = 0; i < nc; i++) 
        pthread_create(&consumers[i], NULL, consumer, &consumerIds[i]);

    for (int i = 0; i < np; i++) 
        pthread_join(producers[i], NULL);
    for (int i = 0; i < nc; i++) 
        pthread_join(consumers[i], NULL);

    double avg_prod_time = 0.0, avg_cons_time = 0.0;
    for (int i = 0; i < producer_times.size(); i++) {
        avg_prod_time += producer_times[i];
    }
    for (int i = 0; i < consumer_times.size(); i++) {
        avg_cons_time += consumer_times[i];
    }

    avg_prod_time /= np;
    avg_cons_time /= nc;
    
    fclose(outfile);
    cout << "Average producer delay: " << avg_prod_time << " milliseconds" << endl;
    cout << "Average consumer delay: " << avg_cons_time << " milliseconds" << endl;

    delete EmptySemaphore;
    delete FullSemaphore;
    delete LockingSemaphore;

    return 0;
}
