#include<stdio.h>
#include<stdlib.h>
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
