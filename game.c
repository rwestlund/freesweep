/*********************************************************************
* $Id: game.c,v 1.2 1999-02-10 23:49:26 hartmann Exp $
*********************************************************************/

#include "sweep.h"

/* The various Check* functions. Each returns 1 if a valid input, else -1 */

int CheckHeight(int NewVal)
{
	return (((NewVal>2)&&(NewVal<=MAX_H))?1:-1);
}

int CheckWidth(int NewVal)
{
	return (((NewVal>2)&&(NewVal<=MAX_W))?1:-1);
}

int CheckPercent(int NewVal)
{
	return (((NewVal>0)&&(NewVal<100))?1:-1);
}

int CheckRefresh(int NewVal)
{
	return (((NewVal>0)&&(NewVal<=10))?1:-1);
}

int CheckFast(int NewVal)
{
	return (((NewVal==0)||(NewVal==1))?1:-1);
}

int CheckColor(int NewVal)
{
	return (((NewVal==0)||(NewVal==1))?1:-1);
}

int CheckLineDraw(int NewVal)
{
	return (((NewVal==0)||(NewVal==1))?1:-1);
}

/* CheckNumMines is the only member of the check* family that requires
   input besides NewVal. Since the number of mines cannot be greater than
   the size of the field, height and width must be ascertained beforehand.
*/
int CheckNumMines(int NewVal,int Height,int Width)
{
	return (((NewVal>0)&&(NewVal<(Height*Width)))?1:-1);
}

int InitGame(GameStats* Game)
{
	Game->Height=DEFAULT_HEIGHT;
	Game->Width=DEFAULT_WIDTH;
	Game->Percent=DEFAULT_PERCENT;
	Game->Color=DEFAULT_COLOR;
	Game->NumMines=DEFAULT_NUMMINES;
	Game->MarkedMines=0;
	Game->Fast=DEFAULT_FASTSTART;
	Game->Alert=DEFAULT_ALERT;
	Game->LargeBoardX=Game->LargeBoardY=0;
	Game->FocusX=Game->FocusY=0;
	Game->Time=0;
	Game->Status=0;
	InitCharSet(Game,DEFAULT_LINEDRAW);
	return 0;
}

void SetCharSet(int Value)
{
	CharSet.Mine='-';
	CharSet.Space=' ';
	CharSet.Bombed='*';
	if (Value==1)
	{
		CharSet.ULCorner=ACS_ULCORNER;
		CharSet.URCorner=ACS_URCORNER;
		CharSet.LLCorner=ACS_LLCORNER;
		CharSet.LRCorner=ACS_LRCORNER;
		CharSet.HLine=ACS_HLINE;
		CharSet.VLine=ACS_VLINE;
		CharSet.UArrow=ACS_UARROW;
		CharSet.DArrow=ACS_DARROW;
		CharSet.RArrow=ACS_RARROW;
		CharSet.LArrow=ACS_LARROW;
		CharSet.Mark=ACS_DIAMOND;
		CharSet.FalseMark=ACS_DIAMOND;
	}
	else
	{
		CharSet.ULCorner='+';
		CharSet.URCorner='+';
		CharSet.LLCorner='+';
		CharSet.LRCorner='+';
		CharSet.HLine='-';
		CharSet.VLine='|';
		CharSet.UArrow='^';
		CharSet.DArrow='v';
		CharSet.RArrow='>';
		CharSet.LArrow='<';
		CharSet.Mark='#';
		CharSet.FalseMark='#';
	}
	return;
}

void SwitchCharSet(GameStats* Game)
{
	if (Game->LineDraw==0)
	{
		Game->LineDraw=1;
		SetCharSet(1);
	}
	else
	{
		Game->LineDraw=0;
		SetCharSet(0);
	}
	return;
}

void InitCharSet(GameStats* Game, int Value)
{
	if (Value==1)
	{
		Game->LineDraw=1;
		SetCharSet(1);
	}
	else
	{
		Game->LineDraw=0;
		SetCharSet(0);
	}
	return;
}

int ReadyGame(GameStats* Game)
{
	int MinesToSet=0, RandX=0, RandY=0, VViewable=0, HViewable=0;
	unsigned char CellVal;

	VViewable=(LINES-6);
	HViewable=((COLS-INFO_W-2)/3);

	if ((Game->Field=calloc((Game->Height*(
		( Game->Width % 2 ? (Game->Width) +1 : Game->Width )))/2,
		sizeof(char)))<=0)
	{
		perror("ReadyGame::AllocField");
		exit(EXIT_FAILURE);
	}
	if (Game->Percent!=0)
	{
		MinesToSet=(Game->Percent*Game->Width*Game->Height)/100;
	}
	else
	{
		MinesToSet=Game->NumMines;
	}

	/* Yeah, I know it's a crappy way to get a random number. */
	srand(time(NULL));

	while (MinesToSet>0)
	{
		RandX=rand()%Game->Width;
		RandY=rand()%Game->Height;
		GetMine(RandX,RandY,CellVal);

		/* This is gonna be ugly... */
		if (!( (abs((((Game->Width)/2)-RandX))<2) && (abs((((Game->Height)/2)-RandY))<2)))
		{
			if (CellVal!=MINE)
			{
				SetMine(RandX,RandY,MINE);
				MinesToSet--;
			}
		}
	}

	Game->CursorX=(Game->Width-1)/2;
	Game->CursorY=Game->Height/2;

	if ((COLS-INFO_W)>=((3*Game->Width)+2))
	{
		Game->LargeBoardX=0;
	}
	else
	{
		Game->LargeBoardX=1;
	}
	
	if ((LINES-4)>=(Game->Height+2))
	{
		Game->LargeBoardY=0;
	}
	else
	{
		Game->LargeBoardY=1;
	}

	/* Determine the correct size of the border window, and allocate it */
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

	return 0;
}

int SourceEnv(GameStats* Game)
{
	int Value;
	char* ValueBuffer;

	if ((ValueBuffer=getenv("SWEEP_COLOR"))!=NULL)
	{
		if (strncasecmp(ValueBuffer,"True",4)==0)
		{
			Game->Color=1;
		}
		else if (strncasecmp(ValueBuffer,"False",5)==0)
		{
			Game->Color=0;
		}
		else
		{
			fprintf(stderr,"Invalid value \"%s\" for $SWEEP_COLOR.\n",ValueBuffer);
			fprintf(stderr,"$SWEEP_COLOR must be either true or false.\n");
		}
	}

	if ((ValueBuffer=getenv("SWEEP_FASTSTART"))!=NULL)
	{
		if (strncasecmp(ValueBuffer,"True",4)==0)
		{
			Game->Fast=1;
		}
		else if (strncasecmp(ValueBuffer,"False",5)==0)
		{
			Game->Fast=0;
		}
		else
		{
			fprintf(stderr,"Invalid value \"%s\" for $SWEEP_FASTSTART.\n",ValueBuffer);
			fprintf(stderr,"$SWEEP_FASTSTART must be either true or false.\n");
		}
	}

	if ((ValueBuffer=getenv("SWEEP_HEIGHT"))!=NULL)
	{
		Value=atoi(ValueBuffer);
		((CheckHeight(Value)>0)?Game->Height=Value:fprintf(stderr,"Invalid value for Height in $SWEEP_HEIGHT.\n"));
	}

	if ((ValueBuffer=getenv("SWEEP_LINEDRAW"))!=NULL)
	{
		if (strncasecmp(ValueBuffer,"True",4)==0)
		{
			Game->LineDraw=1;
		}
		else if (strncasecmp(ValueBuffer,"False",5)==0)
		{
			Game->LineDraw=0;
		}
		else
		{
			fprintf(stderr,"Invalid value \"%s\" for $SWEEP_LINEDRAW.\n",ValueBuffer);
			fprintf(stderr,"$SWEEP_LINEDRAW must be either true or false.\n");
		}
	}

	if ((ValueBuffer=getenv("SWEEP_PERCENT"))!=NULL)
	{
		Value=atoi(ValueBuffer);
		((CheckPercent(Value)>0)?Game->Percent=Value:fprintf(stderr,"Invalid value \"%s\" for Percent in $SWEEP_PERCENT.\n",ValueBuffer)); 
	}

	if ((ValueBuffer=getenv("SWEEP_MINES"))!=NULL)
	{
		Value=atoi(ValueBuffer);
		((CheckNumMines(Value,Game->Height,Game->Width)>0)?Game->NumMines=Value:fprintf(stderr,"Invalid value \"%s\" for NumMines in $SWEEP_MINES.\n",ValueBuffer));
	}
	
	if ((ValueBuffer=getenv("SWEEP_WIDTH"))!=NULL)
	{
		Value=atoi(ValueBuffer);
		((CheckWidth(Value)>0)?Game->Width=Value:fprintf(stderr,"Invalid value \"%s\" for Width in $SWEEP_WIDTH.\n",ValueBuffer));
	}

	return 0;
}

int ParseArgs(GameStats* Game, int Argc, char** Argv)
{
	int Value=0, Opt=0, SaveFlag=0, FastFlag=0, QueryFlag=0, ErrorFlag=0, BestTimesFlag=0, DumpFlag=0;

	/* Clear the error flag. */
	opterr=0;

	/* Parse the command line options. */
	while ((Opt=getopt(Argc,Argv,"%:abdfh:ilm:r:svw:"))!=EOF)
	{
		switch (Opt)
		{
			case '%':
				/* Set percent to optarg */
				Value=atoi(optarg);
				((CheckPercent(Value)>=0)?Game->Percent=Value:fprintf(stderr,"Invalid value for percent.\n"));
				break;
			case 'a':
				SwitchCharSet(Game);
				break;
			case 'b':
				BestTimesFlag++;
				break;
			case 'd':
				DumpFlag++;
				break;
			case 'f':
				FastFlag++;
				break;
			case 'h':
				/* Set height to optarg */
				Value=atoi(optarg);
				((CheckHeight(Value)>=0)?Game->Height=Value:fprintf(stderr,"Invalid value for height.\n"));
				break;
			case 'i':
				QueryFlag++;
				break;
			case 'm':
				/* Set nummines to optarg */
				Value=atoi(optarg);
				((CheckNumMines(Value,Game->Height,Game->Width)>=0)?Game->NumMines=Value:fprintf(stderr,"Invalid value for number of mines.\n"));
				break;
			case 's':
				/* Set the save flag */
				SaveFlag++;
				break;
			case 'v':
				printf("Freesweep version %s by Gus Hartmann (hartmann@cs.wisc.edu).\n",VERSION);
				printf("Compiled for %s on %s at %s by %s\n.",SYSTEM_TYPE,__DATE__,__TIME__,USER);
				exit(EXIT_SUCCESS);
				break;
			case 'w':
				/* Set width to optarg */
				Value=atoi(optarg);
				((CheckWidth(Value)>=0)?Game->Width=Value:fprintf(stderr,"Invalid value for width.\n"));
				break;
			case '?':
			default:
				ErrorFlag++;
				break;
		}
		Value=0;
	}
	/* Make sure there aren't any more arguments. */
	/* Also insure that there was not more than one -s was passed. */
	if (SaveFlag>1)
	{
		fprintf(stderr,"Only one -s can be specified.\n");
		ErrorFlag++;
	}

	if ((BestTimesFlag + DumpFlag) > 1)
	{
		fprintf(stderr,"Only one -b or -d can be specified.\n");
		ErrorFlag++;
	}

	if ((FastFlag+QueryFlag)>1)
	{
		fprintf(stderr,"Only one -f or -i can be specified.\n");
		ErrorFlag=1;
	}

	if (optind!=Argc)
	{
		fprintf(stderr,"Non-option arguments are invalid.\n");
		ErrorFlag++;
	}
	else if (FastFlag!=0)
	{
		Game->Fast=1;
	}
	else if (QueryFlag!=0)
	{
		Game->Fast=0;
	}

	if (ErrorFlag!=0)
	{
		fprintf(stderr,"Usage:\n  freesweep: [-%% percent][-a][-b|-d][-f|-i][-h height][-m mines][-s][-v]\n    [-w width]\n");
		exit(EXIT_FAILURE);
	}

	if (BestTimesFlag==1)
	{
		StartCurses();
		InitCharSet(Game,Game->LineDraw);
		PrintBestTimes(NULL);
		clear();
		noutrefresh();
		doupdate();
		endwin();
		exit(EXIT_SUCCESS);
	}

	if (DumpFlag==1)
	{
		DumpNodeFile(NULL);
		exit(EXIT_SUCCESS);
	}

	if (SaveFlag==1)
	{
		WritePrefsFile(Game);
	}
	return 0;

}

/* DumpGame() prints a great deal of good information about the current Game*
	to the DebugLog. It prints nothing to any other file descriptor. Calling
	it without having defined DEBUG_LOG is utterly useless. */
void DumpGame(GameStats* Game)
{
#ifdef DEBUG_LOG
	fprintf(DebugLog,"-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n");
	fprintf(DebugLog,"Height is %d, Width is %d\n",Game->Height,Game->Width);
	fprintf(DebugLog,"Percent is %d, NumMines is %d\n",Game->Percent,Game->NumMines);
	fprintf(DebugLog,"MarkedMines is %d\n",Game->MarkedMines);
	fprintf(DebugLog,"Fast is %d, LineDraw is %d\n",Game->Fast,Game->LineDraw);
	fprintf(DebugLog,"CursorY is %d, CursorX is %d\n",Game->CursorY,Game->CursorX);
	fprintf(DebugLog,"LargeBoardY is %d, LargeBoardX is %d\n",Game->LargeBoardY,Game->LargeBoardX);
	fprintf(DebugLog,"FocusY is %d, FocusX is %d\n",Game->FocusY,Game->FocusX);
	fprintf(DebugLog,"Alert is %d (",Game->Alert);
	switch (Game->Alert)
	{
		case NO_ALERT:
			fprintf(DebugLog,"None)\n");
			break;
		case FLASH:
			fprintf(DebugLog,"Flash)\n");
			break;
		case BEEP: default:
			fprintf(DebugLog,"Beep)\n");
			break;
	}
	fprintf(DebugLog,"Field is %p\n",Game->Field);
	fprintf(DebugLog,"Border is %p\n",Game->Border);
	fprintf(DebugLog,"Board is %p\n",Game->Board);
	fprintf(DebugLog,"-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n");
	fflush(DebugLog);
#endif /* DEBUG_LOG */
	return;
}
