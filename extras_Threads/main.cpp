//-----------------------------------------------------------------------------
// Created on: 20 December 2021
// Copyright (c) 2021-present, Quaoar Studio (ask@quaoar.pro)
//----------------------------------------------------------------------------

// OpenCascade includes
#include <OSD_Thread.hxx>
#include <Standard_Mutex.hxx>

/*
   This minimalistic example is based on the code snippet provided
   by Roman Lygin in his blog here:

   https://opencascade.blogspot.com/2009/07/developing-parallel-applications-with.html

   It should be noted that OpenCascade threads are nothing but wrappers
   over the operating system's threads. Therefore, you might consider
   using them just for simplicity and OS-independent abstraction.
 */

static Standard_Mutex Mutex;

void* Test_ThreadFunction(void* /*theData*/)
{
  // Enter the critical section (in Windows terminology) by using the scoped object
  // which acquires the lock on construction and releases it on destruction (RAII).
  Standard_Mutex::Sentry sentry(Mutex);

  std::cout << "Running in worker thread id: " << OSD_Thread::Current() << std::endl;

  return NULL;
}

int main(int argc, char *argv[])
{
  const int MAX_THREAD = 5;
  OSD_Thread threads[MAX_THREAD];

  std::cout << "Running in master thread id: " << OSD_Thread::Current() << std::endl;

  // Prepare threads.
  OSD_ThreadFunction pThreadFunc = Test_ThreadFunction;
  int i;
  for ( i = 0; i < MAX_THREAD; ++i )
  {
    // Detaches the currently iterated thread and sets a function pointer
    // in the corresponding OSD_Thread data structure.
    //
    // In posix, the pthread_detach() function marks the target thread as detached.
    // When a detached thread terminates, its resources are automatically released
    // back to the system without the need for another thread to join with the
    // terminated thread. Attempting to detach an already detached thread results in
    // unspecified behavior.
    threads[i].SetFunction(pThreadFunc);
  }

  // Run threads.
  for ( i = 0; i < MAX_THREAD; ++i )
  {
    if ( !threads[i].Run(NULL) )
      std::cerr << "Error: cannot start thread " << i << std::endl;
  }

  for ( i = 0; i < MAX_THREAD; ++i )
  {
    Standard_Address res;

    // In posix, The pthread_join() function waits for the thread to terminate.
    // If that thread has already terminated, then pthread_join() returns
    // immediately.
    if ( !threads[i].Wait(res) )
      std::cerr << "Error: cannot get result of thread " << i << std::endl;
  }

  return 0;
}
