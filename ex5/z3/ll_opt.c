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
	pthread_spin_destroy(&(ll_node->lock));
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
int ll_validate(ll_t *ll,ll_node_t *pred,ll_node_t *curr)
{
	ll_node_t *node;
	node=ll->head;
	while(node->key <= pred->key)
	{
		if(node==pred)
		{
			if(pred->next==curr)
				return 1;
			else
				return 0;
		}	
		node=node->next;
	}
	return 0;
}

int ll_contains(ll_t *ll, int key)
{
  /* printf("ok"); */
  int ret=0;
  ll_node_t *pred,*curr;

  while(1)
    {
      pred=ll->head;
      curr=pred->next;
      while( curr->key <= key) {
  	if(key == curr->key)
  	  break;
  	pred=curr;
  	curr=curr->next;
      }
      pthread_spin_lock( &(pred->lock));
      pthread_spin_lock( &(curr->lock));
      if(ll_validate(ll,pred,curr))
  	{
  	  if(curr->key == key)
  	    {
  	      ret=1;
  	      pthread_spin_unlock(&(pred->lock));
  	      pthread_spin_unlock(&(curr->lock));

  	      break;
  	    }
  	  else
  	    {
  	      ret=0;
  	      pthread_spin_unlock(&(pred->lock));
  	      pthread_spin_unlock(&(curr->lock));
  	      break;
  	    }
  	}
      pthread_spin_unlock(&(pred->lock));
      pthread_spin_unlock(&(curr->lock));
    }
     /* printf("okout\n"); */
  return ret;
}


int ll_add(ll_t *ll, int key)
{

  /* printf("ok2"); */
  int ret=0;
  ll_node_t *pred,*curr;
  ll_node_t *neo;
  while(1)
    {
      pred=ll->head;
      curr=pred->next;
      while( curr->key <= key) {
  	if(key == curr->key)
  	  break;
  	pred=curr;
  	curr=curr->next;
      }
      pthread_spin_lock( &(pred->lock));
      pthread_spin_lock( &(curr->lock));
      if(ll_validate(ll,pred,curr))
  	{
  	  if(curr->key == key)
  	    {
  	      ret=0;
  	      pthread_spin_unlock(&(pred->lock));
  	      pthread_spin_unlock(&(curr->lock));
  	      break;
  	    }
  	  else
  	    {
  	      neo= ll_node_new(key);

  	      neo->next=curr;
	      pred->next=neo;
  	      ret=1;
  	      pthread_spin_unlock(&(pred->lock));
  	      pthread_spin_unlock(&(curr->lock));
  	      break;
  	    }
  	}
      pthread_spin_unlock(&(pred->lock));
      pthread_spin_unlock(&(curr->lock));
    }


  /* printf("ok2out\n"); */
  return ret;

}


int ll_remove(ll_t *ll, int key)
{

  /* printf("ok3"); */
  int ret=0;
  ll_node_t *pred,*curr;
  while(1)
    {
      pred=ll->head;
      curr=pred->next;
      while(curr->key <= key) {
	if(key == curr->key)
	  break;
	pred=curr;
	curr=curr->next;
      }
      pthread_spin_lock( &(pred->lock));
      pthread_spin_lock( &(curr->lock));
      if(ll_validate(ll,pred,curr))
	{
	  if(curr->key == key)
	    {
	      pred->next=curr->next;
	      ret=1;
	      pthread_spin_unlock(&(pred->lock));
	      pthread_spin_unlock(&(curr->lock));
	      /* ll_node_free(curr); */
	      /* printf("insidebreak"); */
	      break;
	    }
	  else
	    {
	      ret=0;
	      pthread_spin_unlock(&(pred->lock));
	      pthread_spin_unlock(&(curr->lock));
	      /* printf("insidebreak"); */
	      break;
	    }
	}
      /* printf("wenthere"); */
      pthread_spin_unlock(&(pred->lock));
      pthread_spin_unlock(&(curr->lock));
      /* printf("endoflock"); */
    }

  /* printf("ok3out\n"); */
  return ret;

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
