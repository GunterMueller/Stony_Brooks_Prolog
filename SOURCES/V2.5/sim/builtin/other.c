/************************************************************************
 
* The SB-Prolog System                                                  *
* Copyright SUNY at Stony Brook, 1986; University of Arizona, 1987      *
*                                                                       *
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
 
/* other.c */
 
#include "builtin.h"
#include <errno.h>
#include <sys/types.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
 
#define STR_LIM 256 /* limit on the length of the name of a constant */
#define isalpha(x)  (((x >= 'a') && (x <= 'z')) || ((x >= 'A') && (x <= 'Z')))
#define isdigit(x)  ((x >= '0') && (x <= '9'))
 
#define TRUE		1
#define FALSE		2
#define SING_QUOTE      '\''
#define DOUB_QUOTE      '\"'
#define NEWLINE         '\n'
#define EOLN		'\0'
#define LEFT_PAREN      '\('
 
#define POUND           '\#'
#define DOLLAR          '\$'
#define AMPERSAND       '\&'
#define STAR            '\*'
#define PLUS            '\+'
#define MINUS           '\-'
#define PERIOD          '\.'
#define SLASH           '\/'
#define COLON           '\:'
#define LESSTHAN        '\<'
#define EQUAL           '\='
#define GREATERTHAN     '\>'
#define QUESTIONMARK    '\?'
#define AT              '\@'
#define BACKSLASH       '\\'
#define CARAT           '\^'
#define BGIN_ONE_QTE    '\`'
#define TILDA           '\~'
#define BLANK           ' '
 
extern float floatval();
extern word insert();
extern int d_trace, d_hitrace;
extern word flags[10];
extern int errno;
 
extern word     *memory;   /* heap, local stack   */
extern word     *pspace; /* psc records, instructions, p-names */
extern word     *tstack;
extern word     *local_bottom;
extern word     *heap_bottom;
extern byte     *curr_fence; /* ptr to next free byte in perm space */
extern word *ereg;                /* last activation record       */
extern word *breg;                /* last choice point            */
extern word *hreg;                /* top of heap                  */
extern word *trreg;               /* top of trail stack           */
extern int maxmem, maxpspace, maxtrail;
 
extern byte *curr_fence; /* ptr to next free byte in perm space */
extern byte *max_fence; /* ptr to last+1 free byte in perm space */
 
FILE *curr_in, *curr_out;   /* current input, output streams */
 
typedef union {
                char *name;
                int num;
        } call_args;
 
static call_args call_arg[10];
static char s[256];
 
b_SYSTEM0()  /* r1: a list of int (string) for CShell commands */
{
    register word op1;
    register pw top;
    char s[256];
 
    op1 = gregc(1); deref(op1);
    namestring(get_str_psc(op1), s);
    if (!unify(makeint(system(s)), gregc(2))) {Fail0;}
}
 
 
/* rno is number of register containing list of args 
   This routine converts them into array cal_arg, and 
   returns the number of args */
getgenargs(rno) 
int rno;
{
    int i;
    register word op2, op3;
    register pw top;
    struct psc_rec *ptr;
 
    op2 = gregc(rno); deref(op2);
    i = 1;
    while (!(isnil(op2))) {
        untag(op2);
        op3 = follow(op2);
        deref(op3);
        if (isatom(op3)) {
            ptr = get_str_psc(op3);
            if ( get_etype(ptr) == T_ORDI ) {
                namestring(ptr, s);
                call_arg[i].name = s;
            }
            else if (get_etype(ptr) == T_BUFF) {
                    call_arg[i].name = get_name(ptr);
            }
        } 
        else if (isinteger(op3)) call_arg[i].num = intval(op3);
        else quit("Unknown syscall argument\n");
        op2 += 4;
        deref(op2);
        i++;
    }
    return(i);
}
 
b_SYSCALL()  /* r1: call # ; R2: a list of parameters; R3: returned value */
{
    int n, r;
    register word op1;
    register pw top;
 
    op1 = gregc(1); deref(op1);
    n = intval(op1);  /* syscall number */
    switch (getgenargs(2)) {
        case 1: r = syscall(n); break;
        case 2: r = syscall(n, call_arg[1]); break;
        case 3: r = syscall(n, call_arg[1], call_arg[2]); break;
        case 4: r = syscall(n, call_arg[1], call_arg[2], call_arg[3]); break;
        case 5: r = syscall(n, call_arg[1], call_arg[2], call_arg[3],
                        call_arg[4]); break;
        case 6: r = syscall(n, call_arg[1], call_arg[2], call_arg[3],
                        call_arg[4], call_arg[5]); break;
        case 7: r = syscall(n, call_arg[1], call_arg[2], call_arg[3],
                        call_arg[4], call_arg[5], call_arg[6]); break;
        default: quit("Too many arguments for syscall\n");
                        break;
        }
    if (!unify(gregc(3), makeint(r))) {Fail0;}
}
 
 
b_BROCALL() /*  R1: call #; 
                R2: buffer containing args in 4 byte fields;
                R3: buffer to put return value in. */
{
    struct psc_rec *rptr;
    pw aptr;
    register word op1, op;
    register pw top;
 
    op1 = gregc(1); deref(op1);  /* brocall number */
    op = gregc(2); deref(op); 
    aptr = (pw)get_name(get_str_psc(op)); /* buff with args */
    op = gregc(3); deref(op); rptr = get_str_psc(op); /* buff for result */
    switch ((int)(intval(op1))) {
 
        case 2: *(pw)get_name(rptr) = (word) getenv(*aptr); break;
 
/* Communication subsystem system calls. Have not included byteorder (ntohl,
   ntohs, htonl, htons). Each call is from manual entry 3N,  except 
   getpeername,  which is from 2.
*/
 
/*      case 21: *(pw)get_name(rptr) = (word) gethostent(); break;      */
        case 22: *(pw)get_name(rptr) = (word) gethostbyname(*aptr); break;
        case 23: *(pw)get_name(rptr) = (word) gethostbyaddr(*aptr); break;
        case 24: *(pw)get_name(rptr) = (word) sethostent(*aptr); break;
        case 25: *(pw)get_name(rptr) = (word) endhostent(); break;
 
        case 26: *(pw)get_name(rptr) = (word) getnetent(); break;
        case 27: *(pw)get_name(rptr) = (word) getnetbyname(*aptr); break;
        case 28: *(pw)get_name(rptr) = (word) getnetbyaddr(*aptr); break;
        case 29: *(pw)get_name(rptr) = (word) setnetent(*aptr); break;
        case 30: *(pw)get_name(rptr) = (word) endnetent(); break;
 
        case 31: *(pw)get_name(rptr) = (word) getprotoent(); break;
        case 32: *(pw)get_name(rptr) = (word) getprotobyname(*aptr); break;
        case 33: *(pw)get_name(rptr) = (word) getprotobynumber(*aptr); break;
        case 34: *(pw)get_name(rptr) = (word) setprotoent(*aptr); break;
        case 35: *(pw)get_name(rptr) = (word) endprotoent(); break;
 
        case 36: *(pw)get_name(rptr) = (word) getservent(); break;
        case 37: *(pw)get_name(rptr) = (word) getservbyname(*aptr); break;
        case 38: *(pw)get_name(rptr) = (word) getservbyport(*aptr); break;
        case 39: *(pw)get_name(rptr) = (word) setservent(*aptr); break;
        case 40: *(pw)get_name(rptr) = (word) endservent(); break;
 
/*      case 41: *(pw)get_name(rptr) = (word) inet_addr(*aptr); break; */
        case 42: *(pw)get_name(rptr) = (word) inet_network(*aptr); break;
        case 43: *(pw)get_name(rptr) = (word) inet_ntoa(*aptr); break;
/*      case 44: *(pw)get_name(rptr) = (word) inet_makeaddr(*aptr); break; */
        case 45: *(pw)get_name(rptr) = (word) inet_lnaof(*aptr); break;
        case 46: *(pw)get_name(rptr) = (word) inet_netof(*aptr); break;
 
/*      case 47: *(pw)get_name(rptr) = (word) get_peername(*aptr); break; */
        case 50: *(pw)get_name(rptr) = (word) perror(*aptr); break;
 
        default: printf("Illegal brocall number\n"); Fail0; return;
    }
}
 
b_ERRNO()
{ 
    if (!unify(gregc(1), makeint(errno))) {Fail0;}
}
 
b_CALL()        /* R1: The predicate to be called */
{
    callv_sub();  /* since cpreg has been saved by call "call",
                                should not be saved again, the same as exec */
}
 
b_LOAD()        /* R1: the byte code file to be loaded */
                /* R2: the return code, 0 => success */
{
    register word op1;
    register pw top;
 
    op1 = gregc(1); deref(op1);
    if (!unify(makeint(dyn_loader(get_str_psc(op1))), gregc(2))) {Fail0;}
}
 
b_STATISTICS()
{
    print_statistics();
}
 
b_STATISTICS0()
{
    if (!unify(gregc(1), makeint(maxpspace*sizeof(word)))) {Fail0;}
                            /* max program area */
    if (!unify(gregc(2), makeint((((int)curr_fence-(int)pspace))))) {Fail0;}
                            /* program area in use */
    if (!unify(gregc(3), makeint((hreg-heap_bottom)))) {Fail0;}
                            /* global stack in use */
    if (!unify(gregc(4), makeint((ereg<breg?ereg:breg)-hreg))) {Fail0;}
                            /* stack area (local, global) free */
    if (!unify(gregc(5), makeint(local_bottom-(ereg<breg?ereg:breg)))) {Fail0;}
                            /* local stack in use */
    if (!unify(gregc(6), makeint(maxmem*sizeof(word)))) {Fail0;}
                            /* total stack area size */
    if (!unify(gregc(7), makeint(maxtrail*sizeof(word)))) {Fail0;}
                            /* total trail size */
    if (!unify(gregc(8), makeint(trreg-tstack))) {Fail0;}
}    
 
b_TRACE()
{
    hitrace = 1;
}
 
b_PILTRACE()
{
    trace = 1;
}
 
b_UNTRACE()
{
    hitrace = trace = 0;
}
 
/* b_DETRACE()
{
    hitrace = d_hitrace;
    trace = d_trace;
} */
 
b_SYMTYPE()     /* R1 term, R2 type field of psc-entry of root sym of term */
{
    register word op1;
    register pw top;
 
    op1 = gregc(1);
    typd: switch ((int)(op1&3)) {
        case FREE:
            nderef(op1, typd);
        case LIST:
        case NUM:
            quit("Symtype: illegal first arg");
        case CS:
            if (!unify(makeint(get_etype(get_str_psc(op1))), gregc(2))) 
                {Fail0;}
        }
}
 
b_HASHVAL()     /* R1 Arg, R2 size of hashtab, R3 hashval for this arg  */
{
    register word op1, op2, op3;
    register pw top;
 
    op1 = gregc(1);
    op2 = gregc(2); deref(op2); op2 = intval(op2);
    op3 = gregc(3); deref(op3);
    sotd0: switch((int)(op1&3)) {
        case FREE: nderef(op1, sotd0);
                   printf("Indexing for asserted predicate with var arg\n");
                   Fail0;
        case NUM:
                if (isinteger(op1))
                    op1 = intval(op1);
                else op1 = (int)(floatval(op1));
                break;
        case LIST:
                op1 = *((pw)untagged(list_str));
                break;
        case CS:
                op1 = (word)get_str_psc(op1);
                break;
    }
    if (! unify(op3, makeint(ihash(op1, op2)))) {Fail0;}
}
 
b_FLAGS()  /* R1 contains number of bit to get or set (must be integer);
              R2 contains setting of 0 or 1, 
                    or is variable and setting will be returned */
{
    register word op1, op2, res;
    register pw top;
 
    op1 = gregc(1); deref(op1); op1 = intval(op1);
    op2 = gregc(2); deref(op2); 
    if (isnonvar(op2)) {
        if (op1>9) flags[op1-10] = op2;
        else {
            op2 = intval(op2);
            switch ((int)(op1)) {
                    case 0: trace = op2; break;
                    case 1: hitrace = op2; break;
                    case 2: overflow_f = op2; break;
                    case 3: trace_sta = op2; break;
            }
            call_intercept = hitrace | trace_sta;
        }
    }
    else {  
            if (op1>9) res = flags[op1-10];
            else {
                switch ((int)(op1)) {
                    case 0: res = trace; break;
                    case 1: res = hitrace; break;
                    case 2: res = overflow_f; break;
                    case 3: res = trace_sta; break;
                }
                res = makeint(res);
            }
            follow(op2) = res;
    }
}
 
print_statistics()
{
    pw lstktop;
 
    if (breg < ereg) lstktop = breg;
    else lstktop = ereg - *(cpreg-5);
 
    printf("Maximum available stack size: %d\n", maxmem);
    printf("  Local stack: %d in use, %d max used.\n", 
        local_bottom-lstktop, local_bottom-mlocaltop);
    printf("  Heap stack: %d in use, %d max used.\n", 
        hreg-heap_bottom, mheaptop-heap_bottom);
 
    printf("Permanent space: %d, %d in use.\n", maxpspace,
        ((int) curr_fence - (int) pspace)/4);
 
    printf("Trail stack: %d, %d in use, %d max used.\n", 
        maxtrail, trail_bottom-trreg, trail_bottom-mtrailtop);
}
/* ------------------------------------------------------------------- */
 
b_READNAME()
{		/* Ch, Name, NextCh: reads a sequence of letters, digits
		   dollar signs and underscores, makes the sequence into a
		   constant and inserts it into the PSC table if necessary,
		   and returns a pointer to the PSC entry as Name.  NextCh is
		   the first character read which cannot join this sequence.

		   It is assumed that reg1 contains a character; and that regs
		   2 and 3 are free.  No checking is done here.  This builtin
		   is only supposed to be called from $read_tokens/3 anyway.
		*/

    char pname[STR_LIM], *name, ch;
    char perm = PERM;
    int len = 1;
    int done = 0;
    word ptr;
    register word op;
    register pw top;

    op = gregc(1); deref(op);

    op = intval(op);

    name = pname;
    *(name++) = (char)op;   /* first character in the sequence */

    while (!done && len <= STR_LIM) {
	ch = getc(curr_in);
	if (isalpha(ch) || isdigit(ch) || (ch == '$') || (ch ==  '_')) {
	    *(name++) = ch;
	    len++;
	}
	else {*name = '\0'; done = 1;}
    }

    if (ch == EOF) {
        clearerr(curr_in);
	printf("! unexpected end of file after %s\n", pname);
    };
    if (len > STR_LIM) {
	*name = '\0'; len--;
	printf("*** Name of constant too long: %s\n", pname);
    };

    ptr = insert(pname, len, 0, &perm) | CS_TAG;
    if (!unify(gregc(2), ptr)) {Fail0;}
    op = gregc(3); deref(op);
    follow(op) = makeint((long)ch); pushtrail(op);
    return;
}
