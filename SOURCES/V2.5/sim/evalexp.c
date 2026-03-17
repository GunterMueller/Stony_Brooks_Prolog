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

#include "sim.h"
#include "aux.h"

extern double floatval();
extern word makefloat();

eval(op, val)
word op, *val;
{
    register pw top;
    struct psc_rec *p;
    char *functor;
    int arity, length, floatp;
    word op1, op2;
    double result;

    switch ((int)(op & 0x3)) {
	case FREE:
	    printf("Error: unbound variable in arithmetic expression\n");
	    Fail0; return -1;
	case CS:
	    floatp = 0;
	    p = get_str_psc(op);
	    functor = get_name(p);
	    arity = get_arity(p);
	    length = get_length(p);
	    if (length == 1)
	      switch (*functor) {
		case '+': switch (arity) {
			case 1:
			    op1 = *(((pw)(untag(op))) + 1); deref(op1);
			    return eval(op1, val);
			case 2: 
			    op1 = *(((pw)(untag(op))) + 1); deref(op1);
			    floatp = eval(op1, &op1);
			    op2 = *(((pw)(untag(op))) + 2); deref(op2);
			    floatp = floatp | eval(op2, &op2);
			    if (floatp >= 0) *val = makenum(numval(op1) + numval(op2));
			    return floatp;
			default: /* switch (arity) */
			    printf("Error: unknown functor %s/%d in arithmetic expression\n", functor, arity);
			    Fail0; return -1;
			};
		case '-':switch (arity) {
			case 1:
			    op1 = *(((pw)(untag(op))) + 1); deref(op1);
			    floatp = eval(op1, &op1);
			    if (floatp >= 0) {
				*val = makenum(-(numval(op1)));
				return floatp;
			    }
			case 2:
			    op1 = *(((pw)(untag(op))) + 1); deref(op1);
			    floatp = eval(op1, &op1);
			    op2 = *(((pw)(untag(op))) + 2); deref(op2);
			    floatp = floatp | eval(op2, &op2);
			    if (floatp >= 0) *val = makenum(numval(op1) - numval(op2));
			    return floatp;
			default: /* switch (arity) */
			    printf("Error: unknown functor %s/%d in arithmetic expression\n", functor, arity);
			    Fail0; return -1;
		    };
		case '*': if (arity == 2) {
			    op1 = *(((pw)(untag(op))) + 1); deref(op1);
			    floatp = eval(op1, &op1);
			    op2 = *(((pw)(untag(op))) + 2); deref(op2);
			    floatp = floatp | eval(op2, &op2);
			    if (floatp >= 0) *val = makenum(numval(op1) * numval(op2));
			    return floatp;
		    }
		else {
		    printf("Error: unknown functor %s/%d in arithmetic expression\n", functor, arity);
		    Fail0; return -1;
		};
		case '/': if (arity == 2) {
			    op1 = *(((pw)(untag(op))) + 1); deref(op1);
			    floatp = eval(op1, &op1);
			    op2 = *(((pw)(untag(op))) + 2); deref(op2);
			    floatp = floatp | eval(op2, &op2);
			    if (numval(op2) == 0) {
				printf("div: division by zero! ... failing execution ...\n");
				return -1;
			    }
			    if (floatp >= 0) {
				result = ((double)(numval(op1)))/((double)(numval(op2))) ;
				if (integral(result)) {
				    floatp = 0;
				    *val = makeint((int)result);
				}
				else {
				    floatp = 1;
				    *val = makefloat(result);
				}
				return floatp;
			    }
		    }
		else {
		    printf("Error: unknown functor %s/%d in arithmetic expression\n", functor, arity);
		    Fail0; return -1;
		};
		case '\\': if (arity == 1) {
			    op1 = *(((pw)(untag(op))) + 1); deref(op1);
			    floatp = eval(op1, &op1);
			    if (floatp == 0) {
			        *val = makeint(~(intval(op1)));
				return floatp;
			    }
			    else {printf("neg: integer required\n"); return -1;}
		    }
		else {
		    printf("Error: unknown functor %s/%d in arithmetic expression\n", functor, arity);
		    Fail0; return -1;
		};
	      }
	      else if (arity == 2) {
		if (!strcmp(functor, "//") && arity == 2) {
			op1 = *(((pw)(untag(op))) + 1); deref(op1);
			floatp = eval(op1, &op1);
			op2 = *(((pw)(untag(op))) + 2); deref(op2);
			floatp = floatp | eval(op2, &op2);
			if (floatp >= 0) *val = makeint(((int)(numval(op1) / numval(op2))));
			return 0;
		    }
		else if (!strcmp(functor, "mod") && arity == 2) {
			op1 = *(((pw)(untag(op))) + 1); deref(op1);
			floatp = eval(op1, &op1);
			op2 = *(((pw)(untag(op))) + 2); deref(op2);
			floatp = floatp | eval(op2, &op2);
			if (floatp == 0) {
			    *val = makeint(intval(op1) % intval(op2));
			    return 0;
			}
			else {printf("mod: integer required\n"); return -1;}
		    }
		else if (!strcmp(functor, "/\\") && arity == 2) {
			op1 = *(((pw)(untag(op))) + 1); deref(op1);
			floatp = eval(op1, &op1);
			op2 = *(((pw)(untag(op))) + 2); deref(op2);
			floatp = floatp | eval(op2, &op2);
			if (floatp == 0) {
			    *val = makeint(intval(op1) & intval(op2));
			    return 0;
			}
			else {printf("and: integer required\n"); Fail0; return -1;}
		    }
		else if (!strcmp(functor, "\\/") && arity == 2) {
			op1 = *(((pw)(untag(op))) + 1); deref(op1);
			floatp = eval(op1, &op1);
			op2 = *(((pw)(untag(op))) + 2); deref(op2);
			floatp = floatp | eval(op2, &op2);
			if (floatp == 0) {
			    *val = makeint(intval(op1) | intval(op2));
			    return 0;
			}
			else {printf("or: integer required\n"); Fail0; return -1;}
		    }
		else if (!strcmp(functor, ">>") && arity == 2) {
			op1 = *(((pw)(untag(op))) + 1); deref(op1);
			floatp = eval(op1, &op1);
			op2 = *(((pw)(untag(op))) + 2); deref(op2);
			floatp = floatp | eval(op2, &op2);
			if (floatp == 0) {
			    *val = makeint(intval(op1) >> intval(op2));
			    return 0;
			}
			else {printf("rshift: integer required\n"); Fail0; return -1;}
		    }
		else if (!strcmp(functor, "<<") && arity == 2) {
			op1 = *(((pw)(untag(op))) + 1); deref(op1);
			floatp = eval(op1, &op1);
			op2 = *(((pw)(untag(op))) + 2); deref(op2);
			floatp = floatp | eval(op2, &op2);
			if (floatp == 0) {
			    *val = makeint(intval(op1) << intval(op2));
			    return 0;
			}
			else {printf("lshift: integer required\n"); Fail0; return -1;}
		    }
		else {printf("Error: unknown functor %s/%d in arithmetic expression\n", functor, arity);
		    Fail0; return -1;
		    }
		}
	     else {printf("Error: unknown functor %s/%d in arithmetic expression\n", functor, arity);
		    Fail0; return -1;
		};
	case NUM:
	    *val = op;
	    return isfloat(op);
	case LIST:
	    Fail0; return -1;
    };
}

