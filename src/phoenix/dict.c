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
#include <string.h>
#include <ctype.h>

#include "parse.h"

int read_dict(char *dir, char *dict_file, Gram *gram, 
	      char **sb_start, char *sym_buf_end)
{
    FILE	*fp;
    int		idx, num_words;
    char	filename[LABEL_LEN];
    char	*sym_ptr;

    sprintf(filename, "%s/%s", dir, dict_file);
    fp= fopen(filename, "r");
    if( !(fp) ) {
	printf("read_dict: can't open %s\n", filename);
	return(0);
    }

    sym_ptr= *sb_start;
    gram->wrds=(char **)calloc(MAX_WRDS, sizeof(char *));
    /* malloc space for word pointers */
    if( !(gram->wrds)){
	printf("can't allocate space for script buffer\n");
	exit(-1);
    }

    for(num_words=0; fscanf(fp, "%s %d", sym_ptr, &idx) == 2; num_words++) {
	if( (idx<0) || (idx>MAX_WRDS) ) {
	    printf("word number for %s out of range\n", sym_ptr);
	    exit(-1);
	}
	gram->wrds[idx]= sym_ptr;
	sym_ptr += strlen(sym_ptr) +1;
	if( sym_ptr >= sym_buf_end ) {
	    fprintf(stderr, "ERROR: overflow SymBufSize %d\n", SymBufSize);
	    exit(-1);
	}
    }

    fclose(fp);
    *sb_start= sym_ptr;
    gram->num_words= num_words;
    return(num_words);
}

int find_word(char *s, Gram *gram){

    unsigned short key;
    char *c;
    int idx;

    /* use first two chars as hash key */
    c= (char *) &key;
    *c = *s;
    c++;
    *c = *(s+1);

    idx= key;
    for( ; idx < MAX_WRDS; idx++) {
	/* if unknown word */
	if( !gram->wrds[idx] ) return(-1);
	if( !strcmp(gram->wrds[idx], s) ) return(idx);
    }
    return(-1);
}

