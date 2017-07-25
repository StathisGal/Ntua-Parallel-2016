#include <stdio.h>
#include <stdlib.h> /* rand() */
#include <limits.h>
#include <pthread.h> /* for pthread_spinlock_t */

#include "../common/alloc.h"
#include "ll.h"

typedef struct ll_node {
  int key, marked;
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
  pthread_spin_init( &(ret->lock),  PTHREAD_PROCESS_PRIVATE);
  ret->key = key;
  ret->marked = 1;
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

int ll_validate(ll_node_t *pred,ll_node_t *curr)
{
  if(pred->marked==1 && curr->marked==1 && pred->next == curr)
    return 1;
  else
    return 0;
}



int ll_contains(ll_t *ll, int key)
{
  ll_node_t *curr= ll->head;
  /* printf("ok3");   */
  if(curr==NULL) printf("problem\n");
  while (curr->key < key) {
    curr = curr->next;
  }
  /* printf("%d\n",curr->key == key && curr->marked); */
  return curr->key == key && curr->marked;
}



int ll_add(ll_t *ll, int key)
{
  /* printf("ok"); */
  ll_node_t *neo;
  ll_node_t *pred;
  ll_node_t *curr;

  int ret = 0;
  while (1) {
    pred = ll->head;
    curr = pred->next;

    while (curr->key <= key) {
      if (key == curr->key)
	break;
      pred = curr;
      curr = curr->next;
    }
    pthread_spin_lock( &(pred->lock));
    pthread_spin_lock( &(curr->lock));

    if (ll_validate(pred,curr)) {
      if (curr->key != key) {
	neo = ll_node_new(key);

	neo->next = curr;
	pred->next = neo;
	pthread_spin_unlock(&(pred->lock));
	pthread_spin_unlock(&(curr->lock));
		ret=1;
	break;
      }
      else {
	pthread_spin_unlock(&(pred->lock));
	pthread_spin_unlock(&(curr->lock));
		ret=0;

	break;
      }
    }
    
    pthread_spin_unlock(&(pred->lock));
    pthread_spin_unlock(&(curr->lock));
  }
  /* printf("okoutret:%d\n",ret); */
  return ret;
}

int ll_remove(ll_t *ll, int key)
{
  ll_node_t *pred,*curr;
  int ret = 0;
  while (1)
    {
      pred = ll->head;
      curr = pred->next;
      while (curr->key <= key)
	{
	  if (key == curr->key)
	    break;
	  pred = curr;
	  curr = curr->next;
	}

      pthread_spin_lock( &(pred->lock));
      pthread_spin_lock( &(curr->lock));

      if (ll_validate(pred,curr)) {
	if (curr->key == key) {

	  curr->marked = 0;
	  pred->next = curr->next;
	  pthread_spin_unlock(&(pred->lock));
	  pthread_spin_unlock(&(curr->lock));
	  ret = 1;

	  /* ll_node_free(curr); */
	  break;
	}
	else {

	    pthread_spin_unlock(&(pred->lock));
	    pthread_spin_unlock(&(curr->lock));
	    ret = 0;
	  break;
	}
      }
      pthread_spin_unlock(&(pred->lock));
      pthread_spin_unlock(&(curr->lock));
    }
  /* printf("ok2outret:%d\n",ret); */

  
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
