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
 * Copyright August 2008-2011 MonetDB B.V.
 * All Rights Reserved.
 */

#include "ODBCGlobal.h"
#include "ODBCStmt.h"

#define ODBC_DESC_MAGIC_NR	21845	/* for internal sanity check only */

/*
 * Creates a new allocated ODBCDesc object and initializes it.
 *
 * Precondition: valid ODBCDbc object
 * Postcondition: returns a new ODBCDesc object
 */
ODBCDesc *
newODBCDesc(ODBCDbc *dbc)
{
	ODBCDesc *desc;

	assert(dbc);

	desc = (ODBCDesc *) malloc(sizeof(ODBCDesc));
	if (desc == NULL) {
		/* Memory allocation error */
		addDbcError(dbc, "HY001", NULL, 0);
		return NULL;
	}
	assert(desc);

	desc->Dbc = dbc;
	desc->Error = NULL;
	desc->RetrievedErrors = 0;
	desc->Stmt = NULL;
	desc->descRec = NULL;
	desc->sql_desc_alloc_type = SQL_DESC_ALLOC_USER;
	desc->sql_desc_array_size = 1;
	desc->sql_desc_array_status_ptr = NULL;
	desc->sql_desc_bind_offset_ptr = NULL;
	desc->sql_desc_bind_type = SQL_BIND_TYPE_DEFAULT;
	desc->sql_desc_count = 0;
	desc->sql_desc_rows_processed_ptr = NULL;

	desc->Type = ODBC_DESC_MAGIC_NR;	/* set it valid */
	return desc;
}


/*
 * Check if the descriptor handle is valid.
 * Note: this function is used internally by the driver to assert legal
 * and save usage of the handle and prevent crashes as much as possible.
 *
 * Precondition: none
 * Postcondition: returns 1 if it is a valid statement handle,
 * 	returns 0 if is invalid and thus an unusable handle.
 */
int
isValidDesc(ODBCDesc *desc)
{
#ifdef ODBCDEBUG
	if (!(desc && desc->Type == ODBC_DESC_MAGIC_NR))
		ODBCLOG("desc " PTRFMT "not a valid descriptor handle\n", PTRFMTCAST desc);
#endif
	return desc && desc->Type == ODBC_DESC_MAGIC_NR;
}

/*
 * Creates and adds an error msg object to the end of the error list of
 * this ODBCDesc struct.
 * When the errMsg is NULL and the SQLState is an ISO SQLState the
 * standard ISO message text for the SQLState is used as message.
 *
 * Precondition: desc must be valid. SQLState and errMsg may be NULL.
 */
void
addDescError(ODBCDesc *desc, const char *SQLState, const char *errMsg, int nativeErrCode)
{
	ODBCError *error = NULL;

#ifdef ODBCDEBUG
	ODBCLOG("addDescError " PTRFMT " %s %s %d\n", PTRFMTCAST desc, SQLState, errMsg ? errMsg : getStandardSQLStateMsg(SQLState), nativeErrCode);
#endif
	assert(isValidDesc(desc));

	error = newODBCError(SQLState, errMsg, nativeErrCode);
	appendODBCError(&desc->Error, error);
}

/*
 * Extracts an error object from the error list of this ODBCDesc struct.
 * The error object itself is removed from the error list.
 * The caller is now responsible for freeing the error object memory.
 *
 * Precondition: desc and error must be valid
 * Postcondition: returns a ODBCError object or null when no error is available.
 */
ODBCError *
getDescError(ODBCDesc *desc)
{
	assert(isValidDesc(desc));
	return desc->Error;
}

static void
cleanODBCDescRec(ODBCDesc *desc, ODBCDescRec *rec)
{
	if (rec->sql_desc_base_column_name)
		free(rec->sql_desc_base_column_name);
	if (rec->sql_desc_base_table_name)
		free(rec->sql_desc_base_table_name);
	if (rec->sql_desc_catalog_name)
		free(rec->sql_desc_catalog_name);
	if (rec->sql_desc_label)
		free(rec->sql_desc_label);
	if (rec->sql_desc_literal_prefix)
		free(rec->sql_desc_literal_prefix);
	if (rec->sql_desc_literal_suffix)
		free(rec->sql_desc_literal_suffix);
	if (rec->sql_desc_local_type_name)
		free(rec->sql_desc_local_type_name);
	if (rec->sql_desc_name)
		free(rec->sql_desc_name);
	if (rec->sql_desc_schema_name)
		free(rec->sql_desc_schema_name);
	if (rec->sql_desc_table_name)
		free(rec->sql_desc_table_name);
	if (rec->sql_desc_type_name)
		free(rec->sql_desc_type_name);
	memset(rec, 0, sizeof(*rec));
	if (desc) {
		if (isAD(desc)) {
			rec->sql_desc_concise_type = SQL_C_DEFAULT;
			rec->sql_desc_type = SQL_C_DEFAULT;
		} else if (isIPD(desc)) {
			rec->sql_desc_parameter_type = SQL_PARAM_INPUT;
			rec->sql_desc_nullable = SQL_NULLABLE;
		}
	}
}

void
setODBCDescRecCount(ODBCDesc *desc, int count)
{
	assert(count >= 0);
	assert(desc->sql_desc_count >= 0);

	if (count == desc->sql_desc_count)
		return;
	if (count < desc->sql_desc_count) {
		int i;

		for (i = count + 1; i <= desc->sql_desc_count; i++)
			cleanODBCDescRec(NULL, &desc->descRec[i]);
	}
	if (count == 0) {
		assert(desc->descRec != NULL);
		free(desc->descRec);
		desc->descRec = NULL;
	} else if (desc->descRec == NULL) {
		assert(desc->sql_desc_count == 0);
		desc->descRec = (ODBCDescRec *) malloc((count + 1) * sizeof(*desc->descRec));
	} else {
		assert(desc->sql_desc_count > 0);
		desc->descRec = (ODBCDescRec *) realloc(desc->descRec, (count + 1) * sizeof(*desc->descRec));
	}
	if (count > desc->sql_desc_count) {
		int i;

		memset(desc->descRec + desc->sql_desc_count + 1, 0, (count - desc->sql_desc_count) * sizeof(*desc->descRec));
		if (isAD(desc)) {
			for (i = desc->sql_desc_count + 1; i <= count; i++) {
				desc->descRec[i].sql_desc_concise_type = SQL_C_DEFAULT;
				desc->descRec[i].sql_desc_type = SQL_C_DEFAULT;
			}
		} else if (isIPD(desc)) {
			for (i = desc->sql_desc_count + 1; i <= count; i++) {
				desc->descRec[i].sql_desc_parameter_type = SQL_PARAM_INPUT;
				desc->descRec[i].sql_desc_nullable = SQL_NULLABLE;
			}
		}
	}
	desc->sql_desc_count = count;
}

/*
 * Destroys the ODBCDesc object including its own managed data.
 *
 * Precondition: desc must be valid.
 * Postcondition: desc is completely destroyed, desc handle is become invalid.
 */
void
destroyODBCDesc(ODBCDesc *desc)
{
	assert(isValidDesc(desc));

	desc->Type = 0;
	deleteODBCErrorList(&desc->Error);
	setODBCDescRecCount(desc, 0);
	free(desc);
}

ODBCDescRec *
addODBCDescRec(ODBCDesc *desc, SQLSMALLINT recno)
{
	assert(desc);
	assert(recno > 0);

	if (desc->sql_desc_count < recno)
		setODBCDescRecCount(desc, recno);
	else {
		assert(desc->descRec != NULL);
		cleanODBCDescRec(desc, &desc->descRec[recno]);
	}

	return &desc->descRec[recno];
}

/* Return either the column size or display size for a column or parameter. */
SQLULEN
ODBCLength(ODBCDescRec *rec, enum ODBCLengthType lengthtype)
{
	switch (rec->sql_desc_concise_type) {
	case SQL_CHAR:
	case SQL_VARCHAR:
	case SQL_LONGVARCHAR:
		if (lengthtype == OctetLength)
			return rec->sql_desc_length * 6;
		else
			return rec->sql_desc_length;
	case SQL_WCHAR:
	case SQL_WVARCHAR:
	case SQL_WLONGVARCHAR:
	case SQL_BINARY:
	case SQL_VARBINARY:
	case SQL_LONGVARBINARY:
		return rec->sql_desc_length;
	case SQL_DECIMAL:
	case SQL_NUMERIC:
		return rec->sql_desc_length + (lengthtype == ColumnSize ? 0 : 2);
	case SQL_BIT:
		return 1;
	case SQL_TINYINT:
		switch (lengthtype) {
		case ColumnSize:
			return 3;
		case DisplaySize:
			return 4;
		case OctetLength:
			return 1;
		}
		break;
	case SQL_SMALLINT:
		switch (lengthtype) {
		case ColumnSize:
			return 5;
		case DisplaySize:
			return 5 + !rec->sql_desc_unsigned;
		case OctetLength:
			return 2;
		}
		break;
	case SQL_INTEGER:
		switch (lengthtype) {
		case ColumnSize:
			return 10;
		case DisplaySize:
			return 10 + !rec->sql_desc_unsigned;
		case OctetLength:
			return 4;
		}
		break;
	case SQL_BIGINT:
		switch (lengthtype) {
		case ColumnSize:
			return 19 + (rec->sql_desc_unsigned != 0);
		case DisplaySize:
			return 20;
		case OctetLength:
			return 20;
		}
		break;
	case SQL_REAL:
		switch (lengthtype) {
		case ColumnSize:
			return 7;
		case DisplaySize:
			/* sign, 7 digits, decimal point, E, sign, 2 digits */
			return 14;
		case OctetLength:
			return 4;
		}
		break;
	case SQL_FLOAT:
	case SQL_DOUBLE:
		switch (lengthtype) {
		case ColumnSize:
			return 15;
		case DisplaySize:
			/* sign, 15 digits, decimal point, E, sign, 3 digits */
			return 24;
		case OctetLength:
			return 8;
		}
		break;
	case SQL_TYPE_DATE:
		if (lengthtype == OctetLength)
			return 6;
		else {
			/* strlen("yyyy-mm-dd") */
			return 10;
		}
	case SQL_TYPE_TIME:
		if (lengthtype == OctetLength)
			return 6;
		else {
			/* strlen("hh:mm:ss.fff") */
			return 12;
		}
	case SQL_TYPE_TIMESTAMP:
		if (lengthtype == OctetLength)
			return 16;
		else {
			/* strlen("yyyy-mm-dd hh:mm:ss.fff") */
			return 23;
		}
	case SQL_INTERVAL_SECOND:
		if (lengthtype == OctetLength)
			return 34;
		/* strlen("INTERVAL -'sss[.fff]' SECOND(p,q)") */
		return 11 + 13 +
			(rec->sql_desc_datetime_interval_precision > 10) +
			(rec->sql_desc_precision > 10) +
			rec->sql_desc_datetime_interval_precision +
			(rec->sql_desc_precision > 0 ?
			 rec->sql_desc_precision + 1 :
			 0);
	case SQL_INTERVAL_DAY_TO_SECOND:
		if (lengthtype == OctetLength)
			return 34;
		/* strlen("INTERVAL -'ddd hh:mm:ss[.fff]' DAY(p) TO SECOND(q)") */
		return 11 + 21 +
			(rec->sql_desc_datetime_interval_precision > 10) +
			(rec->sql_desc_precision > 10) +
			rec->sql_desc_datetime_interval_precision +
			9 +
			(rec->sql_desc_precision > 0 ?
			 rec->sql_desc_precision + 1 :
			 0);
	case SQL_INTERVAL_HOUR_TO_SECOND:
		if (lengthtype == OctetLength)
			return 34;
		/* strlen("INTERVAL -'hhh:mm:ss[.fff]' HOUR(p) TO SECOND(q)") */
		return 11 + 22 +
			(rec->sql_desc_datetime_interval_precision > 10) +
			(rec->sql_desc_precision > 10) +
			rec->sql_desc_datetime_interval_precision +
			6 +
			(rec->sql_desc_precision > 0 ?
			 rec->sql_desc_precision + 1 :
			 0);
	case SQL_INTERVAL_MINUTE_TO_SECOND:
		if (lengthtype == OctetLength)
			return 34;
		/* strlen("INTERVAL -'mmm:ss[.fff]' MINUTE(p) TO SECOND(q)") */
		return 11 + 24 +
			(rec->sql_desc_datetime_interval_precision > 10) +
			(rec->sql_desc_precision > 10) +
			rec->sql_desc_datetime_interval_precision +
			3 +
			(rec->sql_desc_precision > 0 ?
			 rec->sql_desc_precision + 1 :
			 0);
	case SQL_INTERVAL_YEAR:
		if (lengthtype == OctetLength)
			return 34;
		/* strlen("INTERVAL -'yyy' YEAR(p)") */
		return 11 + 9 +
			(rec->sql_desc_datetime_interval_precision > 10) +
			rec->sql_desc_datetime_interval_precision;
	case SQL_INTERVAL_MONTH:
		if (lengthtype == OctetLength)
			return 34;
		/* strlen("INTERVAL -'yyy' MONTH(p)") */
		return 11 + 10 +
			(rec->sql_desc_datetime_interval_precision > 10) +
			rec->sql_desc_datetime_interval_precision;
	case SQL_INTERVAL_DAY:
		if (lengthtype == OctetLength)
			return 34;
		/* strlen("INTERVAL -'yyy' DAY(p)") */
		return 11 + 8 +
			(rec->sql_desc_datetime_interval_precision > 10) +
			rec->sql_desc_datetime_interval_precision;
	case SQL_INTERVAL_HOUR:
		if (lengthtype == OctetLength)
			return 34;
		/* strlen("INTERVAL -'yyy' HOUR(p)") */
		return 11 + 9 +
			(rec->sql_desc_datetime_interval_precision > 10) +
			rec->sql_desc_datetime_interval_precision;
	case SQL_INTERVAL_MINUTE:
		if (lengthtype == OctetLength)
			return 34;
		/* strlen("INTERVAL -'yyy' MINUTE(p)") */
		return 11 + 11 +
			(rec->sql_desc_datetime_interval_precision > 10) +
			rec->sql_desc_datetime_interval_precision;
	case SQL_INTERVAL_YEAR_TO_MONTH:
		if (lengthtype == OctetLength)
			return 34;
		/* strlen("INTERVAL -'yyy' YEAR(p) TO MONTH") */
		return 11 + 18 +
			(rec->sql_desc_datetime_interval_precision > 10) +
			rec->sql_desc_datetime_interval_precision +
			3;
	case SQL_INTERVAL_DAY_TO_HOUR:
		if (lengthtype == OctetLength)
			return 34;
		/* strlen("INTERVAL -'yyy' DAY(p) TO HOUR") */
		return 11 + 16 +
			(rec->sql_desc_datetime_interval_precision > 10) +
			rec->sql_desc_datetime_interval_precision +
			3;
	case SQL_INTERVAL_HOUR_TO_MINUTE:
		if (lengthtype == OctetLength)
			return 34;
		/* strlen("INTERVAL -'yyy' HOUR(p) TO MINUTE") */
		return 11 + 19 +
			(rec->sql_desc_datetime_interval_precision > 10) +
			rec->sql_desc_datetime_interval_precision +
			3;
	case SQL_INTERVAL_DAY_TO_MINUTE:
		if (lengthtype == OctetLength)
			return 34;
		/* strlen("INTERVAL -'yyy' DAY(p) TO MINUTE") */
		return 11 + 18 +
			(rec->sql_desc_datetime_interval_precision > 10) +
			rec->sql_desc_datetime_interval_precision +
			6;
	case SQL_GUID:
		if (lengthtype == OctetLength)
			return 16;
		/* strlen("aaaaaaaa-bbbb-cccc-dddd-eeeeeeeeeeee") */
		return 36;
	default:
		break;
	}
	return SQL_NO_TOTAL;
}
