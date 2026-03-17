/************************************************************************
*									*
* The SB-Prolog System							*
* Copyright SUNY at Stony Brook, 1986; University of Arizona, 1987	*
*									*
************************************************************************/

/*-----------------------------------------------------------------
SB-Prolog is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY.  No author or distributor
accepts responsibility to anyone for the consequences of using it
or for whether it serves any particular purpose or works at all,
unless he says so in writing.  Refer to the SB-Prolog General Public
License for full details.

Everyone is granted permission to copy, modify and redistribute
SB-Prolog, but only under the conditions described in the
SB-Prolog General Public License.   A copy of this license is
supposed to have been given to you along with SB-Prolog so you
can know your rights and responsibilities.  It should be in a
file named COPYING.  Among other things, the copyright notice
and this notice must be preserved on all copies. 
------------------------------------------------------------------ */

/* the builtins in this file manage symbol-table access.  they are
   used by the predicates current_atom/1, current_functor/2 and
   current_predicate/2.							*/

#include "builtin.h"

#define BINDREG(reg, val)  op = gregc(reg); deref(op); follow(op) = val; pushtrail(op)

b_STLOOKUP()
{	    /* reg 1: current bucket no.; reg. 2: current bucket element;
	       reg 3: element type sought; reg 4: 0 if this is a "new" search,
	       (i.e. if search should start at the beginning), 1 o/w; reg 5:
	       new bucket no.; reg 6: new element in bucket (ptr to ptr to
	       PSC table entry); reg 7: ptr to PSC table entry;		*/

   struct psc_rec *psc_ptr;
   pw chain_ptr;
   int i, elt_type;
   word op;
   register pw top;

   op = gregc(1); deref(op);
   i = intval(op);
   op = gregc(4); deref(op);
   if (intval(op)) {
	op = gregc(2); deref(op);
	chain_ptr = (pw)untagged(op);
	chain_ptr++;	    /* ptr to next element */
    }
    else chain_ptr = (pw)&hash_table[0][PERM];	/* init */
    op = gregc(3); deref(op);
    elt_type = intval(op);
    while (chain_ptr != NULL) {
	if (!isfree(chain_ptr)) {
	    chain_ptr = (pw)follow(chain_ptr);
	    psc_ptr = (struct psc_rec *)follow(chain_ptr);
	    if (get_etype(psc_ptr) == elt_type) {  /* found a match */
		BINDREG(5, makeint(i));
		BINDREG(6, ((word)chain_ptr|CS_TAG));
		BINDREG(7, ((word)psc_ptr | CS_TAG));
		return;
	    }
	    else chain_ptr++;	/* didn't match, go to next element */
	}
	else {
	    i += 1;		/* next bucket */
	    if (i < bucket_chain) chain_ptr = (pw)&hash_table[i][PERM];
	    else chain_ptr = NULL;
	} 
    };
    {Fail0;}	/* can get here only if chain_ptr == NULL */
}

