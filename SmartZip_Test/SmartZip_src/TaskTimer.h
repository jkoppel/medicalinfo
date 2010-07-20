//  TaskTimer.h
//
//  An object that tracks time consumed by a task and computes
//  the time remaining for the task to end.
//
//  Copyright (c) 2002 Ravi Bhavnani
//
//  This source code may be used without restriction in any freeware,
//  shareware or commercial application, provided this copyright notice
//  is preserved.  This code may not be distributed in source or compiled
//  form as part of a commercial library or SDK.  This source code is
//  provided without any guarantee of correctness or performance.
//
//  See TaskTimer.h.cpp for revision history

#ifndef _TaskTimer_h_
#define _TaskTimer_h_

//////////////////////////////////////////////////////////////////////
// TaskTimer object
//

class TaskTimer
{
/////////////////////////////////////////////////////////////////////////////
// Construction
public:
	TaskTimer();
	virtual ~TaskTimer();

/////////////////////////////////////////////////////////////////////////////
// Attributes (none)
public:

/////////////////////////////////////////////////////////////////////////////
// Methods
public:
  void    start();                        // start timing

  void    pause();                        // pause timing

  void    resume();                       // resume timing

  void    stop();                         // stop timing

  void    update();                       // update a running timer's elapsed time

  void    updateProgress                  // update progress
            (long nProgress);

  void    updateProgress                  // update progress and get
            (long   nProgress,            // remaining time
             long&  nRemainingTime);

  long    getProgress();                  // return current progress

  long    getElapsedTime();               // get elapsed time

  void    getElapsedTime                  // get elapsed time as components
            (long&  nHours,
             long&  nMinutes,
             long&  nSeconds);

  void    getElapsedTime                  // get elapsed time as formatted string
            (CString& strElapsed);

  long    getRemainingTime();             // get remaining time

  void    getRemainingTime                // get remaining time as components
            (long&  nHours,
             long&  nMinutes,
             long&  nSeconds);

  void    getRemainingTime                // get remaining time as formatted string
            (CString& strRemaining);

  bool    isRunning();                    // is task running?

  bool    isPaused();                     // is task paused

  bool    isStopped();                    // is task stopped?

  static void getHms                      // get hh:mm:ss time components
            (long   nTimeInterval,
             long&  nHours,
             long&  nMinutes,
             long&  nSeconds);

/////////////////////////////////////////////////////////////////////////////
// Implementation
protected:
  enum {
      State_Stopped = 0,
      State_Running = 1,
      State_Paused = 2
  };

  long    m_nState;       // task state
  time_t  m_tmStart;      // timestamp when task was started
  time_t  m_tmCurrent;    // current time
  long    m_nProgress;    // current progress

protected:
  void reset();           // reset the object
};

#endif

// End TaskTimer.h