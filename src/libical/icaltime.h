/*======================================================================
 FILE: icaltime.h
 CREATOR: eric 02 June 2000

 (C) COPYRIGHT 2000, Eric Busboom <eric@civicknowledge.com>

 This library is free software; you can redistribute it and/or modify
 it under the terms of either:

    The LGPL as published by the Free Software Foundation, version
    2.1, available at: https://www.gnu.org/licenses/lgpl-2.1.html

 Or:

    The Mozilla Public License Version 2.0. You may obtain a copy of
    the License at https://www.mozilla.org/MPL/

 The Original Code is eric. The Initial Developer of the Original
 Code is Eric Busboom
======================================================================*/

/**     @file icaltime.h
 *      @brief struct icaltimetype is a pseudo-object that abstracts time
 *      handling.
 *
 *      It can represent either a DATE or a DATE-TIME (floating, UTC or in a
 *      given timezone), and it keeps track internally of its native timezone.
 *
 *      The typical usage is to call the correct constructor specifying the
 *      desired timezone. If this is not known until a later time, the
 *      correct behavior is to specify a NULL timezone and call
 *      icaltime_convert_to_zone() at a later time.
 *
 *      There are several ways to create a new icaltimetype:
 *
 *      - icaltime_null_time()
 *      - icaltime_null_date()
 *      - icaltime_current_time_with_zone()
 *      - icaltime_today()
 *      - icaltime_from_timet_with_zone(time_t tm, int is_date,
 *              icaltimezone *zone)
 *      - icaltime_from_day_of_year(int doy, int year)
 *
 *      italtimetype objects can be converted to different formats:
 *
 *      - icaltime_as_timet(struct icaltimetype tt)
 *      - icaltime_as_timet_with_zone(struct icaltimetype tt,
 *              icaltimezone *zone)
 *      - icaltime_as_ical_string(struct icaltimetype tt)
 *
 *      Accessor methods include:
 *
 *      - icaltime_get_timezone(struct icaltimetype t)
 *      - icaltime_get_tzid(struct icaltimetype t)
 *      - icaltime_set_timezone(struct icaltimetype t, const icaltimezone *zone)
 *      - icaltime_day_of_year(struct icaltimetype t)
 *      - icaltime_day_of_week(struct icaltimetype t)
 *      - icaltime_start_doy_week(struct icaltimetype t, int fdow)
 *      - icaltime_week_number(struct icaltimetype t)
 *
 *      Query methods include:
 *
 *      - icaltime_is_null_time(struct icaltimetype t)
 *      - icaltime_is_valid_time(struct icaltimetype t)
 *      - icaltime_is_date(struct icaltimetype t)
 *      - icaltime_is_utc(struct icaltimetype t)
 *
 *      Modify, compare and utility methods include:
 *
 *      - icaltime_compare(struct icaltimetype a,struct icaltimetype b)
 *      - icaltime_compare_date_only(struct icaltimetype a,
 *              struct icaltimetype b)
 *      - icaltime_adjust(struct icaltimetype *tt, int days, int hours,
 *              int minutes, int seconds);
 *      - icaltime_normalize(struct icaltimetype t);
 *      - icaltime_convert_to_zone(const struct icaltimetype tt,
 *              icaltimezone *zone);
 */

#ifndef ICALTIME_H
#define ICALTIME_H

#include "libical_ical_export.h"

#include <time.h>

/* An opaque struct representing a timezone. We declare this here to avoid
   a circular dependancy. */
#if !defined(ICALTIMEZONE_DEFINED)
#define ICALTIMEZONE_DEFINED
typedef struct _icaltimezone icaltimezone;
#endif

/** icaltime_span is returned by icalcomponent_get_span() */
struct icaltime_span
{
    time_t start;       /**< in UTC */
    time_t end;         /**< in UTC */
    int is_busy;        /**< 1->busy time, 0-> free time */
};

typedef struct icaltime_span icaltime_span;

struct icaltimetype
{
    int year;           /**< Actual year, e.g. 2001. */
    int month;          /**< 1 (Jan) to 12 (Dec). */
    int day;
    int hour;
    int minute;
    int second;

    int is_date;        /**< 1 -> interpret this as date. */

    int is_daylight;    /**< 1 -> time is in daylight savings time. */

    const icaltimezone *zone;  /**< timezone */
};

typedef struct icaltimetype icaltimetype;

/** Return a null time, which indicates no time has been set.
    This time represents the beginning of the epoch. */
LIBICAL_ICAL_EXPORT struct icaltimetype icaltime_null_time(void);

/** Return a null date, which indicates no time has been set. */
LIBICAL_ICAL_EXPORT struct icaltimetype icaltime_null_date(void);

/** Return the current time in the given timezone, as an icaltimetype. */
LIBICAL_ICAL_EXPORT struct icaltimetype icaltime_current_time_with_zone(const icaltimezone *zone);

/** Return the current day as an icaltimetype, with is_date set. */
LIBICAL_ICAL_EXPORT struct icaltimetype icaltime_today(void);

/** @brief Constructor.

    @param tm The time
    @param is_date Boolean: 1 means we should treat tm as a DATE
    @param zone The timezone tm is in, NULL means to treat tm as a
                floating time

    Return a new icaltime instance, initialized to the given time
    expressed as seconds past UNIX epoch, optionally using the given
    timezone.

    If the caller specifies the is_date param as TRUE, the returned
    object is of DATE type, otherwise the input is meant to be of
    DATE-TIME type.
    If the zone is not specified (NULL zone param) the time is taken
    to be floating, that is, valid in any timezone. Note that, in
    addition to the uses specified in [RFC5545], this can be used
    when doing simple math on couples of times.
    If the zone is specified (UTC or otherwise), it's stored in the
    object and it's used as the native timezone for this object.
    This means that the caller can convert this time to a different
    target timezone with no need to store the source timezone. */
LIBICAL_ICAL_EXPORT struct icaltimetype icaltime_from_timet_with_zone(const time_t tm,
                                                                      const int is_date,
                                                                      const icaltimezone *zone);

/** Create a time from an ISO format string. */
LIBICAL_ICAL_EXPORT struct icaltimetype icaltime_from_string(const char *str);

/** Create a new time, given a day of year and a year. */
LIBICAL_ICAL_EXPORT struct icaltimetype icaltime_from_day_of_year(const int doy, const int year);

/**
   Return the time as seconds past the UNIX epoch.

   This function probably won't do what you expect.  In particular, you should
   only pass an icaltime in UTC, since no conversion is done.  Even in that case,
   it's probably better to just use icaltime_as_timet_with_zone(). */
LIBICAL_ICAL_EXPORT time_t icaltime_as_timet(const struct icaltimetype);

/** Return the time as seconds past the UNIX epoch, using the given timezone.

    This convenience method combines a call to icaltime_convert_to_zone()
    with a call to icaltime_as_timet().
    If the input timezone is null, no conversion is done; that is, the
    time is simply returned as time_t in its native timezone. */
LIBICAL_ICAL_EXPORT time_t icaltime_as_timet_with_zone(const struct icaltimetype tt,
                                                       const icaltimezone *zone);

/** Return a string represention of the time, in RFC5545 format.
    The string is owned by libical. */
LIBICAL_ICAL_EXPORT const char *icaltime_as_ical_string(const struct icaltimetype tt);

/** Return a string represention of the time, in RFC5545 format.
    The string is owned by the caller. */
LIBICAL_ICAL_EXPORT char *icaltime_as_ical_string_r(const struct icaltimetype tt);

/** @brief Return the timezone */
LIBICAL_ICAL_EXPORT const icaltimezone *icaltime_get_timezone(const struct icaltimetype t);

/** @brief Return the tzid, or NULL for a floating time */
LIBICAL_ICAL_EXPORT const char *icaltime_get_tzid(const struct icaltimetype t);

/** @brief Set the timezone

    Force the icaltime to be interpreted relative to another timezone.
    If you need to do timezone conversion, applying offset adjustments,
    then you should use icaltime_convert_to_zone instead. */
LIBICAL_ICAL_EXPORT struct icaltimetype icaltime_set_timezone(struct icaltimetype *t,
                                                              const icaltimezone *zone);

/** Return the day of the year of the given time,
    counting from 1 (Jan 1st). */
LIBICAL_ICAL_EXPORT int icaltime_day_of_year(const struct icaltimetype t);

/** Return the day of the week of the given time. Sunday is 1. */
LIBICAL_ICAL_EXPORT int icaltime_day_of_week(const struct icaltimetype t);

/** Return the day of the year for the first day of the week that the
    given time is within. */
LIBICAL_ICAL_EXPORT int icaltime_start_doy_week(const struct icaltimetype t, int fdow);

/** Return the week number for the week the given time is within. */
LIBICAL_ICAL_EXPORT int icaltime_week_number(const struct icaltimetype t);

/** Return true if the time is null. */
LIBICAL_ICAL_EXPORT int icaltime_is_null_time(const struct icaltimetype t);

/** Return false if the time is clearly invalid, but is not null. This
    is usually the result of creating a new time type but not clearing
    it, or setting one of the flags to an illegal value. */
LIBICAL_ICAL_EXPORT int icaltime_is_valid_time(const struct icaltimetype t);

/** @brief Return true if time is of DATE type, false if DATE-TIME */
LIBICAL_ICAL_EXPORT int icaltime_is_date(const struct icaltimetype t);

/** @brief Return true if time is relative to UTC zone */
LIBICAL_ICAL_EXPORT int icaltime_is_utc(const struct icaltimetype t);

/** Return -1, 0, or 1 to indicate that a is less than b, a equals b, or a is greater than b, respectively.
    This calls icaltime_compare function after converting them to the utc timezone. */
LIBICAL_ICAL_EXPORT int icaltime_compare(const struct icaltimetype a, const struct icaltimetype b);

/** Like icaltime_compare, but only use the date parts. */
LIBICAL_ICAL_EXPORT int icaltime_compare_date_only(const struct icaltimetype a,
                                                   const struct icaltimetype b);

/** Like icaltime_compare, but only use the date parts; accepts timezone. */
LIBICAL_ICAL_EXPORT int icaltime_compare_date_only_tz(const struct icaltimetype a,
                                                      const struct icaltimetype b,
                                                      icaltimezone *tz);

/** @brief Internal, shouldn't be part of the public API

    Add or subtract a number of days, hours, minutes and seconds from a icaltimetype. */
LIBICAL_ICAL_EXPORT void icaltime_adjust(struct icaltimetype *tt,
                                         const int days, const int hours,
                                         const int minutes, const int seconds);

/** @brief Normalize the icaltime, so that all fields are within the normal range

    Reset all of the time components to be in their normal ranges. For
    instance, given a time with minutes=70, the minutes will be reduces
    to 10, and the hour incremented. This allows the caller to do
    arithmetic on times without worrying about overflow or
    underflow. */
LIBICAL_ICAL_EXPORT struct icaltimetype icaltime_normalize(const struct icaltimetype t);

/** @brief Convert time to a given timezone

    If tt is a date, the returned time is an exact
    copy of the input. If it's a floating time, the returned object
    represents the same time translated to the given timezone.
    Otherwise the time will be converted to the new
    time zone, and its native timezone set to the right timezone.
    Does nothing if the time is already UTC. */
LIBICAL_ICAL_EXPORT struct icaltimetype icaltime_convert_to_zone(const struct icaltimetype tt,
                                                                 icaltimezone *zone);

/** Return the number of days in the given month. */
LIBICAL_ICAL_EXPORT int icaltime_days_in_month(const int month, const int year);

/** Returns whether the specified year is a leap year. Year is the normal year,
    e.g. 2001. */
LIBICAL_ICAL_EXPORT int icaltime_is_leap_year(const int year);

/** Return the number of days in this year. */
LIBICAL_ICAL_EXPORT int icaltime_days_in_year(const int year);

/** @brief Calculate an icaltimespan given a start and end time. */
LIBICAL_ICAL_EXPORT struct icaltime_span icaltime_span_new(struct icaltimetype dtstart,
                                                           struct icaltimetype dtend, int is_busy);

/** @brief Return true if the two spans overlap */
LIBICAL_ICAL_EXPORT int icaltime_span_overlaps(icaltime_span *s1, icaltime_span *s2);

/** @brief Return true if the span is totally within the containing span */
LIBICAL_ICAL_EXPORT int icaltime_span_contains(icaltime_span *s, icaltime_span *container);

#endif /* !ICALTIME_H */
