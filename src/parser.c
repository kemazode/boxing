/* This file is part of boxing.
 *
 * boxing is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * boxing is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with boxing.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>

#include "parser.h"

#define BOX_ERROR_PREFIX "PARSE ERROR"

struct box_token *_box_push_token(struct box_parser *bp, struct box_token *n);
enum token_type _box_token_type(const char *str);
char *_box_token_value(const char *str, enum token_type type);

int _box_draw_overlap(int length, FILE *fp);

/* Assume that bt->type == BOX_TITLE.
 *
 * indent - number of symbols needed between "| " and " |" or between "+-" and "-+"
 * so for the last case it seems reasonable to put indent + 2 '-' between both '+' 
 */
size_t	_count_indent_for_box(const struct box_token *bt);

struct box_parser *box_parser_create(const char *file)
{
	struct box_parser *bp = NULL;
	
	FILE *fp = fopen(file, "r+");
	if (!fp) {
		perror(file);
		return NULL;
	}
		
	/* Set memory to zero */
	bp = calloc(1, sizeof(struct box_parser));	  
	
	char buf[BOX_ENTRY_MAX_LENGTH];
	for (int i = 0; fgets(buf, BOX_ENTRY_MAX_LENGTH, fp); ++i) {	
		if (buf[BOX_ENTRY_MAX_LENGTH - 2] != 0 &&
			buf[BOX_ENTRY_MAX_LENGTH - 2] != '\n') {
			fprintf(stderr, "%s: %s: %d: line length >= %d\n", 
					BOX_ERROR_PREFIX, file, i, BOX_ENTRY_MAX_LENGTH);
			box_parser_free(bp);
			return NULL;	  			
		}
		
		enum token_type type = _box_token_type(buf);
		if (type == BOX_UNKNOWN) continue;
		
		struct box_token *bt = calloc(1, sizeof(struct box_token));
		bt->row = i;
		bt->type = type;
		bt->value = _box_token_value(buf, bt->type);
		_box_push_token(bp, bt);
	}
	
	fclose(fp);
	
	return bp;
}

void box_parser_free(struct box_parser *bp)
{
	if (!bp) return;
	
	struct box_token *next = NULL;
	for (struct box_token *bt = bp->start; bt; bt = next) {
		next = bt->next;
		free(bt->value);
		free(bt);
	}
	
	free(bp);
}

const struct box_token *box_parser_rewind(struct box_parser *bp)
{
	return bp->cur = bp->start;
}


int box_parser_write(struct box_parser *bp, const char *file) 
{
	FILE *fp = fopen(file, "w");
	
	for (struct box_token *i = bp->start; i; i = i->next)
		if (i->type == BOX_TITLE) {			   
			size_t indent = _count_indent_for_box(i);
			
			fputc(' ', fp);
			fputs(i->value, fp);
			fputc('\n', fp);
			
			_box_draw_overlap(indent + 2, fp);
			
			while (i->next && i->next->type == BOX_ITEM) {
				i = i->next;
				fputc(BOX_VERTICAL_BORDER, fp);
				fputc(' ', fp);
			
				size_t len = strlen(i->value);
				fputs(i->value, fp);
				
				for (size_t i = 0; i < indent - len; ++i) 
					fputc(' ', fp);
			
				fputc(' ', fp);
				fputc(BOX_VERTICAL_BORDER, fp);				 
				fputc('\n', fp);						  
			}	 
			
			_box_draw_overlap(indent + 2, fp);
			fputc('\n', fp);
		}
			
	fclose(fp);
}

const struct box_token *box_parser_token_next(struct box_parser *bp)
{
	/* invalid ptr */
	if (!bp) return NULL;
	
	/* first call after parse */
	if (!bp->cur) {
		bp->cur = bp->start;
		return bp->cur;
	}
	
	/* end of the list */
	if (!bp->cur->next) return NULL;
	
	return bp->cur = bp->cur->next;
}

const struct box_token *box_parser_token_remove(struct box_parser *bp)
{
	if (!bp || !bp->cur) return NULL;
		
	struct box_token *next = NULL;
	
	if (bp->cur->prev) bp->cur->prev->next = bp->cur->next;
	if (bp->cur->next) {
		bp->cur->next->prev = bp->cur->prev;
		next = bp->cur->next;	 
	} 

	free(bp->cur);	  
	return bp->cur = next;	  
}

const struct box_token *box_parser_token_add(struct box_parser *bp, const char *value, 
					 enum token_type type, const struct box_token *bt)
{
	if (!bp) return NULL;
	
	struct box_token *new_token = calloc(1, sizeof(struct box_token));
	new_token->value = strdup(value);
	new_token->type = type;
	 
	if (!bt) {
		new_token->next = bp->start;
		if (bp->start) bp->start->prev = new_token;
		bp->start = new_token;
		if (!bp->end) bp->end = new_token;
			   
	} else if (bt == bp->end) {
		new_token->prev = bp->end;
		bp->end->next = new_token;
		bp->end = new_token;
		
	} else {	
		/* Discard const qualfier for insert new token into the list */
		struct box_token *bt_nonconst = (struct box_token *) bt;
		
		new_token->next = bt->next;
		new_token->prev = bt_nonconst;
		bt->next->prev = new_token;
		bt_nonconst->next = new_token;				  
	}
	
	return new_token;
}

struct box_token *_box_push_token(struct box_parser *bp, struct box_token *n)
{	 
	if (!bp->end) return bp->end = bp->start = n;
	
	bp->end->next = n;
	n->prev = bp->end;
	return bp->end = n;	   
}

enum token_type _box_token_type(const char *str)
{
	/* Check for correctness */
	if (!str || strlen(str) < 2)  return BOX_UNKNOWN;
	if (str[0] == BOX_VERTICAL_BORDER) return BOX_ITEM;	   
	if (str[0] == BOX_CORNER) return BOX_UNKNOWN;
	if (!isspace(str[1])) return BOX_TITLE;
	
	return BOX_UNKNOWN;
}


/* Assume that str != NULL, strlen(str) >= 2 */
char *_box_token_value(const char *str, enum token_type type)
{
	size_t lmargin = (type == BOX_TITLE)? 1 : 2;
	
	const char *beg = (str += lmargin);
	size_t size = strlen(str);
	
	/* Discard spaces from the end */
	const char *end = str + size - 1;
	
	switch (type) {
	case BOX_TITLE:
		 while (isspace(*end)) --end; 
		 break;		 
	case BOX_ITEM:
		 while(*end != '|' && end != beg) --end;
		 while(isspace(*--end));
		 break;
	}
	
	size_t len = end - beg + 1;
	 
	char *value = malloc(len + 1);
	strncpy(value, beg, len);
	value[len] = '\0';
	
	return value;
}

/* Assume that bt->type == BOX_TITLE */
size_t	_count_indent_for_box(const struct box_token *bt)
{	 
	/* Title always takes 1 character less space */
	size_t maxlen = strlen(bt->value) - 1; 
	while ((bt = bt->next) && bt->type == BOX_ITEM) {
		size_t len = strlen(bt->value);
		maxlen = (len > maxlen)? len : maxlen;
	}
	
	return maxlen;
}

int _box_draw_overlap(int length, FILE *fp)
{
	fputc(BOX_CORNER, fp);
	while (length--) fputc(BOX_HORIZONTAL_BORDER, fp);
	fputc(BOX_CORNER, fp);
	fputc('\n', fp);
}