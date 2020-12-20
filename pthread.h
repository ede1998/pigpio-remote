#ifndef _PTHREAD_H
#define _PTHREAD_H 1

#include <stddef.h>

typedef int pthread_t;
typedef int pthread_mutex_t;

#define __SIZEOF_PTHREAD_MUTEX_T 24
#define __SIZEOF_PTHREAD_ATTR_T 36
#define __SIZEOF_PTHREAD_RWLOCK_T 32
#define __SIZEOF_PTHREAD_BARRIER_T 20
#define __SIZEOF_PTHREAD_MUTEXATTR_T 4
#define __SIZEOF_PTHREAD_COND_T 48
#define __SIZEOF_PTHREAD_CONDATTR_T 4
#define __SIZEOF_PTHREAD_RWLOCKATTR_T 8
#define __SIZEOF_PTHREAD_BARRIERATTR_T 4

union pthread_attr_t
{
    char __size[__SIZEOF_PTHREAD_ATTR_T];
    long int __align;
};
#ifndef __have_pthread_attr_t
typedef union pthread_attr_t pthread_attr_t;
#define __have_pthread_attr_t 1
#endif

/* Data structures for mutex handling.  The structure of the attribute
   type is not exposed on purpose.  */
typedef union
{
  char __size[__SIZEOF_PTHREAD_MUTEXATTR_T];
  int __align;
} pthread_mutexattr_t;


enum
{
    PTHREAD_CANCEL_ENABLE,
#define PTHREAD_CANCEL_ENABLE PTHREAD_CANCEL_ENABLE
    PTHREAD_CANCEL_DISABLE
#define PTHREAD_CANCEL_DISABLE PTHREAD_CANCEL_DISABLE
};

/* Initialize thread attribute *ATTR with default attributes
   (detachstate is PTHREAD_JOINABLE, scheduling policy is SCHED_OTHER,
    no user-provided stack).  */
int pthread_attr_init (pthread_attr_t *__attr);

/* Add information about the minimum stack size needed for the thread
   to be started.  This size must never be less than PTHREAD_STACK_MIN
   and must also not exceed the system limits.  */
int pthread_attr_setstacksize (pthread_attr_t *__attr, size_t __stacksize);

/* Create a new thread, starting with execution of START-ROUTINE
   getting passed ARG.  Creation attributed come from ATTR.  The new
   handle is stored in *NEWTHREAD.  */
int pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine)(void *), void *arg);

/* Initialize a mutex.  */
int pthread_mutex_init(pthread_mutex_t *__mutex, const pthread_mutexattr_t *__mutexattr);
/* Locks a mutex.   */
int pthread_mutex_lock(pthread_mutex_t *mutex);
/* Unlocks a mutex.   */
int pthread_mutex_unlock(pthread_mutex_t *mutex);

/* Set cancelability state of current thread to STATE, returning old
   state in *OLDSTATE if OLDSTATE is not NULL.  */
int pthread_setcancelstate (int __state, int *__oldstate);



#endif // _PTHREAD_H