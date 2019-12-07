#include <stdlib.h>
#include <stdio.h>
#include <ucontext.h>
#include <sys/queue.h>
#include <valgrind/valgrind.h>
#include <sys/time.h>
#include <signal.h>
#include <string.h>
#include "../include/tailhead.h"
#include "../include/thread.h"


static int has_been_initialized = 0;
TAILQ_HEAD(, thread_s) queue;
TAILQ_HEAD(, thread_s) queue_finished;
static struct thread_s *thread_main;

/**********************************PREEMPTION***************************/
#ifdef PREEMPTION
#define PERIOD 4000

static struct sigaction sig_action;
static struct itimerval timer;

/* the action to be associated with SIGALRM (signum=14) */
static void handler_timer(int signum){
	thread_yield();
}


static void init_timer(void){
	memset(&sig_action, 0, sizeof(sig_action));
	sig_action.sa_handler = &handler_timer;

	/*sigaction : is a system call used to change the action taken by a process on receipt of a specific signal*/
	sigaction(SIGALRM , &sig_action , NULL);

	//The sa_mask field allows us to specify a set of signals that aren’t permitted to interrupt execution of this handler
	//SIGALRM can't interrupt our handler
	sigemptyset(&sig_action.sa_mask);
	sigaddset(&sig_action.sa_mask , SIGALRM);

	// Configure the timer to expire after PERIOD msec...
  timer.it_value.tv_sec = 0;
  timer.it_value.tv_usec = PERIOD;
	timer.it_interval.tv_sec = 0;
 	timer.it_interval.tv_usec = PERIOD;
  // Start the timer. setitimer() to implement a repetitive timer
	// ITIMER_REAL: to decrements in real time, and delivers SIGALRM upon expiration.
  setitimer(ITIMER_REAL, &timer, NULL);
}

/*this function is used to prevent interrupts during critical parts*/
static void block_timer(void){
	int res = sigprocmask(SIG_BLOCK, &sig_action.sa_mask , NULL);
	if(res == -1){
		perror("Signal blocked");
	}
}

static void unblock_timer(void){
	int res = sigprocmask(SIG_UNBLOCK, &sig_action.sa_mask, NULL);
  if (res == -1){
		perror("Signal unblocked");
  }
}

// static void reinit_timer(void){
// 	timer.it_value.tv_sec = 0;
// 	timer.it_value.tv_sec = PERIOD;
// }

#endif   /**********************************PREEMPTION***************************/


//debug
void printf_queue(){
  struct thread_s * itemth;
   TAILQ_FOREACH(itemth, &queue, entries){
     printf("QUEUE -  item : %p\n",itemth);
   }
}

static void wrapper(void*(*func)(void*), void* funcarg){
  // #ifdef PREEMPTION
  // reinit_timer();
  // #endif
  void * retval = (*func)(funcarg);
  thread_exit(retval);
}

static int thread_init(struct thread_s * thread) {
  int result = getcontext(&(thread->uc));
  if (result == -1) return -1;
  thread->uc.uc_stack.ss_size = 64*1024;
  if((thread->uc.uc_stack.ss_sp = malloc(thread->uc.uc_stack.ss_size))==NULL){
      return -1;
  }
  thread->valgrind_stackid = VALGRIND_STACK_REGISTER((thread->uc).uc_stack.ss_sp, (thread->uc).uc_stack.ss_sp + (thread->uc).uc_stack.ss_size);
  thread->uc.uc_link = NULL;
  thread->status = STATUS_READY;
  thread->retval = NULL;
  thread->th_join = NULL;
  return 0;
}

void free_all(void){
  struct thread_s * item;
  while(!TAILQ_EMPTY(&queue)){
      item = TAILQ_FIRST(&queue);
      TAILQ_REMOVE(&queue,item,entries);
      if(item!=thread_main && (item->uc).uc_stack.ss_sp != NULL){
        VALGRIND_STACK_DEREGISTER(item->valgrind_stackid);
        free((item->uc).uc_stack.ss_sp);
      }
      free(item);
  }

  while(!TAILQ_EMPTY(&queue_finished)){
      item = TAILQ_FIRST(&queue_finished);
      TAILQ_REMOVE(&queue_finished,item,entries);
      if(item!=thread_main && (item->uc).uc_stack.ss_sp != NULL){
        VALGRIND_STACK_DEREGISTER(item->valgrind_stackid);
        free((item->uc).uc_stack.ss_sp);
      }
      free(item);
  }
}

static void init() {
  if (has_been_initialized) return;

  TAILQ_INIT(&queue);
  TAILQ_INIT(&queue_finished);

  struct thread_s * thread =  malloc(sizeof(struct thread_s));
  if(getcontext(&(thread->uc))==-1) return;
  thread->uc.uc_link = NULL;
  thread->retval = NULL;
  thread->th_join = NULL;
  thread->status = STATUS_RUNNING;
  thread_main = thread;
  TAILQ_INSERT_HEAD(&queue , thread , entries);

  has_been_initialized = 1;
	#ifdef PREEMPTION
  init_timer();
  #endif
  atexit(free_all);
}

extern thread_t thread_self(void){
  init();
  return (thread_t)TAILQ_FIRST(&queue);
}

extern int thread_create(thread_t * newthread, void *(*func)(void *), void *funcarg) {
  init();

  struct thread_s * thread = malloc(sizeof(struct thread_s));
  int result = thread_init(thread);
  if(result == -1){
    free(thread);
    return result;
  }
  *newthread = (thread_t *) thread;
  makecontext(&(thread->uc), (void (*)(void)) wrapper, 2, func, funcarg);
  TAILQ_INSERT_TAIL(&queue, thread, entries);

  return result;
}

extern int thread_yield(void){
  init();
	#ifdef PREEMPTION
	block_timer();
	#endif
  if(!TAILQ_EMPTY(&queue)){
    struct thread_s * current = TAILQ_FIRST(&queue);

    if(TAILQ_NEXT(current, entries) !=  NULL){
      TAILQ_REMOVE(&queue, current , entries);
      TAILQ_INSERT_TAIL(&queue, current, entries);
      TAILQ_FIRST(&queue)->status = STATUS_RUNNING;
      current->status = STATUS_READY;
      #ifdef PREEMPTION
      unblock_timer();
			// reinit_timer();
      #endif
      swapcontext(&(current->uc), &(TAILQ_FIRST(&queue)->uc));
    }else{
      #ifdef PREEMPTION
      unblock_timer();
      #endif
    }
  }else{
    #ifdef PREEMPTION
    unblock_timer();
    #endif
  }
  return 0;
}

extern int thread_join(thread_t thread, void **retval) {
  if (thread == NULL) {
    return -1;
  }
  struct thread_s * th = (struct thread_s *) thread;
  while (th->status != STATUS_FINISH) {
		#ifdef PREEMPTION
    thread_yield();
		#else
		struct thread_s * current = TAILQ_FIRST(&queue);
    TAILQ_REMOVE(&queue, th, entries);
    TAILQ_INSERT_HEAD(&queue ,th, entries);
    swapcontext(&(current->uc), &(TAILQ_FIRST(&queue)->uc));
		#endif
  }
  if(retval != NULL){
      *retval = th->retval;
  }
  return 0;
}

extern void thread_exit(void *retval) {
  if(TAILQ_EMPTY(&queue)){
    exit(0);
  }
  struct thread_s * current = TAILQ_FIRST(&queue);
  current->retval = retval;
  current->status = STATUS_FINISH;

  TAILQ_REMOVE(&queue, current , entries);
  TAILQ_INSERT_HEAD(&queue_finished, current, entries);

  setcontext(&(TAILQ_FIRST(&queue)->uc));

  exit(0);
}

/**********************************MUTEX***************************/
int thread_mutex_init(thread_mutex_t *mutex) {
    mutex->flag = 0;
    TAILQ_INIT(&(mutex->queue_blocked));
    return 0;
}

int thread_mutex_destroy(thread_mutex_t *mutex) {
    return 0;
}

int thread_mutex_lock(thread_mutex_t *mutex){
  #ifdef PREEMPTION
  block_timer();
  #endif

  while(!__sync_bool_compare_and_swap(&(mutex->flag) , 0, 1)) {
  	thread_yield();
  }
  #ifdef PREEMPTION
  unblock_timer();
  #endif
  return 0;
	// int oldval = __sync_val_compare_and_swap(&(mutex->flag), 0, 1);
	// if (oldval == 1) {
	//     struct thread_s * current = TAILQ_FIRST(&queue);
	//     TAILQ_REMOVE(&queue, current , entries);
	//     TAILQ_INSERT_TAIL(&(mutex->queue_blocked), current, entries);
	//     thread_yield();
	// }
	// return 0;
}

int thread_mutex_unlock(thread_mutex_t *mutex){
		#ifdef PREEMPTION
		block_timer();
		#endif
		mutex->flag = 0;
		#ifdef PREEMPTION
		unblock_timer();
		#endif
		// if(!TAILQ_EMPTY(&(mutex->queue_blocked))){
		//     struct thread_s * head = TAILQ_FIRST(&(mutex->queue_blocked));
		//     TAILQ_REMOVE(&(mutex->queue_blocked), head , entries);
		//     TAILQ_INSERT_TAIL(&queue, head, entries);
		// }
    return 0;
}

/**********************************END MUTEX***************************/
