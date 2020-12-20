#include "pthread.h"

/* Initialize thread attribute *ATTR with default attributes
   (detachstate is PTHREAD_JOINABLE, scheduling policy is SCHED_OTHER,
    no user-provided stack).  */
int pthread_attr_init (pthread_attr_t *__attr)
{
    return 0;
}

/* Add information about the minimum stack size needed for the thread
   to be started.  This size must never be less than PTHREAD_STACK_MIN
   and must also not exceed the system limits.  */
int pthread_attr_setstacksize (pthread_attr_t *__attr, size_t __stacksize)
{
    return 0;
}


/* Create a new thread, starting with execution of START-ROUTINE
   getting passed ARG.  Creation attributed come from ATTR.  The new
   handle is stored in *NEWTHREAD.  */
int pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine)(void *), void *arg)
{
    return 0;
}


/* Initialize a mutex.  */
int pthread_mutex_init(pthread_mutex_t *__mutex, const pthread_mutexattr_t *__mutexattr)
{
    return 0;
}

/* Locks a mutex.   */
int pthread_mutex_lock(pthread_mutex_t *mutex)
{
    return 0;
}

/* Unlocks a mutex.   */
int pthread_mutex_unlock(pthread_mutex_t *mutex)
{
    return 0;
}


/* Set cancelability state of current thread to STATE, returning old
   state in *OLDSTATE if OLDSTATE is not NULL.  */
int pthread_setcancelstate (int __state, int *__oldstate)
{
    return 0;
}

