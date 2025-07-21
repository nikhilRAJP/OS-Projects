#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <time.h>
#define M 1.0e6

int N,K,n;
int **sudoku;
pthread_t *threads;

typedef struct{
    int thread_id;
    int start;
    int end;
}info;

bool allrow=1,allcol=1,allgrid=1;

FILE *output;

void* runner1(void *arg){
    info *param = (info *)arg;

    for(int row = param->start;row<=param->end;row++){
        bool rowvalid=true;
        // int* present = (int*) malloc((N+1)*sizeof(int));
        int present[N+1];
        for(int i=0;i<N+1;i++){
            present[i]=0;
        }
        for(int coloumn = 0;coloumn<N ;coloumn++){
            int num = sudoku[row][coloumn];
            if(present[num]>0){
                rowvalid=false;
                allrow=false;
                break;
            }
            present[num]+=1;
        }
        if(rowvalid==false){
            fprintf(output,"Thread %d checks row %d and it is invalid\n",param->thread_id+1,row+1);
        }
        else fprintf(output,"Thread %d checks row %d and it is valid\n",param->thread_id+1,row+1);
        // free(present);
    }

    pthread_exit(NULL);
}

void *runner2(void* arg){
    info *param = (info *)arg;

    for(int coloumn = param->start;coloumn<=param->end;coloumn++){
        bool coloumnvalid=true;
        //  int* present = (int*) malloc((N+1)*sizeof(int));
        int present[N+1];
        for(int i=0;i<N+1;i++){
            present[i]=0;
        }
        for(int row = 0;row<N ;row++){
            int num = sudoku[row][coloumn];
            if(present[num]>0){
                coloumnvalid=false;
                allcol=false;
                break;
            }
            present[num]+=1;
        }
        if(coloumnvalid==false){
            fprintf(output,"Thread %d checks coloumn %d and it is invalid\n",param->thread_id+1,coloumn+1);
        }
        else fprintf(output,"Thread %d checks coloumn %d and it is valid\n",param->thread_id+1,coloumn+1);
        // free(present);
    }

    pthread_exit(NULL);
}

void* runner3(void* arg){
    info* param = (info *)arg;

    for(int grid=param->start;grid<=param->end;grid++){
        bool gridvalid=true;
        // int* present = (int*) malloc((N+1)*sizeof(int));
        int present[N+1];
        for(int i=0;i<N+1;i++){
            present[i]=0;
        }
        for(int index=0;index<N;index++){
            int row = (grid/n) * n + index/n;
            int coloumn = (grid%n) * n + index%n;
            int num = sudoku[row][coloumn];
            if(present[num]>0){
                gridvalid=false;
                allgrid=false;
                break;
            }
            present[num]+=1;
        }
        if(gridvalid==false){
            fprintf(output,"Thread %d checks subgrid %d and it is invalid\n",param->thread_id+1,grid+1);
        }
        else fprintf(output,"Thread %d checks coloumn %d and it is valid\n",param->thread_id+1,grid+1);
        // free(present);
    }
    pthread_exit(NULL);
}

int main(){

    //reading input file
    FILE *inputfile = fopen("input.txt","r");
    if(inputfile == NULL){
        printf("input file not found\n");
        return 1;
    }
    
    //Number of threads, size of sudoku
    fscanf(inputfile,"%d %d",&K,&N);
    n = sqrt(N);

    sudoku =(int **) malloc(N*sizeof(int*));
    for(int i = 0;i<N;i++){
        sudoku[i] = (int*)malloc(N*sizeof(int));
    }
    //Taking sudoku from inputfile
    for(int i = 0;i<N;i++){
        for(int j = 0;j<N;j++){
            fscanf(inputfile,"%d",&sudoku[i][j]);
        }
    }
    fclose(inputfile);

    output = fopen("output.txt","w");

    // clock_t timestart,timeend;
    // timestart = clock();
    struct timeval start, end;
    gettimeofday(&start, NULL);

    threads = (pthread_t *) malloc(K*sizeof(pthread_t));

    int K1 = K / 3;
    int K2 = K / 3;
    int K3 = K / 3;

    int remainder = K % 3;

    if (remainder >= 1) K1++;
    if (remainder >= 2) K2++;

    int P1 = N/K1 , P2 =N/K2 ,P3 =N/K3 ;

    info *info1 = (info *) malloc(K1*sizeof(info));
    info *info2 = (info *) malloc(K2*sizeof(info));
    info *info3 = (info *) malloc(K3*sizeof(info));

    for(int i=0;i<K1;i++){
        info1[i].thread_id = i;
        info1[i].start = i*P1;
        if(i==K1-1) info1[i].end = N-1;
        else info1[i].end = (i+1)*P1-1;
        pthread_create(&threads[i],NULL,runner1,(void *)&info1[i]);
    }

    for(int i=0;i<K2;i++){
        info2[i].thread_id = i+K1;
        info2[i].start = i*P2;
        if(i==K2-1) info2[i].end = N-1;
        else info2[i].end = (i+1)*P2-1;
        pthread_create(&threads[i+K1],NULL,runner2,(void *)&info2[i]);
    }

    for(int i=0;i<K3;i++){
        info3[i].thread_id = i+K1+K2;
        info3[i].start = i*P3;
        if(i==K3-1) info3[i].end = N-1;
        else info3[i].end = (i+1)*P3-1;
        pthread_create(&threads[i+K1+K2],NULL,runner3,(void *)&info3[i]);
    }


    for(int i = 0;i<K;i++){
        pthread_join(threads[i],NULL);
    }

    // timeend = clock();
    gettimeofday(&end, NULL);
    double elapsed_time = (end.tv_sec - start.tv_sec) * M + (end.tv_usec - start.tv_usec);


    if(allrow && allcol && allgrid)fprintf(output,"Sudoku is valid\n");
    else fprintf(output,"Sudoku is invalid\n");

    // fprintf(output,"Total time taken is %f\n",((double)(timeend-timestart)*M/CLOCKS_PER_SEC));
    // printf("%f\n",((double)(timeend-timestart)*M/CLOCKS_PER_SEC));

    fprintf(output, "Total time taken: %f microseconds\n", elapsed_time);
    printf("Total time taken: %f microseconds\n", elapsed_time);
    
    fclose(output);

    return 0;
    
}