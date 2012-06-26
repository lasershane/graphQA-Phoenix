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
#ifndef _GRAMMAR_H_
#define _GRAMMAR_H_

#include "linkage.h"

#define MAX_WRDS	0xFFFF	/* max words in lexicon */
#define LABEL_LEN	200	/* max length of labels */
#define LINE_LEN	10000	/* max length line buffer */

/* grammar state structure */
typedef struct gnode
{
	short	n_suc;		/* number of succs */
	short	final;		/* true if final state */
	struct gsucc	*succ;	/* arcs out */
} Gnode;

/* grammar arc structure */
typedef struct gsucc
{
	int	tok;		/* word number, call_netber or 0 */
	int	call_net;	/* 0 or number of net called */
	Gnode	*state;		/* ptr to successor state */
} Gsucc;

typedef struct suc_link
{
	Gsucc	succ;
	struct suc_link *link;
	int nt;
} SucLink;

struct state_set {
	int state;
	char used;
	struct state_set *next;
};
typedef struct state_set set;

typedef struct {
	int tok;
	SucLink *arc;
	char	rw;	/* has been rewritten flag */
} non_term;


typedef unsigned short	Id;

typedef struct formdef {
	Id	n_slot;		/* number of slots in form */
	Id	*slot;		/* net numbers for slots */
} FormDef;

typedef struct gram
{
    FormDef	*form_def;		/* nets used in each form */
    char	**frame_name;		/* frame names */
    int		num_forms;		/* number of frames read in */
    char	**labels;		/* names of nets */
    Gnode	**Nets;			/* pointers to heads of nets */
    int		num_nets;		/* number of nets read in */
    char	**wrds;			/* pointers to strings for words */
    int		num_words;		/* number of words in lexicon */
    int		*node_counts;		/* number of nodes in each net */
    char	*leaf;			/* concept leaf flags */
    unsigned char	*priorities;	/* priorities for nets */
    int		max_pri;		/* max net priority level */
    char	*sym_buf;		/* strings for words and names */
} Gram;

Gram PhxExports *read_grammar(
			char *dir, 
			char *dict_file, 
			char *grammar_file, 
			char *forms_file, 
			char *priority_file);

void PhxExports	read_nets(
			char *net_file,
			Gram *gram, 
			char **sb_start, 
			char *sym_buf_end);

void PhxExports	read_forms(
			char *dir,
			char *forms_file,
			Gram *gram,
			char **sb_start, 
			char *sym_buf_end);

int PhxExports	find_net(char *name, Gram *gram);

#endif
