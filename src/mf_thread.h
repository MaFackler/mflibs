#pragma once
#include <mf.h>

namespace mf { namespace thread {

void mf_thread_run(Thread *tc);

// Threads
typedef void (*thread_proc)(void *param);

struct Thread {
    thread_proc proc;
    void *arg;

    void run();
};


#if defined(MF_OS_WINDOWS)
DWORD __thread_proc_impl(LPVOID param)
{
    Thread *threadContext = (Thread *) param;
    threadContext->proc(threadContext->arg);
    return 0;
}

void Thread::run() {
    DWORD id = 0;
    HANDLE thread = CreateThread(NULL,
                                 0,
                                 __thread_proc_impl,
                                 (LPVOID) tc,
                                 0,
                                 &id);
}
#else

#error "Not supported in this os"
#endif

}} // mf::thread

