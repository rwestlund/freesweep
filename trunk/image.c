/**********************************************************************
*  This source code is copyright 1999 by Gus Hartmann & Peter Keller  *
*  It may be distributed under the terms of the GNU General Purpose   *
*  License, version 2 or above; see the file COPYING for more         *
*  information.                                                       *
*                                                                     *
*  $Id: image.c,v 1.1 2000-11-02 03:46:47 hartmann Exp $
*                                                                     *
**********************************************************************/

#include "sweep.h"

void SaveGameImage(GameStats* Game, char *fname)
{
	int width, height;
	unsigned char value;
	FILE *fo = NULL;

	fo = xfopen(fname, "w");
	
	/* dump the stats out */
	fprintf(fo, "P6\n%d\n%d\n255\n", Game->Width, Game->Height);
	
	for ( width = 0 ; width < Game->Width ; width++ ) {
		for ( height = 0 ; height < Game->Height ; height++ ) {
			if (( abs(width - Game->CursorX) < 2 ) && ( abs( Game->CursorY - height) < 10 )) {
				fwrite("\xff\x00\x00", 1, 3, fo);
			} else if (( abs(width - Game->CursorX) < 10 ) && ( abs(Game->CursorY - height) < 2 )) {
				fwrite("\xff\x00\x00", 1, 3, fo);
			} else {
				GetMine(width, height, value);
				switch ( value ) {
					case UNKNOWN: case MINE:
						fwrite("\x00\x00\x00", 1, 3, fo);
						break;
					case MARKED: case BAD_MARK:
						fwrite("\x00\xff\x00", 1, 3, fo);
						break;
					case DETONATED:
						fwrite("\xff\x00\x00", 1, 3, fo);
						break;
					case EMPTY: default:
						fwrite("\xff\xff\xff", 1, 3, fo);
						break;
				}
			}
		}
	}

	fclose(fo);
}
