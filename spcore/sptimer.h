﻿//--------------------------------------------------------------------------------
// Copyright (c) 2017-2018, sanko-shoko. All rights reserved.
//--------------------------------------------------------------------------------

#ifndef __SP_TIMER_H__
#define __SP_TIMER_H__

#include "spcore/spwrap.h"


#ifndef SP_USE_TIMER
#define SP_USE_TIMER 1
#endif


//--------------------------------------------------------------------------------
// additional include
//--------------------------------------------------------------------------------

#if SP_USE_TIMER
#if WIN32
#include <Windows.h>
#else
#include <chrono>
#endif
#endif


namespace sp {

    class Timer {

    public:

#if SP_USE_TIMER
#if WIN32
        typedef LARGE_INTEGER tpoint;
#else
        typedef std::chrono::system_clock::time_point tpoint;
#endif
#else
        typedef double tpoint;
#endif


    public:

        static tpoint now() {
            tpoint n;

#if SP_USE_TIMER
#if WIN32
            QueryPerformanceCounter(&n);
#else
            n = std::chrono::system_clock::now();
#endif
#else
            n = 0.0;
#endif
            return n;
        }

        static double dif(const tpoint &tp0, const tpoint &tp1) {

            double ms = 0.0;
#if SP_USE_TIMER
#if WIN32
            tpoint freq;
            QueryPerformanceFrequency(&freq);

            ms = static_cast<double>((tp1.QuadPart - tp0.QuadPart) * 1000.0 / freq.QuadPart);
#else
            ms = static_cast<double>(std::chrono::duration_cast<std::chrono::microseconds>(tp1 - tp0).count() / 1000.0);
#endif
#endif
            return (ms > 0) ? +ms : -ms;
        }


    private:

        tpoint tp[2];

    public:

        void start() {
            tp[0] = now();
        }

        void stop() {
            tp[1] = now();
        }

        double getms() {
            return dif(tp[0], tp[1]);
        }

        void print(const char *name = "time") {
            SP_PRINTF("%s: %.1lf[ms]\n", name, getms());
        }

    };

}

#endif
