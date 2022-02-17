
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "my_timers.h"
#include <mpi.h>
#define M 12


void getCofactor(long** mat, long** temp, int p,
                 int q, int n)
{
    int i = 0, j = 0;

    // Looping for each element of the matrix
    for (int row = 0; row < n; row++)
    {
        for (int col = 0; col < n; col++) 
        {
            //  Copying into temporary matrix only those
            //  element which are not in given row and
            //  column
            if (row != p && col != q) 
            {
                temp[i][j++] = mat[row][col];

                // Row is filled, so increase row index and
                // reset col index
                if (j == n - 1) 
                {
                    j = 0;
                    i++;
                }
            }
        }
    }
}

/* Recursive function for finding determinant of matrix.
   n is current dimension of mat[][]. */
long determinantOfMatrix(long** mat, int n)
{
    long D = 0; // Initialize result

    //  Base case : if matrix contains single element
    if (n == 1){
        return mat[0][0];
    }
    long **temp = NULL;
    temp = (long**)malloc(n*sizeof(long));
    int i = 0;
    int j = 0;
    for(i=0; i < n; i++) {
      temp[i] = (long*)malloc(sizeof(long)*M);
    }

    int sign = 1; // To store sign multiplier
    
    // Iterate for each element of first row
    for (j = 0; j < n; j++) 
    {
        // Getting Cofactor of mat[0][f]
        getCofactor(mat, temp, 0, j, n);
        D += sign * mat[0][j]
             * determinantOfMatrix(temp, n - 1);

        // terms are to be added with alternate sign
        sign = -sign;
    }
    for(i = 0; i < n; i++){
    free(temp[i]);
    }
    free(temp);
    return D;
}

void main(int argc,char *argv[]){
    int m = M;
    long det = 0;
    long temp_det = 0;
    time_t tt;
    int tim = 0;
    srand(tim);
    int y = 0;
    long **matrix = NULL;
    matrix = (long**)malloc(M*sizeof(long*));
    for(int i=0; i < M; i++) {
        matrix[i] = (long*)malloc(sizeof(long*)*M);
    }
    // if(matrix != NULL){
    //     for(int i = 0; i < M; i++){
    //         for(int j = 0; j < M; j++){
    //             matrix[i][j] = (long)(rand()%5);
    //         }
    //     }
    // }
    MPI_Status status;
    int num_procs,my_rank;    
    double start = 0.0, stop = 0.0; //variable for recording time

    MPI_Init(&argc, &argv);  
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);  
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank); 

    if(my_rank == 0){
        if(matrix != NULL){
            for(int i = 0; i < M; i++){
                for(int j = 0; j < M; j++){
                    matrix[i][j] = (long)(rand()%5);
                    printf(" %ld ", matrix[i][j]);
                }
                printf("\n");
            }
            printf("*******\n");
        }
    }
    MPI_Bcast(&matrix[0][0],(M+2)*M, MPI_LONG, 0, MPI_COMM_WORLD);  

    
    
   // start = MPI_Wtime(); //Start calculating time
    start_time();
    if(my_rank != 0){   
         for(int i = 0; i < M; i++){
                for(int j = 0; j < M; j++){
                    printf(" %ld ", matrix[i][j]);
                }
                printf("\n");
            }
            printf("*******\n");
        for(y = my_rank-1; y < M; y += num_procs-1){
                long **b = NULL;
                long sum_in_thread = 0;
                b = (long**)malloc(M*sizeof(long*));
                for(int i=0; i < M; i++) {
                    b[i] = (long*)malloc(sizeof(long*)*M);
                }
                for(int i = 0; i < M; i++){
                    for(int j = 0; j < M; j++){
                        b[i][j] = 0;
                    }
                }
                getCofactor(matrix, b, 0, y, M);
                if(y%2){
                    sum_in_thread += (-1)*matrix[0][y]*determinantOfMatrix(b, M-1);
                }
                else{
                    sum_in_thread += matrix[0][y]*determinantOfMatrix(b, M-1);
                }
            temp_det += sum_in_thread;
            for(int i=0; i < M; i++) {
                free(b[i]);  
            }
            free(b);
            
        }       
    }
    MPI_Reduce(&temp_det, &det, 1, MPI_LONG, MPI_SUM, 0, MPI_COMM_WORLD);
    
    if(my_rank == 0){         
        stop_time();
        print_time("Elapsed time for n threads:");
        long det2 = 0;
        start_time();
        det2 = determinantOfMatrix(matrix, M);
        stop_time();
        print_time("Elapsed time for normal:");
        printf("Det: %ld\n", det);
        printf("Det2: %ld\n", det2);

    /**
     * @brief Cleanup
     * 
     */
        for(int i=0; i < M; i++) {
            free(matrix[i]);
        }
        free(matrix);
    }
    MPI_Finalize();
    
}