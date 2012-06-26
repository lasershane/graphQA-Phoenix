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
#include <string.h>
#include <stdlib.h>
#include <malloc.h>

#include "parse.h"
#include "pconf.h"
#include "dict.h"
#include "globals_gram.h"

Gram *read_grammar(char *dir, char *dict_file, char *grammar_file, 
		   char *forms_file, char *priority_file)
{
    Gram	*gram;
    char	name[LABEL_LEN];
    char	*sym_ptr, *sym_buf_end;
    int		i,
		num;
    FILE	*fp;
    
    gram  = (Gram *) calloc(1, sizeof(Gram));
    /* malloc structure to hold grammar configuration */
    if( !(gram) ) {
	fprintf(stderr, "ERROR: malloc for grammar failed\n");
	exit(-1);
    }

    /* malloc space for symbol buffer */
    gram->sym_buf=(char *)malloc(SymBufSize * sizeof(char));
    if( !(gram->sym_buf)){
	fprintf(stderr, "ERROR: can't allocate space for script buffer\n");
	exit(-1);
    }
    sym_ptr= gram->sym_buf;
    sym_buf_end= gram->sym_buf + SymBufSize;


    /* read dictionary word strings */
    gram->num_words= read_dict(dir, dict_file, gram, &sym_ptr, sym_buf_end);

    /* read RTNs */
    sprintf(name, "%s/%s", dir, grammar_file);
    read_nets(name, gram, &sym_ptr, sym_buf_end);

    /* read forms and create active set of nets */
    read_forms(dir, forms_file, gram, &sym_ptr, sym_buf_end);

    gram->priorities= (unsigned char *)0;
    gram->max_pri= 0;

    sprintf(name, "%s/%s", dir, priority_file);
    fp= fopen(name, "r");
    if( (fp != NULL) ) {
    gram->priorities= (unsigned char *) malloc(gram->num_nets);
	if( !(gram->priorities) ) {
	    fprintf(stderr, "ERROR: malloc priority failed\n");
	    exit(-1);
	}
	for(i=0; i < gram->num_nets; i++) gram->priorities[i]= (unsigned char)0;
	name[0]='[';
	while( fscanf(fp, "%s %d", name+1, &num) == 2 ) {
	    strcat(name,"]");
	    i= find_net(name, gram);
	    if( i < 0 ) {
		fprintf(stderr, "WARNING: Can't find net %s for priority\n",
				name);
		continue;
	    }
	    gram->priorities[i]= (unsigned char) num;
	    if( num > gram->max_pri) gram->max_pri= num;
	}
	fclose(fp);
    }
    return(gram);
}

void read_nets(char *net_file,Gram *gram, char **sb_start, char *sym_buf_end)
{
    FILE *fp;
    Gnode	*net,
		*gnode;
    Gsucc	*succ;
    char	name[LABEL_LEN],
	        *sym_ptr;
    int		num_nodes,
		net_num,
		num_nets,
		offset,
		i,
		j;
    int		zz,
		val;

    sym_ptr= *sb_start;
    fp = fopen(net_file, "r");
    if( !(fp ))
	quit(-1, "Cannot open grammar file %s\n", net_file);

    /* read number of nets */
    if( fscanf(fp, "Number of Nets= %d", &num_nets) < 1 ) {
	fprintf(stderr,"ERROR: bad format in grammar file %s\n", net_file);
	exit(-1);
    }
    /* alow for net numbers start at 1 */
    num_nets++;
    gram->num_nets= num_nets;

    /* malloc space for net names pointers */
    gram->labels= (char **) calloc(num_nets, sizeof(char *));
    if( !(gram->labels) ) {
	fprintf(stderr, "ERROR: malloc for labels failed\n");
	exit(-1);
    }

    /* malloc space for pointers to nets */
    gram->Nets= (Gnode **) malloc(num_nets * sizeof(Gnode *));
    if( !(gram->Nets) ) {
	fprintf(stderr, "ERROR: malloc for Nets failed\n");
	exit(-1);
    }

    /* malloc space for node counts to nets */
    gram->node_counts= (int *) calloc(num_nets, sizeof(Gnode *));
    if( !(gram->node_counts) ) {
	fprintf(stderr, "ERROR: malloc for node counts failed\n");
	exit(-1);
    }

    /* malloc space for concept leaf flags */
    gram->leaf= (char *)malloc(num_nets * sizeof(char));
    if( !(gram->leaf) ) {
	printf("malloc failed\n");
	exit(-1);
    }

    /* read net name, net number, number of nodes and concept_leaf flag */
    while( fscanf(fp, "%s %d %d %d", name, &net_num, &num_nodes, &val) == 4 ) {
	gram->leaf[net_num]= (char)val;
	/* save node count */
	gram->node_counts[net_num]= num_nodes;

        /* copy net name to labels array */
        strcpy(sym_ptr, name);
        gram->labels[net_num]= sym_ptr;
        sym_ptr += strlen(sym_ptr) +1;
        if( sym_ptr >= sym_buf_end ) {
	    fprintf(stderr, "ERROR: overflow SymBufSize %d\n", SymBufSize);
	    exit(-1);
        }

        /* malloc space for nodes */
        net= (Gnode *) malloc( num_nodes* sizeof(Gnode) );
        if( !(net ) ) {
	    fprintf(stderr, "ERROR: malloc net failed: net %s  num_nodes %d\n",
		name, num_nodes);
	    exit(-1);
        }

	/* save pointer to start node in Nets array */
	gram->Nets[net_num]= net;

        /* read nodes */
        for( i=0, gnode= net; i < num_nodes; i++, gnode++ ) {
	    /* read node */
	    if( fscanf(fp, "%d %hd %hd", &zz,
		&gnode->n_suc, &gnode->final) != 3) {
		fprintf(stderr,"ERROR: failed reading grammar, node %d\n", zz);
		exit(-1);
	    }
	    if( zz != i ) fprintf(stderr,"WARNING: net %s node %d out of seq\n",
				 name, zz);

	    if( !gnode->n_suc ) {
		gnode->succ= 0;
		continue;
	    }

	    /* malloc space for succs */
      succ= (Gsucc *) malloc( gnode->n_suc * sizeof(Gsucc) ) ;
	    if( !(succ) ) {
	        fprintf(stderr, "ERROR: malloc for succ nodes failed\n");
	        exit(-1);
	    }
	    gnode->succ= succ;

	    /* read succs */
	    for(j=0; j < gnode->n_suc; j++, succ++) {
		if( fscanf(fp, "%d %d %d", &succ->tok,
				&succ->call_net, &offset) != 3) {
			fprintf(stderr, "ERROR: failed reading node succs\n");
			exit(-1);
		}
		succ->state= net + offset;
	    }
        }
    }
    fclose(fp);
    *sb_start= sym_ptr;
}

void read_forms(char *dir,char *forms_file,Gram *gram,
		char **sb_start, char *sym_buf_end)
{
    int		slot_num, fn, j;
    int		slot_per_frame;
    int		num_forms;
    char	name[LABEL_LEN],
		line[LINE_LEN],
		*sym_ptr;
    unsigned short	*form_buf;
    FILE	*fp;

    sym_ptr= *sb_start;

    sprintf(line, "%s/%s", dir, forms_file);
    fp= fopen(line, "r");
    if( !(fp) ) {
	fprintf(stderr, "ERROR: can't open %s\n", line);
	exit(-1);
    }

    /* count frames and slots_per_frame */
    slot_per_frame= 0;
    slot_num= 0;
    for(num_forms= 0; fgets(line, LINE_LEN, fp); ) {

	/* if comment */
	if( line[0] == '#' ) continue;

	/* if end of form marker */
	if( line[0] == ';' ) {
	    num_forms++;
	    if( slot_num > slot_per_frame ) slot_per_frame= slot_num;
	    slot_num= 0;
	    continue;
	}

	if( sscanf(line, "%s", name) < 1 ) continue; /* empty line */
	if( !strcmp(name, "FUNCTION:") ) continue; /* frame name */
	if( !strcmp(name, "NETS:") ) continue; /* start of nets section */
	slot_num++;

    }
    gram->num_forms= num_forms;

    /* malloc space for frame templates */
    gram->form_def= (FormDef *) malloc(num_forms * sizeof(FormDef));
    if( !(gram->form_def) ) {
	fprintf(stderr, "ERROR: malloc Form templates failed\n");
	exit(-1);
    }
    /* malloc space for frame names */
    gram->frame_name= (char **) malloc(num_forms * sizeof(char **));
    if( !(gram->frame_name) ) {
	fprintf(stderr, "ERROR: malloc Frame names failed\n");
	exit(-1);
    }
    /* malloc space for temporary form buffer */
    form_buf= (unsigned short *) malloc(slot_per_frame *
		                            sizeof(unsigned short *));
    if( !(form_buf) ) {
	fprintf(stderr, "ERROR: malloc form buffer failed\n");
	exit(-1);
    }

    rewind(fp);
    slot_num= 0;
    for(fn= 0; fgets(line, LINE_LEN, fp); ) {

	/* if comment */
	if( line[0] == '#' ) continue;

	/* if end of form marker */
	if( line[0] == ';' ) {

	    /* copy slot buffer to frame definition */
	    gram->form_def[fn].n_slot= (unsigned short)slot_num;
	    /* malloc space for slot id array */
      gram->form_def[fn].slot= (unsigned short *)
		           malloc(slot_num * sizeof(unsigned short *));
	    if( !(gram->form_def[fn].slot) ) {
	        fprintf(stderr, "ERROR: malloc form def failed\n");
	        exit(-1);
    	    }
	    for(j= 0; j < slot_num; j++)
		gram->form_def[fn].slot[j]= form_buf[j];
	    fn++;
	    slot_num= 0;
	    continue;
	}

	if( sscanf(line, "%s", name) < 1 ) continue;

	if( !strcmp(name, "FUNCTION:") ) {
	    sscanf(line, "%s%s", name, sym_ptr);
	    gram->frame_name[fn]= sym_ptr;
	    sym_ptr += strlen(sym_ptr) +1;
	    if( sym_ptr >= sym_buf_end ) {
		fprintf(stderr, "ERROR: overflow SymBufSize %d\n", SymBufSize);
		exit(-1);
	    }
	    continue;
	}

	/* if start of nets section */
	if( !strcmp(name, "NETS:") ) continue;

	/* look up net number */
	j= find_net(name, gram);
	/* if name of a network */
	if( j > -1 ) {
		/* insert slot for net in form */
		form_buf[slot_num]= (unsigned short)j;
		slot_num++;
	}
	else {
		fprintf(stderr, "WARNING: can't find net %s for form\n", name);
	}
    }

    *sb_start= sym_ptr;
}


int find_net(char *name, Gram *gram)
{
    int i;

    if( !*name ) return(-1);
    for( i=1; i < gram->num_nets; i++ ) {
	if(!strcmp(gram->labels[i], name) ) break;
    }

    if( i < gram->num_nets ) return(i);
    return(-1);

}

