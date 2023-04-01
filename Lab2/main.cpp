#include <iostream>
#include <mutex>
#include <thread>
#include <condition_variable>

using namespace std;

#define vector_size 100

int product;
mutex mtx;
condition_variable cv1, cv2;
bool ready = false;
bool eov = false;

void produce(const int* a, const int* b){

    for(int i = 0; i < vector_size; i++){
        unique_lock<mutex> lck(mtx); //locks the mutex
        product = a[i]*b[i];
        ready = true;
        cv1.notify_all();
        cv2.wait(lck);
    }
    eov = true;
    cv1.notify_all();
}

void consume(){
    int sum = 0;
    while (!eov){

        unique_lock<mutex> lck(mtx);
        while(!ready){
            cv1.wait(lck);
            if(eov){
                cout << sum << '\n';
                return;
            }
        }

        sum += product;
        //cout << product << '\n';
        ready = false;
        cv2.notify_all();
    }
}

/*
int main() {
    unique_lock<mutex> lock;

    //int* a = (int*)malloc(vector_size * sizeof(int));
    //int* b = (int*)malloc(vector_size * sizeof(int));
    int a[vector_size];
    int b[vector_size];

    for(int i = 0; i<vector_size; i++){
        a[i] = 1;
        b[i] = i+1;
    }

    thread producer(produce, a, b);
    thread consumer(consume);

    producer.join();
    consumer.join();

    return 0;
}*/
