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

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "opts.h"
#include "parser.h"

/* Returns postiton of box or BOX_DOESNT_EXIST if doesn't exist */
// long box_find(FILE *f, const char *box);

int box_append(struct box_parser *bp, const char *box, char *items);
int box_read(struct box_parser *bp, const char *box);
int box_remove(struct box_parser *bp, const char *box);
int box_create(struct box_parser *bp, const char *box);
int box_list(struct box_parser *bp);
int box_rewrite(struct box_parser *bp, const char *file);

static int REWRITE;

int main(int argc, char **argv)
{
    struct gengetopt_args_info args_info;
    
    if (cmdline_parser(argc, argv, &args_info) != 0) {
        return 1;
    }
    
    if (argc == args_info.inputs_num) {
        fprintf(stderr, "%s: no option specified\n", argv[0]);
        return 1;
    }
    
    if (args_info.inputs_num != 1) {
        fprintf(stderr, "%s: specify one file to process\n", argv[0]);
        return 1;
    }
        
    struct box_parser *parser = box_parser_create(args_info.inputs[0]);
    if (!parser) {
        return 1;
    }
        
    if (args_info.create_given) {
        box_create(parser, args_info.box_arg);
    }
    
    if (args_info.append_given) {
        box_append(parser, args_info.box_arg, args_info.append_arg); 
    }
    
    if (args_info.read_given) {
        box_read(parser, args_info.box_arg);
    }
    
    if (args_info.delete_given) {
        box_remove(parser, args_info.box_arg);
    }
    
    if (args_info.list_given) {        
        box_list(parser);
    }
    
    if (REWRITE) {
        box_rewrite(parser, args_info.inputs[0]);
    }    
    
    box_parser_free(parser);
    cmdline_parser_free(&args_info);
    return 0;    
}

int box_append(struct box_parser *bp, const char *box, char *items)
{
    const struct box_token *token = NULL;
    while (token = box_parser_token_next(bp)) {
        if (token->type == BOX_TITLE && !strcmp(token->value, box)) {
            const char *item = strtok(items, ",");
            if (item == NULL) {
                cmdline_parser_print_help();
                return 1;
            }
            token = box_parser_token_add(bp, item, BOX_ITEM, token);
            while (item = strtok(NULL, ",")) {
                token = box_parser_token_add(bp, item, BOX_ITEM, token);
            }
        }        
    }
    
    REWRITE = true;
}

int box_read(struct box_parser *bp, const char *box)
{
    const struct box_token *token = NULL;
    while (token = box_parser_token_next(bp)) {
        if (token->type == BOX_TITLE && !strcmp(token->value, box)) {
            while ((token = box_parser_token_next(bp)) && token->type == BOX_ITEM) {
                puts(token->value);                    
            }
            break;                
        }
    }
}

int box_remove(struct box_parser *bp, const char *box)
{
    const struct box_token *token = NULL;
    while (token = box_parser_token_next(bp)) {
        if (token->type == BOX_TITLE && !strcmp(token->value, box)) {        
            while ((token = box_parser_token_remove(bp)) && token->type == BOX_ITEM);
            break;
        }
    }
    
    REWRITE = true;
}

int box_create(struct box_parser *bp, const char *box)
{
    box_parser_token_add(bp, box, BOX_TITLE, bp->end);
    REWRITE = true;
}

int box_list(struct box_parser *bp)
{
    const struct box_token *token = NULL;
    while (token = box_parser_token_next(bp)) {    
        if (token->type == BOX_TITLE) {
            printf("%s\n", token->value);
        }
    }
}

int box_rewrite(struct box_parser *bp, const char *file)
{
    box_parser_write(bp, file);
}
