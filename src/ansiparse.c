/*
 *  Parses an ansi into a linked list.
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "list.h"
#include "dosfont.h"

#define WRAP 80
#define ESC 0x1b
#define CTRLZ 0x1a
#define MAXCMDLEN 256
#define TABSTOP 8

struct parse_state {
    char attribute;
    int x;
    char *line;
    int saved_x;
    void *saved_pos;
    list_t *list;
};

enum states {
    S_TXT,
    S_CHK_B,
    S_WAIT_LTR,
    S_END
};

enum colors {
    BIN_BLACK,
    BIN_BLUE,
    BIN_GREEN,
    BIN_CYAN,
    BIN_RED,
    BIN_MAGENTA,
    BIN_YELLOW, 
    BIN_WHITE
};


static const char color_conversion_table[] = {
    BIN_BLACK,
    BIN_RED,
    BIN_GREEN,
    BIN_YELLOW,
    BIN_BLUE,
    BIN_MAGENTA,
    BIN_CYAN,
    BIN_WHITE
};

/*
 *  Initializes a line in the list.  Done automagically
 *  by the list manager when this is supplied to its 
 *  constructor...
 */
static void initline(char *ch, int num_bytes) 
{
    int i;
    for (i=0; i<num_bytes; i+=2) {
        ch[i] = ' '; 
        ch[i+1] = BIN_WHITE;
    }
}

/*
 *  Stores a byte in the current position, wrapping if 
 *  necessary, and increments the position.
 */
static void store(struct parse_state *p, char c) 
{
    p->line[p->x*2] = c; 
    p->line[p->x*2 + 1] = p->attribute; 
    p->x++;
    if (p->x == WRAP) { 
        p->x = 0; 
        p->line = list_forward_cat(p->list); 
    }
}

static void get1arg(char *args, int *a1) 
{
    *a1 = atoi(args);
}

static void get2args(char *args, int *a1, int *a2) 
{
    char *tmp;

    *a1 = atoi(args);
    tmp = strchr(args, ';');
    if (!tmp) *a2 = 0;
    else *a2 = atoi(tmp+1);
}

/*
 *  Generates an array of integers for the arguments,
 *  terminated by a -1.
 */
static void getargv(char *args, int **argv) 
{
    char *this_arg;
    int index = 0;
    int num_alloced = 0;
    int num_to_alloc = 2;

    *argv = (int *) malloc(num_to_alloc * sizeof (int));
    num_alloced += num_to_alloc;

    if (!args) {
        **argv = -1;
        return;
    }
    this_arg = args;

    (*argv)[0] = atoi(this_arg);
    index++;
    this_arg = (char *) strchr(this_arg, ';');
    while (this_arg != NULL) {    
        this_arg ++;
        if (index >= num_alloced) {
            num_alloced += num_to_alloc;
            *argv = (int *) realloc(*argv, num_alloced * sizeof (int));
        }
        (*argv)[index] = atoi(this_arg);
        index++;
        this_arg = (char *) strchr(this_arg, ';');
    }     
    if (index >= num_alloced) {
        num_alloced += num_to_alloc;
        *argv = (int *) realloc(*argv, num_alloced * sizeof (int));
    }
    (*argv)[index] = -1;
}

static void move_up(struct parse_state *p, char *args) 
{
    int i;
    int num_move;

    get1arg(args, &num_move);
    if (!num_move) num_move = 1;

    for (i=0; i<num_move; i++) {
        p->line = list_backward_cat(p->list);
    }
}

static void move_down(struct parse_state *p, char *args) {
    int i;
    int num_move;

    get1arg(args, &num_move);
    if (!num_move) num_move = 1;

    for (i=0; i<num_move; i++) {
        p->line = list_forward_cat(p->list);
    }
}

static void move_forward(struct parse_state *p, char *args) 
{
    int num_move;
 
    get1arg(args, &num_move);
    if (!num_move) num_move = 1;

    p->x += num_move;
    if (p->x > WRAP-1) 
        p->x = WRAP-1;
}

static void move_backward(struct parse_state *p, char *args) 
{
    int num_move;

    get1arg(args, &num_move);
    if (!num_move) num_move = 1;

    p->x -= num_move;
    if (p->x < 0) 
        p->x = 0;
}

static void save_position(struct parse_state *p) 
{
    p->saved_x = p->x;
    p->saved_pos = list_get_position(p->list);
}

static void restore_position(struct parse_state *p) 
{
    if (!p->saved_pos)
        return;

    p->x = p->saved_x;
    list_set_position(p->list, p->saved_pos);
    p->line = list_contents(p->list);
}

static void set_position(struct parse_state *p, char *argbuf) 
{
    int pos_x, pos_y;
    int i;

    list_rewind(p->list); 
    get2args(argbuf, &pos_y, &pos_x);
    // adjust if y specified
    if (!pos_y) pos_y = 1;
    if (!pos_x) pos_x = 1;
    for (i=0; i<pos_y; i++) {
        p->line = list_forward_cat(p->list);
    }
    p->x = pos_x - 1;
}

static void clear_screen(struct parse_state *p) 
{
    list_destroy(p->list);
    p->list = list_create(WRAP * 2, initline);
    p->line = list_forward_cat(p->list);
}

static void clear_line(struct parse_state *p) 
{
    int i;
    for(i=p->x; i<WRAP; i++) {
        p->line[i*2] = ' ';
        p->line[i*2+1] = p->attribute; 
    } 
}

static void set_attributes(struct parse_state *p, char *argbuf) 
{
    int i;
    int *args;

    getargv(argbuf, &args);
    for (i=0; args[i] != -1; i++) {
        if (args[i] == 0) {    
            // reset p->attribute 
            p->attribute = BIN_WHITE; 
        } else if (args[i] == 1) {
            // set bold on 
            p->attribute |= 0x08;
        } else if (args[i] == 5) {
            // set blink on
            p->attribute |= 0x80;
        } else if (args[i] >= 30 & args[i] <= 37) {
            // foreground
            p->attribute &= 0xf8;
            p->attribute |= color_conversion_table[ args[i] - 30 ];
        } else if (args[i] >= 40 && args[i] <= 47) {
            p->attribute &= 0x8f;
            // background
            p->attribute |= (color_conversion_table[ args[i] - 40 ] << 4);
        }
    }
    free(args);
}

list_t *ansi_parse(FILE *fp, struct font_info *fi)
{
    struct parse_state p;
    char ch;
    int state = S_TXT;
    char argbuf[ MAXCMDLEN ];
    int arg_index = 0;
    int count, i;

    p.attribute = BIN_WHITE;
    p.x = p.saved_x = 0;
    p.list = list_create(WRAP * 2, initline);
    p.line = list_forward_cat(p.list);
    p.saved_pos = list_get_position(p.list);

    while (!feof(fp)) {
        ch = fgetc(fp);
        switch(state) {
            case S_TXT:    
                switch(ch) {
                    case CTRLZ:
                        state = S_END;
                        break;
                    case ESC:
                        state = S_CHK_B; 
                        break;
                    case '\n':
                        p.line = list_forward_cat(p.list); 
                        p.x = 0;
                        break;
                    case '\r':
                        break;
                    case '\t':
                        count = p.x % TABSTOP;
                        count = TABSTOP - count;
                        for (i=0; i<count; i++) {
                            store(&p, ' '); 
                        } 
                        break;
                    default:
                        store(&p, ch);
                        break;
                }
                break;

            case S_CHK_B:
                if (ch != '[') {
                    store(&p, ESC); 
                    store(&p, ch); 
                    state = S_TXT; 
                } else {
                    state = S_WAIT_LTR;
                }
                break;

            case S_WAIT_LTR:
                if (isalpha(ch)) {
                    argbuf[arg_index] = 0;
                    arg_index = 0;
                    // handle the ansi code
                    switch(ch) {
                        case 'm':              // set attrs
                            set_attributes(&p, argbuf);
                            break;
                        case 'H':              // set position
                        case 'f':
                            set_position(&p, argbuf);
                            break;
                        case 'A':              // move up
                            move_up(&p, argbuf);
                            break;
                        case 'B':              // down
                            move_down(&p, argbuf);
                            break;
                        case 'C':              // right
                            move_forward(&p, argbuf);
                            break;
                        case 'D':              // left
                            move_backward(&p, argbuf);
                            break;
                        case 's':              // save
                            save_position(&p);
                            break;
                        case 'u':              // restore
                            restore_position(&p);
                            break;
                        case 'J':              // clear page
                            clear_screen(&p);
                            break;
                        case 'K':              // clear line
                            clear_line(&p);
                            break;
                        default:
                            break;
                    } 
                    state = S_TXT; 
                } else {
                    argbuf[arg_index] = ch; 
                    if (arg_index != MAXCMDLEN-1)
                        arg_index++;
                }
                break;
            case S_END:
                goto out;
            default:
                state = S_TXT; 
                break;
        }
    }
out:
    return p.list;
}

