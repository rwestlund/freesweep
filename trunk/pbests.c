#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "sweep.h"

static struct BestFileDesc* NewBFD(void);
static void LoadBestTimesFile(struct BestFileDesc *bfd);
static void BFDSort(struct BestFileDesc *bfd);
static void InsertEntry(struct BestFileDesc *bfd, struct BestEntry *n);
static void SaveBestTimesFile(struct BestFileDesc *bfd);
static struct BestEntry* NewBestEntry(GameStats *Game);
static char* FPTBTF(void);
static void Unpack(struct BestFileDesc *bfd, FILE *abyss);
static void Pack(struct BestFileDesc *bfd, FILE *fp);
static int BECmpFunc(const void *l, const void *r);
static void tlockf(FILE *fp, char * name);
static void tunlockf(FILE *fp);
static void DumpBFD(struct BestFileDesc *bfd, int valid);

extern int errno;

/* need a simple line buffer */
struct MBuf
{
	unsigned int len;
	char *buf;
};

/* the one function that does it all */
void UpdateBestTimesFile(GameStats *Game)
{
	struct BestFileDesc *bfd = NULL;
	struct BestEntry *b = NULL;

	bfd = NewBFD();
	b = NewBestEntry(Game);

	DumpGame(Game);

	LoadBestTimesFile(bfd); 

	fprintf(DebugLog, "***LOADED COPY\n");
	DumpBFD(bfd, FALSE);
	fflush(NULL);

	InsertEntry(bfd, b);

	fprintf(DebugLog, "***INSERTED COPY\n");
	DumpBFD(bfd, TRUE);
	fflush(NULL);

	BFDSort(bfd);

	fprintf(DebugLog, "***SORTED/SAVED COPY\n");
	DumpBFD(bfd, TRUE);
	fflush(NULL);

	SaveBestTimesFile(bfd);
	fflush(NULL);

	free(b);
	free(bfd->ents);
	free(bfd);
}

/* construct a new best file descriptor */
struct BestFileDesc* NewBFD(void)
{
	struct BestFileDesc *bfd = NULL;

	bfd = (struct BestFileDesc*)xmalloc(sizeof(struct BestFileDesc) * 1);

	bfd->ents = NULL;
	bfd->numents = 0;
	bfd->replflag = FALSE;

	return bfd;
}

/* summon from the depths of the abyss the best times file */
void LoadBestTimesFile(struct BestFileDesc *bfd)
{
	FILE *abyss = NULL;
	char *truename = NULL;
	
	truename = FPTBTF();

	again:
	abyss = fopen(truename, "rb");
	if (abyss == NULL)
	{
		abyss = xfopen(truename, "wb");

		fprintf(DebugLog, "Creating Besttimes file....\n");
		tlockf(abyss, truename);
		fprintf(abyss, "0\n0\n");
		tunlockf(abyss);
		fclose(abyss);
		goto again;		/* XXX So sue me, I'm lazy */
	}

	tlockf(abyss, truename);
	
	/* take the ascii/binary mess the file is and make it into a nice bfd */
	Unpack(bfd, abyss);

	free(truename);

	tunlockf(abyss);
	fclose(abyss);	/* you just try! */
}

void Unpack(struct BestFileDesc *bfd, FILE *abyss)
{
	unsigned char *space = NULL;
	unsigned int numents = 0;
	unsigned int size = 0;
	unsigned int i = 0;
	unsigned char *p = NULL;
	struct BestEntry *b = NULL;

	/* how many entries do I have? */
	fscanf(abyss, "%u\n", &numents);

	if (numents == 0)	/* uh oh, first time */
	{	
		/* one more than I need, for later */
		bfd->ents = (struct BestEntry*)xmalloc(sizeof(struct BestEntry));
	}
	else
	{
		/* one more than I need, for later */
		bfd->ents = (struct BestEntry*)
					xmalloc(sizeof(struct BestEntry)*numents + 
					sizeof(struct BestEntry));
	}
	bfd->numents = numents;	/* yes, this could be zero */

	/* how many bytes do I need to read? */
	fscanf(abyss, "%u\n", &size);

	/* see if there is any hope in hell */
	if (size != 0)
	{
		space = (unsigned char*)xmalloc(sizeof(unsigned char) * size + 1);
		/* this also null terminates any string I put into it */
		memset(space, 0, sizeof(unsigned char) * size + 1);
	
		/* read it in */
		fread(space, size, 1, abyss);

		/* convert it to real ascii */
		for (i = 0; i < size; i++)
		{
/*			space[i] ^= MAGIC_NUMBER;*/
		}

		/* walk through memory looking for newlines and building the entries
	 	* as you go */
		p = space;
		fprintf(DebugLog, "Doing %d entries....\n", bfd->numents);
		fprintf(DebugLog, "Full string -> '%s'\n", p);
		fflush(NULL);
		for (i = 0; i < bfd->numents; i++)
		{
			b = &bfd->ents[i];	/* save me typing */
	
			/* get all the data out and into the entry */
			sscanf((char *)p, "%[^(](a%dm%dt%d)%s", 
				b->name, &b->area, &b->mines, &b->time, b->date);
			fprintf(DebugLog, "Found --> %s, %u, %u, %u, %s\n",
				b->name, b->area, b->mines, b->time, b->date);

			while(*p++ != '\n');
		}

		free(space);
	}
}

void BFDSort(struct BestFileDesc *bfd)
{
	if (bfd->replflag == TRUE)
	{
		/* I just replaced a node, everything is still sorted */
		return;
	}
	else
	{
		/* qsort the whole mess */
		qsort(bfd->ents, bfd->numents+1, sizeof(bfd->ents[0]), BECmpFunc);
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
	int i = 0;

	/* search until I find a match */
	for (i = 0; i < bfd->numents; i++)
	{
		/* did the area match? */
		if (n->area == bfd->ents[i].area)
		{
			/* did the number of mines match? */
			if (n->mines == bfd->ents[i].mines)
			{
				/* yup, replace it and mark the flag */
				bfd->ents[i].time = n->time;
				strncpy(bfd->ents[i].name, n->name, MAX_NAME);
				strncpy(bfd->ents[i].date, n->date, MAX_DATE);
				replaced = TRUE;
			}
		}
	}
	
	if (replaced == TRUE)
	{
		bfd->replflag = TRUE;
		return;
	}
	else
	{
		/* use the x-tra one I got initially */
		bfd->ents[bfd->numents].area = n->area;
		bfd->ents[bfd->numents].mines = n->mines;
		bfd->ents[bfd->numents].time = n->time;
		strncpy(bfd->ents[bfd->numents].name, n->name, MAX_NAME);
		strncpy(bfd->ents[bfd->numents].date, n->date, MAX_DATE);
	}
}

void SaveBestTimesFile(struct BestFileDesc *bfd)
{
	char *name = NULL;
	FILE *fp = NULL;

	name = FPTBTF();

	fp = xfopen(name, "w");

	tlockf(fp, name);
	
	/* convert the bfd to a mess and write it out */
	Pack(bfd, fp);

	free(name);

	tunlockf(fp);
	fclose(fp);
}

void Pack(struct BestFileDesc *bfd, FILE *fp)
{
	struct MBuf *mbuf = NULL;
	unsigned int bfdnum = 0;
	unsigned int maxsize = 0, totalsize = 0;
	unsigned int i, j;
	struct BestEntry *b = NULL;
	
	/* see how many I need to write out */
	if (bfd->replflag == FALSE)
	{
		bfdnum = bfd->numents + 1;
	}
	else
	{
		bfdnum = bfd->numents;
	}

	mbuf = (struct MBuf*)xmalloc(sizeof(struct MBuf) * bfdnum);

	/* compute the maximum size I will need to allocate for each line
	 * in the buflines array */

	/* this is: three unsigned ints converted to ascii + maximum name len +
	 * maximum date len + 2 paren separators + 3 letters + 1 newline + 1 null */
	maxsize = 3 * (unsigned int)ceil(log10(pow(2, sizeof(unsigned int)*8))) 
				+ MAX_NAME + MAX_DATE + 2 + 3 + 1 + 1;
	
	totalsize = 0;
	/* shove the entries into the line buffer */
	for (i = 0; i < bfdnum; i++)
	{
		/* make a new line */
		mbuf[i].buf = (char*)xmalloc(sizeof(char) * maxsize + 1);

		/* print the line into the buffer */
		b = &bfd->ents[i];
		sprintf(mbuf[i].buf, "%s(a%um%ut%u)%s\n", 
			b->name, b->area, b->mines, b->time, b->date);
		fprintf(DebugLog, "Created mbuf -> %s", mbuf[i].buf);
		fflush(0);

		mbuf[i].len = strlen(mbuf[i].buf);
		
/*		fprintf(DebugLog, "Encrypting -> '%s'", mbuf[i].buf);*/
		/* encrypt the data */
		for (j = 0; j < mbuf[i].len; j++)
		{
/*			mbuf[i].buf[j] ^= MAGIC_NUMBER;*/
		}

		totalsize += mbuf[i].len;
	}

	/* now that the data is encrypted and ready, shove it out to the file */

	fprintf(DebugLog, "***About to write into file\n");
	fflush(0);
	/* number of entries */
	fprintf(fp, "%u\n", bfdnum);
	fprintf(DebugLog, "%u entries...\n", bfdnum);
	fflush(0);

	/* byte size of file */
	fprintf(fp, "%u\n", totalsize);
	fprintf(DebugLog, "%u bytes...\n", totalsize);
	fflush(0);

	/* the data */
	for (i = 0; i < bfdnum; i++)
	{
		fprintf(DebugLog, "Writing -> ");
		fwrite(mbuf[i].buf, mbuf[i].len, 1, DebugLog);
		fflush(NULL);
		fwrite(mbuf[i].buf, mbuf[i].len, 1, fp);
	}

	/* free up all of the shit I just used */
	for (i = 0; i < bfdnum; i++)
	{
		free(mbuf[i].buf);
	}
	free(mbuf);
}

struct BestEntry* NewBestEntry(GameStats *Game)
{
	struct BestEntry *b = NULL;
	time_t now;
	char *buf = NULL, *p = NULL;

	b = (struct BestEntry*)xmalloc(sizeof(struct BestEntry) * 1);

	/* fill in some attributes */
	b->area = Game->Height * Game->Width;
	b->mines = Game->NumMines;
	b->time = Game->Time;

	/* do the username */
	buf = getenv("USER");
	if (buf == NULL)
	{
		SweepError("You do not have a username!");
		buf = "unknown";
	}
	strcpy(b->name, buf);

	/* get the real time it was completed */
	time(&now);
	buf = ctime(&now);
	if (buf == NULL)
	{
		buf = "unknown";
	}
	p = strchr(buf, '\n');	/* some clean up work */
	if (p != NULL)
	{
		*p = 0;
	}
	strcpy(b->date, buf);

	/* add _ instead of spaces for easier data format storage */
	while((p = strchr(b->date, ' ')) != NULL)
	{
		*p = '_';
	}

	return b;
}

/* Full Path To Best Times File */
char* FPTBTF(void)
{
	char *home = NULL;
	char *fp = NULL;

	home = getenv("HOME");

	if (home == NULL)
	{
		SweepError("You don't have a home dir!");
		/* XXX fix this up */
		exit(EXIT_FAILURE);
	}

	/* get me some memory for the string */
	fp = (char*)xmalloc(strlen(home) + strlen(DFL_BESTS_FILE) + 2);

	/* make the full path */
	strcpy(fp, home);
	strcat(fp, "/" DFL_BESTS_FILE);

	return fp;
}


static void DumpBFD(struct BestFileDesc *bfd, int valid)
{
	int i = 0;

	fprintf(DebugLog, "BFD DUMP START\n");
	fprintf(DebugLog, "Numents -> %u\n", bfd->numents);
	fprintf(DebugLog, "Replflag -> %s\n", 
		bfd->replflag?"TRUE":"FALSE");
	for (i = 0; i < bfd->numents; i++)
	{
		fprintf(DebugLog, "- %s %u %u %u %s\n", bfd->ents[i].name,
			bfd->ents[i].area, bfd->ents[i].mines, bfd->ents[i].time,
			bfd->ents[i].date);
	}
	if (valid == TRUE)
	{
		if (bfd->replflag == FALSE)
		{
			fprintf(DebugLog, "AND\n");
			fprintf(DebugLog, "- %s %u %u %u %s\n", bfd->ents[i].name,
				bfd->ents[i].area, bfd->ents[i].mines, bfd->ents[i].time,
				bfd->ents[i].date);
		}
	}
	fprintf(DebugLog, "BFD DUMP END\n");
}


void tlockf(FILE *fp, char *name)
{
	int fd;

	fflush(fp);
	
	fd = fileno(fp);
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
		fprintf(DebugLog, "Can't lock file: (%d)%s\n", errno, name);
		SweepError("Cannot lock file: %s\n", name);
		exit(EXIT_FAILURE);
	}

}

void tunlockf(FILE *fp)
{
	int fd;

	fflush(fp);
	
	fd = fileno(fp);
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
		fprintf(DebugLog, "Can't unlock file\n");
		SweepError("Cannot unlock file\n");
		exit(EXIT_FAILURE);
	}
}
