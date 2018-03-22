/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0.  If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * Copyright 1997 - July 2008 CWI, August 2008 - 2018 MonetDB B.V.
 */

#ifndef _OPT_WELD_
#define _OPT_WELD_
#include "opt_prelude.h"
#include "opt_support.h"
#include "mal_exception.h"

mal_export str OPTweldImplementation(Client cntxt, MalBlkPtr mb, MalStkPtr stk, InstrPtr p);

#endif