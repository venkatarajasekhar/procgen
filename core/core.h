#ifndef _CORE_H_
#define _CORE_H_

void SetGameTitle( const char *newName );
typedef void (*KeyCB)( int symbol, int state );
void SetKeyboardCallback( KeyCB callback );

#endif

