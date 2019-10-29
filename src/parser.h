/* This file is part of boxing.
 *
 * boxing is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * boxing is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with boxing.  If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once

#define BOX_NO_ERRORS         0
#define BOX_ENTRY_IS_TOO_LONG 1

#define BOX_ENTRY_MAX_LENGTH 256

#define BOX_CORNER            '+'
#define BOX_HORIZONTAL_BORDER '-'
#define BOX_VERTICAL_BORDER   '|'

enum token_type { BOX_UNKNOWN, BOX_TITLE, BOX_ITEM };

struct box_parser {
    struct box_token *end;
    struct box_token *start;
    struct box_token *cur;
};

struct box_token {
    enum token_type type;
    long row;
    char *value;   
    struct box_token *next;    
    struct box_token *prev;
};

struct box_parser *box_parser_create(const char *file);
void box_parser_free(struct box_parser *bp);

const struct box_token *box_parser_rewind(struct box_parser *bp);

int box_parser_write(struct box_parser *bp, const char *file);

const struct box_token *box_parser_token_next(struct box_parser *bp);
const struct box_token *box_parser_token_remove(struct box_parser *bp);
const struct box_token *box_parser_token_add(struct box_parser *bp, const char *value, 
                         enum token_type type, const struct box_token *bt);

/*
    TODO:
int box_parser_token_modify(struct box_parser *bp, const char *newstr);
int box_parser_seek(struct box_parser *bp, const struct box_token *bt);
*/
