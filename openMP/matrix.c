
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>
#include "my_timers.h"
#define M 10


/**
 * @brief Get the Cofactor object
 * 
 * @param mat 
 * @param temp 
 * @param p 
 * @param q 
 * @param n 
 */
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

/**
 * @brief Calculates determinant of "mat" n*n size matrix
 * @param mat - pointer to long type matrix
 * @param n - size of square matrix
 * @return determinant of "mat" matrix, type long
 */
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


void main(void){
int m = M;
long det = 0;
time_t tt;
int tim = 0;
srand(tim);
int i = 0;
int j = 0;
int y = 0;


long **a = NULL;
a = (long**)malloc(M*sizeof(long*));

for(i=0; i < m; i++) {
a[i] = (long*)malloc(sizeof(long*)*M);
}
if(a != NULL){
    int j = 0;
    for(i = 0; i < M; i++){
        for(j = 0; j < M; j++){
            a[i][j] = (long)(rand()%5);
            printf(" %d ", a[i][j]);
        }
        printf("\n");
    }
    printf("********\n");
}
start_time();
long **b = NULL;

omp_set_num_threads(2);
#pragma omp parallel for private(i, j, b) reduction(+:det)
for(y = 0; y < M; y++){
    b = (long**)malloc(M*sizeof(long*));
    for(i=0; i < m; i++) {
        b[i] = (long*)malloc(sizeof(long*)*M);
    }
            for(int x = 0; x < M; x++){
                for(int g = 0; g < M; g++){
                   b[x][g] = 0;
                }
            }
   
            getCofactor(a, b, 0, y, m);    
            if(y%2){
                det += (-1)*a[0][y]*determinantOfMatrix(b, m-1);
            }
            else{
                det += a[0][y]*determinantOfMatrix(b, m-1);
            }
    for(i=0; i < m; i++) {
    free(b[i]);  
    }
    free(b);

}
stop_time();
print_time("Elapsed time for n threads:");
long det2 = 0;
start_time();
det2 = determinantOfMatrix(a, m);
stop_time();
print_time("Elapsed time for normal:");
printf("Det: %d\n", det);
printf("Det2: %d\n", det2);

for(i=0; i < m; i++) {
    free(a[i]);
}
free(a);

}