#include "sweep.h"


/* make a linked list of filenames given a directory */
struct FileBuf* CreateFileBuf(char *dir)
{
	char *path = NULL;
	struct FileBuf *head = NULL;
	struct FileBuf *tail = NULL;
	struct FileBuf *tmp = NULL;
	DIR *dent;
	struct dirent *dp = NULL;

	path = xgetcwd(NULL, PATH_MAX);

	dent = xopendir(dir);

	while ((dp = readdir(dent)) != NULL)
	{
		tmp = xmalloc(sizeof(struct FileBuf) * 1);
		tmp->fpath = xmalloc(strlen(dp->d_name) + strlen(path) + 2);
		tmp->next = NULL;

		/* create the full name for it */
		strcpy(tmp->fpath, path);
		strcat(tmp->fpath, "/");
		strcat(tmp->fpath, dp->d_name);
		
		/* insert it into the list, the right way */
		if (head == NULL)
		{
			head = tmp;
		}
		else
		{
			tail->next = tmp;
		}
		tail = tmp;
	}

	closedir(dent);

	return head;
}

/* destroy a linked list of directory names */
void DestroyFileBuf(struct FileBuf *head)
{
	struct FileBuf *curr = NULL;
	struct FileBuf *del = NULL;

	curr = head;

	while(curr != NULL)
	{
		del = curr;
		curr = curr->next;

		free(del->fpath);
		free(del);
	}
}

/* open the window to select a file and print out the results and stuff */
char *FSelector(void)
{
	struct FileBuf *fb = NULL;

	fb = CreateFileBuf(".");

	return NULL;
}


/* This function opens a gui to select, or allow the user to specify a new file
 * with which to save the game. */
char* FSGUI(void)
{
	return NULL;
}



