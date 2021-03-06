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
/* process text input, from stdin by default
*/
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "parse.h"
#include "pconf.h"
#include "globals_parse.h"
#include "print_structs.h"

void strip_line(char *line, char *newLine);

/*char	dict_file[LABEL_LEN],
priority_file[LABEL_LEN],
forms_file[LABEL_LEN],
*grammar_file;
*/
/* input line buffer */
static char	line[LINE_LEN];

static char	utt_name[LABEL_LEN];
static int	utt_num;

static char	outbuf[10000],
*out_ptr= outbuf;

int main(int argc,char ** argv)
{
	FILE	*fp;
	char	*s;
	int		i;
	char	*newLine;
	//printf("STARTING\n");

	/* set command line or config file parms */
	config(argc, argv);

	/* read grammar, initialize parser, malloc space, etc */
	init_parse(get_dir(), get_dict_file(), get_grammar_file(), get_forms_file(), get_priority_file());

	/* terminal input */
	//fp= stdin;
	fp = fopen("parse_input.txt", "rt");
	//fprintf(stderr, "READY\n");

	/* for each utterance */
	utt_num = 1;
	fgets(line, LINE_LEN-1, fp);
	//for( utt_num= 1; fgets(line, LINE_LEN-1, fp);  ) {
		/* if comment */
		//if (*line == ';' ) { printf("%s\n", line); continue; }
		/* if comment */
		//if (*line == '#' ) { continue; }
		/* if blank */
		//for(s= line; isspace(*s); s++); if( strlen(s) < 2 ) continue;

		//printf(line);

		out_ptr= outbuf;
		*out_ptr= 0;


		newLine = malloc(2 * strlen(line));

		/* strip out punctuation, comments, etc, to uppercase */
		strip_line(line, newLine);
		//printf("Stripped Line: %s\n",newLine);
		if( !strncmp(line, "QUIT", 4) ) exit(1);

		/** echo the line **/
		if (get_debug() > 2) printf(";;;%d %s %s\n", utt_num, utt_name, line);


		/* assign word strings to slots in frames */
		parse(newLine, get_gram());

		if( get_PROFILE() ) print_profile();

		if( get_debug() ) {
			for(i= 0; i < get_num_parses(); i++ ) print_debug(i, 0, 0, get_gram());
		}
		else {
			printf("block1");
			for(i= 0; i < get_num_parses(); i++ ) {
				// extract parse here
				int j, frame;
				SeqNode **fptr;
				frame = -1;
				
				fptr = parses + (n_slots * i);
				for(j=0; j<n_slots; j++, fptr++) {
					//if starting a new frame
					if((*fptr)->frame_id->id != frame) {
						frame = (*fptr)->frame_id->id;
						if((frame<0)||(frame > get_gram()->num_forms)) {
							fprintf(stderr, "ERROR: frame id out of range %d\n", frame);
							exit(-1);
						}
						printf("%s\n", get_gram()->frame_name[frame]);
						// now print slot tree
					}
					// print slot tree
					printf("slot tree goes here");
				
				}
				// end parse extraction
				print_parse(i, out_ptr, 0, get_gram());
				out_ptr += strlen(out_ptr);
			}
		}

		if( get_num_parses() && get_extract() ) {
			printf("block2");
			for(i= 0; i < get_num_parses(); i++ ) 
				print_parse(i, out_ptr, 0, get_gram());
			out_ptr += strlen(out_ptr);
		}

		/* clear parser temps */
		reset(get_num_nets());
		/* free the line buffer */
		free(newLine);

		printf("%s\n", outbuf);
		utt_num++;
	//}

	return 0;
}

void strip_line(char *line, char *newLine)
{
	char	*from, *to;
	to = newLine;
	for(from = line; ;from++ ) {
		if( !(*from) ) break;

#ifdef SNOR
		/* for SNOR input */
		if( *from == '(' ) {
			strcpy(utt_name, from);
			break;
		}
#endif

		switch(*from) {

			/* filter these out */
#ifndef SNOR
	case '(' :
	case ')' :
	case '[' :
	case ']' :
#endif
	case ':' :
	case ';' :
	case '?' :
	case '!' :
	case '\n' :
		break;

		/* replace with space */
	case ',' :
	case '\\' :
		*to++ = ' ';
		break;


	case '#' :
		for( ++from; *from != '#' && *from; from++);
		if( *from == '#' ) from++;
		break; 

	case '-' :
		/* if partial word, delete word */
		if( isspace( *(from+1) ) ) {
			while( (to != line) && !isspace( *(--to) ) ) ;
			/* replace with space */
			*to++ = ' ';
		}
		else {
			/* copy char */
			*to++ = *from;
		}
		break;

#ifdef SNOR
	case '[' :
		/* skip past comment */
		while( *from != ']' ) {
			if( !*from ) break;
			from++;
		}
		break;
#endif

		/* add space before character */
	case '.' :
	case '\'' :
		*to++ = ' ';
		/* Break intentionally omitted here */

	default:
		/* copy char */
		*to++ = (islower(*from)) ? toupper(*from) : *from;
		}
		if( !from ) break;

	}
	*to= 0;

}

                                                  
