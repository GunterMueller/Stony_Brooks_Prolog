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
/* aux.h */

#define FREE              0x0
#define CS                0x1
#define NUM               0x2
#define LIST              0x3
#define CS_TAG            0x1
#define NUM_TAG           0x2
#define LIST_TAG          0x3
#define INT_TAG           0x6
#define FLOAT_TAG         0x2
#define EPSILON           0.00001
#define LONGBUFF          65535

/******************************************************************************/

#define FOLLOW(op)        (*(LONG_PTR)(op))     /* return what op points to */
#define TAG(op)           ((op) & 0x3)
#define ISVAR(op)         (TAG(op) == FREE)
#define ISCONSTR(op)      (TAG(op) == CS_TAG)
#define ISNUM(op)         (TAG(op) == NUM_TAG)
#define ISLIST(op)        (TAG(op) == LIST_TAG)
#define ISINTEGER(op)     (((op) & 0x7) == INT_TAG)
#define ISFLOAT(op)       (((op) & 0x7) == FLOAT_TAG)
#define ISNONVAR(op)      (TAG(op) != FREE)     /* must have been derefed */
#define ISFREE(variable)  ((LONG)variable == FOLLOW(variable))
#define ISATOM(op)        (ISCONSTR(op) && (GET_STR_ARITY(op) == 0))
#define ISNIL(op)         ((op) == nil_sym)
#define INTEGRAL(op)      ((LONG)(op) == (op))

#define MAKEINT(op)       ((LONG)((((LONG)(op) << 3) | INT_TAG) & 0x7fffffff))
#define MAKEADD(op)       ((LONG)((((LONG)(op) << 3) | INT_TAG) ))
#define MAKENUM(op)       (floatp ? makefloat(op) : MAKEINT(op))
#define INTVAL(op)        (((LONG)(op) << 1) >> 4)
#define NUMVAL(op)        (ISINTEGER(op) ? INTVAL(op) : floatval(op))
#define ADDVAL(op)        ((LONG)(((LONG)(op) >> 3)  & 0x1fffffff))

#define POS_OVERFLOW(op)  ((LONG)(op) & 0xf8000000)
#define NEG_OVERFLOW(op)  (((LONG)(op) & 0xf8000000) != 0xf8000000)
#define POS_ANS(op1,op2)  (((LONG)(op1)&0x40000000) == ((LONG)(op2)&0x40000000))

#define UNTAG(op)          ((op) &= 0xfffffffc) /* remove the tag from op */
#define UNTAGGED(op)       ((op) & 0xfffffffc)  /* return val of op w/out tag */

#define GET_STR_PSC(op)    ((PSC_REC_PTR)(FOLLOW(UNTAGGED(op))))
#define GET_STR_ARITY(op)  GET_ARITY(GET_STR_PSC(op))
#define GET_STR_LENGTH(op) GET_LENGTH(GET_STR_PSC(op))

#define DEREF(op)        while (ISVAR(op)) {    \
		            top = (LONG_PTR)op; \
                            if (ISFREE(top))    \
			       break;           \
                            op = FOLLOW(top);   \
		         }

#define NDEREF(op,labl)  top = (LONG_PTR)op;  \
			 if (!ISFREE(top)) {  \
			    op = FOLLOW(top); \
			    goto labl;        \
			 }

#define PUSHTRAIL(val)   if (((val) > (LONG)breg) || ((val) < (LONG)hbreg)) { \
			    *trreg-- = val;                                   \
			    if (trreg < tstack)                               \
			       quit("Trail overflow\n");                      \
			 }

#define ENV_SIZE(op)     (*(BYTE_PTR)((LONG)(op) - 5))

#define BUFF_SIZE(ptr)   ( GET_LENGTH(ptr) == LONGBUFF ?                     \
                           5 + ((*(LONG_PTR)(GET_NAME(ptr) - 4) + 3) >> 2) : \
                           4 + ((GET_LENGTH(ptr) + 3) >> 2) )

/******************************************************************************/
/* The following are macros for setting heap values. */

#define MAKE_FREE(type,variable)  (variable) = (type)&(variable)
/* must pass a simple pointer, not an expression */

#define NEW_HEAP_FREE             MAKE_FREE(LONG, *hreg); hreg++
/* make a free variable on the top of the heap */

#define NEW_HEAP_CON(x)           *hreg++ = ((x)|CS_TAG)
/* make a new con node on the heap, with pointer value x, which can
   be any untagged one word type */

#define NEW_HEAP_INT(val)         *hreg++ = MAKEINT(val)
#define NEW_HEAP_FLOAT(val)       *hreg++ = val
/* make a new num node on the heap, with value val */

#define NEW_HEAP_NODE(x)          *hreg++ = x
/* make a new heap node with value x (one word type) */

/******************************************************************************/

/* ! ! ! ! IMPORTANT ! ! ! ! 
 * The following macros are for accessing PSC record fields.  PLEASE use
 * them other than actual field names so that we can change the data
 * structure easily in the future !!!
 *
 * In the following macros, 'ptr' is typed PSC_REC_PTR (struct psc_rec *)
 */

#define GET_ETYPE(ptr)  ((ptr)->entry_type)
#define GET_ARITY(ptr)  ((ptr)->arity)
#define GET_LENGTH(ptr) ((ptr)->length)
#define GET_EP(ptr)     ((ptr)->ep)
#define GET_NAME(ptr)   ((ptr)->nameptr)

#define IS_PRED(psc)    (GET_ETYPE(psc) == T_PRED)
#define IS_DYNA(psc)    (GET_ETYPE(psc) == T_DYNA)
#define IS_ORDI(psc)    (GET_ETYPE(psc) == T_ORDI)
#define IS_BUFF(psc)    (GET_ETYPE(psc) == T_BUFF)

/* macro for computing indexing */
#define GEN_SOT(opcode,arg1,arg2,arg3,ep) \
   *ep++ = opcode;                        \
   *ep++ = arg1;                          \
   *((WORD_PTR)ep)++ = arg2;              \
   *((WORD_PTR)ep)++ = arg3

#define GEN_TRY(opcode,arg1,arg2,ep) \
   *ep++ = opcode;                   \
   *ep++ = arg1;                     \
   *(LONG_PTR)ep = arg2;             \
   ep += 2

#define IHASH(val,size)  ((val & 0x3ffffffc) >> 2 + TAG(val)) % size

struct hrec {
   LONG     l;
   LONG_PTR link;
};

struct hrec indextab[1024];

/******************************************************************************/

#define ERROR(arg) printf ("error %d\n",arg)

#define FAIL1      lpcreg = (WORD_PTR)*(breg + 1)

#define FAIL0      if (hitrace) printf("Fail\n"); pcreg = (WORD_PTR)*(breg + 1)

/******************************************************************************/
