/********************************************************************* * $Id: files.c,v 1.1.1.1 1998-11-23 03:57:08 hartmann Exp $
*********************************************************************/

#include "sweep.h"

int SourceFile(GameStats* Game,FILE* PrefsFile)
{
	char RawBuffer[MAX_LINE];
	char *NameBuffer, *ValueBuffer;
	int Value=0;

	while (feof(PrefsFile)==0)
	{
		if (fgets(RawBuffer,MAX_LINE,PrefsFile)==0)
		{
			return 0;
		}
		if ((NameBuffer=strtok(RawBuffer,DELIMITERS))!=0)
		{
			/* This is essentially a great big switch statement. */
			if ((ValueBuffer=strtok(NULL,DELIMITERS))!=0)
			{
				if (strncasecmp(NameBuffer,"color",5)==0)
				{
					Value=atoi(ValueBuffer);
					((CheckColor(Value)>0)?Game->Color=Value:fprintf(stderr,"Invalid value for color in preference file.\n"));
				}
				else if (strncasecmp(NameBuffer,"faststart",9)==0)
				{
					Value=atoi(ValueBuffer);
					((CheckFast(Value)>0)?Game->Fast=Value:fprintf(stderr,"Invalid value for faststart in preference file.\n"));
				}
				else if (strncasecmp(NameBuffer,"height",6)==0)
				{
					Value=atoi(ValueBuffer);
					((CheckHeight(Value)>0)?Game->Height=Value:fprintf(stderr,"Invalid value for height in preference file.\n"));
				}
				else if (strncasecmp(NameBuffer,"linedraw",8)==0)
				{
					Value=atoi(ValueBuffer);
					((CheckLineDraw(Value)>0)?InitCharSet(Game,Value):fprintf(stderr,"Invalid value for linedraw in preference file.\n"));
				}
				else if (strncasecmp(NameBuffer,"percent",7)==0)
				{
					Value=atoi(ValueBuffer);
					((CheckPercent(Value)>0)?Game->Percent=Value,Game->NumMines=0:fprintf(stderr,"Invaid value for percent in preference file.\n"));
				}
				else if (strncasecmp(NameBuffer,"mines",5)==0)
				{
					Value=atoi(ValueBuffer);
					((CheckNumMines(Value,Game->Height,Game->Width)>0)?Game->NumMines=Value,Game->Percent=0:fprintf(stderr,"Invalid value for mines in preference file.\n"));
				}
				else if (strncasecmp(NameBuffer,"width",5)==0)
				{
					Value=atoi(ValueBuffer);
					((CheckWidth(Value)>0)?Game->Width=Value:fprintf(stderr,"Invalid value for width in preference file.\n"));
				}
				else if (strncasecmp(NameBuffer,"alert",5)==0)
				{
					/* This option takes different strings as arguments. */
					if (strncasecmp(ValueBuffer,"beep",4)==0)
					{
						Game->Alert=BEEP;
					}
					else if (strncasecmp(ValueBuffer,"flash",5)==0)
					{
						Game->Alert=FLASH;
					}
					else if (strncasecmp(ValueBuffer,"none",4)==0)
					{
						Game->Alert=NO_ALERT;
					}
					else
					{
						fprintf(stderr,"Bad value for Alert in preference file.\n");
					}
				}
				else if (strncasecmp(NameBuffer,"#",1)!=0)
				{
					fprintf(stderr,"Unknown tag %s in file.\n",NameBuffer);
				}
			}
		}
	}
	return 0;
}

int SourceHomeFile(GameStats* Game)
{
	FILE* PrefsFile;
	char *Buffer, *Pathname;

	if ((Pathname=malloc(MAX_LINE))==NULL)
	{
		perror("SourceHomeFile::malloc");
		return 1;
	}
	else if ((Buffer=getenv("HOME"))==NULL)
	{
		perror("SourceHomeFile::getenv");
		return 1;
	}
	strcpy(Pathname,Buffer);
	strcat(Pathname,"/");
	strcat(Pathname,DFL_PREFS_FILE);

	if ((PrefsFile=fopen(Pathname,"r"))==NULL)
	{
		/* The user has no personal preferences. */
		/* Try sourcing the older preferences files. */
		return OldPrefsFile(Game);
	}
	else if (SourceFile(Game,PrefsFile)==1)
	{
/*		An error occurred while reading the file. Try closing it. */
		fclose(PrefsFile);
		return 1;
	}
	else
	{
/*		Done, so close the file. */
		fclose(PrefsFile);
		return 0;
	}
}

int SourceGlobalFile(GameStats* Game)
{
	FILE* PrefsFile;

	if ((PrefsFile=fopen(GLOBAL_PREFS_FILE,"r"))==NULL)
	{
/*		The global file is invalid or non-existant. */
		fprintf(stderr,"Unable to open the global prefernces file GLOBAL_PREFS_FILE\n");
/*		perror("SourceGlobalFile::fopen"); */
		return 1;
	}
	else if (SourceFile(Game,PrefsFile)==1)
	{
/*		An error occurred while reading the file. Try closing it. */
		fclose(PrefsFile);
		return 1;
	}
	else
	{
/*		Done, so close the file. */
		fclose(PrefsFile);
		return 0;
	}
}

int WritePrefsFile(GameStats* Game)
{
	FILE* PrefsFile;
	char *Buffer, Pathname[MAX_LINE+1];

	if ((Buffer=getenv("HOME"))==NULL)
	{
		perror("WriteHomeFile::getenv");
		return 1;
	}

	strcpy(Pathname,Buffer);
	strcat(Pathname,"/");
	strcat(Pathname,DFL_PREFS_FILE);

	if ((PrefsFile=fopen(Pathname,"w"))==NULL)
	{
		perror("WritePrefsFile::fopen");
		return 1;
	}
	else
	{
		fprintf(PrefsFile,"# Freesweep version %s\n",VERSION);
		fprintf(PrefsFile,"# This is a generated file, but you can edit it if you like.\n");
		fprintf(PrefsFile,"Height=%d\n",Game->Height);
		fprintf(PrefsFile,"Width=%d\n",Game->Width);
		if (Game->NumMines==0)
		{
			fprintf(PrefsFile,"Percent=%d\n",Game->Percent);
		}
		else
		{
			fprintf(PrefsFile,"Mines=%d\n",Game->NumMines);
		}
		fprintf(PrefsFile,"FastStart=%d\n",Game->Fast);
		fprintf(PrefsFile,"LineDraw=%d\n",Game->LineDraw);
		switch (Game->Alert)
		{
			case BEEP:
				fprintf(PrefsFile,"Alert=Beep\n");
				break;
			case FLASH:
				fprintf(PrefsFile,"Alert=Flash\n");
				break;
			case NO_ALERT:
				fprintf(PrefsFile,"Alert=None\n");
				break;
			default:
				break;
		}

		fclose(PrefsFile);
	}
	return 0;
}

/* OldPrefsFile() is a desperate attempt to live down the horrid file
   format of the early versions. It reads the older file, saves it in
   the new format, and then deletes the older file.
*/
int OldPrefsFile(GameStats* Game)
{
	FILE* PrefsFile;
	char Buffer[MAX_LINE+1], PathBuffer[MAX_LINE];
	char* Pathname;
	int Value=0;

	if ((Pathname=getenv("HOME"))==NULL)
	{
		perror("OldPrefsFile::getenv");
		return 1;
	}
	strcpy(PathBuffer,Pathname);
	strcat(PathBuffer,"/.minesrc");

	if ((PrefsFile=fopen(PathBuffer,"r"))>0)
	{
		/* Source the file for preferences. */
		while ((fgets(Buffer,MAX_LINE,PrefsFile)!=0)&&(Buffer[0]!='~'))
		{
			switch (Buffer[0])
			{
				case '#':
					/* Ignore the entire line. */
					break;
				case '%':
					Buffer[0]=' ';
					Value=atoi(Buffer);
					((CheckPercent(Value)>0)?Game->Percent=Value,Game->NumMines=0:fprintf(stderr,"Invaid value for percent.\n"));
					break;
				case 'a':
					Buffer[0]=' ';
					Value=atoi(Buffer);
					Value=(Value==0?1:0);
					((CheckLineDraw(Value)>0)?InitCharSet(Game,Value):fprintf(stderr,"Invalid value for linedraw.\n"));
					break;
				case 'c':

					break;
				case 'h':
					Buffer[0]=' ';
					Value=atoi(Buffer);
					((CheckHeight(Value)>0)?Game->Height=Value:fprintf(stderr,"Invalid value for height.\n"));
					break;
				case 'm':
					Buffer[0]=' ';
					Value=atoi(Buffer);
					((CheckNumMines(Value,Game->Height,Game->Width)>0)?Game->NumMines=Value,Game->Percent=0:fprintf(stderr,"Invalid value for mines.\n"));
					break;
				case 'w':
					Buffer[0]=' ';
					Value=atoi(Buffer);
					((CheckWidth(Value)>0)?Game->Width=Value:fprintf(stderr,"Invalid value for width.\n"));
					break;
				default:
					/* Just in case something unexpected happens. */
					break;
			}
		}
		/* Write the preferences in the new file format. */
		if (((WritePrefsFile(Game))+(MigrateBestTimes(PrefsFile)))==0)
		{
			fclose(PrefsFile);
			unlink(PathBuffer);
			return 0;
		}
		else
		{
			fprintf(stderr,"Error converting old preference file.\n");
#ifdef DEBUG_LOG
			fprintf(DebugLog,"Error converting old preference file.\n");
#endif /* DEBUG_LOG */
			return 1;
		}
	}
	else
	{
		perror("SourceHomeFile::Open old file");
#ifdef DEBUG_LOG
		fprintf(DebugLog,"Unable to open old prefernce file %s.\n",PathBuffer);
#endif /* DEBUG_LOG */
		return 1;
	}
}
