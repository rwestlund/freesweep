/**********************************************************************
*  This source code is copyright 1999 by Gus Hartmann & Peter Keller  *
*  It may be distributed under the terms of the GNU General Purpose   *
*  License, version 2 or above; see the file COPYING for more         *
*  information.                                                       *
*                                                                     *
*  $Id: error.c,v 1.7 1999-08-09 05:25:35 hartmann Exp $
*                                                                     *
**********************************************************************/

#include "sweep.h"

static WINDOW* ErrFrame;
static WINDOW* ErrWin;
/* Although the alert preference is set by the user, SweepAlert() needs to
	not take an argument. So it gets set into a static variable. Gross, but
	effective. */
static int ErrAlert;

/* SweepError() is the method for putting erros into the console window
at the bottom of the screen. Passing a null pointer as the message will
invoke a call to ClearError(). */
void SweepError(char* format, ...)
{
	char NewErrMsg[42];
	va_list args;

	if (format==NULL)
	{
		ClearError();
	}
	else 
	{
		ClearError();

		va_start(args, format);
#if defined HAVE_VSNPRINTF
		vsnprintf(NewErrMsg,41,format,args);
#elif HAVE_VSPRINTF
		vsprintf(NewErrMsg,format,args);
#else
#error "Need either vsnprintf() (preferred) or vsprintf()"
#endif
		va_end(args);

		mvwprintw(ErrWin,0,0,NewErrMsg);

		SweepAlert();
	}
	wnoutrefresh(ErrWin);
	move(0,0);
	noutrefresh();
	return;
}

int InitErrorWin(GameStats* Game)
{
	if ((ErrFrame=newwin(3,42,LINES-4,(COLS/2)-21))==NULL)
	{
		return 1;
	}
	else if ((ErrWin=derwin(ErrFrame,1,40,1,1))==NULL)
	{
		return 1;
	}
	wborder(ErrFrame,CharSet.VLine,CharSet.VLine,CharSet.HLine,CharSet.HLine,
		CharSet.ULCorner, CharSet.URCorner,CharSet.LLCorner,CharSet.LRCorner); 
	wnoutrefresh(ErrWin);
	wnoutrefresh(ErrFrame);
	ErrAlert=Game->Alert;
	return 0;
}

void ClearError()
{
	wmove(ErrWin,0,0);
	wclrtoeol(ErrWin);
	wnoutrefresh(ErrWin);
	move(0,0);
	noutrefresh();
	return;
}

int RedrawErrorWin()
{
	return ((wborder(ErrFrame,CharSet.VLine,CharSet.VLine,CharSet.HLine,
		CharSet.HLine,CharSet.ULCorner,CharSet.URCorner,CharSet.LLCorner,
		CharSet.LRCorner)!=OK)?ERR:wnoutrefresh(ErrFrame));
}

void SweepAlert()
{
	switch (ErrAlert)
	{
		case NO_ALERT:
			break;
		case FLASH:
			flash();
			break;
		case BEEP: default:
			/* No preference gets a beep. */
			beep();
			break;
	}
	return;
}

void SweepMessage(char* format, ...)
{
	char NewErrMsg[42];
	va_list args;

	ClearError();

	if ( format == NULL)
	{
		return;
	}
	
	va_start(args, format);
#if defined HAVE_VSNPRINTF
	vsnprintf(NewErrMsg,41,format,args);
#elif HAVE_VSPRINTF
		vsprintf(NewErrMsg,format,args);
#else
#error "Need either vsnprintf() (preferred) or vsprintf()"
#endif
	va_end(args);
	mvwprintw(ErrWin,0,0,NewErrMsg);
	wnoutrefresh(ErrWin);
	move(0,0);
	noutrefresh();
	return;
}
