/*********************************************************************
* $Id: drawing.c,v 1.1.1.1 1998-11-23 03:57:08 hartmann Exp $
*********************************************************************/

#include "sweep.h"

void StartCurses()
{
#ifdef SWEEP_MOUSE
	mmask_t Mask;
#endif
	initscr();
	if (has_colors())
	{
		start_color();
		init_pair(1,COLOR_CYAN,COLOR_BLACK);
		init_pair(2,COLOR_MAGENTA,COLOR_BLACK);
		init_pair(3,COLOR_BLUE,COLOR_BLACK);
		init_pair(4,COLOR_GREEN,COLOR_BLACK);
		init_pair(5,COLOR_RED,COLOR_BLACK);
		init_pair(6,COLOR_WHITE,COLOR_BLACK);
		init_pair(7,COLOR_BLUE,COLOR_BLACK);
		init_pair(8,COLOR_CYAN,COLOR_WHITE);
		init_pair(9,COLOR_MAGENTA,COLOR_WHITE);
		init_pair(10,COLOR_BLUE,COLOR_WHITE);
		init_pair(11,COLOR_GREEN,COLOR_WHITE);
		init_pair(12,COLOR_RED,COLOR_WHITE);
		init_pair(13,COLOR_BLACK,COLOR_WHITE);
	}
	noecho();
	keypad(stdscr, TRUE);
	intrflush(stdscr, TRUE);
	cbreak();
	nonl();
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
	wborder(InfoWin,CharSet.VLine,CharSet.VLine,CharSet.HLine,CharSet.HLine,CharSet.ULCorner,CharSet.URCorner,CharSet.LLCorner,CharSet.LRCorner);
	mvwprintw(InfoWin,1,2,"-=-FreeSweeper-=-");
	mvwprintw(InfoWin,2,8,"by Gus!");
	wmove(InfoWin,3,1);
	whline(InfoWin,CharSet.HLine,19);
	mvwprintw(InfoWin,4,2,"Hit \'?\' for help");
	move(0,0);
	wnoutrefresh(InfoWin);
	return;
}

void AskPrefs(GameStats* Game)
{
	char ValueBuffer[(L_MAX_W+L_MAX_H+1)];
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
		mvgetnstr(CurrentLine,(26+L_MAX_H),ValueBuffer,L_MAX_H);
		refresh();
		if (ValueBuffer[0]==0)
		{
			Status=1;
			mvprintw(CurrentLine,(26+L_MAX_H),"%d",Game->Height);
		}
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
		mvgetnstr(CurrentLine,(25+L_MAX_W),ValueBuffer,L_MAX_W);
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
		mvgetnstr(CurrentLine,(41+L_MAX_H+L_MAX_W),ValueBuffer,(L_MAX_H+L_MAX_W));
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

	/* Now for the charater set. */
	mvprintw(CurrentLine,0,"Use the PC linedraw character set? [");
	if (Game->LineDraw==1)
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
			mvprintw(LINES-1,0,"Invalid entry for character set.");
			mvclrtoeol(CurrentLine,42);
		}
		ValueBuffer[0]=mvgetch(CurrentLine,42);
		refresh();

		switch (ValueBuffer[0])
		{
			case '\n': case '\r':
				Status=1;
				if (Game->LineDraw==1)
				{
					mvprintw(CurrentLine,42,"Yes");
				}
				else
				{
					mvprintw(CurrentLine,42,"No");
				}
				break;

			case 'n': case 'N':
				Value=0;
				mvprintw(CurrentLine,42,"No");
				InitCharSet(Game,Value);
				Status=1;
				break;

			case 'y': case 'Y':
				Value=1;
				mvprintw(CurrentLine,42,"Yes");
				InitCharSet(Game,Value);
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
	int Input=0, CurrentLine=0;
	char* Messages[]=
	{
		"Arrow keys and vi-style movement keys move the cursor.",
		"The space bar exposes a square.",
		"\'f\' flags a space as a mine.",
		"\'r\' redraws the screen.",
		"\'0\' moves the cursor to the upper left corner.",
		"\'$\' moves the cursor to the lower right corner.",
		"\'.\' repeats the last command.",
		"\'a\' toggles the character set.",
		"\'?\' displays this help screen.",
		"Any non-zero number multiplies the next action."
	};

	if ((HelpWin=newwin(0,0,0,0))==NULL)
	{
		perror("Help::newwin");
		exit(1);
	}
	wborder(HelpWin,CharSet.Mark,CharSet.Mark,CharSet.Mark,CharSet.Mark,CharSet.Mark,CharSet.Mark,CharSet.Mark,CharSet.Mark);
	mvwprintw(HelpWin,1,2,"Time out - Freesweep Help");
	mvwhline(HelpWin,2,1,CharSet.HLine,COLS-2);
	mvwprintw(HelpWin,3,2,"Useful Keys:");

	/* Be sure to update this to account for all of the error messages. */
	for (CurrentLine=0;CurrentLine<10;CurrentLine++)
	{
		mvwprintw(HelpWin,CurrentLine+4,6,Messages[CurrentLine]);
	}

	mvwprintw(HelpWin,LINES-1,1,"--Press \'q\' to quit, any other key to continue.--");
	wmove(HelpWin,0,0);
	wrefresh(HelpWin);
	Input=wgetch(HelpWin);
	if ((Input=='q')||(Input=='Q'))
	{
		clear();
		refresh();
		endwin();
		exit(0);
	}
	else
	{
		wclear(HelpWin);
		delwin(HelpWin);
		clear();
		noutrefresh();
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
					waddch(Game->Board,'-');
					break;
				case EMPTY:
					waddch(Game->Board,CharSet.Space);
					break;
				case 1: case 2: case 3: case 4:
				case 5: case 6: case 7: case 8:
					waddch(Game->Board,(CellVal)+'0');
					break;
				case MINE:
/*					waddch(Game->Board,CharSet.Mine);*/
					waddch(Game->Board,'+');
/*					waddch(Game->Board,(CoordX%10)+'0');*/
					break;
				case MARKED:
					waddch(Game->Board,CharSet.Mark);
					break;
				case BAD_MARK:
					waddch(Game->Board,CharSet.FalseMark);
					break;
				default:
					break;
			}
		}
	}
	move(0,0);

	wnoutrefresh(Game->Board);
	move(0,0);
	return 0;
}

/* PrintBestTimes() opens
   each entry in a new window, BestTimesWin, which is deleted afterwards.
*/
void PrintBestTimes(char* Filename)
{
	int Input=0,CurrentLine=0,Time=0;
	char* Decoder;
	char* Buffer;
	WINDOW* BestTimesWin;
	BestTimeNode *Current, *Head;

	if ((BestTimesWin=newwin(0,0,0,0))==NULL)
	{
		perror("PrintHighs::AllocWin");
		exit(1);
	}
	wborder(BestTimesWin,CharSet.Mark,CharSet.Mark,CharSet.Mark,CharSet.Mark,CharSet.Mark,CharSet.Mark,CharSet.Mark,CharSet.Mark);
	mvwprintw(BestTimesWin,1,2,"Best times");
	mvwhline(BestTimesWin,2,1,CharSet.HLine,COLS-2);
	mvwprintw(BestTimesWin,3,4,"Username");
	mvwhline(BestTimesWin,4,4,CharSet.HLine,8);
	mvwprintw(BestTimesWin,3,15,"Height");
	mvwhline(BestTimesWin,4,15,CharSet.HLine,6);
	mvwprintw(BestTimesWin,3,25,"Width");
	mvwhline(BestTimesWin,4,25,CharSet.HLine,5);
	mvwprintw(BestTimesWin,3,34,"Mines");
	mvwhline(BestTimesWin,4,33,CharSet.HLine,7);
	mvwprintw(BestTimesWin,3,44,"Time");
	mvwhline(BestTimesWin,4,42,CharSet.HLine,9);
	mvwprintw(BestTimesWin,3,62,"Date");
	mvwhline(BestTimesWin,4,54,CharSet.HLine,24);

	Head=Current=LoadNodeList(Filename);
	
	CurrentLine=5;
	while (Current!=NULL)
	{
		if (CurrentLine==LINES-1)
		{
			mvwprintw(BestTimesWin,LINES-1,1,"--Press \'q\' to quit, Space for more best times, any other key to continue.--");
			wmove(BestTimesWin,0,0);
			wrefresh(BestTimesWin);
			Input=wgetch(BestTimesWin);
			if ((Input=='q')||(Input=='Q'))
			{
				clear();
				refresh();
				endwin();
				exit(0);
			}
			else if (Input==' ')
			{
				wmove(BestTimesWin,5,0);
				wclrtobot(BestTimesWin);
				wborder(BestTimesWin,CharSet.Mark,CharSet.Mark,CharSet.Mark,CharSet.Mark,CharSet.Mark,CharSet.Mark,CharSet.Mark,CharSet.Mark);
				CurrentLine=5;
			}
			else
			{
				wclear(BestTimesWin);
				delwin(BestTimesWin);
				clear();
				noutrefresh();
				FreeNode(Head);
				return;
			}
		}
	
		mvwprintw(BestTimesWin,CurrentLine,4,"%s",Current->Username);

		/* Make a copy of the Attributes string. */
		if  ((Buffer=strdup(Current->Attributes))==NULL)
		{
			perror("PrintBestTimes::AttribDup");
			return;
		}

		/* Get the width, delimited by a 'w' on one side, and a 'h' on the other. */
		if ((Decoder=strtok(Buffer,"wh"))==NULL)
		{

		}
		else
		{
			mvwprintw(BestTimesWin,CurrentLine,30-strlen(Decoder),"%s",Decoder);
		}

		/* Get the height, delimited by a 'h' on one side, and a 'm' on the other. */
		if ((Decoder=strtok(NULL,"hm"))==NULL)
		{
			fprintf(stderr,"Invalid entry in %s.\n",DFL_BESTS_FILE);
			return;
		}
		else
		{
			mvwprintw(BestTimesWin,CurrentLine,21-strlen(Decoder),"%s",Decoder);
		}

		/* Get the number of mines, delimited by an 'm' on one side, and a 't' on the other. */
		if ((Decoder=strtok(NULL,"mt"))==NULL)
		{
			fprintf(stderr,"Invalid entry in %s.\n",DFL_BESTS_FILE);
			return;
		}
		else
		{
			mvwprintw(BestTimesWin,CurrentLine,40-strlen(Decoder),"%s",Decoder);
		}

		Time=Current->Time;
		if (Time>=3600)
		{
			/* print time in HH:MM:SSs format. */
			mvwprintw(BestTimesWin,CurrentLine,43-(Time/36000),"%d:%02d:%0ds",Time/3600,(Time%3600)/60,Time%60);
		}
		else if (Time>=60)
		{
			/* print time in MM:SSs format. */
			mvwprintw(BestTimesWin,CurrentLine,45,"%2d:%02ds",Time/60,Time%60);
		}
		else
		{
			/* print time in SSs format. */
			mvwprintw(BestTimesWin,CurrentLine,48,"%02ds",Time);
		}

		mvwprintw(BestTimesWin,CurrentLine,54,"%s",Current->Date);
		
		/* Increment CurrentLine. */
		Current=Current->Next;
		CurrentLine++;
	}

	mvwprintw(BestTimesWin,LINES-1,1,"--Press \'q\' to quit, any other key to continue.--");
	wmove(BestTimesWin,0,0);
	wrefresh(BestTimesWin);
	Input=wgetch(BestTimesWin);
	if ((Input=='q')||(Input=='Q'))
	{
		clear();
		refresh();
		endwin();
		FreeNode(Head);
		exit(0);
	}
	else
	{
		wclear(BestTimesWin);
		delwin(BestTimesWin);
		clear();
		noutrefresh();
	}
	FreeNode(Head);
	return;
}

void DrawCursor(GameStats* Game)
{
/*	mvwaddch(Game->Board,Game->CursorY,(3*Game->CursorX),'<');*/
/*	mvwaddch(Game->Board,Game->CursorY,(3*Game->CursorX)+2,'>');*/
	mvwaddch(Game->Board,(Game->CursorY-Game->FocusY),(3*(Game->CursorX-Game->FocusX)),'<');
	mvwaddch(Game->Board,(Game->CursorY-Game->FocusY),(3*(Game->CursorX-Game->FocusX))+2,'>');
	move(0,0);
	return;
}

void UndrawCursor(GameStats* Game)
{
/*	mvwaddch(Game->Board,Game->CursorY,(3*Game->CursorX),' ');*/
/*	mvwaddch(Game->Board,Game->CursorY,(3*Game->CursorX)+2,' ');*/
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
	if ((Game->LargeBoardX + Game->LargeBoardY)==0)
	{
		wborder(Game->Border,CharSet.VLine,CharSet.VLine,CharSet.HLine,CharSet.HLine,CharSet.ULCorner,CharSet.URCorner,CharSet.LLCorner,CharSet.LRCorner);
		wnoutrefresh(Game->Border);
		return 0;
	}

	/* Now to figure out which way to go... */
	VViewable=(LINES-6);
	HViewable=((COLS-INFO_W-2)/3)-1;

	/* See if the current cursor is even in the focus. */
	if ((Game->CursorY < Game->FocusY) || ((Game->FocusY+VViewable) < Game->CursorY))
	{
		CenterY(Game);
	}
	else if ((Game->CursorY-Game->FocusY) < 2)
	{
		/* Try to pan up. */
		CenterY(Game);
	}
	else if (((Game->FocusY+VViewable)-Game->CursorY) <= 2)
	{
		/* Try to pan down. */
		CenterY(Game);
	}

	if ((Game->CursorX < Game->FocusX) || ((Game->FocusX+HViewable) < Game->CursorX))
	{
		CenterX(Game);
	}
	else if ((Game->CursorX-Game->FocusX) < 2)
	{
		/* Try to pan left. */
		CenterX(Game);
	}
	else if (((Game->FocusX+HViewable)-Game->CursorX) < 2)
	{
		/* Try to pan right. */
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
