/**********************************************************************
*  This source code is copyright 1999 by Gus Hartmann & Peter Keller  *
*  It may be distributed under the terms of the GNU General Purpose   *
*  License, version 2 or above; see the file COPYING for more         *
*  information.                                                       *
*                                                                     *
*  $Id: files.c,v 1.8 1999-08-09 05:25:35 hartmann Exp $
*                                                                     *
**********************************************************************/

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

	Pathname=(char*)xmalloc(MAX_LINE);

	if ((Buffer=getenv("HOME"))==NULL)
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
		free(Pathname);
		return 0;
	}
	else if (SourceFile(Game,PrefsFile)==1)
	{
/*		An error occurred while reading the file. Try closing it. */
		fclose(PrefsFile);
		free(Pathname);
		return 1;
	}
	else
	{
/*		Done, so close the file. */
		fclose(PrefsFile);
		free(Pathname);
		return 0;
	}

}

int SourceGlobalFile(GameStats* Game)
{
	FILE* PrefsFile;

	if ((PrefsFile=fopen(GLOBAL_PREFS_FILE,"r"))==NULL)
	{
/*		The global file is invalid or non-existant. */
		fprintf(stderr,"Unable to open the global prefernces file %s\n", GLOBAL_PREFS_FILE);
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
