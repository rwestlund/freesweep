#include <stdio.h>
#include <stdlib.h>
#include "math.h"
#include "sweep.h"

static struct BestFileDesc* NewBFD(void);
static void LoadBestTimesFile(struct BestFileDesc *bfd);
static void BFDSort(struct BestFileDesc *bfd);
static void InsertEntry(struct BestFileDesc *bfd, struct BestEntry *n);
static void SaveBestTimesFile(struct BestFileDesc *bfd);
static struct BestEntry* NewBestEntry(GameStats *Game);
static char* FPTBTF(void);
static void Unpack(struct BestFileDesc *bfd, FILE *abyss);
static void Zorch(char *p, struct BestEntry *b);
static void Pack(struct BestFileDesc *bfd, FILE *fp);
static int BECmpFunc(const void *l, const void *r);

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

	InsertEntry(bfd, b);

/*	BFDSort(bfd);*/

	SaveBestTimesFile(bfd);

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

		fprintf(abyss, "0\n0\n");
		fclose(abyss);
		goto again;		/* XXX So sue me, I'm lazy */
	}
	
	/* take the ascii/binary mess the file is and make it into a nice bfd */
	Unpack(bfd, abyss);

	free(truename);

	fclose(abyss);	/* you just try! */
}

void Unpack(struct BestFileDesc *bfd, FILE *abyss)
{
	unsigned char *space = NULL;
	unsigned int numents = 0;
	unsigned int size = 0;
	unsigned int i = 0;
	char *p = NULL, *q = NULL;
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
					xmalloc(sizeof(struct BestEntry)*numents + 1);
	}
	bfd->numents = numents;	/* yes, this could be zero */

	/* how many bytes do I need to read? */
	fscanf(abyss, "%u\n", &size);

	/* see if there is any hope in hell */
	if (size != 0)
	{
		space = (unsigned char*)xmalloc(sizeof(unsigned char) * size);
	
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
		for (i = 0; i < bfd->numents; i++)
		{
			b = &bfd->ents[i];	/* save me typing */
	
			/* get all the data out and into the entry */
			/* XXX This could overflow if someone fucked with the data */
			Zorch(p, b);

			while(*p++ != '\n');
		}
	}
}

void Zorch(char *p, struct BestEntry *b)
{
	char *t = NULL;
	char *q = NULL;

	q = strchr(p, '\n');
	if (q == NULL)
	{
		SweepError("BestTimes file is corrupted.");
		/* XXX FIX ME */
		exit(EXIT_FAILURE);
	}

	t = (char*)xmalloc(sizeof(char) * (q - p));

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
		return -1;
	}
	else if (bl->area > br->area)
	{
		return 1;
	}
	else if (bl->area == br->area)
	{
		if (bl->mines < br->mines)
		{
			return -1;
		}
		else if (bl->mines > br->mines)
		{
			return 1;
		}
		else if (bl->mines == br->mines)
		{
			if (bl->time < br->time)
			{
				return -1;
			}
			else if (bl->time > br->time)
			{
				return 1;
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

	fp = xfopen(name, "wb");

	/* convert the bfd to a mess and write it out */
	Pack(bfd, fp);

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
	 * maximum date len + 2 pipe separators + 3 letters + 1 newline + 1 null */
	maxsize = 3 * (unsigned int)ceil(log10(pow(2, sizeof(unsigned int)*8))) 
				+ MAX_NAME + MAX_DATE + 2 + 3 + 1 + 1;
	
	totalsize = 0;
	/* shove the entries into the line buffer */
	for (i = 0; i < bfdnum; i++)
	{
		/* make a new line */
		mbuf[i].buf = (char*)xmalloc(sizeof(char) * maxsize);

		/* print the line into the buffer */
		b = &bfd->ents[i];
		sprintf(mbuf[i].buf, "%s|a%um%ut%u|%s\n", 
			b->name, b->area, b->mines, b->time, b->date);
		
/*		fprintf(DebugLog, "Encrypting -> '%s'", mbuf[i].buf);*/
		/* encrypt the data */
		mbuf[i].len = strlen(mbuf[i].buf);
		for (j = 0; j < mbuf[i].len; j++)
		{
/*			mbuf[i].buf[j] ^= MAGIC_NUMBER;*/
		}

		totalsize += mbuf[i].len;
	}

	/* now that the data is encrypted and ready, shove it out to the file */

	/* number of entries */
	fprintf(fp, "%u\n", bfdnum);

	/* byte size of file */
	fprintf(fp, "%u\n", totalsize);

	/* the data */
	for (i = 0; i < bfdnum; i++)
	{
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
	fp = (unsigned char*)xmalloc(strlen(home) + strlen(DFL_BESTS_FILE) + 2);

	/* make the full path */
	strcpy(fp, home);
	strcat(fp, "/" DFL_BESTS_FILE);

	return fp;
}


