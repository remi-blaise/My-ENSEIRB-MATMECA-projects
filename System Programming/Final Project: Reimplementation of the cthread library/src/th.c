#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>
#include <sys/queue.h>
#include <valgrind/valgrind.h>
#include <sys/time.h>
#include <signal.h>
#include <string.h>
#include "../include/thread.h"
#include "../include/tailhead.h"


static int has_been_initialized = 0;
TAILQ_HEAD(, thread_s) queue; // queue.tqh_first
TAILQ_HEAD(, thread_s) queue_finished;
static struct thread_s *thread_main;


#ifdef PREEMPTION  /**********************************PREEMPTION***************************/
#define PERIOD 50

static struct sigaction sig_action;
static struct itimerval timer;

/* the action to be associated with SIGALRM (signum=14) */
void handler_timer(int signum){
	// printf("yield **************************************, signum : %d\n", signum);
	thread_yield();
}
// static void handler_timer(int sig, siginfo_t *si, void *unused){
//   printf("Got SIGSEGV at address: 0x%lx\n",(long) si->si_addr);
// }

void init_timer(void){
	// printf("Initialising timer ..\n");
	memset(&sig_action, 0, sizeof(sig_action));
	//associer timer_handler comme gestionnaire du signal SIGALRM
	sig_action.sa_handler = &handler_timer;

	/*sigaction : is a system call used to change the action taken by a process on receipt of a specific signal*/
	sigaction(SIGALRM , &sig_action , NULL);

	/*The sa_mask field allows us to specify a set of signals that aren’t permitted to interrupt execution of this handler
	*SIGALRM can't interrupt our handler*/
	sigemptyset(&sig_action.sa_mask);
	sigaddset(&sig_action.sa_mask , SIGALRM);

	// Configure the timer to expire after PERIOD msec...
  timer.it_value.tv_sec = 0;
  timer.it_value.tv_usec = PERIOD;
	timer.it_interval.tv_sec = 0;
 	timer.it_interval.tv_usec = PERIOD;
  // Start the timer. setitimer() to implement a repetitive timer
	/* ITIMER_REAL: to decrements in real time, and delivers SIGALRM upon expiration.*/
  setitimer(ITIMER_REAL, &timer, NULL);
}

/*this function is used to prevent interrupts during critical parts*/
void block_timer(void){
	// printf("Stoping timer..\n");
	int res = sigprocmask(SIG_BLOCK, &sig_action.sa_mask , NULL);
	if(res == -1){
		perror("Signal blocked");
	}
}

void unblock_timer(void){
	// printf("Re-executing timer.. \n");
	int res = sigprocmask(SIG_UNBLOCK, &sig_action.sa_mask, NULL);
  if (res == -1){
		perror("Signal unblocked");
  }
}

void reinit_timer(void){
	timer.it_value.tv_sec = 0;
	timer.it_value.tv_sec = PERIOD;
}

#endif   /**********************************PREEMPTION***************************/


//debug
void printf_queue(){
  struct thread_s * itemth;
   TAILQ_FOREACH(itemth, &queue, entries){
     printf("QUEUE -  item : %p\n",itemth);
   }
}

static void wrapper(void*(*func)(void*), void* funcarg){
  #ifdef PREEMPTION
  reinit_timer();
  #endif

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
  //printf_queue();
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
    //printf("thread_create thread : %p\n",thread);
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
  struct thread_s * current = TAILQ_FIRST(&queue);
  while (th->status != STATUS_FINISH) {
    // thread_yield();
    TAILQ_REMOVE(&queue, th, entries);
    TAILQ_INSERT_HEAD(&queue ,th, entries);
    swapcontext(&(current->uc), &(TAILQ_FIRST(&queue)->uc));
  }
  if(retval != NULL){
      *retval = th->retval;
  }
  // TAILQ_REMOVE(&queue, th , entries);
  // VALGRIND_STACK_DEREGISTER(th->valgrind_stackid);
  // free((th->uc).uc_stack.ss_sp);
  // free(th);
  return 0;
}

extern void thread_exit(void *retval) {
  if(TAILQ_EMPTY(&queue)){
    exit(0);
  }
  struct thread_s * current = TAILQ_FIRST(&queue);
  current->retval = retval;
  current->status = STATUS_FINISH;

  struct thread_s * next = TAILQ_NEXT(current, entries);

  TAILQ_REMOVE(&queue, current , entries);
  TAILQ_INSERT_HEAD(&queue_finished, current, entries);

  if(next != NULL)
    setcontext(&(next->uc));

  exit(0);
}

/******************************** MUTEX *********************************************/

int thread_mutex_init(thread_mutex_t *mutex){
    if(mutex != NULL) {
        mutex->owner = NULL;
        TAILQ_INIT(&(mutex->threads));
        mutex->Blocked = false;
        return 0;
    }
    return -1;
}
/*

int thread_mutex_lock(thread_mutex_t* mutex){
    //disable preemption
    #ifdef PREEMPTION
    pause_timer();
    #endif
    if(mutex->Blocked) {
        if (TAILQ_FIRST(&queue) == mutex->owner)
            return -1;
        TAILQ_INSERT_HEAD(&queue_mutex ,TAILQ_FIRST(&queue) , entries); // sera suspendu jusqu'a ce que le mutex soit verouillé
        //current_thread = queue_pop(readythreads);
        setcontext(&TAILQ_FIRST(&queue)->context);
    } else {
        mutex->Blocked = true;
        mutex->owner = TAILQ_FIRST(&queue);
        thread_yield();
    }
    //enable preemption
    #ifdef PREEMPTION
    resume_timer();
    #endif
    return 0;

}

int thread_mutex_unlock(thread_mutex_t *mutex){
    //disable preemption
    #ifdef PREEMPTION
    pause_timer();
    #endif
    if (!mutex->Blocked) {
        return -1;
    }
    if(mutex->owner != current_thread) {
        TAILQ_INSERT_HEAD(&queue_mutex ,TAILQ_FIRST(&queue) , entries);
        //current_thread = queue_pop(readythreads);
        setcontext(&TAILQ_FIRST(&queue)->context);
    } else {
        mutex->Blocked = false;
        mutex->owner = NULL;
        //if (!queue_isempty(mutex->threads)) {
        if (!TAILQ_EMPTY(&queue_mutex)) {
            //queue_push(readythreads, (void *) queue_pop(mutex->threads));
            TAILQ_INSERT_HEAD(&queue_mutex ,TAILQ_FIRST(&queue_mutex) , entries);
        }
    }
    #ifdef PREEMPTION
    resume_timer();
    #endif
    //enable preemption
    return 0;
}


int thread_mutex_destroy(thread_mutex_t *mutex){
    if ((mutex == NULL)) {
        return -1;
    }
    if (!TAILQ_EMPTY(&queue_mutex))
        return -1;

    free(queue_mutex);
    return 0;
