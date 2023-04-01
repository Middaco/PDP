#include <iostream>
#include <thread>
#include <fstream>
#include "thread-pool-master/include/ThreadPool.h"
#include <chrono>

using namespace std::chrono;

using namespace std;

/*
 * Create an actual thread for each task
 * I will follow the first approach
 */

#define NO_THREADS 4

const int rows1 = 8, columns1 = 8,
    rows2 = 8, columns2 = 8;

int matrix1[rows1][columns1], matrix2[rows2][columns2], result_matrix[rows1][columns2];

int process_element(int row, int column){
    int result = 0;
    for(int i = 0; i < columns1; i++){
        result += matrix1[row][i] * matrix2[i][column];
    }
    return result;
}

void multiply(int start_row, int start_column, int processable){
    for(int i = start_row; processable != 0; i++){
        for(int j = start_column; j < columns2 && processable != 0; j++){
            result_matrix[i][j] = process_element(i,j);
            processable--;
        }
    }
}

void print_result_matrix(){
    for(int i = 0; i < rows1; i++){
        for(int j = 0; j < columns2; j++){
            cout << result_matrix[i][j] << " ";
        }
        cout<< endl;
    }
}

int main() {
    ifstream fin1(R"(C:\Users\Asus\OneDrive\Desktop\Facultate\Laboratoare\An3-Semestu1-Engleza\pdp\Lab3_1\matrix1.txt)");
    ifstream fin2(R"(C:\Users\Asus\OneDrive\Desktop\Facultate\Laboratoare\An3-Semestu1-Engleza\pdp\Lab3_1\matrix2.txt)");

    if(columns1 != rows2){
        cout << "Cannot multiply these matrices";
        return -1;
    }

    int i = 0, j = 0;

    while(fin1 >>  matrix1[i][j]){
        if(j != columns1-1){
            j++;
        } else {
            if(i != rows1-1){
                i++;
                j = 0;
            }
        }
    }

    i=j=0;
    while(fin2 >>  matrix2[i][j]){
        if(j != columns2-1){
            j++;
        } else {
            if(i != rows2-1){
                i++;
                j = 0;
            }
        }
    }
    fin1.close();
    fin2.close();

    //Computes how many elements one thread should process
    int processable = rows1 * columns2 / NO_THREADS;

    thread thread_list[NO_THREADS];

    int start_row = 0, start_column = 0;

    milliseconds ms1 = duration_cast< milliseconds >(
            system_clock::now().time_since_epoch()
    );

    for(i = 0; i < NO_THREADS; i++){
        if( i == NO_THREADS - 1){
            thread_list[i] = thread(multiply, start_row, start_column, processable + rows1 * columns2 % NO_THREADS);
        }else{
            thread_list[i] = thread(multiply, start_row, start_column, processable);
        }

        start_column += processable;
        while(start_column >= columns2){
            start_row += 1;
            start_column = start_column - columns2;
        }
    }



    for(i = 0; i < NO_THREADS; i++){
        thread_list[i].join();
    }

    milliseconds ms2 = duration_cast< milliseconds >(
            system_clock::now().time_since_epoch()
    );
    cout << "Threads: \n";
    print_result_matrix();
    cout << "Time: "<< ms2.count()-ms1.count();

    ThreadPool pool(NO_THREADS);
    pool.init();

    milliseconds ms3 = duration_cast< milliseconds >(
            system_clock::now().time_since_epoch()
    );
    for(i = 0; i < NO_THREADS; i++){
        pool.submit(multiply, start_row, start_column, processable);
        start_column += processable;
        while(start_column >= columns2){
            start_row += 1;
            start_column = start_column - columns2;
        }
    }
    pool.shutdown();
    milliseconds ms4 = duration_cast< milliseconds >(
            system_clock::now().time_since_epoch()
    );

    cout << "\n\nThread pool: \n";
    print_result_matrix();
    cout << "Time: " <<ms4.count() - ms3.count();

    return 0;
}
