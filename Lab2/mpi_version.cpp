#include <mpi.h>
#include <cstdio>
#include <vector>
#include <iostream>

void worker(int rank){
    int n;
    MPI_Status status;
    MPI_Recv(&n, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);

    std::vector<int> chunk1, chunk2;
    chunk1.resize(n);

    MPI_Recv(chunk1.data(), n, MPI_INT, 0, 2, MPI_COMM_WORLD, &status);
    MPI_Recv(chunk2.data(), n, MPI_INT, 0, 3, MPI_COMM_WORLD, &status);
    int i = 0, sum = 0;
    while(i != n){
        sum += chunk1[i] * chunk2[i];
    }
    MPI_Ssend(&sum, 1, MPI_INT, 0, 4, MPI_COMM_WORLD);
}

int scalar_prod(const std::vector<int>& a, const std::vector<int>& b, int nrProcs){
    unsigned int n = a.size();
    for(int i = 1; i < nrProcs; i++){
        int st = (i*n) / nrProcs;
        int fn = ((i+1)*n) / nrProcs;
        int nElem = fn - st;
        MPI_Ssend(&nElem, 1, MPI_INT, i, 1, MPI_COMM_WORLD);
        MPI_Ssend(a.data()+st, nElem, MPI_INT, i, 2, MPI_COMM_WORLD);
        MPI_Ssend(b.data()+st, nElem, MPI_INT, i, 3, MPI_COMM_WORLD);
    }
    int sum = 0;
    for(int i = 0; i < n/nrProcs; i++){
        sum += a[i] * b[i];
    }
    for(int i = 1; i < nrProcs; i++){
        int current_sum;
        MPI_Status status;
        MPI_Recv(&current_sum, 1, MPI_INT, i, 4, MPI_COMM_WORLD, &status);
        sum += current_sum;
    }
    return sum;
}

int main(int argc, char** argv) {
    // Initialize the MPI environment
    MPI_Init(0, 0);

    // Get the number of processes
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // Get the rank of the process
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    std::vector<int> a = {1, 1, 1, 1, 1}, b = {1, 2, 3, 4, 5};

    if(rank == 0){
        int sum = scalar_prod(a, b, world_size);
        std::cout << sum;
    }else{
        worker(rank);
    }

    // Finalize the MPI environment.
    MPI_Finalize();
}