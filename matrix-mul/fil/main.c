#include <stdio.h>

#include <mpi.h>

#include "utils/random.h"
#include "utils/macros.h"
#include "utils/matrix.h"
#include "utils/logging.h"

int main(int argc, char **argv) {

    setvbuf(stderr,NULL,  _IONBF, 0);
    Control(MPI_Init(&argc, &argv));

    fatal_if(argc != 2, "numero sbagliato di parametri");

    int order = atoi(argv[1]);
    log_info("order = %d", order);
    int numeroCelle = order*order;

    int rank, size;

    Control(MPI_Comm_rank(MPI_COMM_WORLD, &rank));
    Control(MPI_Comm_size(MPI_COMM_WORLD, &size));

    int *matrix1 = NULL;
    int *matrix2 = NULL;
    int *risultato = NULL;

    if (rank ==0 ){
        risultato = malloc(numeroCelle*sizeof(int));
        init_random();
        matrix1 = generate_random_matrix(order,order,0,10);
        matrix2 = generate_random_matrix(order,order,0,10);
        square_trasposed_matrix(matrix2,order);
    }else{
        matrix1 = (int*)malloc(numeroCelle*sizeof(int));
        matrix2 = (int*)malloc(numeroCelle*sizeof(int));
    }
    log_info("sono arrivato qua dal rank %d",rank);
    Control(MPI_Bcast(
        matrix1,numeroCelle, MPI_INT,
        0, MPI_COMM_WORLD
    ));

    Control(MPI_Bcast(
        matrix2, numeroCelle, MPI_INT,
        0, MPI_COMM_WORLD
    ));
    log_info("sono arrivato qua dal rank %d",rank);

    for (int y = rank ; y<order ; y+=size){
        int *risultato_linea = malloc(order*sizeof(int));
        for (int x = 0 ; x <= order ; x++){
            risultato_linea[x]=dot_product(matrix1+(y*order),matrix2+(x*order),order);   
        }
        Control(MPI_Send(risultato_linea,order,MPI_INT,0,y,MPI_COMM_WORLD));
        free(risultato_linea);
    }

    if(rank == 0 ){
        for (int r = 1;r<size;r++){
            for(int t = 1;t <= (order+size-1)/size;t++){ //divisione con resto arrotondato superiormente 
                if((r*t)<order){
                    void *partenza = risultato+((r+t)*order);
                    MPI_Recv(partenza,order,MPI_INT,
                                r,t,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
                }
            }
        }
    }
    printMatrix(risultato,order,order);
    Control(MPI_Finalize());
    return 0;
}