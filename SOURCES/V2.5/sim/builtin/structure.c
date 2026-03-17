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

/* structure.c */

#include "builtin.h"

static char perm = PERM;

b_ARG0()
{	/* (I, T, X) */
	    /* I is bound to a number; T is bound to a structure;
		X may be free or bound; no checking for above is done;
		I out of range will cause failure */

	register word op1, op2;
	register word *top;

	    op1 = gregc(1); deref(op1);
	    if (!isinteger(op1)) {
		printf("arg0: Index must be an integer.\n");
		Fail0; return;
	    }
	    op1 = intval(op1);

	    op2 = gregc(2); deref(op2);
	    if (isconstr(op2) && op1 <= get_str_arity(op2) && op1 > 0) 
		if (unify(*(((pw)(untag(op2))) + op1), gregc(3))) return;
	    if (islist(op2) && op1 <= 2)
		if (unify(*(((pw)(untag(op2))) + op1 - 1), gregc(3))) return;
	    Fail0;
}

b_ARITY()
{ 	/* (T, N) */
	    /*  T is bound to a structure or constant; N will be unified with 
		the arity of T */

	register word op1, op2;
	register word *top;

	    op1 = gregc(1);
	    deref(op1);
	    if (isconstr(op1)) op2 = makeint(get_arity(get_str_psc(op1)));
	    else if (islist(op1)) op2 = makeint(2);
	    else if (isnum(op1)) op2 = makeint(0);
	    else {printf("arity: Term must be bound.\n"); Fail0; return;}
	    if (!unify(gregc(2), op2)) {Fail0;} 
}

b_FUNCTOR0()
{	/* (T, F) */
	    /* T is bound to a structure (no checking), F will be unified
		with the functor of T */

    register word op1, op2;
    register pw top;
    struct psc_rec *psc_p;
    word insert();

    op1 = gregc(1);
    deref(op1);
    if (islist(op1)) 
	op2 = insert(".", 1, 0, &perm);
    else {
    	psc_p = get_str_psc(op1);
	if (get_arity(psc_p) != 0)
	    op2 = insert(get_name(psc_p), get_length(psc_p), 0, &perm);
	else op2 = op1;
	}
    op2 |= CS_TAG;
    if (!unify(op2, gregc(2))) {Fail0;}
    
}

b_BLDSTR()
{	/* (F, N, T) */
	    /* F is bound to an atom, N is bound to an integer, (>0)
	       T is free, and will be set to the most general structure
	       with functor F; incomplete error checking */
    int i;
    char a;
    struct psc_rec *psc_p;
    register word op1, op2;
    register pw top;

    op1 = gregc(1); deref(op1);
    if (!isconstr(op1)) 
	{printf("bldstr: first arg must be constant\n");
	Fail0; return;}
    op2 = gregc(2); deref(op2);
    if (!isinteger(op2))
	{printf("bldstr: second arg must be integer\n");
	Fail0; return;}
    a = intval(op2);
    if ((a < 0) || (a > 127))
	{printf("bldstr: arity must be between 0 and 127\n");
	 Fail0; return;}
    op2 = gregc(3);
    deref(op2);
    psc_p = get_str_psc(op1);
    if (get_arity(psc_p)>0) 
	{printf("bldstr: first arg must be constant\n");
	Fail0; return;}
    if ((a==2) && (get_name(psc_p)[0]=='.') && (get_length(psc_p)==1)) 
	follow(op2) = (word)hreg | LIST_TAG;
    else if (a==0) follow(op2) = op1;
    else {
	op1 = insert(get_name(psc_p), get_length(psc_p), a, &perm);
	/* printf("bldstr: %c %d %d %d\n", *get_name(psc_p),
	   get_length(psc_p), a, op1); */
	op1 = follow(op1); /* returns addr of addr of psc */
	follow(op2) = (word)hreg | CS_TAG;
	*hreg++ = op1;
	}
    pushtrail(op2);
    for (i = 0; i < a; hreg++, i++) make_free(*hreg);
}

b_MKSTR()
{	    /*  (F, T, A) : F is a pointer to the PSC table entry of a function
		symbol f/n.  b_MKSTR creates T, a most general instance of
		a term with principal functor f/n on the heap, and sets the
		variable register 2 points to,  to it; the variable that
		register 3 points to is set to the arity n.

	        At this point, this is used only for decompilation.   */

    int i, arity;
    struct psc_rec *psc_p;
    register word op;
    register pw top;

    op = gregc(1); deref(op);
    if (!isconstr(op)) {
	printf("mkstr: first arg must be a function symbol!\n");
	Fail0; return; }
    psc_p = (struct psc_rec *)(untagged(op));
    arity = get_arity(psc_p);
    op = gregc(2); deref(op);
    if (!isfree(op)) {
	printf("mkstr: second argument must be a variable!\n");
	Fail0; return; }
    if ((arity==2) && (get_name(psc_p)[0]=='.') && (get_length(psc_p)==1)) 
	follow(op) = (word)hreg | LIST_TAG;
    else {
	follow(op) = (word)hreg | CS_TAG;
	*hreg++ = (word)psc_p;
	if (arity > 0)
	    for (i = 0; i < arity; hreg++, i++) make_free(*hreg);
	};
    pushtrail(op);
    op = gregc(3); deref(op);
    follow(op) = makeint(arity);
    pushtrail(op);
}
