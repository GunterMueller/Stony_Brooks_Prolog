/************************************************************************
*									*
* The SB-Prolog System							*
* Copyright SUNY at Stony Brook, 1986; University of Arizona, 1987	*
*									*
************************************************************************/

#include "sim.h"

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
/* aux.c */

quit(s)
CHAR_PTR s;
{
   printf(s);
   exit(0);
}

/* concatinate s1 and s2 into s3 */

scat(s1, s2, s3)
CHAR_PTR s1, s2, s3;
{
   while (*s1)             /* copy s1 into s3, without the EOS */
      *s3++ = *s1++;
   while (*s3++ = *s2++)   /* add s2 onto s3, including the EOS */
      ;
}
