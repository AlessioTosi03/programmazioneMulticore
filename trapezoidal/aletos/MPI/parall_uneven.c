#include <stdio.h>
#include <mpi.h>
#include <string.h>

//questo codice a differenza del codice parallelo classico, accetta una quantità 
//di lavoro non divisibile in modo uniforme distribuendo il resto nei thread
//grazie a questa implementazione ottengo i valori iniziali tramite input

double f(double x){
    //funzione variabile, in questo caso è stata scelta la funzione f(x) = x^2
    x *= x;
    return x;
};

double Trap(
    double left_endpt,
    double right_endpt,
    int trap_count,
    double base_len
){
    double estimate, x;
    int i;
    estimate = (f(left_endpt) + f(right_endpt))/2.0;
    for (i = 1; i <= trap_count-1; i++){
        x = left_endpt + i*base_len;
        estimate += f(x);
    }
    estimate = estimate*base_len;

    return estimate;
}

void Get_Input(
        int my_rank,
        int comm_sz,
        double* a_p,
        double* b_p,
        int* n_p
    ){
        if(my_rank == 0){
            printf("Enter a, b, and n\n"),
            scanf("%lf %lf %d",a_p, b_p, n_p);
        }
        MPI_Bcast(a_p, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        MPI_Bcast(b_p, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        MPI_Bcast(n_p, 1, MPI_INT, 0, MPI_COMM_WORLD);
    };

int main(void){
    int my_rank, comm_sz, n, local_n;
    double a, b, h, local_a, local_b;
    double local_int, total_int;
    int source;

    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD,&comm_sz);

    Get_Input(my_rank,comm_sz,&a,&b,&n);
    
    h = (b-a)/n;
    local_n = n/comm_sz;
    int remainder = n % comm_sz;
    if (my_rank < remainder){
        local_a = a + my_rank*(local_n + 1)*h;
        local_b = local_a + (local_n + 1)*h;
    }else{
        local_a = a + my_rank*(local_n + remainder)*h;
        local_b = local_a + (local_n + remainder)*h;
    }
    printf("%.3f\n",local_a);
    local_int = Trap(local_a,local_b,local_n,h);
    if (my_rank != 0){
        MPI_Send(&local_int, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
    }else{
        total_int = local_int;
        for(source = 1; source < comm_sz; source++){
            MPI_Recv(&local_int, 1, MPI_DOUBLE, source, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            total_int += local_int;
        }
    }

    if(my_rank == 0){
        printf("With n = %d trapezoids, our estimate\n",n);
        printf("of the integral from %f to %f = %.15e\n", a,b,total_int);
    }
    MPI_Finalize();
    return 0;
};