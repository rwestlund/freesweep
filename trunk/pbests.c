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
void Zorch(char *p, char *q, struct BestEntry *b);

/* the one function that does it all */
void UpdateBestTimesFile(GameStats *Game)
{
	/* Make entry for Game */
	/* Read best times file into n + 1 array size */
	/* Sort best times File up to n */
	/* Determine where the entry goes, or who it replaces */
	/* if it gets added, then add it to correct location moving all other
		entries up one spot by hand (memove?) */
	/* else replace it and do nothing else */
	/* Write out either n or n + 1 entries to file */

	struct BestFileDesc *bfd;

	bfd = NewBFD();

	LoadBestTimesFile(bfd); 

	BFDSort(bfd);

	InsertEntry(bfd, NewBestEntry(Game));

	SaveBestTimesFile(bfd);

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
	char *p = NULL, *q = NULL;

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
	
	/* how many bytes do I need to read? */
	fscanf(abyss, "%u\n", &size);

	/* read it in */
	fread(space, size, 1, abyss);

	/* convert it to real ascii */
	for (i = 0; i < size; i++)
	{
		space[i] ^= MAGIC_NUMBER;
	}

	/* walk through memory looking for newlines and building the entries
	 * as you go */
	p = q = space;
	for (i = 0; i < numents; i++)
	{
		while(*q++ != '\n');	/* yes, I want that semicolon there */

		/* get all the data out and into the entry */
		Zorch(p, q-1, &bfd->ents[i]);

		p = q;
	}

}

/* get the fields of data out from between the p and q pointers */
void Zorch(char *p, char *q, struct BestEntry *b)
{
}

void BFDSort(struct BestFileDesc *bfd)
{
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


