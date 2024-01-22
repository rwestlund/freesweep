/**********************************************************************
*  This source code is copyright 1999 by Gus Hartmann & Peter Keller  *
*  It may be distributed under the terms of the GNU General Purpose   *
*  License, version 2 or above; see the file COPYING for more         *
*  information.                                                       *
*                                                                     *
*  $Id: pbests.c,v 1.30 2003-10-11 20:50:50 hartmann Exp $
*                                                                     *
**********************************************************************/

#include "sweep.h"
#include <pwd.h>

static void BFDSort(struct BestFileDesc *bfd);
static void InsertEntry(struct BestFileDesc *bfd, struct BestEntry *n);
static void SaveBestTimesFile(struct BestFileDesc *bfd, char *filename);
static struct BestEntry* NewBestEntry(GameStats *Game);
static int BECmpFunc(const void *l, const void *r);
static void tlockf(FILE *fp, char * name);
static void tunlockf(FILE *fp);

extern int errno;

/* need a simple line buffer */
struct MBuf
{
        unsigned int len;
        char *buf;
};

/* the one function that does it all */
void UpdateBestTimesFile(GameStats *Game, char *filename)
{
        struct BestFileDesc *bfd = NULL;
        struct BestEntry *b = NULL;

        if (!Game->Cheated)
        {
                bfd = NewBFD();
                b = NewBestEntry(Game);


                DumpGame(Game);

                LoadBestTimesFile(bfd, filename);

                DumpBFD(bfd, FALSE);
                fflush(NULL);

                InsertEntry(bfd, b);

                DumpBFD(bfd, TRUE);
                fflush(NULL);

                BFDSort(bfd);

                DumpBFD(bfd, TRUE);
                fflush(NULL);

                SaveBestTimesFile(bfd, filename);
                fflush(NULL);

                free(b);
                free(bfd->ents);
                free(bfd);
        }
}

/* construct a new best file descriptor */
struct BestFileDesc* NewBFD()
{
        struct BestFileDesc *bfd = NULL;

        bfd = (struct BestFileDesc*)xmalloc(sizeof(struct BestFileDesc) * 1);

        bfd->ents = NULL;
        bfd->numents = 0;
        bfd->alloced = 0;
        bfd->sorted = TRUE;

        return bfd;
}

/* summon from the depths of the abyss the best times file */
void LoadBestTimesFile(struct BestFileDesc *bfd, char *filename)
{
        FILE *fp = NULL;

#ifdef DEBUG_LOG
	fprintf(DebugLog, "Readings best times file.\n  -> %s\n", filename);
#endif

        fp = fopen(filename, "r+");
        if (fp != NULL)
        {
                struct BestEntry b;

                tlockf(fp, filename);

                while (!feof(fp))
                {
                        fscanf(fp, "%u,%u,%u,%u,%lu",
                               &b.user, &b.area, &b.mines, &b.time,
                               (unsigned long *)&b.date);

                        InsertEntry(bfd, &b);
                }

                tunlockf(fp);
                fclose(fp);	/* you just try! */
        }

        BFDSort(bfd);
}

void BFDSort(struct BestFileDesc *bfd)
{
	if (bfd->sorted == TRUE)
	{
		/* I just replaced a node, everything is still sorted */
		return;
	}
	else
	{
		/* qsort the whole mess */
#ifdef DEBUG_LOG
                fprintf(DebugLog, "Sorting the best times.\n");
#endif
		qsort(bfd->ents, bfd->numents, sizeof(bfd->ents[0]), BECmpFunc);
                bfd->sorted = TRUE;
	}
}

/* the Best Entry comparison function */
int BECmpFunc(const void *l, const void *r)
{
	struct BestEntry *bl, *br;

	bl = (struct BestEntry*)l;
	br = (struct BestEntry*)r;

	if (bl->area < br->area)
	{
		return 1;
	}
	else if (bl->area > br->area)
	{
		return -1;
	}
	else if (bl->area == br->area)
	{
		if (bl->mines < br->mines)
		{
			return 1;
		}
		else if (bl->mines > br->mines)
		{
			return -1;
		}
		else if (bl->mines == br->mines)
		{
			if (bl->time < br->time)
			{
				return 1;
			}
			else if (bl->time > br->time)
			{
				return -1;
			}
			else if (bl->time == br->time)
			{
				return 0;
			}
		}
	}

	/* shut the compiler up */
	/* this never happens */
	return 0;
}

void InsertEntry(struct BestFileDesc *bfd, struct BestEntry *n)
{
	int replaced = FALSE;
	unsigned int i = 0;

	/* search until I find a match */
	for (i = 0; i < bfd->numents; i++)
	{
                if (n->user == bfd->ents[i].user &&
                    n->area == bfd->ents[i].area &&
                    n->mines == bfd->ents[i].mines)
	        {
                        /* yup, replace it and mark the flag */
                        if (n->time < bfd->ents[i].time)
                        {
                                /* Only update if the time is better */
                                bfd->ents[i].time = n->time;
                                bfd->ents[i].date = n->date;
                        }
                        replaced = TRUE;
                }
	}

	if (replaced != TRUE)
	{
                if (bfd->alloced == 0)
                {
                        bfd->ents = xmalloc(sizeof(struct BestEntry) * 5);
                        bfd->alloced += 5;
                }
                else if (bfd->numents >= bfd->alloced)
                {
                        struct BestEntry *orig = bfd->ents;
                        bfd->ents = xmalloc(sizeof(struct BestEntry) *
                                            (bfd->alloced + 5));

                        memcpy(bfd->ents, orig, bfd->alloced);
                        free(orig);
                        bfd->alloced += 5;
                }

		/* use the x-tra one I got initially */
		bfd->ents[bfd->numents].area = n->area;
		bfd->ents[bfd->numents].mines = n->mines;
		bfd->ents[bfd->numents].time = n->time;
		bfd->ents[bfd->numents].user = n->user;
		bfd->ents[bfd->numents].date = n->date;
                bfd->numents++;

                bfd->sorted = FALSE;
	}
}

void SaveBestTimesFile(struct BestFileDesc *bfd, char *name)
{
	FILE *fp = NULL;
        struct BestEntry *b = NULL;
        unsigned int i;

#ifdef DEBUG_LOG
	fprintf(DebugLog, "Writing best times file.\n  -> %s\n", name);
#endif

	if ((fp = fopen(name, "w")) != NULL)
        {
                tlockf(fp, name);

                /* convert the bfd to a mess and write it out */
                for (i = 0; i < bfd->numents; i++)
                {
                        b = &bfd->ents[i];
                        fprintf(fp, "%u,%u,%u,%u,%u\n",
                                b->user, b->area, b->mines, b->time,
                                (unsigned int)b->date);
                }

                tunlockf(fp);
                fclose(fp);
        }
}

struct BestEntry* NewBestEntry(GameStats *Game)
{
	struct BestEntry *b = NULL;

	b = (struct BestEntry*)xmalloc(sizeof(struct BestEntry) * 1);

	/* fill in some attributes */
	b->area = Game->Height * Game->Width;
	b->mines = Game->NumMines;
	b->time = Game->Time;
        b->user = getuid();
        b->date = time(NULL);

	return b;
}

/* Full Path To Best Times File in HOME */
char* FPTBTF(void)
{
	char *home = NULL;
	char *fp = NULL;

	home = XDGDataHome();
	if (home == NULL)
	{
		SweepError("You don't have a home dir!");
		/* XXX fix this up */
		exit(EXIT_FAILURE);
	}

	/* get me some memory for the string */
	fp = (char*)xmalloc(strlen(home) + strlen(DFL_BESTS_FILE) + 2);

	/* make the full path */
#if defined(HAS_SNPRINTF)
	snprintf(fp, strlen(home) + strlen(DFL_BESTS_FILE) + 2, "%s/%s", home, DFL_BESTS_FILE);
#else
	sprintf(fp, "%s/%s", home, DFL_BESTS_FILE);
#endif
        free(home);

	return fp;
}

#if defined USE_GROUP_BEST_FILE
/* full path to group best times file */
char* FPTGBTF(void)
{
	char *fp = NULL;

	/* get me some memory for the string */
	fp = (char*)xmalloc(strlen(mkstr2(SCORESDIR)) + 11);

	/* make the full path */
#if defined(HAS_SNPRINTF)
	snprintf(fp, (strlen(mkstr2(SCORESDIR)) + 11), "%s/sweeptimes", mkstr2(SCORESDIR));
#else
	sprintf(fp, "%s/sweeptimes", mkstr2(SCORESDIR));
#endif

	return fp;
}
#endif /* USE_GROUP_BEST_FILE */


void DumpBFD(struct BestFileDesc *bfd, int valid)
{
#ifdef DEBUG_LOG
	int i = 0;

	fprintf(DebugLog, "Best Times Dump\n");
	fprintf(DebugLog, " Entries -> %u\n", bfd->numents);
	fprintf(DebugLog, " Sorted -> %s\n",
		bfd->sorted ? "True" : "False");
	for (i = 0; i < bfd->numents; i++)
	{
                const struct passwd *pw = getpwuid(bfd->ents[i].user);
		fprintf(DebugLog, "  - %s %u %u %u %s", pw->pw_name,
			bfd->ents[i].area, bfd->ents[i].mines, bfd->ents[i].time,
			ctime(&bfd->ents[i].date));
	}
#endif /* DEBUG_LOG */
}


/* Lock a file for exclusive use */
void tlockf(FILE *fp, char *name)
{
	int fd;

	/* this is actually dark magic. You aren't supposed to mix fileno
	 * and streams */
	fflush(fp);

#if defined(HAVE_FILENO)
	fd = fileno(fp);
#else
#error "Need fileno() replacement"
#endif

	lseek(fd, 0L, SEEK_SET);

#if defined(HAVE_FLOCK) && defined(HAVE_LOCKF)
	if(flock(fd, LOCK_EX) == -1)
#elif defined(HAVE_FLOCK)
	if(flock(fd, LOCK_EX) == -1)
#elif defined(HAVE_LOCKF)
	if(lockf(fd, F_LOCK, 0L) == -1)
#else
#error "Need flock() or lockf()"
#endif

	{
#ifdef DEBUG_LOG
		fprintf(DebugLog, "Can't lock file: (%d)%s\n", errno, name);
#endif /* DEBUG_LOG */
		SweepError("Cannot lock file: %s\n", name);
		exit(EXIT_FAILURE);
	}

}

/* unlock a locked file */
void tunlockf(FILE *fp)
{
	int fd;

	/* this is actually dark magic. You aren't supposed to mix fileno
	 * and streams */
	fflush(fp);

#if defined(HAVE_FILENO)
	fd = fileno(fp);
#else
#error "Need fileno() replacement"
#endif

	lseek(fd, 0L, SEEK_SET);

#if defined(HAVE_FLOCK) && defined(HAVE_LOCKF)
	if(flock(fd, LOCK_UN) == -1)
#elif defined(HAVE_FLOCK)
	if(flock(fd, LOCK_UN) == -1)
#elif defined(HAVE_LOCKF)
	if(lockf(fd, F_ULOCK, 0L) == -1)
#else
#error "Need flock() or lockf()"
#endif
	{
#ifdef DEBUG_LOG
		fprintf(DebugLog, "Can't unlock file: %d\n", errno);
#endif /* DEBUG_LOG */
		SweepError("Cannot unlock file: %d\n", errno);
		exit(EXIT_FAILURE);
	}
}
