/**********************************************************************
*  This source code is copyright 1999 by Gus Hartmann & Peter Keller  *
*  It may be distributed under the terms of the GNU General Purpose   *
*  License, version 2 or above; see the file COPYING for more         *
*  information.                                                       *
*                                                                     *
*  $Id: sl.c,v 1.9 2000-04-02 00:52:11 hartmann Exp $
*                                                                     *
**********************************************************************/

#include "sweep.h"

void SaveGame(GameStats* Game, char *fname)
{
	FILE *fo = NULL;

	fo = xfopen(fname, "w");
	
	/* dump the stats out */
	fprintf(fo, "%d\n%d\n%d\n%u\n%u\n%u\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n"
			"%u\n%u\n%u\n",
			Game->Height, Game->Width, Game->Percent, Game->NumMines, 
			Game->MarkedMines, Game->BadMarkedMines, Game->Color, 
			Game->Fast, Game->Alert, 
			Game->LineDraw, Game->CursorX, Game->CursorY,
			Game->LargeBoardX, Game->LargeBoardY, Game->Status,
			Game->FocusX, Game->FocusY, Game->Time);
	
	/* dump the field out */
	fwrite(Game->Field, 
		(Game->Height*((Game->Width % 2 ? (Game->Width) +1 : Game->Width ))/2)
		* sizeof(unsigned char), 1, fo);

	fclose(fo);
}

GameStats* LoadGame(char *fname)
{
	FILE *fi = NULL;
	GameStats *Game = NULL;
	int VViewable = 0, HViewable = 0;

	VViewable=(LINES-6);
	HViewable=((COLS-INFO_W-2)/3);

	Game = (GameStats*)xmalloc(sizeof(GameStats));

	fi = xfopen(fname, "r");

	/* Load the Game Stats */
	fscanf(fi, "%d\n%d\n%d\n%u\n%u\n%u\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n"
			"%u\n%u\n%u\n",
			&Game->Height, &Game->Width, &Game->Percent, &Game->NumMines, 
			&Game->MarkedMines, &Game->BadMarkedMines, &Game->Color, 
			&Game->Fast, &Game->Alert, 
			&Game->LineDraw, &Game->CursorX, &Game->CursorY,
			&Game->LargeBoardX, &Game->LargeBoardY, &Game->Status,
			&Game->FocusX, &Game->FocusY, &Game->Time);

	/* make the field I need to write into */
	if ((Game->Field=calloc((Game->Height*(
		( Game->Width % 2 ? (Game->Width) +1 : Game->Width )))/2,
		sizeof(char)))==NULL)
	{
		SweepError("Out of Memory.");
		/* XXX clean this up */
		exit(EXIT_FAILURE);
	}

	/* load the board */
	fread(Game->Field, 
		(Game->Height*((Game->Width % 2 ? (Game->Width) +1 : Game->Width ))/2)
		* sizeof(unsigned char), 1, fi);
	
	/* make the new window setup for it */
	if (Game->LargeBoardX && Game->LargeBoardY)
	{
		Game->Border=newwin((LINES-4),(COLS-INFO_W),0,0);
/*		Game->Board=newwin(VViewable,(3*HViewable),1,1);*/
		Game->Board=derwin(Game->Border,VViewable,(3*HViewable),1,1);
	}
	else if (Game->LargeBoardX)
	{
		Game->Border=newwin((Game->Height+2),(COLS-INFO_W),0,0);
/*		Game->Board=newwin(Game->Height,(3*HViewable),1,1);*/
		Game->Board=derwin(Game->Border,Game->Height,(3*HViewable),1,1);
	}
	else if (Game->LargeBoardY)
	{
		Game->Border=newwin((LINES-4),((3*Game->Width)+2),0,0);
/*		Game->Board=newwin(VViewable,(3*Game->Width),1,1);*/
		Game->Board=derwin(Game->Border,VViewable,(3*Game->Width),1,1);
	}
	else
	{
		Game->Border=newwin((Game->Height+2),((3*Game->Width)+2),0,0);
/*		Game->Board=newwin(Game->Height,(3*Game->Width),1,1);*/
		Game->Board=derwin(Game->Border,Game->Height,(3*Game->Width),1,1);
	}

	if ((Game->Border==NULL)||(Game->Board==NULL))
	{
		perror("ReadyGame::AllocWin");
		exit(EXIT_FAILURE);
	}

	/* Set the game clock as the real clock */
	g_tick = Game->Time;

	return Game;
}
