/**********************************************************************
*  This source code is copyright 1999 by Gus Hartmann & Peter Keller  *
*  It may be distributed under the terms of the GNU General Purpose   *
*  License, version 2 or above; see the file COPYING for more         *
*  information.                                                       *
*                                                                     *
*  $Id: main.c,v 1.34 1999-08-09 05:25:36 hartmann Exp $
*                                                                     *
**********************************************************************/

#include "sweep.h"

int main(int argc, char** argv)
{
	GameStats* Game;
	chtype Input=0;

	/* Set up the curses cleaner in case of disaster */
	signal(SIGSEGV, sighandler);
	signal(SIGBUS, sighandler);
	signal(SIGILL, sighandler);

#ifdef DEBUG_LOG
	if ((DebugLog=fopen("debug.log","a"))==0)
	{
		perror("Main::OpenDebugLog");
		return 1;
	}
#endif /* DEBUG_LOG */
	
	if ((Game=malloc(sizeof(GameStats)))==NULL)
	{
		perror("Main::AllocGame");
		return 1;
	}

	InitGame(Game);

	SourceGlobalFile(Game);
	SourceHomeFile(Game);

	if (ParseArgs(Game,argc,argv)>0)
	{
		return 1;
	}

	/* Output all the relevant information about the compilation. */
	printf("Freesweep v%s by\nGus Hartmann (hartmann@cs.wisc.edu) and Pete Keller (psilord@cs.wisc.edu).\n",VERSION);
	printf("Freesweep comes with ABSOLUTELY NO WARRANTY; see the file COPYING for more info.\n");
#ifdef DEBUG_LOG
	fprintf(DebugLog,"Freesweep v%s by Gus Hartmann and Pete Keller.\n",VERSION);
#endif /* DEBUG_LOG */
	
	/* Start ncurses */
	StartCurses();

	/* Prompt the user for new setings, if necessary. */
	if (Game->Fast==0)
	{
		AskPrefs(Game);
	}

	clear();
	noutrefresh();

	if (InitErrorWin(Game) + InitStatsWin() > 0)
	{
		/* Do something bad. */
	}
	ReadyGame(Game);
	noutrefresh();

	/* set up signal handler and stuff */

	while (1)
	{
		/* Make sure the right character set is in use. */
		InitCharSet(Game,Game->LineDraw);

		/* Touch a couple windows */
		RedrawErrorWin();
		RedrawStatsWin();

		ReReadyGame(Game);
		PrintInfo();
		noutrefresh();
		PrintStats(Game);
		doupdate();
		Center(Game);

		StartTimer();
	/*	This is the main loop.*/
		while (Game->Status==INPROG)
		{
			PrintStats(Game);
			Pan(Game);
			DrawCursor(Game);
			DrawBoard(Game);
			doupdate();
			UndrawCursor(Game);
			SweepError(NULL);
			GetInput(Game);
			Game->Time = g_tick;
		}
		g_tick = 0;
		Game->Time = g_tick;	/* If we won, the game is already saved */
		StopTimer();

		/* Update the final action of the player */
		Pan(Game);
		DrawCursor(Game);
		DrawBoard(Game);
		doupdate();
		UndrawCursor(Game);

		/* see what happened */
		switch(Game->Status)
		{
			case ABORT:
				ReReadyGame(Game);
				break;
			case RECONF:
				StopTimer();
				clear();
				noutrefresh();
				Wipe(Game);
				AskPrefs(Game);
				ReadyGame(Game);
				Center(Game);
				clear();
				noutrefresh();
				StartTimer();
				break;
			case LOSE:
				StopTimer();
				DrawCursor(Game);
				SweepMessage("BOOM! Any key to continue, 'q' to quit.");
				nodelay(Game->Board,FALSE);
				Input=mvwgetch(Game->Board,0,0);
				wnoutrefresh(Game->Board);
				refresh();

				if (Input == 'q')
				{
					clear();
					refresh();
					endwin();
#ifdef DEBUG_LOG
					fprintf(DebugLog,"========================================\n");
					fclose(DebugLog);
#endif /* DEBUG_LOG */
					return 0;
				}
				else
				{
					SweepMessage(NULL);
					UndrawCursor(Game);
				}
				break;
			case WIN:
				StopTimer();
				DrawCursor(Game);
				SweepMessage("You Win! Press a key to continue.");
				nodelay(Game->Board,FALSE);
				Input=mvwgetch(Game->Board,0,0);
				wnoutrefresh(Game->Board);
				refresh();

				if (Input == 'q')
				{
					clear();
					refresh();
					endwin();
#ifdef DEBUG_LOG
					fprintf(DebugLog,"========================================\n");
					fclose(DebugLog);
#endif /* DEBUG_LOG */
					return 0;
				}
				else
				{
					SweepMessage(NULL);
					UndrawCursor(Game);
				}
				break;
			default:
				break;
		}

		/* It might be nice to log this before reseting it. */
		Game->Status=INPROG;

		flushinp();
	}

	endwin();

#ifdef DEBUG_LOG
	fprintf(DebugLog,"========================================\n");
	fclose(DebugLog);
#endif /* DEBUG_LOG */

	return 0;
}
