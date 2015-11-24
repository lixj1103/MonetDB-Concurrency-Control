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

#ifndef _SQL_RDF_REL_H_
#define _SQL_RDF_REL_H_
#include "sql.h"		
#ifdef HAVE_RAPTOR
#include <rdfjgraph.h>
#endif

#ifdef WIN32
#ifndef LIBRDF
#define rdf_export extern __declspec(dllimport)
#else
#define rdf_export extern __declspec(dllexport)
#endif
#else
#define rdf_export extern
#endif

rdf_export sql_rel * rdf_rel_join(sql_allocator *sa, sql_rel *l, sql_rel *r, list *exps, operator_type join);

rdf_export sql_rel *rdf_rel_simple_combine_with_optional_cols(sql_allocator *sa, sql_rel *rel1, sql_rel *rel2); 

#endif /* _SQL_RDF_REL_H_ */