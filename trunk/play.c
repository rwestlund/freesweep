/*********************************************************************
* $Id: play.c,v 1.2 1999-02-10 23:49:27 hartmann Exp $
*********************************************************************/

#include "sweep.h"

static int LastNum;
static int LastKey;

int GetInput(GameStats* Game)
{
	unsigned char RetVal;
	int Multiplier=1;
	int UserInput=0;

#ifdef SWEEP_MOUSE
	MEVENT MouseInput;
#endif /* SWEEP_MOUSE */

	UserInput=getch();
	
	/* Check to see if it's a number; if so build up the multiplier. */
	/* '0' isn't a valid first digit, since it means "go to the origin" */
	/* '0' will be valid once there is a different first digit. */
	if ((UserInput > '0')&&(UserInput <= '9'))
	{
		Multiplier=(UserInput-'0');
		UserInput=getch();
		while ((UserInput >= '0')&&(UserInput <= '9'))
		{
			Multiplier=((Multiplier*10)+(UserInput-'0'));
			UserInput=getch();
		}
	}

	if (UserInput=='.')
	{
		UserInput=LastKey;
		Multiplier=(Multiplier*LastNum);
	}
	else
	{
		LastKey=UserInput;
		LastNum=Multiplier;
	}

	switch (UserInput)
	{
		/* Going Left? */
		case 'h':
		case KEY_LEFT:
			MoveLeft(Game,Multiplier);
			break;

		/* Going Down? */
		case 'j':
		case KEY_DOWN:
			MoveDown(Game,Multiplier);
			break;

		/* Going Up? */
		case 'k':
		case KEY_UP:
			MoveUp(Game,Multiplier);
			break;

		/* Going Right? */
		case 'l':
		case KEY_RIGHT:
			MoveRight(Game,Multiplier);
			break;


		/* Diagonals. */
		case KEY_A1:
			MoveUp(Game,Multiplier);
			MoveLeft(Game,Multiplier);
			break;

		case KEY_A3:
			MoveUp(Game,Multiplier);
			MoveRight(Game,Multiplier);

			break;

		case KEY_C1:
			MoveDown(Game,Multiplier);
			MoveLeft(Game,Multiplier);
			break;

		case KEY_C3:
			MoveDown(Game,Multiplier);
			MoveRight(Game,Multiplier);
			break;

		/* The non-motion keys. */
		/* The accepted values for flagging a space as a mine. */
		case 'f':
		case 'F':
		case KEY_B2:
			if (Multiplier!=1)
			{
				SweepError("Can only mark the mine once.");
				Multiplier=1;
			}
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
					Game->MarkedMines--;
					break;
				case UNKNOWN:
					SetMine(Game->CursorX,Game->CursorY,BAD_MARK);
					Game->MarkedMines++;
					break;
				default:
					SweepError("Cannot mark as a mine.");
					break;
			}
			break;

		/* The accepted keys to expose a space. */
		case ' ':
		case KEY_IC:
			if (Multiplier!=1)
			{
				SweepError("Can only expose the square once.");
				Multiplier=1;
			}
			GetMine(Game->CursorX,Game->CursorY,RetVal);
			switch (RetVal)
			{
				case BAD_MARK:
				case MARKED:
					SweepError("Cannot expose a flagged mine.");
					break;
				case MINE:
					/* BOOM! */
					Boom();
					SetMine(Game->CursorX,Game->CursorY,DETONATED);
					CharSet.FalseMark='x';
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
		case 'R':
		case KEY_REFRESH:
		case '\f':
			if (Multiplier!=1)
			{
				SweepError("Can only refresh the display once.");
				Multiplier=1;
			}

			break;

		/* The accepted keys to display the help screen. */
		case '?':
		case 'm':
		case 'M':
		case '/':
		case KEY_HELP:
			if (Multiplier!=1)
			{
				SweepError("Can only display help screen once.");
				Multiplier=1;
			}
			Help();
			PrintInfo();
			RedrawErrorWin();
			break;

		/* The accepted keys to display the best times screen. */
		case 'b':
			if (Multiplier!=1)
			{
				SweepError("Can only display best times once.");
				Multiplier=1;
			}
			PrintBestTimes(NULL);
			PrintInfo();
			RedrawErrorWin();
			noutrefresh();
			doupdate();
			break;

		/* The accepted values to suspend the game. */
		case KEY_SUSPEND:
		case 'z':
		case 'Z':
			if (Multiplier!=1)
			{
				SweepError("Can only suspend the program once.");
				Multiplier=1;
			}

			break;

		case 'q':
		case 'Q':
			if (Multiplier!=1)
			{
				SweepError("Can only quit once.");
				Multiplier=1;
			}
			/* FOO! */
			/* Make a node for now. */
/*			AddNodeToFile(NULL,GenerateFakeNode());*/
/*			AddNodeToFile(NULL,InitNode(Game));*/
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
			if (Multiplier!=1)
			{
				SweepError("Can only return to the origin once.");
				Multiplier=1;
			}
			Game->CursorY=0;
			Game->CursorX=0;
			break;

		case '$':
			if (Multiplier!=1)
			{
				SweepError("Can only go to the end once.");
				Multiplier=1;
			}
			Game->CursorY=Game->Height-1;
			Game->CursorX=Game->Width-1;
			break;

		case 'a':
			/* FOO - needs to redraw everything. */
			if (Multiplier!=1)
			{
				if ((Multiplier%2)!=0)
				{
					SweepError("Toggling character set.");
					SwitchCharSet(Game);
					PrintInfo();
					RedrawErrorWin();
				}
				else
				{
					SweepError("Not toggling character set.");
				}
			}
			else
			{
				SwitchCharSet(Game);
				PrintInfo();
				RedrawErrorWin();
			}
			break;

		case 'd':
		case 'D':
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

#ifdef DEBUG_LOG
		case '|':
			fprintf(DebugLog,"Entering cheat mode.\n");
			CharSet.Mine='+';
			break;
#endif /* DEBUG_LOG */

		case ERR:
#ifdef DEBUG_LOG
			fprintf(DebugLog,"Got no keystroke.\n");
#endif /* DEBUG_LOG */
			return 1;
			break;

		default:
#ifdef DEBUG_LOG
			fprintf(DebugLog,"Got unknown keystroke: %c\n", UserInput);
#endif /* DEBUG_LOG */
			LastNum=1;
			LastKey=ERR;
			SweepError("Bad Input.");
			return 1;
			break;
	}

	LastNum=Multiplier;
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

void Boom()
{
	WINDOW* BoomWin;

	if ((BoomWin=newwin(LINES/3,COLS/3,LINES/3,COLS/3))==NULL)
	{
		perror("Boom::Alloc Window");
		return;
	}

	if (wborder(BoomWin,CharSet.Mark,CharSet.Mark,CharSet.Mark,CharSet.Mark,CharSet.Mark,CharSet.Mark,CharSet.Mark,CharSet.Mark)!=OK)
	{
		perror("Boom::Draw Border");
		return;
	}

	mvwprintw(BoomWin,(LINES/6)-1,(COLS-15)/6,"Boom!");

	wrefresh(BoomWin);
	napms(1000);
	wclear(BoomWin);
	wnoutrefresh(BoomWin);
	delwin(BoomWin);

	return;
}
