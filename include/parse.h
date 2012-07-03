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

#ifndef __PARSE_H__
#define __PARSE_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "grammar.h"

#define	PATH_TREE_DEPTH	50	/* max call-nets in single rewrite rule */

/* cells of chart */
typedef struct edge {
    Id			net;
    Id			sw;
    Id			ew;
    Id			score;
    char		nchld;
    struct edge		**chld;
    struct edge		*link;
} Edge;


/* temporary structure used in matching nets to create edges */
typedef struct buf {
	Gnode	*state;		/* current state */
	Id	net;		/* number of top-level net */
	Id	sw;		/* number of top-level net */
	Id	ew;		/* number of top-level net */
	Id	score;		/* number of top-level net */
	char	nchld;
	Edge	*chld[PATH_TREE_DEPTH];	/* pointers to sub-trees (children) */
	int	word_pos;	/* extended flag */
}Path;


#ifdef OLD

/* temporary structure used in matching nets to create edges */
typedef struct buf {
	Gnode	*state;		/* current state */
	int	net;		/* number of top-level net */
	char	tree_idx;
	Val	tree[PATH_TREE_DEPTH];		/* parse tree */
	int	word_pos;	/* extended flag */
}Path;

typedef struct edge {
	Val	*tree;		/* parse tree */
	char	idx;		/* number of elements on tree */
	struct edge *link;
}Edge;
#endif

/* structure for linking edges into chart */
typedef struct edge_link {
	int	sw;
	struct edge_link *link;
	Edge *edge;
}EdgeLink;


typedef struct frame_id {
	unsigned short id;
	unsigned short count;
} Fid;

typedef struct seq_node {
	Edge *edge;
	unsigned short	n_frames;	/* frame count for path */
	unsigned short	n_act;	/* number of active frames for path */
	Fid	*frame_id;
	struct seq_node *back_link;
	struct seq_node *link;
	unsigned short *pri;
} SeqNode;

typedef struct seq_cell {
	Id score;
	Id n_slots;
	Id n_frames;
	SeqNode *link;
} SeqCell;

typedef struct frame_node {
	int n_frames;
	int frame;
	SeqNode *slot;
	struct frame_node *bp;
	struct frame_node *link;
} FrameNode;

extern int	SymBufSize;	/* buffer size to hold char strings */


void PhxExports init_parse(char *dir,char *dict_file, 
		char *grammar_file, char *forms_file,
		char *priority_file);
void PhxExports parse(char *line, Gram *gram);
void PhxExports reset(int num_nets);
void PhxExports config(int argc,char **argv);
void PhxExports print_profile(void);
void PhxExports print_debug(int parse_num, char *out_str, int extract, Gram *gram);
void PhxExports print_parse(int parse_num, char *out_str, int extract, Gram *gram);


extern PhxExports int get_num_nets();
extern PhxExports int get_extract();
extern PhxExports int get_num_parses();
extern PhxExports int get_PROFILE();
extern PhxExports int get_debug();
extern PhxExports Gram* get_gram();
extern PhxExports char *get_dir();

extern PhxExports char	*get_grammar_file();
extern PhxExports char	*get_dict_file();
extern PhxExports char	*get_priority_file();
extern PhxExports char	*get_forms_file();

#ifdef __cplusplus
}  /* extern "C" */
#endif /* __cplusplus */

#endif


