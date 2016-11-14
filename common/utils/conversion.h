/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0.  If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * Copyright 1997 - July 2008 CWI, August 2008 - 2016 MonetDB B.V.
 */

// This file holds conversion code that is used by both mserver5
// and the database clients (mclient and ODBC)


#ifndef _SEEN_CONVERSION_H
#define _SEEN_CONVERSION_H

#include "monetdb_config.h"

/*
 * @- maximum atomic string lengths
 */
#define bitStrlen	8
#define bteStrlen	8
#define shtStrlen	12
#define intStrlen	24
#define oidStrlen	48
#define ptrStrlen	48
#define lngStrlen	48
#ifdef HAVE_HGE
#define hgeStrlen	96
#endif
#define fltStrlen	48
#define dblStrlen	96
#define dateStrlen 15
#define daytimeStrlen 12


// decimals can be stored in either: signed char (typelen=1), 
//	short (typelen=2), int (typelen=4), lng (typelen=8) or hge (typelen=16)
// scale represents where the decimal point is placed
// returns the amount of bytes written in the buffer
int conversion_decimal_to_string(const void *value, char *buffer, int buflen, int scale, int typelen, const void *null_value);
// convert a bit (boolean) to a string
int conversion_bit_to_string(char *dst, int len, const signed char *src, signed char null_value);

#define numeric_conversion(TYPE, TYPENAME)     \
	int conversion_##TYPENAME##_to_string(char *dst, int len, const TYPE *src, TYPE null_value)

numeric_conversion(signed char, bit);
numeric_conversion(signed char, bte);
numeric_conversion(short, sht);
numeric_conversion(int, int);
numeric_conversion(lng, lng);
typedef void* __pointer_type;
numeric_conversion(__pointer_type, ptr);
numeric_conversion(double, dbl);
numeric_conversion(float, flt);
#ifdef HAVE_HGE
numeric_conversion(hge, hge);
#endif

// date conversion, *src is days since 01-01-0001
int conversion_date_to_string(char *dst, int len, const int *src, int null_value);
void conversion_date_get_data(const int date, short *out_year, unsigned short *out_month, unsigned short *out_day);

// time conversion, *src is ms since 00:00:00
int conversion_time_to_string(char *dst, int len, const int *src, int null_value, int digits);
int conversion_timetz_to_string(char *dst, int len, const int *src, int null_value, int digits, int timezone_diff);

void conversion_time_get_data(int time, int timezone_diff, unsigned short *hour, unsigned short *minute, unsigned short *second, unsigned int *nanosecond);

// timestamp conversion, *src is time since epoch in ms
int conversion_epoch_to_string(char *dst, int len, const lng *src, lng null_value, int digits);
int conversion_epoch_tz_to_string(char *dst, int len, const lng *src, lng null_value, int digits, int timezone_diff);

void conversion_timestamp_get_data(lng timestamp, int timezone_diff, short *year, unsigned short *month, unsigned short *day, unsigned short *hour, unsigned short *minute, unsigned short *second, unsigned int *nanosecond);

// blobs are converted to strings in hexadecimal format (i.e. two characters per byte)
int conversion_blob_to_string(char *dst, int len, const char *blobdata, size_t nitems);

#endif