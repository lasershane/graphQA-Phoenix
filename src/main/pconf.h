/*************************************************************************/
/*                                                                       */
/*                     Carnegie Mellon University                        */
/*                      Copyright (c) 1989-2002                          */
/*                        All Rights Reserved.                           */
/*                                                                       */
/*  Permission is hereby granted, free of charge, to use and distribute  */
/*  this software and its documentation without restriction, including   */
/*  without limitation the rights to use, copy, modify, merge, publish,  */
/*  distribute, sublicense, and/or sell copies of this work, and to      */
/*  permit persons to whom this work is furnished to do so, subject to   */
/*  the following conditions:                                            */
/*   1. The code must retain the above copyright notice, this list of    */
/*      conditions and the following disclaimer.                         */
/*   2. Any modifications must be clearly marked as such.                */
/*   3. Original authors' names are not deleted.                         */
/*   4. The authors' names are not used to endorse or promote products   */
/*      derived from this software without specific prior written        */
/*      permission.                                                      */
/*                                                                       */
/*  CARNEGIE MELLON UNIVERSITY AND THE CONTRIBUTORS TO THIS WORK         */
/*  DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING      */
/*  ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT   */
/*  SHALL CARNEGIE MELLON UNIVERSITY NOR THE CONTRIBUTORS BE LIABLE      */
/*  FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES    */
/*  WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN   */
/*  AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,          */
/*  ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF       */
/*  THIS SOFTWARE.                                                       */
/*                                                                       */
/*************************************************************************/
/* PCONF.H
 *------------------------------------------------------------*
 * Fil Alleva and Carnegie Mellon University.
 *------------------------------------------------------------*
 * HISTORY
 */

#ifndef _PCONF_
#define _PCONF_

#include <sys/types.h>
#if WIN32
#include "posixwin32.h"
#endif

#include "linkage.h"

typedef enum {NOTYPE,
	      BYTE, SHORT, INT, LONG,
	      U_BYTE, U_SHORT, U_INT, U_LONG,
	      FLOAT, DOUBLE,
	      BooL, CHAR, STRING,
	      DATA_SRC
} arg_t;

typedef enum {
	SRC_NONE, SRC_HSA, SRC_VQFILE, SRC_CEPFILE, SRC_ADCFILE
} data_src_t;


typedef union _ptr {
    char	*CharP;
    char	*ByteP;
    u_char	*UByteP;
    short	*ShortP;
    u_short	*UShortP;
    int		*IntP;
    u_int	*UIntP;
    long	*LongP;
    u_long	*ULongP;
    float	*FloatP;
    double	*DoubleP;
    int		*BoolP;
    char	**StringP;
    data_src_t	*DataSrcP;

} ptr_t;

typedef struct _config {
	char	*LongName;		/* Long Name */
	char	*Doc;			/* Documentation string */
	char	*swtch;			/* Switch Name */
	arg_t	arg_type;		/* Argument Type */
	caddr_t	var;			/* Pointer to the variable */
} config_t;

typedef struct _InternalConfig {
	char	*LongName;		/* Long Name */
	char	*Doc;			/* Documentation string */
	char	*swtch;			/* Switch Name */
	arg_t	arg_type;		/* Argument Type */
	ptr_t	var;			/* Pointer to the variable */
} Config_t;

int PhxExports pconf (int argc, char **argv, config_t *config_p, 
	   char **display, char **geometry, 	
	   char *(*GetDefault)());
int PhxExports ppconf (int argc,char **argv, config_t *config_p, 
	    char **display,char **geometry, 
	    char *(*GetDefault)(),
	    char last);
int PhxExports fpconf (FILE *config_fp, config_t *config_p, 
	    char **display, char **geometry,
	    char *(*GetDefault)());
void PhxExports pusage (char *prog,config_t *cp);
void PhxExports quit(int err,char *fmt,char *str);

#endif 
