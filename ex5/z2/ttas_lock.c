#include "lock.h"
#include "../common/alloc.h"

typedef enum {
  UNLOCKED = 0,
  LOCKED = 1
} lock_state_t;

struct lock_struct {
  lock_state_t state;
};

lock_t *lock_init(int nthreads)
{
  lock_t *lock;

  XMALLOC(lock, 1);
  lock->state=UNLOCKED;
  
  return lock;
}

void lock_free(lock_t *lock)
{
  XFREE(lock);
}

void lock_acquire(lock_t *lock)
{

  while(1) {
    /* if(lock->state == UNLOCKED) { */
    while(lock->state==LOCKED) ;
    if(!(__sync_lock_test_and_set(&lock->state, LOCKED)))
      return;
  }
}

void lock_release(lock_t *lock)
{	
  __sync_lock_release(&lock->state);
}
