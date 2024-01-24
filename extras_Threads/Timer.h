//-----------------------------------------------------------------------------
// Copyright (c) 2020-present, Quaoar Studio
// All rights reserved.
//-----------------------------------------------------------------------------

#ifndef Timer_h
#define Timer_h

// OCCT includes
#pragma warning(push, 0)
#include <OSD_Timer.hxx>
#include <OSD_Thread.hxx>
#include <TCollection_AsciiString.hxx>
#pragma warning(pop)

/************************************************************************
                           MEASURING PERFORMANCE
 ************************************************************************/

//! Example:
//!
//! #ifdef TIMER_NEW
//!   TIMER_NEW
//!   TIMER_RESET
//!   TIMER_GO
//! #endif
//!
//! ... YOUR AD_ALGO GOES HERE ...
//!
//! #ifdef TIMER_NEW
//!   TIMER_FINISH
//!   TIMER_COUT_RESULT
//! #endif

#define TIMER_NEW \
  OSD_Timer __aux_debug_Timer; \
  double __aux_debug_Seconds, __aux_debug_CPUTime; \
  int __aux_debug_Minutes, __aux_debug_Hours;

#define TIMER_RESET \
  __aux_debug_Seconds = __aux_debug_CPUTime = 0.0; \
  __aux_debug_Minutes = __aux_debug_Hours = 0; \
  __aux_debug_Timer.Reset();

#define TIMER_GO \
  __aux_debug_Timer.Start();

#define TIMER_FINISH \
  __aux_debug_Timer.Stop(); \
  __aux_debug_Timer.Show(__aux_debug_Seconds, __aux_debug_Minutes, __aux_debug_Hours, __aux_debug_CPUTime);

#define TIMER_COUT_RESULT \
  { \
    TIMER_COUT_RESULT_MSG(""); \
  }

#define TIMER_COUT_RESULT_MSG(Msg) \
  { \
    std::cout << Msg \
              << " in thread " \
              << OSD_Thread::Current() \
              << " (" \
              << __aux_debug_Seconds << " sec. " \
              << __aux_debug_Minutes << " min. " \
              << __aux_debug_Hours << " h. " \
              << __aux_debug_CPUTime << " CPU" \
              << ")" \
              << std::endl; \
  }

#endif
