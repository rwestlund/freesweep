#include <stdio.h>
#include <stdlib.h>
#include "sweep.h"

static struct BestFileDesc* NewBFD(void);
static void LoadBestTimesFile(struct BestFileDesc *bfd);
static void BFDSort(struct BestFileDesc *bfd);
static void InsertEntry(struct BestFileDesc *bfd, struct BestEntry *n);
static void SaveBestTimesFile(struct BestFileDesc *bfd);
static struct BestEntry* NewBestEntry(GameStats *Game);
static char* FPTBTF(void);
void Unpack(struct BestFileDesc *bfd, FILE *abyss);

/* the one function that does it all */
void UpdateBestTimesFile(GameStats *Game)
{
	struct BestFileDesc *bfd = NULL;
	struct BestEntry *b = NULL;

	bfd = NewBFD();
	b = NewBestEntry(Game);

	LoadBestTimesFile(bfd); 

	BFDSort(bfd);

	InsertEntry(bfd, b);

	SaveBestTimesFile(bfd);

	free(b);
	free(bfd->ents);
	free(bfd);
}

/* construct a new best file descriptor */
struct BestFileDesc* NewBFD(void)
{
	struct BestFileDesc *bfd = NULL;

	bfd = (struct BestFileDesc*)malloc(sizeof(struct BestFileDesc) * 1);
	if (bfd == NULL)
	{
		SweepError("Out of Memory. Sorry.");
		/* XXX fix me */
		exit(EXIT_FAILURE);
	}

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

	abyss = fopen(truename, "r");
	if (abyss == NULL)
	{
		SweepError("Could not find best times file");
		/* XXX Fix me */
		exit(EXIT_FAILURE);
	}
	
	/* take the binary mess the file is and make it into a nice bfd */
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
	char *p = NULL;
	struct BestEntry *b = NULL;

	/* how many entries do I have? */
	fscanf(abyss, "%u\n", &numents);

	/* one more than I need, for later */
	bfd->ents = (struct BestEntry*)malloc(sizeof(struct BestEntry)*numents + 1);
	if (bfd->ents == NULL)
	{
		SweepError("Out of Memory. Sorry.");
		/* XXX fix me */
		exit(EXIT_FAILURE);
	}
	bfd->numents = numents;
	
	/* how many bytes do I need to read? */
	fscanf(abyss, "%u\n", &size);

	space = (unsigned char*)malloc(sizeof(unsigned char) * size);
	if (space == NULL)
	{
		SweepError("Out of Memory. Sorry.");
		/* XXX fix me */
		exit(EXIT_FAILURE);
	}

	/* read it in */
	fread(space, size, 1, abyss);

	/* convert it to real ascii */
	for (i = 0; i < size; i++)
	{
		space[i] ^= MAGIC_NUMBER;
	}

	/* walk through memory looking for newlines and building the entries
	 * as you go */
	p = space;
	for (i = 0; i < numents; i++)
	{
		b = &bfd->ents[i];	/* save me typing */

		/* get all the data out and into the entry */
		/* XXX This could overflow if someone fucked with the data */
		sscanf(p, "%s|a%um%ut%u|%s\n", b->name, &b->area, &b->mines, &b->time,
				b->date);

		while(*p++ != '\n');	/* yes, I want that semicolon there */
	}
}

void BFDSort(struct BestFileDesc *bfd)
{
	/* preform a radix sort, effectively */
	/* sort according to area */
	/* sort according to mines for each area subsection */
	/* sort each mine subsection according to time */
}

void InsertEntry(struct BestFileDesc *bfd, struct BestEntry *n)
{
}

void SaveBestTimesFile(struct BestFileDesc *bfd)
{
}

struct BestEntry* NewBestEntry(GameStats *Game)
{
	return NULL;
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
	fp = (unsigned char*)malloc(strlen(home) + strlen(DFL_BESTS_FILE) + 1);
	if (fp == NULL)
	{
		SweepError("Out of Memory. Sorry.");
		/* XXX fix this up */
		exit(EXIT_FAILURE);
	}	

	/* make the full path */
	strcpy(fp, home);
	strcat(fp, "\\" DFL_BESTS_FILE);

	return fp;
}


