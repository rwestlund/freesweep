/**********************************************************************
*  This source code is copyright 1999 by Gus Hartmann & Peter Keller  *
*  It may be distributed under the terms of the GNU General Purpose   *
*  License, version 2 or above; see the file COPYING for more         *
*  information.                                                       *
*                                                                     *
*  $Id: utils.c,v 1.6 1999-08-09 05:25:36 hartmann Exp $
*                                                                     *
**********************************************************************/

#include "sweep.h"

void* xmalloc(size_t num)
{
	void *vec = NULL;

	vec = (void*)malloc(sizeof(unsigned char) * num);

	if (vec == NULL)
	{
		SweepError("Out of Memory. Sorry");
		exit(EXIT_FAILURE);
	}

	return vec;
}

FILE* xfopen(char *name, char *type)
{
	FILE *fp = NULL; 

	fp = fopen(name, type);
	if (fp == NULL)
	{
		SweepError("Could not open file: %s", name);
		exit(EXIT_FAILURE);
	}

	return fp;
}

#ifndef HAVE_STRDUP
char *strdup(char *s)
{
	char *c = NULL;

	c = (char*)xmalloc(strlen(s) + 1);
	
	strcpy(c, s);

	return(c);

}
#endif

char* xgetcwd(char *buf, size_t size)
{
	char *path = NULL;

	path = getcwd(buf, size);

	if (path == NULL)
	{
		SweepError("Could not get current working directory.");
		exit(EXIT_FAILURE);
	}

	return path;
}

DIR* xopendir(const char *path)
{
	DIR *dirent = NULL;

	dirent = opendir(path);

	if (dirent == NULL)
	{
		SweepError("Could not open directory: %s", path);
		exit(EXIT_FAILURE);
	}

	return dirent;
}

/* start and stop the timer */
void StartTimer(void)
{
	signal(SIGALRM, sighandler);
	alarm(1);
}

/* stop the timer */
void StopTimer(void)
{
	signal(SIGALRM, SIG_IGN);
}