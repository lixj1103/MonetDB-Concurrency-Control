/*
 * The contents of this file are subject to the MonetDB Public License
 * Version 1.1 (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://www.monetdb.org/Legal/MonetDBLicense
 *
 * Software distributed under the License is distributed on an "AS IS"
 * basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
 * License for the specific language governing rights and limitations
 * under the License.
 *
 * The Original Code is the MonetDB Database System.
 *
 * The Initial Developer of the Original Code is CWI.
 * Portions created by CWI are Copyright (C) 1997-July 2008 CWI.
 * Copyright August 2008-2014 MonetDB B.V.
 * All Rights Reserved.
 */

/*
 * This file was generated by using the script sql_cast.sh.
 */

#define CAST_INTEGER_2_NUMERIC_2(T1,T2)						\
sql5_export str T1##_2_##T2(T2 *res, T1 *v);					\
sql5_export str bat##T1##_2_##T2(bat *res, bat *v);				\
sql5_export str T1##_dec2_##T2(T2 *res, int *s1, T1 *v);			\
sql5_export str bat##T1##_dec2_##T2(bat *res, int *s1, bat *v);			\
sql5_export str T1##_dec2dec_##T2(T2 *res, int *S1, T1 *v, int *d2, int *S2);	\
sql5_export str bat##T1##_dec2dec_##T2(bat *res, int *S1, bat *v, int *d2, int *S2);\
sql5_export str T1##_num2dec_##T2(T2 *res, T1 *v, int *d2, int *s2);		\
sql5_export str bat##T1##_num2dec_##T2(bat *res, bat *v, int *d2, int *s2)

#ifdef HAVE_HGE
#define CAST_INTEGER_2_NUMERIC_2_hge(T1) CAST_INTEGER_2_NUMERIC_2(T1,hge)
#else
#define CAST_INTEGER_2_NUMERIC_2_hge(T1)
#endif

#define CAST_INTEGER_2_NUMERIC_1(T1)	\
CAST_INTEGER_2_NUMERIC_2(T1,bte);	\
CAST_INTEGER_2_NUMERIC_2(T1,sht);	\
CAST_INTEGER_2_NUMERIC_2(T1,int);	\
CAST_INTEGER_2_NUMERIC_2(T1,wrd);	\
CAST_INTEGER_2_NUMERIC_2(T1,lng);	\
CAST_INTEGER_2_NUMERIC_2_hge(T1);	\
CAST_INTEGER_2_NUMERIC_2(T1,flt);	\
CAST_INTEGER_2_NUMERIC_2(T1,dbl)

CAST_INTEGER_2_NUMERIC_1(bte);
CAST_INTEGER_2_NUMERIC_1(sht);
CAST_INTEGER_2_NUMERIC_1(int);
CAST_INTEGER_2_NUMERIC_1(wrd);
CAST_INTEGER_2_NUMERIC_1(lng);
#ifdef HAVE_HGE
CAST_INTEGER_2_NUMERIC_1(hge);
#endif


#define CAST_FLOATINGPOINT_2_INTEGER_2(T1,T2)				\
sql5_export str T1##_2_##T2(T2 *res, T1 *v);				\
sql5_export str bat##T1##_2_##T2(bat *res, bat *v);			\
sql5_export str T1##_num2dec_##T2(T2 *res, T1 *v, int *d2, int *s2);	\
sql5_export str bat##T1##_num2dec_##T2(bat *res, bat *v, int *d2, int *s2)

#ifdef HAVE_HGE
#define CAST_FLOATINGPOINT_2_INTEGER_2_hge(T1) CAST_FLOATINGPOINT_2_INTEGER_2(T1,hge)
#else
#define CAST_FLOATINGPOINT_2_INTEGER_2_hge(T1)
#endif

#define CAST_FLOATINGPOINT_2_INTEGER_1(T1)	\
CAST_FLOATINGPOINT_2_INTEGER_2(T1,bte);	\
CAST_FLOATINGPOINT_2_INTEGER_2(T1,sht);	\
CAST_FLOATINGPOINT_2_INTEGER_2(T1,int);	\
CAST_FLOATINGPOINT_2_INTEGER_2(T1,wrd);	\
CAST_FLOATINGPOINT_2_INTEGER_2(T1,lng); \
CAST_FLOATINGPOINT_2_INTEGER_2_hge(T1)

CAST_FLOATINGPOINT_2_INTEGER_1(flt);
CAST_FLOATINGPOINT_2_INTEGER_1(dbl);
