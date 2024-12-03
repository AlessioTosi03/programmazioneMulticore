#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>

void matrix_build(int **matrix1, int **matrix2, int ords[]);
void matrix_mul(int **matrix1, int **matrix2, int **result, int ords[]);

int main(int argc, char *argv[]){
    if (argc != 5) {
        printf("Usage: %s rows1 cols1 rows2 cols2\n", argv[0]);
        return 1;
    }

    srand(time(NULL));
    int rows1 = atoi(argv[1]);
    int cols1 = atoi (argv[2]);
    int rows2 = atoi(argv[3]);
    int cols2 = atoi(argv[4]);
    int thread_count = 4;

    int **matrix1 = malloc(rows1 * sizeof(int *));
    for (int i=0;i < rows1;i++) {
        matrix1[i] = malloc(cols1 * sizeof(int));
    }
    int **matrix2 = malloc(rows2 * sizeof(int *));
    for (int i=0;i < rows2;i++) {
        matrix2[i] = malloc(cols2 * sizeof(int));
    }
    int **result = malloc(rows1 * sizeof(int *));
    for (int i=0;i < rows1;i++) {
        result[i] = malloc(cols2 * sizeof(int));
    }

    int ords[] = {rows1,cols1,rows2,cols2};
    #pragma omp parallel if(omp_get_thread_num == thread_count-1)
    matrix_build(matrix1,matrix2,ords);
    if(ords[1] == ords[2]){
        #pragma omp parallel num_threads(thread_count)
        matrix_mul(matrix1, matrix2, result, ords);
    }else{
        printf("ERROR:counts of column of the first matrix and rows of the second should be equal");
        return 1;
    }
    printf("The result matrix is:\n");
    printf("[");
    for(int i = 0;i< ords[0];i++){
        for (int j = 0; j<ords[3];j++){
            if(i != 0 && j == 0){
                printf(" ");
            }
            printf("%d,",result[i][j]);
        }
        if(i<ords[0]-1){
            #pragma omp critical
            printf("\n");
        }
    }
    printf("]\n");
}

void matrix_mul(int **matrix1, int **matrix2, int **result, int ords[]){
    #pragma omp parallel for collapse(3)
    for (int i = 0;i < ords[0];i++){
        for (int j = 0;j < ords[3];j++){
            int x = 0;
            for(int k = 0;k < ords[1];k++){
                x += matrix1[i][k] * matrix2[k][j];
            }
            result[i][j] = x;
        }
    }
}

void matrix_build(int **matrix1, int **matrix2, int ords[]){
    #pragma omp critical
    printf("The first auto-generated matrix is:\n[");
    #pragma omp parallel for collapse(2)
    for (int i = 0;i < ords[0];i++){
        for (int j = 0;j < ords[1];j++){
            matrix1[i][j] = rand() % 10;
            if(i != 0 && j == 0){
                #pragma omp critical
                printf(" ");
            }
            #pragma omp critical
            printf("%d,",matrix1[i][j]);
        }
        if(i<ords[0]-1){
            #pragma omp critical
            printf("\n");
        };
    }
    #pragma omp critical
    printf("]\n");

    #pragma omp critical
    printf("The second auto-generated matrix is:\n[");
    #pragma omp parallel for collapse(2)
    for (int i = 0;i < ords[2];i++){
        for (int j = 0;j < ords[3];j++){
            matrix2[i][j] = rand() % 10;
            if(i != 0 && j == 0){
                #pragma omp critical
                printf(" ");
            }
            #pragma omp critical
            printf("%d,",matrix2[i][j]);
        }
        if(i<ords[2]-1){
            #pragma omp critical
            printf("\n");
        }
    }
    #pragma omp critical
    printf("]\n");
}