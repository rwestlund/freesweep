#include "sweep.h"

/* percentages of screen space utilized from LINES and COLS */
/* This means I want a window FSWIDTH% of the screen, centered and 
 * FSHEIGHT% of the height, centered */
#define FSWIDTH 0.80
#define FSHEIGHT 0.80

static char* Choose(struct FileBuf *fb);
static void Display(WINDOW *fgui, struct FileBuf *fb, struct FileBuf *find, 
	int cursor, int amount);

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
		tmp->prev = NULL;

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
			tmp->prev = tail;
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
		fb = CreateFileBuf(selection);
		free(selection);
		selection = Choose(fb);
		stat(selection, &buf);
	}

	DestroyFileBuf(fb);

	return selection;
}

/* this is the nasty function that parses the filedesc and spits up a window
 * for you so you can select something out of it */
char* Choose(struct FileBuf *fb)
{
	WINDOW *gui = NULL, *fgui = NULL;
	int nlines, ncols, bx, by;
	int xdist, ydist;
	char in;
	int cursor = 0; /* where the cursor starts in relation to the window */
	struct FileBuf *find = NULL; /* index to top of window display in list */

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
	find = fb;
	werase(fgui);
	Display(fgui, fb, find, cursor, nlines-2);
	wrefresh(fgui);

	in=wgetch(fgui);
	while(1)
	{
		switch (in)
		{
			/* move the cursor down */
			case 'j':
				if (cursor < nlines-3)
				{
					if (find->next != NULL)
					{
						cursor++;
					}
				}
				else if (find->next != NULL) /* are there more? */
				{
					find = find->next;
				}
				break;
			case 'k':
				if (cursor != 0)
				{
					cursor--;
				}
				else if (find->prev != NULL) /* are there more? */
				{
					find = find->prev;
				}
				break;
			default:
				break;
		}
		werase(fgui);
		Display(fgui, fb, find, cursor, nlines-2);
		wrefresh(fgui);
		in=wgetch(fgui);
	}

	return NULL;
}

void Display(WINDOW *fgui, struct FileBuf *fb, struct FileBuf *find, 
	int cursor, int amount)
{
	int i;
	struct FileBuf *curr = NULL;

	curr = find;

	for (i = 0; i < amount && curr != NULL; i++)
	{
		mvwprintw(fgui, i, 1, curr->fpath);
		curr = curr->next;
	}

	/* highlight the cursor line */
	mvwprintw(fgui, cursor, 0, "*");

}

/* This controls all of the logic for saving a file, when it is done, it
 * returns the file name to where to save the game. */
char* FSGUI(void)
{
	StopTimer();
	FSelector();
	StartTimer();
	return NULL;
}



