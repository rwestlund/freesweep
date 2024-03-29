/**********************************************************************
*  This source code is copyright 1999 by Gus Hartmann & Peter Keller  *
*  It may be distributed under the terms of the GNU General Purpose   *
*  License, version 2 or above; see the file COPYING for more         *
*  information.                                                       *
*                                                                     *
*  $Id: files.c,v 1.10 2003-10-11 20:50:50 hartmann Exp $
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
                                else if (strncasecmp(NameBuffer,"theme",5)==0)
                                {
                                        Value=atoi(ValueBuffer);
                                        (CheckTheme(Value)?Game->Theme=Value:fprintf(stderr,"Invalid value for theme in preference file.\n"));
                                }
                                else if (strncasecmp(NameBuffer,"percent",7)==0)
                                {
                                        Value=atoi(ValueBuffer);
                                        ((CheckPercent(Value)>0)?Game->Percent=Value,Game->NumMines=0:fprintf(stderr,"Invalid value for percent in preference file.\n"));
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
        char *ConfigPath, *Pathname;

        if ((ConfigPath = XDGConfigHome()) == NULL)
        {
                return 1;
        }

        Pathname = (char*)xmalloc(MAX_LINE);
#if defined(HAVE_SNPRINTF)
        snprintf(Pathname, MAX_LINE, "%s/%s", ConfigPath, DFL_PREFS_FILE);
#else
        sprintf(Pathname, "%s/%s", ConfigPath, DFL_PREFS_FILE);
#endif
        free(ConfigPath);

#ifdef DEBUG_LOG
        fprintf(DebugLog, "Loading configuration file.\n  -> %s\n", Pathname);
#endif /* DEBUG_LOG */

        if ((PrefsFile = fopen(Pathname, "r")) == NULL)
        {
#ifdef DEBUG_LOG
                fprintf(DebugLog, "  No configuration found.\n");
#endif /* DEBUG_LOG */
                /* The user has no personal preferences. */
                free(Pathname);
                return 0;
        }
        else if (SourceFile(Game, PrefsFile) == 1)
        {
#ifdef DEBUG_LOG
                fprintf(DebugLog, "  Error reading configuration file.\n");
#endif /* DEBUG_LOG */
                /* An error occurred while reading the file. Try closing it.
                 */
                fclose(PrefsFile);
                free(Pathname);
                return 1;
        }
        else
        {
#ifdef DEBUG_LOG
                fprintf(DebugLog, "  Done.\n");
#endif /* DEBUG_LOG */
                /* Done, so close the file. */
                fclose(PrefsFile);
                free(Pathname);
                return 0;
        }

}

int SourceGlobalFile(GameStats* Game)
{
        FILE* PrefsFile;

        if ((PrefsFile = fopen(GLOBAL_PREFS_FILE, "r")) == NULL)
        {
/*              The global file is invalid or non-existant. */
                fprintf(stderr, "Unable to open the global preferences file %s\n", GLOBAL_PREFS_FILE);
/*              perror("SourceGlobalFile::fopen"); */
                return 1;
        }
        else if (SourceFile(Game,PrefsFile)==1)
        {
/*              An error occurred while reading the file. Try closing it. */
                fclose(PrefsFile);
                return 1;
        }
        else
        {
/*              Done, so close the file. */
                fclose(PrefsFile);
                return 0;
        }
}

int WritePrefsFile(GameStats* Game)
{
        FILE* PrefsFile;
        char *ConfigPath, *Pathname;

        Pathname = (char*)xmalloc(MAX_LINE);
        ConfigPath = XDGConfigHome();

        if ((ConfigPath = XDGConfigHome()) == NULL)
        {
                return 1;
        }

        Pathname = (char*)xmalloc(MAX_LINE);
#if defined(HAVE_SNPRINTF)
        snprintf(Pathname, MAX_LINE, "%s/%s", ConfigPath, DFL_PREFS_FILE);
#else
        sprintf(Pathname, "%s/%s", ConfigPath, DFL_PREFS_FILE);
#endif
        free(ConfigPath);

        if ((PrefsFile = fopen(Pathname, "w")) == NULL)
        {
                perror("WritePrefsFile::fopen");
                free(Pathname);
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
                fprintf(PrefsFile,"Theme=%d\n",Game->Theme);
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
                free(Pathname);
        }
        return 0;
}
