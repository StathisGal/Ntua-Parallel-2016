#include <stdio.h>
#include <stdlib.h> /* rand() */
#include <limits.h>

#include "../common/alloc.h"
#include "ll.h"

typedef struct ll_node {
  int key;
  struct ll_node *next;
  char padding[64 - sizeof(int) - sizeof(struct ll_node *)];
  
} ll_node_t;

struct linked_list {
	ll_node_t *head;
	/* other fields here? */
};

typedef struct window_node {
  ll_node_t *pred,*curr;
} *window;
/**
 * Create a new linked list node.
 **/
static ll_node_t *ll_node_new(int key)
{
	ll_node_t *ret;

	XMALLOC(ret, 1);
	ret->key = key;
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
	ret->head->next = (ll_node_t*)((long long) ret->head->next | 1);
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
	  next = (ll_node_t*)((long long)curr->next & ~1);
		ll_node_free(curr);
		curr = next;
	}
	XFREE(ll);
}

ll_node_t * getReference(ll_node_t * llnode)
{
  return (ll_node_t *) (((long long) llnode) & (~1)) ; // TODO check it again if it works
}  

ll_node_t * getmark(ll_node_t * ll,int * mark)
{

  *mark= (int)( ((long long) ll) & 1); // takes the mark of current node
  return getReference( ll); // points to the next

}


window find(ll_t *ll, int key)
{
  int snip;
  ll_node_t *pred,*curr,*succ;
  window neo;
  int marked[]={0};

  pred=curr=succ=NULL;

  while(1)
    {
    retry:
      pred=ll->head;
      curr=getReference(pred->next); //gets the real address of the next of the 
      while(1)
	{
	  succ=getmark(curr->next,marked);  //finds next node which we store in succ and  get mark of current node

	  while(marked[0])
	    {
	      snip= __sync_bool_compare_and_swap(&pred->next,(ll_node_t*)((long long)curr | 1),(ll_node_t*)((long long)succ | 1));
	      if(!snip)
	  	goto retry;
	      curr=succ;
	      succ=getmark(curr->next,marked);
	    }

	  if(curr->key >= key){
	    neo=(window) malloc(sizeof(struct window_node));
	    neo->pred=pred;
	    neo->curr=curr;
	    return neo;
	  }
	  pred=curr;
	  curr=succ;
	}
    }
}


int ll_contains(ll_t *ll, int key)
{
  int marked;
  ll_node_t *curr,*succ;
  curr=ll->head;
  while(curr->key< key)
    curr=(ll_node_t *)((long long) (curr->next) & (~1));
  succ=getmark(curr->next,&marked);
  return(curr->key == key  && (marked));
}

int ll_add(ll_t *ll, int key)
{

  /* int splice; */
  window win;
  ll_node_t *pred,*curr,*neo;
  /* printf("start add\n"); */
  while(1)
    {
      win=find(ll,key);
      pred=win->pred;
      curr=win->curr;
      if(curr->key==key) // already exists
	{ printf("already exists\n");
    	return 0;
	}
      else
    	{
    	  neo=ll_node_new(key);
    	  neo->next = (ll_node_t *)((long long) curr | 1);
    	  if(__sync_bool_compare_and_swap(&pred->next,(ll_node_t*) (((long long) curr) | 1),(ll_node_t*) ((long long) neo | 1)) )
    	      return 1;
    	}
    }
    return 0;
}

int ll_remove(ll_t *ll, int key)
{
  int snip;
  window win;
  ll_node_t *pred, *curr,*succ;

  while(1)
    {
      win= find(ll,key);
      pred=win->pred;
      curr=win->curr;
      if(curr->key != key)
	  return 0;
      else
  	{
  	  succ=getReference(curr->next); // in succ we have the address of the curr next node
  	  snip= __sync_bool_compare_and_swap(&curr->next,(long long)(curr->next),((long long) curr->next) & (~1)); // we set mark to 0 in order to delete it
  	  if(!snip)
	      continue; // if there was a problem just try loop
  	  __sync_bool_compare_and_swap(&pred->next,(ll_node_t*)((long long)curr | 1),(ll_node_t*)((long long) succ | 1)); // we want pred to point to the succ node
  	  return 1;
  	}
    }
    return 0;
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
