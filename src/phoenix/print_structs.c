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
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "print_structs.h"
#include "parse.h"
#include "globals_parse.h"

int	pwp;

char *print_edge(Edge *edge, Gram *gram, char *s)
{
    Edge	**cp;
    int		nxt, i;

    if( s ) {
	sprintf(s, "%s ( ", gram->labels[edge->net]);
	s += strlen(s);
    }
    else printf("%s ( ", gram->labels[edge->net]);

    pwp= edge->sw;
    /* print children */
    for( cp= edge->chld, i=0; i < edge->nchld; i++, cp++) {
	for(nxt= (*cp)->sw; pwp < nxt; pwp++) {
	    if( script[pwp] == -1 ) continue;
	    if( script[pwp] == start_token) continue;
	    if( script[pwp] == end_token) continue;
	    if( s ) {
		sprintf(s, "%s ", gram->wrds[ script[pwp] ] );
		s += strlen(s);
	    }
	    else printf("%s ", gram->wrds[ script[pwp] ] );
	}
	s= print_edge( *cp, gram, s );
	pwp= (*cp)->ew+1;
    }
    for( ; pwp <= edge->ew; pwp++) {
	    if( script[pwp] == -1 ) continue;
	    if( script[pwp] == start_token) continue;
	    if( script[pwp] == end_token) continue;
	    if( s ) {
		sprintf(s, "%s ", gram->wrds[ script[pwp] ] );
		s += strlen(s);
	    }
	    else printf("%s ", gram->wrds[ script[pwp] ] );
    }
    if( s ) {sprintf(s, ") "); s += strlen(s); }
    else printf(") ");

    return(s);
}

#ifdef whw
static void check_edge_ambig(int net,int sw,int ew)
{
    Edge	*edge,
		*e;
    int		i, j,
		min_idx,
		count;

    /* find edge in chart */
    if( !(edge= find_edge(net, sw, ew)) ) return;

    min_idx= 100;
    for(e= edge, count=0;
        e && (e->tree[0].ew == ew); e= e->link) {
	count++;
	if( e->idx < min_idx ) min_idx= e->idx; 
    }

    if( count > 1 ) {
	printf("**** Ambiguous Edge  =%d ****\n", count);
	for(e= edge; e && (e->tree[0].ew == ew); e= e->link) {
	    if(e->idx == min_idx ) printf("*");
	    else printf(" ");
	    printf("%s ( ", labels[e->tree[0].net]);
            j= e->tree[0].sw;
	    if( e->idx == 1 ) {
		for( ; j <= e->tree[0].ew; j++ ) {
                    if( script[j] == -1 ) continue;
                    printf("%s ", wrds[ script[j] ] );
                }
	    }
	    else {
		for( ; j < e->tree[1].sw; j++ ) {
                    if( script[j] == -1 ) continue;
                    printf("%s ", wrds[ script[j] ] );
                }
                for(i=1; i < e->idx; i++) {
		    printf("%s ( ", labels[e->tree[i].net]);
	            for( ; j <= e->tree[i].ew; j++ ) {
	                if( script[j] == -1 ) continue;
	                printf("%s ", wrds[ script[j] ] );
	            }
		    printf(") ");
		}
	    }
	    printf(")\n");
	    fflush(stdout);
	}
    }

    /* check sub-edges */
    for(e= edge; e && (e->tree[0].ew == ew); e= e->link) {
        for(i=1; i < e->idx; i++) {
	    check_edge_ambig(e->tree[i].net,
		e->tree[i].sw, e->tree[i].ew);
	}
    }
}

check_ambig()
{
}
#endif


#ifdef whw
void print_highlight(FrameNode **inp) {
  int	i, j, sw, ew;
  char	*wd, unk;

    /* mark matched word positions */
    matched_script[0]= 1;
    for(i=1; i < InputBufSize; i++) matched_script[i] = 0;
    for(i=0; i < n_slots; i++) {
	sw= inp[i]->edge->tree[0].sw;
	ew= inp[i]->edge->tree[0].ew;
	for(j= sw; j <= ew; j++) matched_script[j]= 1;
    }

    /* if unmatched words */
    for(j=0, sw=0; j < script_len; j++) if(matched_script[j]) sw++;
    if( sw < script_len ) {
	wd = print_line;
	for(j = 0, unk = 0; j < script_len; j++, unk =0) {
	    while (*wd && (*wd == '-' || *wd == '!')) {
		unk = 1;
		for( ; *wd && (*wd != ' '); wd++ ) printf("%c", *wd);
	        if (*wd == ' ') printf(" ");
	        while (*wd == ' ') wd++;
                if (!IGNORE_OOV) break;
      	    }

	    if (unk && !IGNORE_OOV) {
      	    }
      	    else if (matched_script[j]) {
		for( ;*wd && *wd != ' '; wd++) printf("%c", *wd);
      	    }
            else {
		printf("\%");
		for( ;*wd && *wd != ' '; wd++) printf("c", toupper(*wd));
	    }
      	    if (*wd == ' ') printf("%c", *wd++);
	}
	for( ;*wd; *wd++) printf("%c", *wd);
    }
    else {
	printf ("%s", print_line);
    }
    printf("\n");
}
#endif

char *print_extracts(Edge *edge, Gram *gram, char *s)
{
    Edge	**cp;
    int		nxt, i;
    char	concept,
		name[LABEL_LEN];

    /* get rid of [ */
    strcpy(name, gram->labels[edge->net]+1);
    if( isupper(name[0]) ) concept= 1;
    else concept= 0;

    /* if flag */
    if( name[0] == '_' ) {
	/* get rid of ] */
	name[strlen(name)-1]= 0;
	if( s ) {
	    sprintf(s, "%s ", name+1 );
	    s += strlen(s);
	}
	else printf("%s ", name+1 );
/*
	pwp= edge->ew+1;
	return(s);
*/
    }

    /* if concept */
    else if( concept ) {
        if( s ) {
	    sprintf(s, "%s.", gram->labels[edge->net]);
	    s += strlen(s);
        }
        else printf("%s.", gram->labels[edge->net]);
    }

    pwp= edge->sw;
    /* if concept leaf, print associated string */
    if( gram->leaf[edge->net] ) {
	/* if flag value */
	if( edge->nchld && (*(gram->labels[edge->chld[0]->net]+1) == '_') ) {
    	    strcpy(name, gram->labels[edge->chld[0]->net]+1);
	    /* get rid of ] */
	    name[strlen(name)-1]= 0;
	    if( s ) {
		/* get rid of _ */
	        sprintf(s, "%s ", name +1 );
	        s += strlen(s);
	    }
	    else printf("%s ", name+1 );
	    pwp= edge->ew+1;
	}
	/* else print string */
	else {
            for( ; pwp <= edge->ew; pwp++) {
	        if( script[pwp] == -1 ) continue;
	        if( script[pwp] == start_token) continue;
	        if( script[pwp] == end_token) continue;
	        if( s ) {
		    sprintf(s, "%s ", gram->wrds[ script[pwp] ] );
		    s += strlen(s);
	        }
	        else printf("%s ", gram->wrds[ script[pwp] ] );
            }
	}
    }
    else {
        /* print children */
        for( cp= edge->chld, i=0; i < edge->nchld; i++, cp++) {
	    for(nxt= (*cp)->sw; pwp < nxt; pwp++) ;
	    s= print_extracts( *cp, gram, s );
	    pwp= (*cp)->ew+1;
        }
    }

    return(s);
}

void print_seq(SeqNode *ss, char **lbl)
{
    SeqNode	*sq, *np;

	for( sq= ss; sq; sq= sq->link) {
	    for( np= sq; np; np= np->back_link) {
		printf("%s ", lbl[np->edge->net]);
	    }
	    printf("\n");
    	}
}

