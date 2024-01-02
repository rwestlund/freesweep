/**********************************************************************
*  This source code is copyright 1999 by Gus Hartmann & Peter Keller  *
*  It may be distributed under the terms of the GNU General Purpose   *
*  License, version 2 or above; see the file COPYING for more         *
*  information.                                                       *
*                                                                     *
*  $Id: play.c,v 1.41 2002-12-19 07:04:05 hartmann Exp $
*                                                                     *
**********************************************************************/

#include "sweep.h"

static int LastKey;

int GetInput(GameStats* Game)
{
        unsigned char RetVal;
        int UserInput=0;
        char *pathname = NULL;
        GameStats* LoadedGame;

#ifdef SWEEP_MOUSE
        MEVENT MouseInput;
#endif /* SWEEP_MOUSE */

        UserInput=getch();

        if (UserInput=='.')
        {
                UserInput=LastKey;
        }
        else
        {
                LastKey=UserInput;
        }

        switch (UserInput)
        {
                /* Going Left? */
                case 'h':
                case KEY_LEFT:
                        MoveLeft(Game,1);
                        break;

                /* Going Down? */
                case 'j':
                case KEY_DOWN:
                        MoveDown(Game,1);
                        break;

                /* Going Up? */
                case 'k':
                case KEY_UP:
                        MoveUp(Game,1);
                        break;

                /* Going Right? */
                case 'l':
                case KEY_RIGHT:
                        MoveRight(Game,1);
                        break;


                /* Diagonals. */
                case KEY_A1:
                        MoveUp(Game,1);
                        MoveLeft(Game,1);
                        break;

                case KEY_A3:
                        MoveUp(Game,1);
                        MoveRight(Game,1);

                        break;

                case KEY_C1:
                        MoveDown(Game,1);
                        MoveLeft(Game,1);
                        break;

                case KEY_C3:
                        MoveDown(Game,1);
                        MoveRight(Game,1);
                        break;

                /* The non-motion keys. */
                /* The accepted values for flagging a space as a mine. */
                case 'f':
                case KEY_B2:
                        GetMine(Game->CursorX,Game->CursorY,RetVal);
                        switch (RetVal)
                        {
                                case MINE:
                                        SetMine(Game->CursorX,Game->CursorY,MARKED);
                                        Game->MarkedMines++;
                                        break;
                                case MARKED:
                                        SetMine(Game->CursorX,Game->CursorY,MINE);
                                        Game->MarkedMines--;
                                        break;
                                case BAD_MARK:
                                        SetMine(Game->CursorX,Game->CursorY,UNKNOWN);
                                        Game->BadMarkedMines--;
                                        break;
                                case UNKNOWN:
                                        SetMine(Game->CursorX,Game->CursorY,BAD_MARK);
                                        Game->BadMarkedMines++;
                                        break;
                                default:
                                        SweepError("Cannot mark as a mine.");
                                        break;
                        }

                        if (Game->MarkedMines == Game->NumMines &&
                                Game->BadMarkedMines == 0)
                        {
                                StopTimer();
/*                              YouWin();*/
                                Game->Status = WIN;
#ifdef DEBUG_LOG
                                fprintf(DebugLog, "Num %d, Marked %d, Bad %d\n",
                                        Game->NumMines, Game->MarkedMines, Game->BadMarkedMines);
                                fflush(DebugLog);
#endif /* DEBUG_LOG */
                                pathname = FPTBTF();
                                UpdateBestTimesFile(Game, pathname);
                                free(pathname);

                                #ifdef USE_GROUP_BEST_FILE
                                        pathname = FPTGBTF();
                                        UpdateBestTimesFile(Game, pathname);
                                        free(pathname);
                                #endif

                        }
                        StartTimer();
                        break;

                /* the accepted key to make a 'new' game */
                case 'n':
                        Game->Status = ABORT;
                        break;

                /* The accepted key to make a reconfigure if the game */
                case 'x':
                        Game->Status = RECONF;
                        break;

                /* The accepted key to open the save gui */
                case 'w':
                        if ( (pathname = FSGUI() ) != NULL )
                        {
                                if ((  LoadedGame = LoadGame(pathname) ) == NULL )
                                {
                                        SweepError("Error loading game %s", pathname);
                                }
                                else
                                {
                                        werase(Game->Board);
                                        wnoutrefresh(Game->Board);
                                        werase(Game->Border);
                                        wnoutrefresh(Game->Border);
                                        delwin(Game->Board);
                                        delwin(Game->Border);
                                        free(Game->Field);
                                        free(Game);
                                        Game = LoadedGame;
                                        SweepError("Done Loading");
                                }
                                break;
                        }
                        else
                        {
                                SweepError("Unable to open file");
                        }
                break;

                /* The accepted keys to expose a space. */
                case ' ':
                case KEY_IC:
                        GetMine(Game->CursorX,Game->CursorY,RetVal);
                        switch (RetVal)
                        {
                                case BAD_MARK:
                                case MARKED:
                                        SweepError("Cannot expose a flagged mine.");
                                        break;
                                case MINE:
                                        StopTimer();
                                        /* BOOM! */
/*                                      Boom();*/
                                        SetMine(Game->CursorX,Game->CursorY,DETONATED);
                                        Game->Status=LOSE;
#ifdef DEBUG_LOG
                                        fprintf(DebugLog,"Mine exposed! Setting Status to LOSE.\n");
                                        fflush(DebugLog);
#endif /* DEBUG_LOG */
                                        break;
                                case UNKNOWN:
                                        Clear(Game);
                                        break;
                                case EMPTY:
                                        SweepError("Square already exposed.");
                                        break;
                                case 1: case 2: case 3: case 4:
                                case 5: case 6: case 7: case 8:
                                        /* Double-click */
                                        SuperClear(Game);

                                        break;
                                default:
                                        break;
                        }
                        break;

                /* The accepted keys to redraw the screen. */
                case 'r':
                case KEY_REFRESH:
                case '\f':
                        PrintInfo();
                        RedrawStatsWin();
                        RedrawErrorWin();
                        touchwin(Game->Border);
                        touchwin(Game->Board);

                        noutrefresh();

                        break;

                /* The accepted keys to display the help screen. */
                case '?':
                case KEY_HELP:
                        StopTimer();
                        Help();
                        PrintInfo();
                        RedrawErrorWin();
                        RedrawStatsWin();
                        StartTimer();
                        break;

                /* The accepted keys to display the license screen. */
                case 'g':
                        PrintGPL();
                        PrintInfo();
                        RedrawErrorWin();
                        RedrawStatsWin();
                        break;

                /* The accepted keys to display the best times screen. */
                case 'b':
                        PrintBestTimes(NULL);
                        PrintInfo();
                        RedrawErrorWin();
                        RedrawStatsWin();
                        noutrefresh();
                        doupdate();
                        break;

                /* The accepted values to center the cursor on board */
                case 'c':
                        Game->CursorX=Game->Width/2;
                        Game->CursorY=Game->Height/2;
                        break;

                /* A test key. Sort of feature-of-the-day. */
                case 't':
                        SweepMessage("_________0_________0(%u, %u)_________0_________0", Game->CursorX, Game->CursorY);
                        break;

                case 'e':
                        FindNearest(Game);
                        break;

                case 'y':
                        FindNearestBad(Game);
                        break;

                /* The accepted values to suspend the game. */
                case KEY_SUSPEND:
                case 'z':
                        break;

                case 'q':
#ifdef DEBUG_LOG
                        fprintf(DebugLog,"Quitting quietly.\n========================================\n");
                        fclose(DebugLog);
#endif /* DEBUG_LOG */
                        clear();
                        refresh();
                        endwin();
                        exit(EXIT_SUCCESS);
                        break;

                case '0':
                        Game->CursorY=0;
                        Game->CursorX=0;
                        break;

                case '1': case '2': case '3': case '4':
                case '5': case '6': case '7': case '8':
                case '9':
                        SweepMessage("Switching themes");
                        Game->Theme=UserInput-'0';
                        SetTheme(Game);
                        PrintInfo();
                        RedrawErrorWin();
                        RedrawStatsWin();
                        break;

                case '$':
                        Game->CursorY=Game->Height-1;
                        Game->CursorX=Game->Width-1;
                        break;

                case 'd':
                        DumpGame(Game);
                        break;

#ifdef SWEEP_MOUSE
                case KEY_MOUSE:
                        if (NCURSES_MOUSE_VERSION==1)
                        {
                                if (getmouse(&MouseInput)==ERR)
                                {

                                }

                        }
                        break;
#endif /* SWEEP_MOUSE */

                /* XXX Save a game */
                case 's':
                        SaveGame(Game, "./foo.svg");
                        SweepError("Done Saving");
                        break;

                /* XXX load a game */
                case 'o':
                        werase(Game->Board);
                        wnoutrefresh(Game->Board);
                        werase(Game->Border);
                        wnoutrefresh(Game->Border);
                        delwin(Game->Board);
                        delwin(Game->Border);
                        free(Game->Field);
                        free(Game);
                        Game = LoadGame("./foo.svg");
                        SweepError("Done Loading");
                        break;

                case 'i':
                        SaveGameImage(Game, "foo.ppm");
                        break;

                case ERR:
                        return 1;
                        break;

                default:
#ifdef DEBUG_LOG
                        fprintf(DebugLog,"Got unknown keystroke: %c\n", UserInput);
#endif /* DEBUG_LOG */
                        LastKey=ERR;
                        SweepError("Bad Input.");
                        return 1;
                        break;
        }

        LastKey=UserInput;
        return 0;
}

void MoveLeft(GameStats* Game,int Num)
{
        if (Game->CursorX >= Num)
        {
                Game->CursorX-=Num;
        }
        else
        {
                SweepError("Cannot move past left side of board.");
                Game->CursorX=0;
        }
        return;
}

void MoveRight(GameStats* Game,int Num)
{
        if ((Game->CursorX + Num) < Game->Width)
        {
                Game->CursorX+=Num;
        }
        else
        {
                SweepError("Cannot move past right side of board.");
                Game->CursorX=(Game->Width-1);
        }
        return;
}

void MoveUp(GameStats* Game,int Num)
{
        if (Game->CursorY >= Num)
        {
                Game->CursorY-=Num;
        }
        else
        {
                SweepError("Cannot move past top of board.");
                Game->CursorY=0;
        }
        return;
}

void MoveDown(GameStats* Game,int Num)
{
        if ((Game->CursorY + Num) < Game->Height)
        {
                Game->CursorY+=Num;
        }
        else
        {
                SweepError("Cannot move past bottom of board.");
                Game->CursorY=(Game->Height-1);
        }
        return;
}

void Boom(void)
{
        WINDOW* BoomWin;

        if ((BoomWin=newwin(LINES/3,COLS/3,LINES/3,COLS/3))==NULL)
        {
                perror("Boom::Alloc Window");
                return;
        }

        if (wborder(BoomWin,CharSet.VLine,CharSet.VLine,CharSet.HLine,CharSet.HLine,CharSet.ULCorner,CharSet.URCorner,CharSet.LLCorner,CharSet.LRCorner)!=OK)
        {
                perror("Boom::Draw Border");
                return;
        }

        mvwprintw(BoomWin,(LINES/6)-1,(COLS-15)/6,"Boom!");

        wrefresh(BoomWin);
        werase(BoomWin);
        wnoutrefresh(BoomWin);
        delwin(BoomWin);

        return;
}

void YouWin(void)
{
        WINDOW* YouWin;

        if ((YouWin=newwin(LINES/3,COLS/3,LINES/3,COLS/3))==NULL)
        {
                perror("YouWin::Alloc Window");
                return;
        }

        if (wborder(YouWin,CharSet.VLine,CharSet.VLine,CharSet.HLine,CharSet.HLine,CharSet.ULCorner,CharSet.URCorner,CharSet.LLCorner,CharSet.LRCorner)!=OK)
        {
                perror("YouWin::Draw Border");
                return;
        }

        mvwprintw(YouWin,(LINES/6)-1,(COLS-15)/6,"You Win!");

        wrefresh(YouWin);
        napms(1000);
        werase(YouWin);
        wnoutrefresh(YouWin);
        delwin(YouWin);

        return;
}

RETSIGTYPE ResizeSignal(int signo)
{
        endwin(); // Recreate stdscr
        clear();
        refresh();

        PrintInfo();

        ClearStats();
        InitStatsWin();
        RedrawStatsWin();

        RedrawErrorWin();
        touchwin(Game->Border);
        touchwin(Game->Board);

        noutrefresh();
        doupdate();
}
