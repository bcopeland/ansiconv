/*
 *  Routine to grab Sauce from a file.  
 */
#include <stdio.h>
#include "sauce.h"

static Sauce s;

Sauce *getSauce( FILE *fp ) {

  long cur_pos;

  cur_pos = ftell( fp );  
  fseek( fp, -128, SEEK_END );

  /* 
   * can't count on Sauce structure to be char aligned, 
   * so this bit reads it piecemeal.  Hate to add all these 
   * numbers without #defines, but they're in sauce.h if it 
   * really is confusing.
   */ 
  fread( s.ID, 1, 5, fp );
  fread( s.Version, 1, 2, fp );
  fread( s.Title, 1, 35, fp );
  fread( s.Author, 1, 20, fp );
  fread( s.Group, 1, 20, fp );
  fread( s.Date, 1, 8, fp );

  // NOTE: not portable.  It'll be backwards on big-endian machines
  fread( &s.FileSize, sizeof( long ), 1, fp );

  fread( &s.DataType, 1, 1, fp );
  fread( &s.FileType, 1, 1, fp );

  // See above warning
  fread( &s.TInfo1, sizeof( short ), 1, fp );
  fread( &s.TInfo2, sizeof( short ), 1, fp );
  fread( &s.TInfo3, sizeof( short ), 1, fp );
  fread( &s.TInfo4, sizeof( short ), 1, fp );

  fread( &s.Comments, 1, 1, fp );
  fread( &s.Flags, 1, 1, fp );

  fseek( fp, cur_pos, SEEK_SET );

  if( strncmp( s.ID, "SAUCE", 5 ) == 0 ) 
    return &s;

  return NULL;
}
