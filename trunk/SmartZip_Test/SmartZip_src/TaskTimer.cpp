//  TaskTimer.cpp
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
//  Revision history:
//
//    07-Aug-2002   Ravi B      Added methods update(), getElapsedTime (CString&)
//                              and getRemainingTime (CString&)
//
//    21-Apr-2002   Ravi B      Bug fix: stop() was not updating current time.
//
//    16-Apr-2002   Ravi B      Initial version

#include "stdafx.h"
#include "TaskTimer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////
// Construction/destruction

TaskTimer::TaskTimer()
{
  reset();
}

TaskTimer::~TaskTimer()
{
}

//////////////////////////////////////////////////////////////////////
// Public methods

void TaskTimer::start()
{
  reset();
  m_nState = State_Running;
  time (&m_tmStart);
  m_tmCurrent = m_tmStart;
}

void TaskTimer::pause()
{
  if (isRunning()) {
      m_nState = State_Paused;
  }
}

void TaskTimer::resume()
{
  if (isPaused()) {
      m_nState = State_Running;
  }
}

void TaskTimer::stop()
{
  if (!isStopped()) {
      time (&m_tmCurrent);
      m_nState = State_Stopped;
      m_nProgress = 100;
  }
}

void TaskTimer::update()
{
  if (!isStopped()) {
      time (&m_tmCurrent);
  }
}

void TaskTimer::updateProgress
  (long nProgress)
{
  if (isRunning()) {
      time (&m_tmCurrent);
      m_nProgress = nProgress;
      if (m_nProgress < 0) {
          m_nProgress = 0;
      } else {
          if (m_nProgress > 100) {
              m_nProgress = 100;
          }
      }
  }
  if (m_nProgress == 100) {
      m_nState = State_Stopped;
  }
}

void TaskTimer::updateProgress
  (long   nProgress,
   long&  nRemainingTime)
{
  updateProgress (nProgress);
  nRemainingTime = getRemainingTime();
}

long TaskTimer::getElapsedTime()
{
  if (isRunning() && (m_nProgress <= 100)) {
      time (&m_tmCurrent);
  }
  return ((long) m_tmCurrent - (long) m_tmStart);
}

long TaskTimer::getProgress()
{
  return (m_nProgress);
}

void TaskTimer::getElapsedTime
  (long&  nHours,
   long&  nMinutes,
   long&  nSeconds)
{
  getHms (getElapsedTime(), nHours, nMinutes, nSeconds);
}

void TaskTimer::getElapsedTime
  (CString&   strElapsed)
{
  long nHours, nMinutes, nSeconds;
  getElapsedTime (nHours, nMinutes, nSeconds);
  strElapsed.Format ("%d:%02d:%02d", nHours, nMinutes, nSeconds);
}

long TaskTimer::getRemainingTime()
{
  if (isStopped()) {
      return (0);
  }
  long nElapsedTime = getElapsedTime();
  float fTotalEstimatedTime = (float) 100.0 / (float) m_nProgress * (float) nElapsedTime;
  return (long) (fTotalEstimatedTime - nElapsedTime);
}

void TaskTimer::getRemainingTime
  (long&  nHours,
   long&  nMinutes,
   long&  nSeconds)
{
  getHms (getRemainingTime(), nHours, nMinutes, nSeconds);
}

void TaskTimer::getRemainingTime
  (CString&   strRemaining)
{
  long nHours, nMinutes, nSeconds;
  getRemainingTime (nHours, nMinutes, nSeconds);
  strRemaining.Format ("%d:%02d:%02d", nHours, nMinutes, nSeconds);
}

bool TaskTimer::isRunning()
{
  return (m_nState == State_Running);
}

bool TaskTimer::isPaused()
{
  return (m_nState == State_Paused);
}

bool TaskTimer::isStopped()
{
  return (m_nState == State_Stopped);
}

void TaskTimer::getHms
  (long   nTimeInterval,
   long&  nHours,
   long&  nMinutes,
   long&  nSeconds)
{
  // Time interval must be valid
  nHours = 0;
  nMinutes = 0;
  nSeconds = 0;
  if (nTimeInterval < 0) {
      return;
  }

  // Get components
  nHours = nTimeInterval / 3600;
  nMinutes = nTimeInterval / 60 - (nHours * 60);
  nSeconds = nTimeInterval % 60;
}

//////////////////////////////////////////////////////////////////////
// Implementation

void TaskTimer::reset()
{
  m_nState = State_Stopped;
  m_tmStart = 0;
  m_tmCurrent = 0;
  m_nProgress = 0;
}

// End TaskTimer.cpp