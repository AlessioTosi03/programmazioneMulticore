#include <stdio.h>
#include <math.h>
#include <time.h>

float f(float x){
    x=pow(x,2);
    return x;
};

int main(void){
    #include <time.h>
     
    clock_t start, end;
    double cpu_time_used;
     
    start = clock();
    float n;
    printf("input the number of trapezoids for approximation accuracy:\n");
    scanf("%f",&n);
    float a = 0.0,b = 3.0;
    float h = (b - a)/n;
    float approx = (f(a) + f(b))/2;
    for(int i = 1;i <= n-1;i++){
        float x_i = a + i*h;
        approx = approx+f(x_i);
    }
    approx = approx*h;
    printf("\n%.4f\n",approx);
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("%.8f\n",cpu_time_used);
};