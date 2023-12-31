/**********************************************************************
*  This source code is copyright 1999 by Gus Hartmann & Peter Keller  *
*  It may be distributed under the terms of the GNU General Purpose   *
*  License, version 2 or above; see the file COPYING for more         *
*  information.                                                       *
*                                                                     *
*  $Id: error.c,v 1.8 2000-11-07 05:34:17 hartmann Exp $
*                                                                     *
**********************************************************************/

#include "sweep.h"

//static WINDOW* ErrFrame;
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

                mvwprintw(ErrWin,0,0,"%s",NewErrMsg);

                SweepAlert();
        }
        wnoutrefresh(ErrWin);
        move(0,0);
        noutrefresh();
        return;
}

int InitErrorWin(GameStats* Game)
{
        if ((ErrWin=newwin(1,COLS,LINES-1,0))==NULL)
        {
                return 1;
        }
        wbkgdset(ErrWin, ' ' | COLOR_PAIR(CLR_MSGBAR));
        wclear(ErrWin);
        wnoutrefresh(ErrWin);
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
        return OK;
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
        vsnprintf(NewErrMsg,COLS-1,format,args);
#elif HAVE_VSPRINTF
        vsprintf(NewErrMsg,format,args);
#else
#error "Need either vsnprintf() (preferred) or vsprintf()"
#endif
        va_end(args);
        mvwprintw(ErrWin,0,0,"%s",NewErrMsg);
        wnoutrefresh(ErrWin);
        move(0,0);
        noutrefresh();
        return;
}

void ChangeSweepAlert(int NewAlert)
{
        ErrAlert = NewAlert;
        return;
}
