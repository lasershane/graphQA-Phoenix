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
#include "globals_gram.h"

/* run-time parameters */
extern int	EdgeBufSize,	/* max number of paths in beam */
		ChartBufSize,	/* max number of paths in beam */
		PeBufSize,	/* number of Val slots for trees  */
		InputBufSize,	/* max words in line of input */
		SlotSeqLen,	/* max number of slots in a sequence */
		ParseBufSize,	/* buffer for parses */
		SeqBufSize,	/* buffer for sequence nodes */
		FrameBufSize;	/* buffer for frame nodes */

/* flags */
extern int	debug,		/* output verbosity level */
		extract,
		PROFILE,
		CHECK_AMBIG,
		IGNORE_OOV;	/* ignore oov words in match if set */

/* debugging options */
extern int	MAX_PATHS;	/* if set, limit max paths expanded */

/* file and dir names */
extern char	*dir,
		*config_file;

/* lists of nets used in parse*/
extern int	*cur_nets,	/* set of nets to be used in this parse */
		num_active,	/* length of cur_nets */
		num_nets,	/* number of nets read in */
		num_nets,
		num_forms,
		max_pri;
extern Gram	*gram;			/* grammar */

extern int	start_token, end_token;

/* parser structures */
extern int	*script,	/* array of word nums for input line */
		script_len;	/* number of words in script */
extern int	*matched_script;	/* words included in parse */
extern char	fun_wrds[];	/* flags indicating function words */
extern int	num_parses;

/* parser buffers */
extern EdgeLink	**chart;	/* chart of matched nets */
extern EdgeLink	*edge_link_buf,	/* buffer of nodes to link edges in lists */
		*edge_link_end,
		*edge_link_ptr;
extern Edge 	*edge_buf,	/* buffer of chart edges */
		*edge_buf_end, 
		*edge_ptr;	/* ptr to next free edge in buf */
extern Edge	**pe_buf,	/* buffer for trees pointed to by edges */
		**pe_buf_end,
		**pe_buf_ptr;

extern char	*print_line;	/* buffer for output */

