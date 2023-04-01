#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <windows.h>
#include <stdbool.h>

typedef struct{
    int id;
    int unit_price;
    int quantity;
}PRODUCT;

pthread_mutex_t* mutex_vector;
#define no_Products 2
#define no_threads 4
int tracker[no_Products];

int generate_random(int quantity){
    int upper_bound = quantity;
    int digits = 1;
    bool pass = false;;
    while (upper_bound!=0){
        digits *= 10;
        upper_bound /= 10;
    }

    int random_number = rand();

    while (!pass){
        if (random_number % digits > quantity){
            random_number = rand();
        } else {
            if (random_number % digits == 0){
                random_number = rand();
            } else {
                pass = true;
            }
        }
    }
    return random_number % digits;
}

bool available_products(PRODUCT* list_of_products){
    for(int i=0; i<no_Products; i++){
        if(list_of_products[i].quantity!=0){
            return true;
        }
    }
    return false;
}

void *start_sales(void* list_of_products){
    while(available_products((PRODUCT*)list_of_products)) {
        int target = rand() % no_Products;
        while(((PRODUCT*)list_of_products)[target].quantity == 0){
            target = rand() % no_Products;
        }
        int sold_quantity;
        int amount_of_money;

        if(((PRODUCT*)list_of_products)[target].quantity != 0){
            sold_quantity = generate_random(((PRODUCT*)list_of_products)[target].quantity);
            amount_of_money = sold_quantity * ((PRODUCT*)list_of_products)[target].unit_price;

            pthread_mutex_lock(&mutex_vector[target]);
            ((PRODUCT*)list_of_products)[target].quantity -= sold_quantity;
            //Sleep(1000);
            pthread_mutex_unlock(&mutex_vector[target]);

            tracker[target] += sold_quantity;

            printf("%d bought %d PR%d worth %d\n", pthread_self(), sold_quantity, target, amount_of_money);
        }
    }
}

int main() {
    PRODUCT* list_of_products = malloc(sizeof(PRODUCT)*no_Products);
    mutex_vector = (pthread_mutex_t*)malloc(sizeof (pthread_mutex_t)*no_Products);
    printf("Products:\n");
    for(int i = 0; i < no_Products; i++){
        list_of_products[i].id = i;
        list_of_products[i].unit_price = generate_random(500);
        list_of_products[i].quantity = generate_random(999);

        pthread_mutex_init(&mutex_vector[i], NULL);
        printf("PR%d: PRICE: %d QUANTITY: %d\n", i, list_of_products[i].unit_price, list_of_products[i].quantity);
    }

    //list_of_products[0].id = 1;
    //list_of_products[0].unit_price = 15;
    //list_of_products[0].quantity = 10;
    //list_of_products[1].id = 2;
    //list_of_products[1].unit_price = 55;
    //list_of_products[1].quantity = 20;



    pthread_t* sales = (pthread_t*) malloc(sizeof(pthread_t) * no_threads);
    for(int i = 0; i < no_threads; i++){
        pthread_create(&sales[i], NULL, start_sales, (void*)list_of_products);
    }
    for(int i = 0; i < no_threads; i++){
        pthread_join(sales[i], NULL);
    }

    printf("Sold quantities:\n");
    for(int i = 0; i < no_Products; i++){
        printf("PR%d: %d quantities\n", i, tracker[i]);
    }

    return 0;
}
