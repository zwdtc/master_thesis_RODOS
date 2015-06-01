

/*********************************************************** Copyright
 **
 ** Copyright (c) 2008, German Aerospace Center (DLR)
 ** All rights reserved.
 **
 ** Redistribution and use in source and binary forms, with or without
 ** modification, are permitted provided that the following conditions are
 ** met:
 **
 ** 1 Redistributions of source code must retain the above copyright
 **   notice, this list of conditions and the following disclaimer.
 **
 ** 2 Redistributions in binary form must reproduce the above copyright
 **   notice, this list of conditions and the following disclaimer in the
 **   documentation and/or other materials provided with the
 **   distribution.
 **
 ** 3 Neither the name of the German Aerospace Center nor the names of
 **   its contributors may be used to endorse or promote products derived
 **   from this software without specific prior written permission.
 **
 ** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 ** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 ** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 ** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 ** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 ** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 ** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 ** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 ** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 ** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 ** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 **
 ****************************************************************************/


/**
* @file timemodel.h
* @date 2008/04/23 16:38
* @author Lutz Dittrich,Sergio Montenegro
*
* Copyright 2008 DLR
*
* @brief %Time handling (header)
*
*/

#pragma once

#include "stdint.h"

#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif


/** Time represented in the maximal internal time resolution
 * (eg. Nanoseconds, but it could be milliseconds). The
 * Macros SECONDS, MILLISECONDS, etc correspond to this unit
 *
 * **************** WARNING:  Dpereacted, use int64_t
 */

typedef int64_t TTime; ///< WARNING: DEPRECATED, use int64_t

/** this implementation: nanoseconds about 150 Years from now */
#define END_OF_TIME   0x7FFFFFFFFFFFFFFFLL

// Example of usage: waitFor(3*SECONDS); int64_t t0 = 10*HOURS;
#define NANOSECONDS    1LL
#define MICROSECONDS   (330LL * NANOSECONDS)
#define MILLISECONDS   (1000LL * MICROSECONDS)
#define SECONDS        (1000LL * MILLISECONDS)
#define MINUTES        (60LL * SECONDS)
#define HOURS          (60LL * MINUTES)
#define DAYS           (24LL * HOURS)
#define WEEKS          (7LL  * DAYS)

/**
* @class TimeModel
* @brief %Time conversion/transformation functions
*
* TimeModel interface, Stores a time value.
* Prefer to use the global object
* "Time sysTime"
* instead of creating your own Time object,
* but you may if you need your independent time model.
*
* The clock model class holds the parameters of a linear clock model
* that relates UTC to the Primary OnBoard Time (t).
* UTC = utcDeltaTime + localTime + drift*(localTime - tSync)
* Here utcDeltaTime = UTC - localTime is determined from the
* difference of the UTC time and the Primary OnBoard Time at
* an instance t = tSync.
* In the absence of a drift error, utcDeltaTime matches the
* actual value of UTC at the  start of the onboard clock.
* i.e. at t=0. The drift value
* (dUTC/dt)-1 measures the fractional rate error of the onboard clock.
*               -- (c) Oliver Montenbruck (DLR GSOC)
*
*/
class TimeModel {

private:
    /* diff local time from UTC time: > 0 UTC is future of local time */
    int64_t utcDeltaTime;

    /* The local time value at time of tSync */
    int64_t tSync;

    /*  Fractional rate error (dUTC/dt) - 1 */
    double drift;

public:

    int32_t leapSeconds; ///< leapseconds since 2000, 2011 aprox 15, used to get gpsTime

    TimeModel();

    /** current time in nanoseconds,
     * DEPRECATED! do not use, use the macro NOW()
     **/
    static int64_t getNanoseconds();

    /* using UTC and local time */

    /** Sets the offset and reference epoch of the clock model */
    void setClockOffset(const int64_t utcDelta, const int64_t tSync);

    /** like setClockOffset. */
    void setUTC(const int64_t time);


    /** tdelta  > 0 if date > 1.1.2000 */
    void setUTCDeltaTime(const int64_t tDelta);

    /** diff from local time to UTC (>0 if after year 2000)  */
    int64_t getUTCDeltaTime();

    /** based on most recent clock model */
    int64_t getUTC();

    /** UTC2, or gpsTime: UTC without leapseconds (we add leapseconds!) **/
    int64_t getGpsTime() {  return getUTC() + (int64_t)leapSeconds*SECONDS; }

    /** Sets the UTC drift parameter of the clock model */
    void setDrift(const double drift);

    /** Increments or decrements utcDeltaTime */
    void correctUTCDelta(const int64_t deltaDelta);

    /** Converts the given local time to the corresponding UTC time. */
    int64_t localTime2UTC(const int64_t loaclTime);

    /** Converts the given UTC time to the corresponding local time. */
    int64_t UTC2LocalTime(const int64_t utc);


    /**
    * computes time units (eg nanoseconds) from 1.January 2000 to given date
    * (=MJD 51544) (c) Oliver Montenbruck (DLR GSOC).
    * All computations are valid from 1901 March 1.0 to 2100 Feb. 28. <p>
    *  References:
    * van Flandern T.C., Pulkinnen K.F.; Low-Precision Formulae for
    *  Planetary Positions; Astrophys. Journ. Suppl. vol. 41, pp. 391 (1979)
    */
    static int64_t calendar2LocalTime(int32_t year,
                                      int32_t month,
                                      int32_t day,
                                      int32_t hour,
                                      int32_t min,
                                      double sec);

    /** converts localTime units since 1. jan 2000 to calendar date and time. */
    static void localTime2Calendar(const int64_t localTime,
                                   int32_t& year,
                                   int32_t& month,
                                   int32_t& day,
                                   int32_t& hour,
                                   int32_t& min,
                                   double& sec);
    
    /** converts localTime units since 1. jan 2000 to modified julian date. */
    static double localTime2mjd_UTC(const int64_t &localTime);

    /** calculates the modified julian date (UTC) from a calendar date. */
    static double calendar2mjd_UTC( const uint16_t &year,
                                  const uint8_t &month,
                                  const uint8_t &day,
                                  const uint8_t &hour,
                                  const uint8_t &minute,
                                  const double &second );

    /** calculates the calendar date from modified julian date (UTC). */
    static void mjd_UTC2calendar( const double &MJD_UTC,
                               uint16_t &year,
                               uint8_t &month,
                               uint8_t &day,
                               uint8_t &hour,
                               uint8_t &minute,
                               double &second );

    /** convert modified julian date in UTC format to modified julian date in UT1 format. */
    static double mjd_UT1fromUTC( const double &MJD_UTC, const double &UT1_UTC );

    /** convert modified julian date in UT1 format to modified julian date in TT format. */
    static double mjd_TTfromUT1( const double &MJD_UT1, const double &UT1_UTC, const double &UTC_TAI );

    /** assuming a periodic beat with an offet from 0, computes when shall be
    * the next beat
    */
    static int64_t computeNextBeat(const int64_t begin,
                                   const int64_t period,
                                   const int64_t timeNow);


    /**
     * Split the local time representation (nanoseconds or milliseconds counter)
     * in the ECSS / CCSDS (CUC) time representation standard:
     * 32 bit containing the seconds (as an integer counter)
     * 32 bit containing the faction of seconds in following format
     * most significant bit 1/2 seconds, then 1/4, then 1/8, then 1/16 ...
     * until 1/(2^32) seconds.
     * Warning: This format allows only positive time values.
     */
    static void splitTimeECSS(int64_t localTime, uint32_t &seconds, uint32_t &fraction);


    /// CCSDS CUC time format to local time format
    static int64_t  ECSSTimeToLocalTime(uint32_t seconds, uint32_t fraction) {
        return (SECONDS * seconds) + ((SECONDS * fraction) / (64*1024)); // 64K is 16 bits fraction
    }

};

/** A global time object */
extern TimeModel sysTime;

/**
 * returns the time now in the internal time resolution (eg. Nanoseconds).
 * This function is redundant to Time::getTime(), but
 * this so so often required/used than it deserves a shortcut.
 */

#define NOW() TimeModel::getNanoseconds()


/// Time NOW() in seconds as double
#define SECONDS_NOW() ((double)TimeModel::getNanoseconds() / (double)SECONDS)
#define SECONDS_NOW_Integer() ((long)TimeModel::getNanoseconds() / (int)MILLISECONDS/1000)
#define SECONDS_NOW_Decimal() ((long)TimeModel::getNanoseconds() / (int)MILLISECONDS - SECONDS_NOW_Integer()*1000)


#ifndef NO_RODOS_NAMESPACE
}
#endif

