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
 * Copyright August 2008-2012 MonetDB B.V.
 * All Rights Reserved.
 */

#ifndef SQL_CATALOG_H
#define SQL_CATALOG_H

#include <sql_mem.h>
#include <sql_list.h>
#include <stream.h>

#define tr_none		0
#define tr_readonly	1
#define tr_writable	2
#define tr_serializable 4

#define ACT_NO_ACTION 0
#define ACT_CASCADE 1
#define ACT_RESTRICT 2
#define ACT_SET_NULL 3
#define ACT_SET_DEFAULT 4

#define DROP_RESTRICT 0
#define DROP_CASCADE 1
#define DROP_CASCADE_START 2

#define PRIV_SELECT 1
#define PRIV_UPDATE 2
#define PRIV_INSERT 4
#define PRIV_DELETE 8
#define PRIV_EXECUTE 16
#define PRIV_GRANT 32

#define SCHEMA_DEPENDENCY 1
#define TABLE_DEPENDENCY 2
#define COLUMN_DEPENDENCY 3
#define KEY_DEPENDENCY 4
#define VIEW_DEPENDENCY 5
#define USER_DEPENDENCY 6
#define FUNC_DEPENDENCY 7
#define TRIGGER_DEPENDENCY 8
#define OWNER_DEPENDENCY 9
#define INDEX_DEPENDENCY 10
#define FKEY_DEPENDENCY 11
#define TYPE_DEPENDENCY 11
#define SEQ_DEPENDENCY 12
#define PROC_DEPENDENCY 13
#define BEDROPPED_DEPENDENCY 14		/*The object must be dropped when the dependent object is dropped independently of the DROP type.*/
#define NO_DEPENDENCY 0
#define HAS_DEPENDENCY 1
#define CICLE_DEPENDENCY 2

#define NO_TRIGGER 0
#define IS_TRIGGER 1

#define ROLE_PUBLIC   1
#define ROLE_SYSADMIN 2
#define USER_MONETDB  3

#define ISO_READ_UNCOMMITED 1
#define ISO_READ_COMMITED   2
#define ISO_READ_REPEAT	    3
#define ISO_SERIALIZABLE    4

#define SCALE_NONE	0
#define SCALE_FIX	1	/* many numerical functions require equal
                           scales/precision for all their inputs */
#define SCALE_NOFIX	2
#define SCALE_MUL	3	/* multiplication gives the sum of scales */
#define SCALE_DIV	4	/* div on the other hand reduces the scales */ 
#define DIGITS_ADD	5	/* some types grow under functions (concat) */
#define INOUT		6	/* output type equals input type */
#define SCALE_EQ	7	/* user defined functions need equal scales */

#define TR_OLD 0
#define TR_NEW 1

#define RDONLY 0
#define RD_INS 1
#define RD_UPD 2

#define FRAME_ROWS  0 
#define FRAME_RANGE 1

#define EXCLUDE_NONE 0
#define EXCLUDE_CURRENT_ROW 1
#define EXCLUDE_GROUP 2
#define EXCLUDE_TIES 3
#define EXCLUDE_NO_OTHERS 4

#define cur_user 1
#define cur_role 2

#define sql_max(i1,i2) ((i1)<(i2))?(i2):(i1)

#define dt_schema 	"%dt%"
#define isDeclaredSchema(s) 	(strcmp(s->base.name, dt_schema) == 0)

typedef enum temp_t { 
	SQL_PERSIST,
	SQL_LOCAL_TEMP,
	SQL_GLOBAL_TEMP,
	SQL_DECLARED_TABLE,	/* table variable inside a stored procedure */
	SQL_DECLARED_ARRAY,	/* array variable inside a stored procedure */
	SQL_MERGE_TABLE,
	SQL_STREAM,
	SQL_REMOTE,
	SQL_REPLICA_TABLE
} temp_t;

typedef enum commit_action_t { 
	CA_COMMIT, 	/* commit rows, only for persistent tables */
	CA_DELETE, 	/* delete rows */
	CA_PRESERVE,	/* preserve rows */
	CA_DROP,	/* drop table */
	CA_ABORT	/* abort changes, internal only */
} ca_t;

typedef int sqlid;

typedef struct sql_base {
	int wtime;
	int rtime;
	int flag;
	sqlid id;
	char *name;
} sql_base;

extern void base_init(sql_allocator *sa, sql_base * b, sqlid id, int flag, char *name);
extern void base_set_name(sql_base * b, char *name);
extern void base_destroy(sql_base * b);

typedef struct changeset {
	sql_allocator *sa;
	fdestroy destroy;
	struct list *set;
	struct list *dset;
	node *nelm;
} changeset;

extern void cs_new(changeset * cs, sql_allocator *sa, fdestroy destroy);
extern void cs_destroy(changeset * cs);
extern void cs_add(changeset * cs, void *elm, int flag);
extern void cs_add_before(changeset * cs, node *n, void *elm);
extern void cs_del(changeset * cs, node *elm, int flag);
extern int cs_size(changeset * cs);
extern node *cs_find_name(changeset * cs, char *name);
extern node *cs_find_id(changeset * cs, int id);
extern node *cs_first_node(changeset * cs);
extern node *cs_last_node(changeset * cs);

typedef void *backend_code;
typedef size_t backend_stack;

typedef struct sql_trans {
	char *name;
	int stime;		/* transaction time stamp (aka start time) */
	int rtime;
	int wtime;
	int schema_number;	/* schema timestamp */
	int schema_updates;	/* set on schema changes */
	int status;		/* status of the last query */
	list *dropped;  	/* protection against recursive cascade action*/

	changeset schemas;

	sql_allocator *sa;	/* transaction allocator */

	struct sql_trans *parent;	/* multilevel transaction support */
	backend_stack stk;		
} sql_trans;

typedef struct sql_schema {
	sql_base base;
	int auth_id;
	int owner;
	// TODO? int type;		/* persistent, session local, transaction local */

	changeset tables;
	changeset types;
	changeset funcs;
	changeset seqs;
	list *keys;		/* Names for keys, idxs and triggers are */
	list *idxs;		/* global, but these objects are only */
	list *triggers;		/* useful within a table */

	char *internal; 	/* optional internal module name */
	sql_trans *tr;
} sql_schema;

typedef struct sql_type {
	sql_base base;

	char *sqlname;
	unsigned int digits;
	unsigned int scale;	/* indicates how scale is used in functions */
	int localtype;		/* localtype, need for coersions */
	unsigned char radix;
	unsigned int bits;
	unsigned char eclass; 	/* types are grouped into equivalence classes */
	sql_schema *s;
} sql_type;

typedef struct sql_alias {
	char *name;
	char *alias;
} sql_alias;

typedef struct sql_subtype {
	sql_type *type;
	unsigned int digits;
	unsigned int scale;

	struct sql_table *comp_type;	
} sql_subtype;

typedef struct sql_dimspec {
	char *start; /* NULL means unbounded */
	char *step;
	char *stop;
} sql_dimspec;

/* sql_func need type transform rules types are equal if underlying
 * types are equal + scale is equal if types do not mach we try type
 * conversions which means for simple 1 arg functions
 */

typedef struct sql_arg {
	char *name;
	sql_subtype type;
} sql_arg;

#define F_FUNC 1
#define F_PROC 2
#define F_AGGR 3
#define F_FILT 4
#define F_UNION 5

#define IS_FUNC(f) (f->type == F_FUNC)
#define IS_PROC(f) (f->type == F_PROC)
#define IS_AGGR(f) (f->type == F_AGGR)
#define IS_FILT(f) (f->type == F_FILT)
#define IS_UNION(f) (f->type == F_UNION)

typedef struct sql_func {
	sql_base base;

	char *imp;
	char *mod;
	int type;
	list *ops;	/* param list */
	sql_subtype res;
	int nr;
	int sql;	/* 0 native implementation
			   1 sql 
			   2 sql instantiated proc 
			*/
	char *query;	/* sql code */
	int side_effect;
	int fix_scale;
			/*
	   		   SCALE_NOFIX/SCALE_NONE => nothing
	   		   SCALE_FIX => input scale fixing,
	   		   SCALE_ADD => leave inputs as is and do add scales
	   		   example numerical multiplication
	   		   SCALE_SUB => first input scale, fix with second scale
	   		   result scale is equal to first input
	   		   example numerical division
	   		   DIGITS_ADD => result digits, sum of args
	   		   example string concat
	 		*/
	sql_schema *s;
	sql_allocator *sa;
} sql_func;

typedef struct sql_subfunc {
	sql_func *func;
	sql_subtype res;
} sql_subfunc;

typedef struct sql_subaggr {
	sql_func *aggr;
	sql_subtype res;
} sql_subaggr;

typedef enum key_type {
	pkey,
	ukey,
	fkey
} key_type;

typedef struct sql_kc {
	struct sql_column *c;
	int trunc;		/* 0 not truncated, >0 colum is truncated */
} sql_kc;

typedef enum idx_type {
	hash_idx,
	join_idx,
	oph_idx,		/* order preserving hash */
	no_idx,			/* no idx, ie no storage */
	new_idx_types
} idx_type;

#define hash_index(t) 		(t == hash_idx || t == oph_idx )
#define idx_is_column(t) 	(hash_index(t) || t == join_idx || t == no_idx)
#define idx_has_column(t) 	(hash_index(t) || t == join_idx)

typedef struct sql_idx {
	sql_base base;
	idx_type type;		/* unique */
	struct list *columns;	/* list of sql_kc */
	struct sql_table *t;
	struct sql_key *key;	/* key */
	void *data;
} sql_idx;

/* fkey consists of two of these */
typedef struct sql_key {	/* pkey, ukey, fkey */
	sql_base base;
	key_type type;		/* pkey, ukey, fkey */
	sql_idx *idx;		/* idx to accelerate key check */

	struct list *columns;	/* list of sql_kc */
	struct sql_table *t;
	int drop_action;	/* only needed for alter drop key */
} sql_key;

typedef struct sql_ukey {	/* pkey, ukey */
	sql_key k;
	list *keys;
} sql_ukey;

typedef struct sql_fkey {	/* fkey */
	sql_key k;
	/* no action, restrict (default), cascade, set null, set default */
	int on_delete;	
	int on_update;	
	struct sql_ukey *rkey;	/* only set for fkey and rkey */
} sql_fkey;

typedef struct sql_trigger {
	sql_base base;
	sht time;		/* before or after */
	sht orientation; 	/* row or statement */
	sht event;		/* insert, delete, update */
	/* int action_order;	 TODO, order within the set of triggers */
	struct list *columns;	/* update trigger on list of (sql_kc) columns */

	struct sql_table *t;
	char *old_name;		/* name referencing the old values */
	char *new_name;		/* name referencing the new values */
	
	char *condition; 	/* when search condition, ie query */
	char *statement;	/* action, ie list of sql statements */
} sql_trigger;

typedef struct sql_sequence {
	sql_base base;
	lng start;
	lng minvalue;
	lng maxvalue;
	lng increment;
	lng cacheinc;
	bit cycle;
	bit bedropped;		/*Drop the SEQUENCE if you are dropping the column, e.g., SERIAL COLUMN".*/
	sql_schema *s;
} sql_sequence;

/* histogram types */
typedef enum sql_histype {
       X_EXACT,
       X_EQUI_WIDTH,
       X_EQUI_HEIGHT
} sql_histype;

typedef struct sql_column {
	sql_base base;
	sql_subtype type;
	sql_dimspec *dim;
	int colnr;
	bit null;
	char *def;
	char unique; 		/* NOT UNIQUE, UNIQUE, SUB_UNIQUE */
	int drop_action;	/* only used for alter statements */
	char *storage_type;
	int sorted;		/* for DECLARED (dupped tables) we keep order info */

	struct sql_table *t;
	void *data;
} sql_column;

typedef enum table_types {
	tt_table = 0, 		/* table */
	tt_view = 1, 		/* view */
	tt_generated = 2,	/* generated (functions can be sql or c-code) */
	tt_merge_table = 3,	/* multiple tables form one table */
	tt_stream = 4,		/* stream */
	tt_remote = 5,		/* stored on a remote server */
	tt_replica_table = 6,	/* multiple replica of the same table */
	tt_array = 7		/* arrays */
} table_types;

#define isTable(x) 	(x->type==tt_table)
#define isView(x)  	(x->type==tt_view)
#define isGenerated(x)  (x->type==tt_generated)
#define isMergeTable(x) (x->type==tt_merge_table)
#define isStream(x)  	(x->type==tt_stream)
#define isRemote(x)  	(x->type==tt_remote)
#define isReplicaTable(x) (x->type==tt_replica_table)
#define isArray(x)  	(x->type==tt_array)
#define isTableOrArray(x)(x->type==tt_array || x->type==tt_table)
#define isFixedArray(a) (a->type == tt_array && a->fixed)
#define isFixedDim(d)   (d->start && d->step && d->stop && d->start[0] != '\0' && d->step[0] != '\0' && d->stop[0] != '\0')

typedef struct sql_table {
	sql_base base;
	sht type;		/* table, view, generated, merge_table, stream or array*/
	bit system;		/* system or user table */
	temp_t persistence;	/* persistent, global or local temporary */
	ca_t commit_action;  	/* on commit action */
	bit readonly;	
	char *query;		/* views and generated may require some query 

				   A generated without a query is simply 
					a type definition
				*/
	int  sz;

	sql_ukey *pkey;
	changeset columns;
	changeset idxs;
	changeset keys;
	changeset triggers;
	changeset tables;
	int drop_action;	/* only needed for alter drop table */

	int cleared;		/* cleared in the current transaction */
	bit fixed; /* for arrays: fixed or unbounded */
	int ndims; /* for arrays: number of dimensions */

	void *data;
	struct sql_schema *s;
	struct sql_table *p;
} sql_table;

typedef struct res_col {
	char *tn;
	char *name;
	sql_subtype type;
	bat b;
	int mtype;
	ptr *p;
} res_col;

typedef struct res_table {
	int id;
	int query_type;
	int nr_cols;
	int cur_col;
	char *tsep;
	char *rsep;
	char *ssep;
	char *ns;
	res_col *cols;
	bat order;
	struct res_table *next;
} res_table;

typedef struct sql_session {
	sql_trans *tr; 		/* active transaction */	

	char *schema_name;
	sql_schema *schema;

	char ac_on_commit;	/* if 1, auto_commit should be enabled on
	                           commit, rollback, etc. */
	char auto_commit;
	int level;		/* TRANSACTION isolation level */
	int active;		/* active transaction */
	int status;		/* status, ok/error */
	backend_stack stk;
} sql_session;

extern void schema_destroy(sql_schema *s);
extern void table_destroy(sql_table *t);
extern void column_destroy(sql_column *c);
extern void kc_destroy(sql_kc *kc);
extern void key_destroy(sql_key *k);
extern void idx_destroy(sql_idx * i);

extern node *list_find_name(list *l, char *name);
extern node *list_find_id(list *l, int id);
extern node *list_find_base_id(list *l, int id);

extern sql_key *find_sql_key(sql_table *t, char *kname);

extern sql_idx *find_sql_idx(sql_table *t, char *kname);

extern sql_column *find_sql_column(sql_table *t, char *cname);

extern sql_table *find_sql_table(sql_schema *s, char *tname);
extern sql_table *find_sql_table_id(sql_schema *s, int id);
extern char *tt2string(int tt);

extern sql_sequence *find_sql_sequence(sql_schema *s, char *sname);

extern sql_schema *find_sql_schema(sql_trans *t, char *sname);

extern sql_type *find_sql_type(sql_schema * s, char *tname);
extern sql_type *sql_trans_bind_type(sql_trans *tr, sql_schema *s, char *name);

extern sql_func *find_sql_func(sql_schema * s, char *tname);
extern list *find_all_sql_func(sql_schema * s, char *tname, int type);
extern sql_func *sql_trans_bind_func(sql_trans *tr, char *name);

extern node *find_sql_key_node(sql_table *t, char *kname, int id);
extern node *find_sql_idx_node(sql_table *t, char *kname, int id);
extern node *find_sql_column_node(sql_table *t, char *cname, int id);
extern node *find_sql_table_node(sql_schema *s, char *tname, int id);
extern node *find_sql_sequence_node(sql_schema *s, char *sname, int id);
extern node *find_sql_schema_node(sql_trans *t, char *sname, int id);
extern node *find_sql_type_node(sql_schema * s, char *tname, int id);
extern node *find_sql_func_node(sql_schema * s, char *fname, int id);
#endif /* SQL_CATALOG_H */
