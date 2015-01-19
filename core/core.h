#ifndef _CORE_H_
#define _CORE_H_

void SetGameTitle( const char *newName );
typedef void (*KeyCB)( int symbol, int state );
void SetKeyboardCallback( KeyCB callback );

typedef void (*V_FP_V)();
struct DoAtTime {
	DoAtTime( V_FP_V fp, const char *when );
	DoAtTime *p;
	const char *w;
	V_FP_V f;
};
#define DO_PREBOOT(X) DoAtTime gDoAtTime( X, "preboot" )

#endif

