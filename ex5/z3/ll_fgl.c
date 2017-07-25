#include <stdio.h>
#include <stdlib.h> /* rand() */
#include <limits.h>
#include <pthread.h> /* for pthread_spinlock_t */

#include "../common/alloc.h"
#include "ll.h"

typedef struct ll_node {
	int key;
	pthread_spinlock_t lock; 
	struct ll_node *next;
	/* other fields here? */
} ll_node_t;

struct linked_list {
	ll_node_t *head;
	/* other fields here? */
};

/**
 * Create a new linked list node.
 **/
static ll_node_t *ll_node_new(int key)
{
	ll_node_t *ret;

	XMALLOC(ret, 1);
	ret->key = key;
	pthread_spin_init( &(ret->lock),  PTHREAD_PROCESS_PRIVATE);
	ret->next = NULL;
	/* Other initializations here? */

	return ret;
}

/**
 * Free a linked list node.
 **/
static void ll_node_free(ll_node_t *ll_node)
{
	XFREE(ll_node);
}

/**
 * Create a new empty linked list.
 **/
ll_t *ll_new()
{
	ll_t *ret;

	XMALLOC(ret, 1);
	ret->head = ll_node_new(-1);
	ret->head->next = ll_node_new(INT_MAX);
	ret->head->next->next = NULL;

	return ret;
}

/**
 * Free a linked list and all its contained nodes.
 **/
void ll_free(ll_t *ll)
{
	ll_node_t *next, *curr = ll->head;
	while (curr) {
		next = curr->next;
		ll_node_free(curr);
		curr = next;
	}
	XFREE(ll);
}

int ll_contains(ll_t *ll, int key)
{
  	int ret=0;
  	ll_node_t *pred, *curr;

  	curr = ll->head;
  	pthread_spin_lock( &(curr->lock));

  //starts try
  	pred = ll->head;
  	curr = pred->next;
  	pthread_spin_lock( &(curr->lock));

  	while( curr->next != NULL && curr->key < key  ) {
    		pthread_spin_unlock(&(pred->lock));
    		pred = curr;
    		curr = curr->next;
    		pthread_spin_lock(&(curr->lock));
  	}

  	pthread_spin_unlock(&(curr->lock));
  	if( curr->key == key)
  		ret=1;
  	else
    	{
      		ret= 0;
    	}

  	pthread_spin_unlock(&(pred->lock));

  	return ret;
  /* return 0; */
}

int ll_add(ll_t *ll, int key)
{
  	int ret=0;
  	ll_node_t *pred, *curr;

  	pred = ll->head;
  	pthread_spin_lock( &(pred->lock));

  //starts try
  	curr = pred->next;
  	pthread_spin_lock( &(curr->lock));
	
  	while( curr != NULL && curr->key < key  ) {
    		pthread_spin_unlock(&(pred->lock));
    		pred = curr;
    		curr = pred->next;
    		pthread_spin_lock(&(curr->lock));
  	}

  	if(curr->key == key)
  		ret=0;
  	else
    	{
     	 	ll_node_t *neo;
      		neo = ll_node_new(key);
      		pred->next=neo;
      		neo->next=curr;
      		ret= 1;
    	}
  	pthread_spin_unlock(&(curr->lock));
  	pthread_spin_unlock(&(pred->lock));

  	return ret;
    /* return 0; */
}

int ll_remove(ll_t *ll, int key)
{
  	int ret=0;
  	ll_node_t *pred, *curr;

  	curr = ll->head;
  	pthread_spin_lock( &(curr->lock));

  //starts try
  	pred = ll->head;
  	curr = pred->next;
  	pthread_spin_lock( &(curr->lock));

  	while(curr->key < key && curr->next != NULL) {
    		pthread_spin_unlock(&(pred->lock));
    		pred = curr;
    		curr = curr->next;
    		pthread_spin_lock(&(curr->lock));
  	}
  	if(curr->key == key)
	  {
	    pred->next = curr->next;

	    pthread_spin_unlock(&(pred->lock));
	    pthread_spin_unlock(&(curr->lock));
	    ll_node_free(curr);

	    ret=1;
	  }
	else
	  {
	    pthread_spin_unlock(&(curr->lock));
	    pthread_spin_unlock(&(pred->lock));
	    ret=0;
	  }
  	return ret;
	/* return 9; */
}

/**
 * Print a linked list.
 **/
void ll_print(ll_t *ll)
{
	ll_node_t *curr = ll->head;
	printf("LIST [");
	while (curr) {
		if (curr->key == INT_MAX)
			printf(" -> MAX");
		else
			printf(" -> %d", curr->key);
		curr = curr->next;
	}
	printf(" ]\n");
}
