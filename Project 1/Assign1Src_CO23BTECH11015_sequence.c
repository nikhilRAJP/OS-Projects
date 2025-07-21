#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <time.h>
#define M 1.0e6

int N,K,n;
int **sudoku;

bool allrow=1,allcol=1,allgrid=1;

FILE *output;

int main(){

    //reading input file
    FILE *inputfile = fopen("input.txt","r");
    if(inputfile == NULL){
        printf("input file not found\n");
        return 0;
    }
    
    //Number of threads, size of sudoku
    fscanf(inputfile,"%d %d",&K,&N);
    n = sqrt(N);

    sudoku =(int **) malloc(N*sizeof(int*));
    for(int i = 0;i<N;i++){
        sudoku[i] = malloc(N*sizeof(int));
    }
    //Taking sudoku from inputfile
    for(int i = 0;i<N;i++){
        for(int j = 0;j<N;j++){
            fscanf(inputfile,"%d",&sudoku[i][j]);
        }
    }
    fclose(inputfile);

    output = fopen("output.txt","w");

    struct timeval start, end;
    gettimeofday(&start, NULL);

    //check for row
    for(int row = 0;row<N;row++){
        int* present = (int*) malloc((N+1)*sizeof(int));
        for(int i=1;i<N+1;i++){
            present[i]=0;
        }
        bool rowvalid=true;
        for(int coloumn = 0;coloumn<N;coloumn++){
            int num = sudoku[row][coloumn];
            if(present[num]>0){
                rowvalid=false;
                allrow=false;
                break;
            }
            present[num]+=1;
        }
        if(rowvalid==false){
            fprintf(output,"Row %d and it is invalid\n",row+1);
        }
        else fprintf(output,"Row %d and it is valid\n",row+1);
    }

    //check for coloumn
    for(int coloumn = 0;coloumn<N;coloumn++){
        int* present = (int*) malloc((N+1)*sizeof(int));
        for(int i=1;i<N+1;i++){
            present[i]=0;
        }
        bool coloumnvalid=true;
        for(int row = 0;row<N;row++){
            int num = sudoku[row][coloumn];
            if(present[num]>0){
                coloumnvalid=false;
                allcol=false;
                break;
            }
            present[num]+=1;
        }
        if(coloumnvalid==false){
            fprintf(output,"Coloumn %d and it is invalid\n",coloumn+1);
        }
        else fprintf(output,"Coloumn %d and it is valid\n",coloumn+1);
    }

    //check for grid
    for(int grid=0;grid<N;grid++){
        int* present = (int*) malloc((N+1)*sizeof(int));
        for(int i=1;i<N+1;i++){
            present[i]=0;
        }
        bool gridvalid=true;
        for(int index=0;index<N;index++){
        int row = (grid / n) * n + (index / n);
        int coloumn = (grid % n) * n + (index % n);
            int num = sudoku[row][coloumn];
            if(present[num]>0){
                gridvalid=false;
                allgrid=false;
                break;
            }
            present[num]+=1;
        }    
        if(gridvalid==false){
            fprintf(output,"Grid %d and it is invalid\n",grid+1);
        }
        else fprintf(output,"Grid %d and it is valid\n",grid+1);
    }
    
    gettimeofday(&end, NULL);
    double elapsed_time = (end.tv_sec - start.tv_sec) * M + (end.tv_usec - start.tv_usec);


    if(allrow && allcol && allgrid)fprintf(output,"Sudoku is valid\n");
    else fprintf(output,"Sudoku is invalid\n");

    fprintf(output, "Total time taken: %f microseconds\n", elapsed_time);
    printf("Total time taken: %f microseconds\n", elapsed_time);
    fclose(output);

    return 0;
    
}