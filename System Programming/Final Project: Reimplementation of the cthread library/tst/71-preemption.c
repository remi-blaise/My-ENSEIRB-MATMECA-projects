#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "../include/thread.h"
#include <unistd.h>
#include <sys/time.h>

/* Test de la préemption
  2 threads created, each executing for 5s  (preemption timeslice implemented in our library : 100000us)
*/

static void * thfunc1(void *dummy __attribute__((unused)))
{
  int i;
  for (i = 0; i<5; i++){
    printf("th1 : %d\n",i);
    sleep(1);
  }
  thread_exit(NULL);
}

static void * thfunc2(void *dummy __attribute__((unused)))
{
  int i;
  for (i = 0; i<5; i++){
    printf("th2 : %d\n",i);
    sleep(1);
  }
  thread_exit(NULL);
}

int main(int argc, char *argv[])
{

  thread_t th1, th2;
  int err1, err2;

  err1 = thread_create(&th1, thfunc1, NULL);
  assert(!err1);
  err2 = thread_create(&th2, thfunc2, NULL);
  assert(!err2);

  thread_join(th1, NULL);
  thread_join(th2, NULL);


  return 0;
}
