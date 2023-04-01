#include <iostream>
#include "mpi.h"
#include <stdint.h>

/*
 * The main process does the first split on the numbers then passes those numbers
 * to its workers
 * The workers call karatsuba method
 *
 * */

int karatsuba(int num1, int num2);

void worker(){
    int num1, num2;
    MPI_Status status;
    MPI_Recv(&num1, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
    MPI_Recv(&num2, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);

    int prod = karatsuba(num1, num2);

    MPI_Ssend(&prod, 1, MPI_INT, 0, 3, MPI_COMM_WORLD);
}

int karatsuba(){
    vector<int> numbs;
    MPI_Recv(&numbs[0], 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
    MPI_Recv(&numbs[1], 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
    if(numbs[0] < 100) or (numbs[1] < 100){
        return numbs[0] * numbs[1];
    }else{
        int len1 = std::to_string(numbs[0]).length();
        int len2 = std::to_string(numbs[1]).length()
        int n = max(len1, len2);

        int a, b, c, d, ac, bd, ad_plus_bc;

        a = num1 / pow(10, n/2);
        b = num1 % pow(10, n/2);
        c = num2 / pow(10, n/2);
        d = num2 % pow(10, n/2);

        vector<int> a_c = {a,c};
        MPI_Ssend(&a_c, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
        vector<int> b_d = {b,d};
        MPI_Ssend(&b_d, 1, MPI_Type_vector)
        ac = karatsuba(a, c);
        bd = karatsuba(b, d);
        ad_plus_bc = karatuba(a+b, c+d) -ac -bd;
        return ac * pow(10, n) + ad_plus_bc * pow(10, n/2) + bd;
    }
    return 0;
}

int main(int argc, char* argv[]){
    MPI_Init(0,0);
    int me;
    int nrProcs;
    MPI_Comm_size(MPI_COMM_WORLD, &nrProcs);
    MPI_Comm_rank(MPI_COMM_WORLD, &me);

    unsigned n;
    int num1, num2;

    if(me == 0){
        parent();
        int prod = karatsuba(num1, num2, nrProcs);
        cout << prod;
    }else{
        worker();
    }
    MPI_Finalize();
    return 0;
}