/*********************************************************************
* $Id: main.c,v 1.22 1999-02-24 05:40:18 psilord Exp $
*********************************************************************/

#include "sweep.h"

int main(int argc, char** argv)
{
	GameStats* Game;

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
	printf("Freesweep v%s for %s on %s at %s by %s\n",VERSION,SYSTEM_TYPE,__DATE__,__TIME__,USER);
	printf("Freesweep comes with ABSOLUTELY NO WARRANTY; see the file COPYING for more info.\n");
#ifdef DEBUG_LOG
	fprintf(DebugLog,"Freesweep v%s for %s on %s at %s by %s\n",VERSION,SYSTEM_TYPE,__DATE__,__TIME__,USER);
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

	InitErrorWin(Game);
	ReadyGame(Game);
	noutrefresh();

	/* set up signal handler and stuff */

	while (1)
	{
		/* Make sure the right character set is in use. */
		InitCharSet(Game,Game->LineDraw);

		/* Touch a couple windows */
		RedrawErrorWin();

		ReReadyGame(Game);
		PrintInfo();
		noutrefresh();
		doupdate();
		Center(Game);

		StartTimer();
	/*	This is the main loop.*/
		while (Game->Status==INPROG)
		{
			Pan(Game);
			DrawCursor(Game);
			DrawBoard(Game);
			doupdate();
			UndrawCursor(Game);
			SweepError(NULL);
			GetInput(Game);
			SweepMessage("%u", g_tick);
			Game->Time = g_tick;
		}
		g_tick = 0;
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
			case WIN:
				napms(2000);
				break;
			case LOSE:
				napms(2000);
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
