/*
 *  Parses an ansi into a linked list.
 */
#include <stdio.h>
#include "list.h"


#define WRAP 80
#define ESC 0x1b
#define CTRLZ 0x1a
#define MAXCMDLEN 256
#define TABSTOP 8

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

/* These are globals for purposes of easing calls to many routines */
static char attribute = BIN_WHITE;
static int x = 0;
static char *line;
static int saved_x = 0;
static void *saved_pos = NULL;

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
 *  "constructor"...
 */
void initline( char *ch, int num_bytes ) {
  int i;
  for (i=0; i<num_bytes; i+=2 ) {
    ch[i] = ' '; 
    ch[i+1] = BIN_WHITE;
  }
}

/*
 *  Stores a byte in the current position, wrapping if 
 *  necessary, and increments the position.
 */
void store( char c ) {
  line[x*2] = c; 
  line[x*2 + 1] = attribute; 
  x++;
  if ( x == WRAP ) { 
    x = 0; 
    line = listForwardCat(); 
  }
}

void get1arg( char *args, int *a1 ) {
  *a1 = atoi( args );
}

void get2args( char *args, int *a1, int *a2 ) {
  char *tmp;

  *a1 = atoi( args );
  tmp = (char *) strchr( args, ';' );
  if (!tmp) *a2 = 0;
  else *a2 = atoi( tmp+1 );
}

/*
 *  Generates an array of integers for the arguments,
 *  terminated by a -1.
 */
void getargv( char *args, int **argv ) {
  char *this_arg;
  int index = 0;
  int num_alloced = 0;
  int num_to_alloc = 2;

  *argv = (int *) malloc( num_to_alloc * sizeof (int) );
  num_alloced += num_to_alloc;

  if (!args) {
    **argv = -1;
    return;
  }
  this_arg = args;

  (*argv)[0] = atoi( this_arg );
  index++;
  this_arg = (char *) strchr( this_arg, ';' );
  while ( this_arg != NULL ) {  
    this_arg ++;
    if ( index >= num_alloced ) {
      num_alloced += num_to_alloc;
      *argv = (int *) realloc( *argv, num_alloced * sizeof (int) );
    }
    (*argv)[index] = atoi( this_arg );
    index++;
    this_arg = (char *) strchr( this_arg, ';' );
  }   
  if ( index >= num_alloced ) {
    num_alloced += num_to_alloc;
    *argv = (int *) realloc( *argv, num_alloced * sizeof (int) );
  }
  (*argv)[index] = -1;
}

void moveUp( char *args ) {
  int i;
  int num_move;

  get1arg( args, &num_move );
  if (!num_move) num_move = 1;

  #ifdef DEBUG
    printf ("moveUp: %d\n", num_move);
  #endif
  for (i=0; i<num_move; i++) {
    line = listBackwardCat();
  }
}

void moveDown( char *args ) {
  int i;
  int num_move;

  #ifdef DEBUG
    printf ("moveDown: %d\n", num_move);
  #endif

  get1arg( args, &num_move );
  if (!num_move) num_move = 1;

  for (i=0; i<num_move; i++) {
    line = listForwardCat();
  }
}

void moveForward( char *args ) {
  int num_move;
 
  get1arg( args, &num_move );
  if (!num_move) num_move = 1;

  #ifdef DEBUG
    printf ("moveForward: %d\n", num_move);
  #endif

  x += num_move;
  if ( x > WRAP-1 ) x = WRAP-1;
}

void moveBackward( char *args ) {
  int num_move;

  get1arg( args, &num_move );
  if (!num_move) num_move = 1;

  #ifdef DEBUG
    printf ("moveBackward: %d\n", num_move);
  #endif  

  x -= num_move;
  if ( x < 0 ) x = 0;
}

void savePosition() {
  saved_x = x;
  saved_pos = listGetPosition();
}

void restorePosition() {
  x = saved_x;
  listSetPosition( saved_pos );
  line = listContents();
}

void setPosition( char *argbuf ) {
  int pos_x, pos_y;
  int i;

  listRewind(); 
  get2args( argbuf, &pos_y, &pos_x );
  // adjust if y specified
  if (!pos_y) pos_y = 1;
  if (!pos_x) pos_x = 1;
  for ( i=0; i<pos_y; i++ ) {
    line = listForwardCat();
  }
  x = pos_x - 1;
}

void clearScreen() {
  listDestroy();
  listInit( WRAP*2, initline );
  line = listForwardCat();
}

void clearLine() {
  int i;
  for( i=x; i<WRAP; i++ ) {
    line[i*2] = ' ';
    line[i*2+1] = attribute; 
  } 
}

void setAttributes( char *argbuf ) {
  int i;
  int *args;


  getargv( argbuf, &args );
  for ( i=0; args[i] != -1; i++ ) {
    if ( args[i] == 0 ) {  
      // reset attribute 
      attribute = BIN_WHITE; 
    } else if ( args[i] == 1 ) {
      // set bold on 
      attribute |= 0x08;
    } else if ( args[i] == 5 ) {
      // set blink on
      attribute |= 0x80;
    } else if ( args[i] >= 30 & args[i] <= 37 ) {
      // foreground
      attribute &= 0xf8;
      attribute |= color_conversion_table[ args[i] - 30 ];
    } else if ( args[i] >= 40 && args[i] <= 47 ) {
      attribute &= 0x8f;
      // background
      attribute |= ( color_conversion_table[ args[i] - 40 ] << 4 );
    }
  }
  free( args );
}

int ansiParse( char *filename ) {

  FILE *fp;
  char ch;
  int state = S_TXT;
  char argbuf[ MAXCMDLEN ];
  int arg_index = 0;
  int count, i;
  
  listInit( WRAP*2, initline );
  line = listForwardCat();
  saved_pos = listGetPosition();
  fp = fopen( filename, "r" );
  if (!fp) 
    return( -1 );

  while (!feof( fp )) {
    ch = fgetc( fp );
    switch( state ) {
      case S_TXT:  
        switch( ch ) {
          case CTRLZ:
            state = S_END;
            break;
          case ESC:
            state = S_CHK_B; 
            break;
          case '\n':
            line = listForwardCat(); 
            x = 0;
            break;
          case '\r':
            break;
          case '\t':
            count = x % TABSTOP;
            if (count) {
              count = TABSTOP - count;
            }
            for (i=0; i<count; i++) {
              store(' '); 
            } 
            break;
          default:
            store( ch );
            break;
        }
        break;

      case S_CHK_B:
        if ( ch != '[' ) {
          store( ESC); 
          store( ch ); 
          state = S_TXT; 
        } else  {
          state = S_WAIT_LTR;
        }
        break;

      case S_WAIT_LTR:
        if ( isalpha( ch ) ) {
          argbuf[ arg_index ] = 0;
          arg_index = 0;
          // handle the ansi code
          switch( ch ) {
            case 'm':                   // set attrs
              setAttributes (argbuf);
              break;
            case 'H':                   // set position
            case 'f':
              setPosition (argbuf);
              break;
            case 'A':                   // move up
              moveUp (argbuf);
              break;
            case 'B':                   // down
              moveDown (argbuf);
              break;
            case 'C':                   // right
              moveForward (argbuf);
              break;
            case 'D':                   // left
              moveBackward (argbuf);
              break;
            case 's':                   // save
              savePosition();
              break;
            case 'u':                   // restore
              restorePosition();
              break;
            case 'J':                   // clear page
              clearScreen (argbuf);
              break;
            case 'K':                   // clear line
              clearLine (argbuf);
              break;
            default:
              break;
          } 
          state = S_TXT; 
        } else {
          argbuf[ arg_index ] = ch; 
          if (arg_index != MAXCMDLEN-1)
            arg_index++;
        }
        break;
      case S_END:
        return 1; 
      default:
        state = S_TXT; 
        break;
    }
  }
  return 1;
}

