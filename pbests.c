#include <stdio.h>
#include <stdlib.h>
#include "sweep.h"

struct BestEntry
{
	unsigned int area;
	unsigned int mines;
	unsigned int time;
};

struct BestFileDesc
{
	/* the array of entries from the file, with one more in it.
	struct BestEntry *ents;

	/* the number of entries in the descriptor */
	int numents;

	/* did I replace, or add? */
	int replflag;
};

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

	BTSort(bfd);

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


