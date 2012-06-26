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
/* process annotated text input into grammar files
   grammar directory and input file must be specified
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <sys/time.h>
#include <ctype.h>
#include "pconf.h"

#define LABEL_LEN	200	/* max length of labels */
#define LINE_LEN	2000	/* max length line buffer */

int AddLabelledString(char *str);

/* file parameters */
char	nets_file[LABEL_LEN] =		"nets";

/*  Command Line Parameters  */
char	*grammar,
     	*input;

  static config_t conf[] = {
  {"grammar",	"dir containing NETS and *.net files",
       "-grammar", 	STRING,	(caddr_t) &grammar},
  {"input",	"file containing labelled input",
       "-input", 	STRING,	(caddr_t) &input},
  {NULL, NULL, NULL, NOTYPE, NULL}
  };

/******* Global Variables ***********/
FILE	*fp_nets;
char	line[LINE_LEN];

int main(int argc,char **argv)
{
    char *s;
    FILE	*fp_input;
    int left, right;
    char  filename[LABEL_LEN];

    /* set command line parms */
    if (pconf(argc,argv,conf,NULL,NULL,NULL))
	pusage(argv[0],conf),exit(-1);

    /* open nets file */
    sprintf(filename, "%s/%s", grammar, nets_file);
    if( !(fp_nets = fopen(filename, "r")) ) {
	/* create nets file */
	if( !(fp_nets= fopen(filename, "w")) ) {
	    printf("can't open %s\n", filename);
	    exit(-1);
	}
    }


    if( !(fp_input= fopen(input, "r")) ) {
	printf("can't open %s\n", input);
	exit(-1);
    }

    /* process annotated input, one line at a time */
    while(  fgets(line, LINE_LEN-1, fp_input) ) {
	/* if comment */
	if( line[0] == ';' ) return -1;

	/* to uppercase */
	for( s= line; *s; s++) {
	    /* skip net labels */
	    if( *s == '[' ) {
		while( *s && (*s != ']') ) s++;
		s++;
	    }
	    if( islower(*s) )  *s= toupper(*s);
	}

	/* add each slot-level net */
	for(s= line; *s; ) {
	    AddLabelledString(s);

	    /* scan past end of rule */
	    while( *s && (*s != '(') ) s++;
	    if( !*s ) break;
	    for( ++s, left=1, right= 0; *s && (left != right); s++) {
		if( *s == '(' ) left++;
		else if( *s == ')' ) right++;
	    }
	    s++;
	}
    }
    fclose(fp_nets);

    return 0;
}

int AddLabelledString(char *str)
{
    char	name[LABEL_LEN],
		rule[LINE_LEN],
		word[LABEL_LEN],
		fname[LABEL_LEN],
		tmp[LABEL_LEN],
		*f, *t;
    FILE	*fp_gra;

    /* separte string into net name and rule */
    sscanf(str, " %[^ (]%[^\n]", name, rule);

    /* skip spaces */
    for( f= rule; *f && (isspace(*f)); ) f++;
    /* if no rule */
    if( *f != '(' ) return(0);

    /* format rule */
    for( ++f; *f ; ) {
	/* get next token */
	while( *f && (isspace(*f)) ) f++;
	if( !*f || (*f == ')') ) break;
	if( sscanf(f, " %[^ ()]", word) != 1 ) break;

	/* if token is network call */
	if( *word == '[' ) {
	    /* add rule to subnet */
	    AddLabelledString( f );

	    /* flush parenthesized expression */
	    f += strlen(word);
	    while( *f && (isspace(*f)) ) f++;
	    if( *f == '(' ) {
		int left, right;

	        for( t= f+1, left=1, right=0; *t && (left > right); t++ ) {
	            if(*t == '(') left++;
	            if(*t == ')') right++;
		}
		if( left == right ) {
		    strcpy(f, t);
		}
		else {
		    printf("unbalanced parens for token %s\n", word);
		}
	    }
	}
	else {
	    f += strlen(word);
	}
    }
    if( *f == ')' ) {
	f++;
	*f= 0;
    }
    /* rule to lowercase */
    for(f= rule; *f; f++)  if( isupper(*f) )  *f= tolower(*f);

    /* open grammar file */
    sscanf(name, "[%[^]]", tmp);  /* get rid of brackets */
    sprintf(fname, "%s/%s.gra", grammar, tmp);
    if( !(fp_gra= fopen(fname, "a")) ) {
	/* create grammar file */
	if( !(fp_gra= fopen(fname, "w")) ) {
	    printf("can't create %s\n", fname);
	    exit(-1);
	}
	fprintf(fp_gra, "%s\n", name);
    }

    /* add rule to .gra file */
    fprintf(fp_gra, "\t%s\n", rule);
    fclose(fp_gra);

    /* add to nets file */
    fprintf(fp_nets, "%s\n", tmp);

    return 0;
}
