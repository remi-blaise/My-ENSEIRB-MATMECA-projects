#ifndef __TAILHEAD_H__
#define __TAILHEAD_H__


enum status {
    STATUS_READY,
    STATUS_RUNNING,
    STATUS_FINISH
};


struct thread_s {
  ucontext_t uc;
  TAILQ_ENTRY(thread_s) entries;
  enum status status;
  void * retval;
  int valgrind_stackid;
  struct thread_s * th_join;
};

#endif
