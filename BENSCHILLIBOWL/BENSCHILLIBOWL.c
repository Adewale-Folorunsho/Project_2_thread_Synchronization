#include "BENSCHILLIBOWL.h"

#include <assert.h>
#include <stdlib.h>
#include <time.h>

bool IsEmpty(BENSCHILLIBOWL* bcb);
bool IsFull(BENSCHILLIBOWL* bcb);
void AddOrderToBack(BENSCHILLIBOWL *bcb, Order *order);
void Print(BENSCHILLIBOWL* bcb); 

MenuItem BENSCHILLIBOWLMenu[] = { 
    "BensChilli", 
    "BensHalfSmoke", 
    "BensHotDog", 
    "BensChilliCheeseFries", 
    "BensShake",
    "BensHotCakes",
    "BensCake",
    "BensHamburger",
    "BensVeggieBurger",
    "BensOnionRings",
};
int BENSCHILLIBOWLMenuLength = 10;

/* Select a random item from the Menu and return it */
MenuItem PickRandomMenuItem() {
    int item = rand() % BENSCHILLIBOWLMenuLength;
  return BENSCHILLIBOWLMenu[item];
}

/* Allocate memory for the Restaurant, then create the mutex and condition variables needed to instantiate the Restaurant */

BENSCHILLIBOWL* OpenRestaurant(int max_size, int expected_num_orders) {
  BENSCHILLIBOWL* restaurantDetails = malloc(sizeof(BENSCHILLIBOWL));
  
  restaurantDetails->orders = NULL;
  restaurantDetails->current_size = 0;
  restaurantDetails->next_order_number = 0;
  restaurantDetails->max_size = max_size;
  restaurantDetails->orders_handled = 0;
  restaurantDetails->expected_num_orders = expected_num_orders;
  
  pthread_mutex_init(&restaurantDetails->mutex, NULL);
  pthread_cond_init(&restaurantDetails->can_add_orders, NULL);
  pthread_cond_init(&restaurantDetails->can_get_orders, NULL);
  
  printf("Restaurant is open!\n");
  return restaurantDetails;
}


/* check that the number of orders received is equal to the number handled (ie.fullfilled). Remember to deallocate your resources */

void CloseRestaurant(BENSCHILLIBOWL* bcb) {
  pthread_mutex_destroy(&bcb->mutex);
  pthread_cond_destroy(&bcb->can_add_orders);
  pthread_cond_destroy(&bcb->can_get_orders);
  
  free(bcb);
  
  printf("Restaurant is closed!\n");
}

/* add an order to the back of queue */
int AddOrder(BENSCHILLIBOWL* bcb, Order* order) {
  pthread_mutex_lock(&bcb->mutex);
  while(IsFull(bcb) == true){
    pthread_cond_wait(&bcb->can_add_orders, &bcb->mutex);
  }
  
    order->next = NULL;

    AddOrderToBack(bcb, order);

    bcb->current_size += 1;

    pthread_cond_signal(&bcb->can_get_orders);
    pthread_mutex_unlock(&bcb->mutex);
  
  return order->order_number;
}

/* remove an order from the queue */
Order *GetOrder(BENSCHILLIBOWL* bcb) {
  pthread_mutex_lock(&bcb->mutex);
  
  while(IsEmpty(bcb) == true){
    if(bcb->orders_handled >= bcb->expected_num_orders){
      pthread_cond_signal(&bcb->can_add_orders);
      pthread_cond_signal(&bcb->can_get_orders);
      pthread_mutex_unlock(&bcb->mutex);
      return NULL;
    }
    
    pthread_cond_wait(&bcb->can_get_orders, &bcb->mutex);
  }
  
  if(bcb->orders ==NULL){
    pthread_cond_signal(&bcb->can_add_orders);
    pthread_mutex_unlock(&bcb->mutex);
    return NULL;
  }
  
  Order* order = bcb->orders;
  bcb->orders = order->next;
  bcb->current_size -= 1;
  bcb->orders_handled += 1;
  
  pthread_cond_signal(&bcb->can_add_orders);
  pthread_mutex_unlock(&bcb->mutex);
  return order;
}

// Optional helper functions (you can implement if you think they would be useful)
bool IsEmpty(BENSCHILLIBOWL* bcb) {
  if (bcb->current_size <= 0) {
    return true;
  }
  return false;
}

bool IsFull(BENSCHILLIBOWL* bcb) {
  if (bcb->current_size >= bcb->max_size) {
    return true;
  }
  return false;
}

void Print(BENSCHILLIBOWL* bcb) {
  Order *ordersLList = bcb->orders;
  
  while(ordersLList != NULL) {
    printf("%d -> ", ordersLList->order_number);
    ordersLList = ordersLList->next;
  }
  printf("NULL\n");
}

/* this methods adds order to rear of queue */
void AddOrderToBack(BENSCHILLIBOWL *bcb, Order *order) {
  Order* ordersLList;
  
  if(!bcb->orders){
    bcb->orders = order;
  }else{
    ordersLList = bcb->orders;
    while(ordersLList->next != NULL){
      ordersLList = ordersLList->next;
    }
    ordersLList->next = order;
  }
}
  

