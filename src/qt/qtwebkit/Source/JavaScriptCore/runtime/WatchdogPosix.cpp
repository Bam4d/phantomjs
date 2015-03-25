/*
 * Copyright (C) 2013 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
 */

#include "config.h"
#include "Watchdog.h"

#include <time.h>
#include <stdio.h>
#include <signal.h>

#include "CallFrame.h"


namespace JSC {

// This is run ever second from the watchdog poll
void watchdogPoll(sigval_t sigval)
{
    Watchdog* watchdog = static_cast<Watchdog*>(sigval.sival_ptr);
    watchdog->m_timerDidFire = true;
}

// This is a POSIX implementation of a watchdog timer
void Watchdog::initTimer()
{
}

void Watchdog::destroyTimer() 
{
	if(wdt != NULL) {
	    timer_delete(wdt);
	    wdt = NULL;
	}
}


void Watchdog::startTimer(double limit) 
{
    struct sigevent sevp;
    struct itimerspec its;

    memset (&sevp, 0, sizeof (struct sigevent));
    sevp.sigev_notify=SIGEV_THREAD;
    sevp.sigev_notify_attributes = NULL;
    sevp.sigev_signo=SIGRTMIN;
    sevp.sigev_value.sival_ptr=this;
    sevp.sigev_notify_function=watchdogPoll;

    timer_create(CLOCK_REALTIME, &sevp, &wdt);

    its.it_value.tv_sec = 1;
    its.it_value.tv_nsec = 0;
    its.it_interval.tv_sec = its.it_value.tv_sec;
    its.it_interval.tv_nsec = its.it_value.tv_nsec;

    timer_settime(wdt, 0, &its, NULL);
}

void Watchdog::stopTimer()
{
    if(wdt != NULL) {
	    timer_delete(wdt);
	    wdt = NULL;
	}
}

} // namespace JSC
