#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#include "BENSCHILLIBOWL.h"

// Feel free to play with these numbers! This is a great way to
// test your implementation.
#define BENSCHILLIBOWL_SIZE 100
#define NUM_CUSTOMERS 90
#define NUM_COOKS 10
#define ORDERS_PER_CUSTOMER 3
#define EXPECTED_NUM_ORDERS NUM_CUSTOMERS * ORDERS_PER_CUSTOMER

// Global variable for the restaurant.
BENSCHILLIBOWL *bcb;

/**
 * Thread funtion that represents a customer. A customer should:
 *  - allocate space (memory) for an order.
 *  - select a menu item.
 *  - populate the order with their menu item and their customer ID.
 *  - add their order to the restaurant.
 */
void* BENSCHILLIBOWLCustomer(void* tid) {
  int id =  rand() % 1000;
  
  for(int i = 0; i < ORDERS_PER_CUSTOMER; i++){
    Order* cOrder = malloc(sizeof(Order));
    
    cOrder->menu_item = PickRandomMenuItem();
    cOrder->customer_id = id;
    cOrder->order_number = rand() % 1000;
    
    AddOrder(bcb, cOrder);
    
    printf("This customer with is: %d has ordered: %d\n", cOrder->customer_id, cOrder->order_number);
  }
	
  return NULL;
}

/**
 * Thread function that represents a cook in the restaurant. A cook should:
 *  - get an order from the restaurant.
 *  - if the order is valid, it should fulfill the order, and then
 *    free the space taken by the order.
 * The cook should take orders from the restaurants until it does not
 * receive an order.
 */
void* BENSCHILLIBOWLCook(void* tid) {
  int id = rand() % 1000;
	int oFulfilled = 0;  //fulfilled orders
  
  Order* getOrder = GetOrder(bcb);
  
  while(getOrder != NULL){
    oFulfilled += 1;
    Order* getOrder = GetOrder(bcb);
    
    if(getOrder == NULL){
      break;
    }
  }
      printf("Cook #%d fulfilled %d orders\n", id, oFulfilled);
      return NULL;
}

/**
 * Runs when the program begins executing. This program should:
 *  - open the restaurant
 *  - create customers and cooks
 *  - wait for all customers and cooks to be done
 *  - close the restaurant.
 */
int main() {
  bcb = OpenRestaurant(BENSCHILLIBOWL_SIZE, EXPECTED_NUM_ORDERS);
  pthread_t customers[NUM_CUSTOMERS];
  pthread_t cooks[NUM_COOKS];
  
  for (int i = 0; i <NUM_CUSTOMERS; i++){
    int* size = malloc(sizeof(int));
    *size = i;
    
    pthread_create(&customers[i], NULL, BENSCHILLIBOWLCustomer, size);
  } 
  
  for (int j = 0; j < NUM_COOKS; j++){
    int *size =  malloc(sizeof(int));
    *size = j;
    pthread_create(&cooks[j], NULL, BENSCHILLIBOWLCook, size);
  }
  
  for (int m = 0; m < NUM_CUSTOMERS; m++){
    pthread_join(customers[m], NULL);
  }
  
  for(int k = 0; k < NUM_COOKS; k++){
    pthread_join(cooks[k], NULL);
  }
  
  CloseRestaurant(bcb);

  return 0;
}