/**********************************************************************
*  This source code is copyright 1999 by Gus Hartmann & Peter Keller  *
*  It may be distributed under the terms of the GNU General Purpose   *
*  License, version 2 or above; see the file COPYING for more         *
*  information.                                                       *
*                                                                     *
*  $Id: fgui.c,v 1.16 2000-11-07 05:32:36 hartmann Exp $
*                                                                     *
**********************************************************************/

#include "sweep.h"

/* percentages of screen space utilized from LINES and COLS */
/* This means I want a window FSWIDTH% of the screen, centered and 
 * FSHEIGHT% of the height, centered */
#define FSWIDTH 0.80
#define FSHEIGHT 0.80

static struct FileBuf* CreateFileBuf(char *dir);
static void DestroyFileBuf(struct FileBuf *head);
static char *FSelector(void);
static char* Choose(struct FileBuf *fb);
static void Display(WINDOW *fgui, struct FileBuf *fb, int find, int cursor, 
	int amount);
static int qstrcmp(const void *l, const void *r);

/* make a linked list of filenames given a directory */
struct FileBuf* CreateFileBuf(char *dir)
{
	char *path = NULL;
	struct FileBuf *head = NULL;
	struct FileBuf *tail = NULL;
	struct FileBuf *del = NULL;
	struct FileBuf *tmp = NULL;
	DIR *dent = NULL;
	struct dirent *dp = NULL;
	struct FileBuf *farray = NULL;
	int count = 0;
	char **sort = NULL;

	chdir(dir);
#if defined(PATH_MAX)
	path = xgetcwd(NULL, PATH_MAX);
#elif defined(HAVE_GET_CURRENT_DIR_NAME)
	path = get_current_dir_name();
#else /* Now what? No idea! */
#error "Don't know how to proceed on systems without PATH_MAX or get_current_dir_name"
#endif

	dent = xopendir(dir);

	if ( dent == NULL )
	{
		return NULL;
	}

	/* create a linked list of entries, don't use some of the fields for now */
	while ((dp = readdir(dent)) != NULL)
	{
		tmp = xmalloc(sizeof(struct FileBuf) * 1);
		tmp->fpath = xmalloc(strlen(dp->d_name) + strlen(path) + 2);
		tmp->next = NULL;

		/* create the full name for it */
		strcpy(tmp->fpath, path);
		if (strcmp(path,"/")!=0)
		{
			strcat(tmp->fpath, "/");
		}
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

	/* ok, take that linked list, and make an array out of it. Then free it */
	
	tmp = head;
	while(tmp != NULL)
	{
		count++;
		tmp = tmp->next;
	}
	
	farray = (struct FileBuf *)xmalloc(sizeof(struct FileBuf) * count);
	/* the first element in the array holds how many there are */
	farray[0].numents = count;
	farray[0].path = path;

	/* stick it into the temporary array for sorting */
	sort = (char**)xmalloc(sizeof(char*) * farray[0].numents);
	count = 0;
	tmp = head;
	while(tmp != NULL)
	{
		/* move the memory over to it */
		sort[count] = tmp->fpath;
		count++;
		tmp = tmp->next;
	}

	/* sort it nicely */
	qsort(sort, farray[0].numents, sizeof(char*), qstrcmp);

	/* now copy it out of the tmp array into the fb */
	count = 0;
	tmp = head;
	while(tmp != NULL)
	{
		/* move the memory over to it */
		farray[count].fpath = sort[count];
		count++;
		tmp = tmp->next;
	}
	free(sort);

	/* free the old list, fpath will be owned by farray now */
	tmp = head;
	while(tmp != NULL)
	{
		del = tmp;
		tmp = tmp->next;
		free(del);
	}

	return farray;
}

/* a glorified strcmp for qsort */
int qstrcmp(const void *l, const void *r)
{
	char *left = NULL;
	char *right = NULL;

	left = *(char**)l;
	right = *(char**)r;

	return strcmp(left,right);
}

/* destroy a list of directory names */
void DestroyFileBuf(struct FileBuf *head)
{
	int i = 0;

	free(head[0].path);

	for (i = 0; i < head[0].numents; i++)
	{
		free(head[i].fpath);
	}

	free(head);
}

/* allow the user to continue selecting stuff until a file is selected */
char *FSelector(void)
{
	char *selection = NULL;
	struct stat buf;
	struct FileBuf *fb = NULL;

	fb = CreateFileBuf(".");

	selection = Choose(fb);
	stat(selection, &buf);

	/* keep going until I get something that isn't a directory */
	while(S_ISDIR(buf.st_mode))
	{
		DestroyFileBuf(fb);
		if ( ( fb = CreateFileBuf(selection) ) == NULL )
		{
			return NULL;
		}
		free(selection);
		selection = Choose(fb);
		stat(selection, &buf);
	}

	DestroyFileBuf(fb);

	return selection;
}

/* this is the nasty function that parses the filedesc and spits up a window
	for you so you can select something out of it. XXX Must implement "this
	is correct" option */
char* Choose(struct FileBuf *fb)
{
	WINDOW *gui = NULL, *fgui = NULL;
	int nlines, ncols, bx, by;
	int xdist, ydist;
	chtype in;
	int cursor = 1; /* where the cursor starts in relation to the window */
	int find = 0;	/* the index of the top of the fb display */

	/* the area I have to put the window */
	xdist = (COLS-INFO_W-2);
	ydist = (LINES-6);

	/* find out how big, and where the window is, it is also centered */
	nlines = (int)(ydist * FSHEIGHT);
	ncols = (int)(xdist * FSWIDTH);
	bx = xdist/2 - ncols/2;
	by = ydist/2 - nlines/2;

	/* make border window */
	gui = newwin(nlines, ncols, by, bx);
	if (gui == NULL)
	{
		perror("Choose::newwin");
		exit(EXIT_FAILURE);
	}
	wborder(gui,CharSet.VLine,CharSet.VLine,CharSet.HLine,CharSet.HLine,
		CharSet.ULCorner,CharSet.URCorner,CharSet.LLCorner,CharSet.LRCorner);
	wmove(gui, by, bx);
	wrefresh(gui);

	/* make the window that will hold the information I want */
	fgui = derwin(gui, nlines-2, ncols-2, 1, 1);

	/* set it to the head of the list */
	werase(fgui);
	Display(fgui, fb, find, cursor, nlines-3);
	wrefresh(fgui);

	/* most of the nlines-3 stuff is because the first line is reserved for
	 * the path. */

	in=getch();
/*	while ((in != ' ') && (in != KEY_RIGHT) && (in != KEY_ESC))*/
	while ((in != ' ') && (in != KEY_RIGHT))
	{
		switch (in)
		{
			/* move the cursor down */
			case 'j': case KEY_DOWN:

				if (cursor < (nlines - 3) && cursor < fb[0].numents)
				{
					cursor++;
				}
				else if (find != fb[0].numents - (nlines-3) && 
					fb[0].numents > nlines-3)
				{
					find++;
				}
				break;
			/* move the cursor up */
			case 'k': case KEY_UP:
				if (cursor > 1)
				{
					cursor--;
				}
				else if (find > 0)
				{
					find--;
				}
				break;
			default:
				break;
		}
		werase(fgui);
		Display(fgui, fb, find, cursor, nlines-3);
		wrefresh(fgui);
		in=getch();
	}
	
	werase(fgui);
	werase(gui);
	wrefresh(fgui);
	wrefresh(gui);
	delwin(fgui);
	delwin(gui);

	return strdup(fb[find + cursor - 1].fpath);
}

/* draw the part of the FileBuf I have specified in the window */
void Display(WINDOW *fgui, struct FileBuf *fb, int find, 
	int cursor, int amount)
{
	char *p = NULL;
	int i;

	/* display the path, on the first line */
	mvwprintw(fgui, 0, 0, "-->");
	mvwprintw(fgui, 0, 3, fb[0].path);

	for (i = 0; i < amount && i+find < fb[0].numents; i++)
	{
		p = fb[find+i].fpath + strlen(fb[find+i].fpath) - 1;
		while(*p != '/') p--; p++; /* there will always be a / in it */
		mvwprintw(fgui, i+1, 1, p);
	}

	/* highlight the cursor line */
	mvwprintw(fgui, cursor, 0, ">");
}

/* This controls all of the logic for saving a file, when it is done, it
	returns the file name to where to save the game. */
char* FSGUI(void)
{
	char *file = NULL;

	StopTimer();
	file = FSelector();
	StartTimer();

	return file;
}
