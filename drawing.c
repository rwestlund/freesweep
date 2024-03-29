/**********************************************************************
*  This source code is copyright 1999 by Gus Hartmann & Peter Keller  *
*  It may be distributed under the terms of the GNU General Purpose   *
*  License, version 2 or above; see the file COPYING for more         *
*  information.                                                       *
*                                                                     *
*  $Id: drawing.c,v 1.25 2003-10-11 20:50:50 hartmann Exp $
*                                                                     *
**********************************************************************/

#include "sweep.h"
#include <pwd.h>

void StartCurses()
{
#ifdef SWEEP_MOUSE
        mmask_t Mask;
#endif
        initscr();
        if (has_colors())
        {
                start_color();
#ifdef DEBUG_LOG
                fprintf(DebugLog,"The number of color pairs is %d\n",COLOR_PAIRS);
                fprintf(DebugLog,"The number of colors available is %d\n",COLORS);
#endif /* DEBUG_LOG */
        }

        noecho();
        keypad(stdscr, TRUE);
        intrflush(stdscr, TRUE);
        cbreak();
        nonl();
        curs_set(0); // Hide the mouse

#ifdef SWEEP_MOUSE
        Mask=REPORT_MOUSE_POSITION|BUTTON1_CLICKED|BUTTON1_DOUBLE_CLICKED|BUTTON3_CLICKED;
        Mask=mousemask(Mask,NULL);
#endif
        return;
}

void PrintInfo()
{
        WINDOW* InfoWin;
        InfoWin=newwin(6,21,0,(COLS-INFO_W));
        wbkgdset(InfoWin, ' ' | COLOR_PAIR(CLR_INFOBAR));
        wcolor_set(InfoWin,CLR_INFOBAR,NULL);
        wclear(InfoWin);

        wattron(InfoWin,A_BOLD);
        mvwprintw(InfoWin,0,2,"-=- FreeSweep -=-");
        wattroff(InfoWin,A_BOLD);
        mvwprintw(InfoWin,1,2,"by Gus! & Psilord");
        wmove(InfoWin,2,1);
        whline(InfoWin,CharSet.HLine,19);
        mvwprintw(InfoWin,3,2,"Hit \'?\' for help");
        mvwprintw(InfoWin,4,3,CharSet.name);
        wmove(InfoWin,5,1);
        whline(InfoWin,CharSet.HLine,19);

        move(0,0);
        wnoutrefresh(InfoWin);
        delwin(InfoWin);
        return;
}

void AskPrefs(GameStats* Game)
{
        char ValueBuffer[(L_MAX_W+L_MAX_H+3)];
        int Value=0, Status=0, CurrentLine=0;

        /* User input needs to be echoed to the creen at this point. */
        echo();

        CurrentLine=0;

        ValueBuffer[0]=0;
        /* Ask user for grid height. */
        mvprintw(CurrentLine,0,"Enter the grid height [%d]:",Game->Height);
        while (Status<=0)
        {
                if (Status<0)
                {
                        beep();
                        mvprintw(LINES-1,0,"Invalid entry for Height.");
                        mvclrtoeol(CurrentLine,(26+L_MAX_H));
                }
#if defined HAVE_LIBNCURSES
                mvgetnstr(CurrentLine,(26+L_MAX_H),ValueBuffer,L_MAX_H);
#else
                mvgetnstr(CurrentLine,(26+L_MAX_H),ValueBuffer,L_MAX_H+2);
#endif /* HAVE_LIBNCURSES */
                refresh();
                if (ValueBuffer[0]==0)
                {
                        Status=1;
                        mvprintw(CurrentLine,(26+L_MAX_H),"%d",Game->Height);
                }
                /* Maybe put help here? */
                else
                {
                        Value=atoi(ValueBuffer);
                        if (CheckHeight(Value)>0)
                        {
                                Game->Height=Value;
                                Status=1;
                        }
                        else
                        {
                                Status=-1;
                        }
                }
        }
        mvclrtoeol(LINES-1,0);
        Status=0;
        ValueBuffer[0]=0;
        CurrentLine++;

        /* Now for width. */
        mvprintw(CurrentLine,0,"Enter the grid width [%d]:",Game->Width);
        while (Status<=0)
        {
                if (Status<0)
                {
                        beep();
                        mvprintw(LINES-1,0,"Invalid entry for Width.");
                        mvclrtoeol(CurrentLine,(25+L_MAX_W));
                }
#if defined HAVE_LIBNCURSES
                mvgetnstr(CurrentLine,(25+L_MAX_W),ValueBuffer,L_MAX_W);
#else
                mvgetnstr(CurrentLine,(25+L_MAX_W),ValueBuffer,L_MAX_W+2);
#endif /* HAVE_LIBNCURSES */
                refresh();
                if (ValueBuffer[0]==0)
                {
                        Status=1;
                        mvprintw(CurrentLine,(25+L_MAX_W),"%d",Game->Width);
                }
                else
                {
                        Value=atoi(ValueBuffer);
                        if (CheckWidth(Value)>0)
                        {
                                Game->Width=Value;
                                Status=1;
                        }
                        else
                        {
                                Status=-1;
                        }
                }
        }
        mvclrtoeol(LINES-1,0);
        Status=0;
        ValueBuffer[0]=0;
        CurrentLine++;

        /* Now for number of mines or percentage. */
        mvprintw(CurrentLine,0,"Enter the number or percentage of mines [");
        if (Game->NumMines==0)
        {
                printw("%d%%]:",Game->Percent);
        }
        else
        {
                printw("%d]:",Game->NumMines);
        }
        while (Status<=0)
        {
                if (Status<0)
                {
                        beep();
                        mvprintw(LINES-1,0,"Invalid entry for Mines.");
                        mvclrtoeol(CurrentLine,(41+L_MAX_H+L_MAX_W));
                }
#if defined HAVE_LIBNCURSES
                mvgetnstr(CurrentLine,(41+L_MAX_H+L_MAX_W),ValueBuffer,(L_MAX_H+L_MAX_W));
#else
                mvgetnstr(CurrentLine,(41+L_MAX_H+L_MAX_W),ValueBuffer,(L_MAX_H+L_MAX_W)+2);
#endif /* HAVE_LIBNCURSES */
                refresh();
                /* If they accept the default. */
                if (ValueBuffer[0]==0)
                {
                        if (Game->NumMines==0)
                        {
                                mvprintw(CurrentLine,(41+L_MAX_H+L_MAX_W),"%d%%",Game->Percent);
                                Status=1;
                        }
                        else
                        {
                                /* Since height and width may have changed, check the */
                                /* default number of mines to make sure it's still valid. */
                                if (CheckNumMines(Game->NumMines,Game->Height,Game->Width)>0)
                                {
                                        mvprintw(CurrentLine,(41+L_MAX_H+L_MAX_W),"%d",Game->NumMines);
                                        Status=1;
                                }
                                else
                                {
                                        Status=-1;
                                }
                        }
                }
                else
                {
                        Value=atoi(ValueBuffer);
                        if (strchr(ValueBuffer,'%')==0)
                        {
                                /* The value is an actual number. */
                                if (CheckNumMines(Value,Game->Height,Game->Width)>0)
                                {
                                        Game->NumMines=Value;
                                        Game->Percent=0;
                                        Status=1;
                                }
                                else
                                {
                                        Status=-1;
                                }
                        }
                        else
                        {
                                /* The value is a percent. */
                                Game->NumMines=0;
                                if (CheckPercent(Value)>0)
                                {
                                        Game->Percent=Value;
                                        Game->NumMines=0;
                                        Status=1;
                                }
                                else
                                {
                                        Status=-1;
                                }
                        }
                }
        }
        mvclrtoeol(LINES-1,0);
        Status=0;
        ValueBuffer[0]=0;
        CurrentLine++;

        /* For the Yes-or-no questions, user input is not echoed. */
        noecho();

        /* Now for the default starting behavior. */
        mvprintw(CurrentLine,0,"Use FastStart mode for new games? [");
        if (Game->Fast>=1)
        {
                printw("Y/n]:");
        }
        else
        {
                printw("y/N]:");
        }
        while (Status<=0)
        {
                if (Status<0)
                {
                        beep();
                        mvprintw(LINES-1,0,"Invalid entry for FastStart mode.");
                        mvclrtoeol(CurrentLine,41);
                }
                ValueBuffer[0]=mvgetch(CurrentLine,41);
                refresh();
                switch(ValueBuffer[0])
                {
                        case '\n': case '\r':
                                Status=1;
                                if (Game->Fast==1)
                                {
                                        mvprintw(CurrentLine,41,"Yes");
                                }
                                else
                                {
                                        mvprintw(CurrentLine,41,"No");
                                }
                                break;

                        case 'n': case 'N':
                                Game->Fast=0;
                                mvprintw(CurrentLine,41,"No");
                                Status=1;
                                break;

                        case 'y': case 'Y':
                                Game->Fast=1;
                                mvprintw(CurrentLine,41,"Yes");
                                Status=1;
                                break;

                        default:
#ifdef DEBUG_LOG
                                fprintf(DebugLog, "Unknown character: %c\n", ValueBuffer[0]);
#endif
                                Status=-1;
                                break;
                }
        }
        mvclrtoeol(LINES-1,0);
        Status=0;

        ValueBuffer[0]=0;
        CurrentLine++;

        /* Now ask about the alert prefs. */
        mvprintw(CurrentLine,0,"Use Beep/Flash/No alert? [");
        switch (Game->Alert)
        {
                case BEEP: default:
                        printw("B/f/n]:");
                        break;
                case FLASH:
                        printw("b/F/n]:");
                        break;
                case NO_ALERT:
                        printw("b/f/N]:");
                        break;
        }

        while (Status<=0)
        {
                if (Status<0)
                {
                        beep();
                        mvprintw(LINES-1,0,"Invalid entry for Alert mode.");
                        mvclrtoeol(CurrentLine,41);
                }
                ValueBuffer[0]=mvgetch(CurrentLine,34);
                refresh();
                switch(ValueBuffer[0])
                {
                        case '\n': case '\r':
                                Status=1;
                                switch (Game->Alert)
                                {
                                        case BEEP: default:
                                                mvprintw(CurrentLine,34,"Beep");
                                                break;
                                        case FLASH:
                                                mvprintw(CurrentLine,34,"Flash");
                                                break;
                                        case NO_ALERT:
                                                mvprintw(CurrentLine,34,"None");
                                                break;
                                }
                                break;

                        case 'b': case 'B':
                                Game->Alert=BEEP;
                                mvprintw(CurrentLine,34,"Beep");
                                Status=1;
                                break;

                        case 'f': case 'F':
                                Game->Alert=FLASH;
                                mvprintw(CurrentLine,34,"Flash");
                                Status=1;
                                break;

                        case 'n': case 'N':
                                Game->Alert=NO_ALERT;
                                mvprintw(CurrentLine,34,"None");
                                Status=1;
                                break;

                        default:
#ifdef DEBUG_LOG
                                fprintf(DebugLog, "Unknown character: %c\n", ValueBuffer[0]);
#endif
                                Status=-1;
                                break;
                }
        }
        mvclrtoeol(LINES-1,0);
        Status=0;

        ValueBuffer[0]=0;
        CurrentLine++;

        /* Ask about saving these prefs. */
        mvprintw(CurrentLine,0,"Save these preferences? [Y/n]:");
        while (Status<=0)
        {
                if (Status<0)
                {
                        beep();
                        mvprintw(LINES-1,0,"Invalid response for saving preferences.");
                        mvclrtoeol(CurrentLine,31);
                }
                ValueBuffer[0]=mvgetch(CurrentLine,31);
                refresh();

                switch (ValueBuffer[0])
                {
                        case '\n': case '\r':
                                Status=1;
                                mvprintw(CurrentLine,31,"Yes");
                                WritePrefsFile(Game);
                                break;

                        case 'n': case 'N':
                                mvprintw(CurrentLine,31,"No");
                                Status=1;
                                break;

                        case 'y': case 'Y':
                                mvprintw(CurrentLine,31,"Yes");
                                WritePrefsFile(Game);
                                Status=1;
                                break;

                        default:
#ifdef DEBUG_LOG
                                fprintf(DebugLog, "Unknown character: %c\n", ValueBuffer[0]);
#endif
                                Status=-1;
                }
        }
        mvclrtoeol(LINES-1,0);

        /* Do one last refresh. */
        refresh();
        return;
}

void Help()
{
        WINDOW* HelpWin;
        int Input=0, CurrentLine=0, LinesLeft=0, CurrentY=0;
#define HELP_MESSAGES 13
        char* Messages[]=
        {
                "Arrow keys and vi-style movement keys move the cursor.",
                "The space bar exposes a square.",
                "\'f\' flags a space as a mine.",
                "\'r\' redraws the screen.",
                "\'0\' moves the cursor to the upper left corner.",
                "\'$\' moves the cursor to the lower right corner.",
                "\'c\' centers the cursor.",
                "\'.\' repeats the last command.",
                "\'b\' show the best times.",
                "\'?\' displays this help screen.",
                "\'g\' displays the GNU General Public License.",
                "\'n\' starts a new game.",
                "\'x\' prompts for new game parameters.",
                "\'q\' quits the game.",
                "Any non-zero number multiplies the next action."
        };

        if ((HelpWin=newwin(0,0,0,0))==NULL)
        {
                perror("Help::newwin");
                exit(EXIT_FAILURE);
        }
        wborder(HelpWin,CharSet.VLine,CharSet.VLine,CharSet.HLine,CharSet.HLine,CharSet.ULCorner,CharSet.URCorner,CharSet.LLCorner,CharSet.LRCorner);
        mvwprintw(HelpWin,1,2,"Time out - Freesweep Help");
        mvwhline(HelpWin,2,1,CharSet.HLine,COLS-2);
        mvwprintw(HelpWin,3,2,"Useful Keys:");
        mvwprintw(HelpWin,LINES-1,1,"--Press \'q\' to quit, space for more, any other key to continue.--");

        /* Be sure to update this to account for all of the error messages. */
        LinesLeft=HELP_MESSAGES;
        while (LinesLeft> (LINES - 6))
        {
                CurrentY=4;
                while (CurrentY< (LINES -2 ))
                {
                        mvwprintw(HelpWin,CurrentY++,8,"%s",Messages[CurrentLine++]);
                        LinesLeft--;
                }
                /* Now get a keystroke to continue. */
                wmove(HelpWin,0,0);
                wrefresh(HelpWin);
                Input=wgetch(HelpWin);

                switch (Input)
                {
                        case 'q':
                                clear();
                                refresh();
                                endwin();
                                exit(EXIT_SUCCESS);
                                break;

                        case ' ':
                                wmove(HelpWin,3,0);
                                wclrtobot(HelpWin);
                                wborder(HelpWin,CharSet.VLine,CharSet.VLine,CharSet.HLine,CharSet.HLine,CharSet.ULCorner,CharSet.URCorner,CharSet.LLCorner,CharSet.LRCorner);
                                mvwprintw(HelpWin,LINES-1,1,"--Press \'q\' to quit, space for more, any other key to continue.--");
                                wrefresh(HelpWin);
                                break;

                        default:
                                wclear(HelpWin);
                                delwin(HelpWin);
                                clear();
                                noutrefresh();
                                return;
                                break;
                }
        }
        /* Now print the last few lines */
        wborder(HelpWin,CharSet.VLine,CharSet.VLine,CharSet.HLine,CharSet.HLine,CharSet.ULCorner,CharSet.URCorner,CharSet.LLCorner,CharSet.LRCorner);
        mvwprintw(HelpWin,LINES-1,2," Press \'q\' to quit, or any other key to continue. ");
        CurrentY=4;
        while (LinesLeft > 0)
        {
                mvwprintw(HelpWin,CurrentY++,8,"%s",Messages[CurrentLine++]);
                LinesLeft--;
        }
        wmove(HelpWin,0,0);
        wrefresh(HelpWin);
        Input=wgetch(HelpWin);

        switch (Input)
        {
                case 'q':
                        clear();
                        refresh();
                        endwin();
                        exit(EXIT_SUCCESS);
                        break;

                default:
                        wclear(HelpWin);
                        delwin(HelpWin);
                        clear();
                        noutrefresh();
                        return;
                        break;
        }
        return;
}

int DrawBoard(GameStats* Game)
{
        int CoordX=0, CoordY=0, HViewable=0, VViewable=0;
        unsigned char CellVal;

        VViewable=((Game->LargeBoardY!=0)?(LINES-6):(Game->Height));
        HViewable=((Game->LargeBoardX!=0)?((COLS-INFO_W-2)/3):(Game->Width));

        for (CoordY=Game->FocusY;CoordY<(Game->FocusY+VViewable);CoordY++)
        {
                for (CoordX=Game->FocusX;CoordX<(Game->FocusX+HViewable);CoordX++)
                {
                        wmove(Game->Board,(CoordY-Game->FocusY),3*(CoordX-Game->FocusX)+1);
                        CellVal=0;
                        GetMine(CoordX,CoordY,CellVal);
                        switch (CellVal)
                        {
                                case UNKNOWN:
                                        if (has_colors() == TRUE)
                                        {
                                                wcolor_set(Game->Board,CLR_UNKNOWN,NULL);
                                        }
                                        xwaddch(Game->Board,&CharSet,CharSet.Unknown);
                                        if (has_colors() == TRUE)
                                        {
                                                wcolor_set(Game->Board,CLR_NORMAL,NULL);
                                        }
                                        break;
                                case EMPTY:
                                        if (has_colors() == TRUE)
                                        {
                                                wcolor_set(Game->Board,CLR_SPACE,NULL);
                                        }
                                        xwaddch(Game->Board,&CharSet,CharSet.Space);
                                        if (has_colors() == TRUE)
                                        {
                                                wcolor_set(Game->Board,CLR_NORMAL,NULL);
                                        }
                                        break;
                                case 1:
                                        if (has_colors() == TRUE)
                                        {
                                                wcolor_set(Game->Board,CLR_VAL1,NULL);
                                        }
                                        waddch(Game->Board,(CellVal)+'0');
                                        if (has_colors() == TRUE)
                                        {
                                                wcolor_set(Game->Board,CLR_NORMAL,NULL);
                                        }
                                        break;
                                case 2:
                                        if (has_colors() == TRUE)
                                        {
                                                wcolor_set(Game->Board,CLR_VAL2,NULL);
                                        }
                                        waddch(Game->Board,(CellVal)+'0');
                                        if (has_colors() == TRUE)
                                        {
                                                wcolor_set(Game->Board,CLR_NORMAL,NULL);
                                        }
                                        break;
                                case 3:
                                        if (has_colors() == TRUE)
                                        {
                                                wcolor_set(Game->Board,4,NULL);
                                        }
                                        waddch(Game->Board,(CellVal)+'0');
                                        if (has_colors() == TRUE)
                                        {
                                                wcolor_set(Game->Board,1,NULL);
                                        }
                                        break;
                                case 4:
                                        if (has_colors() == TRUE)
                                        {
                                                wcolor_set(Game->Board,5,NULL);
                                        }
                                        waddch(Game->Board,(CellVal)+'0');
                                        if (has_colors() == TRUE)
                                        {
                                                wcolor_set(Game->Board,1,NULL);
                                        }
                                        break;
                                case 5:
                                        if (has_colors() == TRUE)
                                        {
                                                wcolor_set(Game->Board,6,NULL);
                                        }
                                        waddch(Game->Board,(CellVal)+'0');
                                        if (has_colors() == TRUE)
                                        {
                                                wcolor_set(Game->Board,1,NULL);
                                        }
                                        break;
                                case 6:
                                        if (has_colors() == TRUE)
                                        {
                                                wcolor_set(Game->Board,6,NULL);
                                        }
                                        waddch(Game->Board,(CellVal)+'0');
                                        if (has_colors() == TRUE)
                                        {
                                                wcolor_set(Game->Board,1,NULL);
                                        }
                                        break;
                                case 7:
                                        if (has_colors() == TRUE)
                                        {
                                                wcolor_set(Game->Board,CLR_VAL7,NULL);
                                        }
                                        waddch(Game->Board,(CellVal)+'0');
                                        if (has_colors() == TRUE)
                                        {
                                                wcolor_set(Game->Board,CLR_NORMAL,NULL);
                                        }
                                        break;
                                case 8:
                                        if (has_colors() == TRUE)
                                        {
                                                wcolor_set(Game->Board,CLR_VAL8,NULL);
                                        }
                                        waddch(Game->Board,(CellVal)+'0');
                                        if (has_colors() == TRUE)
                                        {
                                                wcolor_set(Game->Board,CLR_NORMAL,NULL);
                                        }
                                        break;
                                case MINE:
                                        if (Game->Status == LOSE)
                                        {
                                                if (has_colors() == TRUE)
                                                {
                                                        wcolor_set(Game->Board,CLR_SHOWMINE,NULL);
                                                }
                                                xwaddch(Game->Board,&CharSet,CharSet.ShowMine);
                                                if (has_colors() == TRUE)
                                                {
                                                        wcolor_set(Game->Board,CLR_NORMAL,NULL);
                                                }
                                        }
                                        else
                                        {
                                                if (has_colors() == TRUE)
                                                {
                                                        wcolor_set(Game->Board,CLR_MINE,NULL);
                                                }
                                                xwaddch(Game->Board,&CharSet,CharSet.Mine);
                                                if (has_colors() == TRUE)
                                                {
                                                        wcolor_set(Game->Board,CLR_NORMAL,NULL);
                                                }
                                        }
                                        break;
                                case MARKED:
                                        if (has_colors() == TRUE)
                                        {
                                                wcolor_set(Game->Board,CLR_MARK,NULL);
                                        }
                                        xwaddch(Game->Board,&CharSet,CharSet.Mark);
                                        if (has_colors() == TRUE)
                                        {
                                                wcolor_set(Game->Board,CLR_NORMAL,NULL);
                                        }
                                        break;
                                case BAD_MARK:
                                        if (Game->Status == LOSE)
                                        {
                                                if (has_colors() == TRUE)
                                                {
                                                        wcolor_set(Game->Board,CLR_SHOWFALSEMARK,NULL);
                                                }
                                                xwaddch(Game->Board,&CharSet,CharSet.ShowFalseMark);
                                                if (has_colors() == TRUE)
                                                {
                                                        wcolor_set(Game->Board,CLR_NORMAL,NULL);
                                                }
                                        }
                                        else
                                        {
                                                if (has_colors() == TRUE)
                                                {
                                                        wcolor_set(Game->Board,CLR_FALSEMARK,NULL);
                                                }
                                                xwaddch(Game->Board,&CharSet,CharSet.FalseMark);
                                                if (has_colors() == TRUE)
                                                {
                                                        wcolor_set(Game->Board,CLR_NORMAL,NULL);
                                                }
                                        }
                                        break;
                                case DETONATED:
                                        if (has_colors() == TRUE)
                                        {
                                                wcolor_set(Game->Board,CLR_DETONATED,NULL);
                                        }
                                        xwaddch(Game->Board,&CharSet,CharSet.Bombed);
                                        if (has_colors() == TRUE)
                                        {
                                                wcolor_set(Game->Board,CLR_NORMAL,NULL);
                                        }
                                        break;
                                default:
                                        break;
                        }
                }
        }
        wmove(Game->Board,0,0);

        wnoutrefresh(Game->Board);
        return 0;
}

/* PrintBestTimes() opens
   each entry in a new window, BestTimesWin, which is deleted afterwards.
*/
void PrintBestTimes(char* Filename)
{
        int height, width, Input = 0;
        WINDOW* BestTimesWin;
        struct BestFileDesc *bfd = NULL;
        unsigned int i;
        char *pathname;
        time_t enttime;

        if ((BestTimesWin = newwin(0,0,0,0)) == NULL)
        {
                perror("PrintHighs::AllocWin");
                exit(EXIT_FAILURE);
        }
        getmaxyx(BestTimesWin, height, width);

        nodelay(BestTimesWin, FALSE);
        mvwprintw(BestTimesWin, 1, 2,  "Best times");
        mvwhline(BestTimesWin,  2, 1,  CharSet.HLine, COLS-2);
        mvwprintw(BestTimesWin, 3, 7,  "Player");
        mvwhline(BestTimesWin,  4, 4,  CharSet.HLine, 12);
        mvwprintw(BestTimesWin, 3, 22, "Area");
        mvwhline(BestTimesWin,  4, 19, CharSet.HLine, 8);
        mvwprintw(BestTimesWin, 3, 31, "Mines");
        mvwhline(BestTimesWin,  4, 30, CharSet.HLine, 7);
        mvwprintw(BestTimesWin, 3, 42, "Time");
        mvwhline(BestTimesWin,  4, 40, CharSet.HLine, 9);
        mvwprintw(BestTimesWin, 3, 61, "Date");
        mvwhline(BestTimesWin,  4, 53, CharSet.HLine, 24);

        bfd = NewBFD();
        pathname = FPTBTF();
        LoadBestTimesFile(bfd, pathname);
        free(pathname);

#ifdef USE_GROUP_BEST_FILE
        pathname = FPTGBTF();
        LoadBestTimesFile(bfd, pathname);
        free(pathname);
#endif
        DumpBFD(bfd, 1);

        for (i = 0; i < bfd->numents && i < height - 6; i++)
        {
                const struct passwd *pw = getpwuid(bfd->ents[i].user);
                mvwprintw(BestTimesWin, 5 + i, 4, pw->pw_name);
                mvwprintw(BestTimesWin, 5 + i, 19, "%u", bfd->ents[i].area);
                mvwprintw(BestTimesWin, 5 + i, 31, "%u", bfd->ents[i].mines);
                enttime = bfd->ents[i].time;
                if (enttime >= 86400)
                {
                        mvwprintw(BestTimesWin, 5 + i, 40, "%d:%02d:%02d:%02ds",
                                  enttime / 86400, (enttime % 86400) / 3600,
                                  (enttime % 3600) / 60 , enttime % 60);
                }
                else if (enttime >= 3600)
                {
                        mvwprintw(BestTimesWin, 5 + i, 40, "%d:%02d:%02ds",
                                  enttime / 3600, (enttime % 3600) / 60, enttime % 60);
                }
                else if (enttime >= 60)
                {
                        mvwprintw(BestTimesWin, 5 + i, 40, "%d:%02ds",
                                  enttime / 60, enttime % 60);
                }
                else
                {
                        mvwprintw(BestTimesWin, 5 + i, 40, "%ds", enttime);
                }
                mvwprintw(BestTimesWin, 5 + i, 53, "%s", ctime(&bfd->ents[i].date));
        }
        free(bfd->ents);
        free(bfd);

        wborder(BestTimesWin,CharSet.VLine,CharSet.VLine,CharSet.HLine,CharSet.HLine,CharSet.ULCorner,CharSet.URCorner,CharSet.LLCorner,CharSet.LRCorner);
        mvwprintw(BestTimesWin, LINES-1, 2,
                  " Press \'q\' to quit, any other key to continue. ");
        wmove(BestTimesWin,0,0);
        wrefresh(BestTimesWin);
        cbreak();
        Input=wgetch(BestTimesWin);
#ifdef DEBUG_LOG
        fprintf(DebugLog, "Leaving Bets Times view with %c\n", Input);
        fflush(DebugLog);
#endif /* DEBUG_LOG */

        if (Input=='q')
        {
                clear();
                refresh();
                endwin();
                exit(EXIT_SUCCESS);
        }
        else
        {
                wclear(BestTimesWin);
                delwin(BestTimesWin);
                clear();
                noutrefresh();
        }
        return;
}

void DrawCursor(GameStats* Game)
{
        wmove(Game->Board,(Game->CursorY-Game->FocusY),(3*(Game->CursorX-Game->FocusX)));
        xwaddch(Game->Board,&CharSet,CharSet.CursorLeft);

        wmove(Game->Board,(Game->CursorY-Game->FocusY),(3*(Game->CursorX-Game->FocusX))+2);
        xwaddch(Game->Board,&CharSet,CharSet.CursorRight);

        move(0,0);
        return;
}

void UndrawCursor(GameStats* Game)
{
        mvwaddch(Game->Board,(Game->CursorY-Game->FocusY),(3*(Game->CursorX-Game->FocusX)),' ');
        mvwaddch(Game->Board,(Game->CursorY-Game->FocusY),(3*(Game->CursorX-Game->FocusX))+2,' ');
        move(0,0);
        return;
}

/* Pan() is respondsible for keeping the relevant part of the board displayed.
In addition, it also draws the border, including doing any "arrows" to
indicate that part of the board is not presently displayed.
The panning itself it triggered when the cursor gets within two squares of
the edge of the currently displayed area. */
int Pan(GameStats* Game)
{
        chtype Right,Left,Top,Bottom;
        int VViewable=0, HViewable=0;

        Right=Left=CharSet.VLine;
        Top=Bottom=CharSet.HLine;

        /* The basic border needs to be drawn regardless. */

        /* If the board isn't larger than the screen, no need to pan. */
        if ((Game->LargeBoardX + Game->LargeBoardY) == 0)
        {
                wborder(Game->Border,CharSet.VLine,CharSet.VLine,CharSet.HLine,CharSet.HLine,CharSet.ULCorner,CharSet.URCorner,CharSet.LLCorner,CharSet.LRCorner);
                wnoutrefresh(Game->Border);
                return 0;
        }

        /* Now to figure out which way to go... */
        VViewable=(LINES-6);
        HViewable=((COLS-INFO_W-2)/3)-1;

        /* See if the current cursor is even in the focus. */
        if ((Game->CursorY < Game->FocusY) ||
                ((Game->FocusY+VViewable) < Game->CursorY) ||
                ((Game->CursorY-Game->FocusY) < 2) ||
                (((Game->FocusY+VViewable)-Game->CursorY) <= 2))
        {
                CenterY(Game);
        }

        if ((Game->CursorX < Game->FocusX) ||
                ((Game->FocusX+HViewable) < Game->CursorX) ||
                ((Game->CursorX-Game->FocusX) < 2) ||
                (((Game->FocusX+HViewable)-Game->CursorX) < 2))
        {
                CenterX(Game);
        }

        /* Then draw the borders. */
        if (Game->LargeBoardX)
        {
                /* Check to see if the left side needs to be an arrow boundary. */
                if (Game->FocusX>0)
                {
                        Left=CharSet.LArrow;
                }

                /* Check to see if the right side needs to be an arrow boundary. */
                if ((Game->FocusX + ((COLS-INFO_W-2)/3) ) < Game->Width )
                {
                        Right=CharSet.RArrow;
                }
        }

        if (Game->LargeBoardY)
        {
                /* Check to see if the top side needs to be an arrow boundary. */
                if (Game->FocusY>0)
                {
                        Top=CharSet.UArrow;
                }

                /* Check to see if the bottom side needs to be an arrow boundary. */
                if ((Game->FocusY + (LINES-6)) < Game->Height )
                {
                        Bottom=CharSet.DArrow;
                }

        }

        wborder(Game->Border,Left,Right,Top,Bottom,CharSet.ULCorner,CharSet.URCorner,CharSet.LLCorner,CharSet.LRCorner);
        wnoutrefresh(Game->Border);

        return 0;
}

int Center(GameStats* Game)
{
        CenterX(Game);
        CenterY(Game);
        return 0;
}

int CenterX(GameStats* Game)
{
        int HOffset=0;

        if (Game->LargeBoardX)
        {
                HOffset=(((COLS-INFO_W-2)/3)-1)/2;

                if (Game->CursorX > (Game->Width - HOffset))
                {
                        Game->FocusX=(Game->Width-(2*HOffset)-1);
                }
                else if ((Game->CursorX - HOffset) >= 0)
                {
                        Game->FocusX=(Game->CursorX-HOffset);
                }
                else
                {
                        Game->FocusX=0;
                }
        }

        return 0;
}

int CenterY(GameStats* Game)
{
        int VOffset=0;

        if (Game->LargeBoardY)
        {
                VOffset=(LINES-6)/2;

                if (Game->CursorY > (Game->Height - VOffset))
                {
                        Game->FocusY=(Game->Height - (LINES-6));
                }
                else if ((Game->CursorY - VOffset) >= 0)
                {
                        Game->FocusY = (Game->CursorY - VOffset);
                }
                else
                {
                        Game->FocusY=0;
                }
        }

        return 0;
}
