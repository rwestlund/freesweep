/**********************************************************************
 *  This source code is copyright 2023 by Ron R Wills		      *
 *  It may be distributed under the terms of the GNU General Purpose  *
 *  License, version 2 or above; see the file COPYING for more	      *
 *  information.						      *
 *								      *
 **********************************************************************/

#include "sweep.h"

static void SetStockColors()
{
  if (!has_colors())
  {
    int c;
    short f,b;

    pair_content(0,&f,&b);
    for (c = 1; c < 20; c++)
    {
      init_pair(c,f,b);
    }
  }
  else
  {
    init_pair(CLR_NORMAL,COLOR_WHITE,COLOR_BLACK);
    init_pair(CLR_VAL1,COLOR_BLUE,COLOR_BLACK);
    init_pair(CLR_VAL2,COLOR_CYAN,COLOR_BLACK);
    init_pair(CLR_VAL3,COLOR_GREEN,COLOR_BLACK);
    init_pair(CLR_VAL4,COLOR_MAGENTA,COLOR_BLACK);
    init_pair(CLR_VAL5,COLOR_RED,COLOR_BLACK);
    init_pair(CLR_VAL6,COLOR_RED,COLOR_BLACK);
    init_pair(CLR_VAL7,COLOR_RED,COLOR_BLACK);
    init_pair(CLR_VAL8,COLOR_MAGENTA,COLOR_BLACK);
    init_pair(CLR_UNKNOWN,COLOR_WHITE,COLOR_BLACK);
    init_pair(CLR_MINE,COLOR_WHITE,COLOR_BLACK);
    init_pair(CLR_SHOWMINE,
              xcolor(105,105,105,COLOR_WHITE),
              COLOR_BLACK);

    init_pair(CLR_SPACE,COLOR_WHITE,COLOR_BLACK);
    init_pair(CLR_MARK,
              xcolor(255,255,148,COLOR_WHITE),
              COLOR_BLACK);
    init_pair(CLR_FALSEMARK,
              xcolor(255,255,148,COLOR_WHITE),
              COLOR_BLACK);
    init_pair(CLR_SHOWFALSEMARK,
              xcolor(255,0,0,COLOR_RED),
              COLOR_BLACK);
    init_pair(CLR_DETONATED,COLOR_YELLOW,COLOR_BLACK);

    init_pair(CLR_INFOBAR,
              xcolor(34,34,34,COLOR_BLACK),
              xcolor(173,173,173,COLOR_WHITE));
    init_pair(CLR_STATBAR,
              xcolor(0,0,88,COLOR_BLACK),
              xcolor(173,173,173,COLOR_WHITE));
    init_pair(CLR_MSGBAR,
              xcolor(88,0,0,COLOR_BLACK),
              xcolor(173,173,173,COLOR_WHITE));
  }
}

void SetTheme(GameStats* Game)
{
  /* If there are no colors then assume no unicode either! */
  if (!has_colors())
  {
    if (Game->Theme > 2) Game->Theme=2;
  }

  switch (Game->Theme)
  {
  case 1: // ASCII theme that should work on the worst of hardware :P
    SetStockColors();

    CharSet.ch_flag = CH_TYPE;
    CharSet.name = "ASCII";
    CharSet.ULCorner='+';
    CharSet.URCorner='+';
    CharSet.LLCorner='+';
    CharSet.LRCorner='+';
    CharSet.HLine='-';
    CharSet.VLine='|';
    CharSet.UArrow='^';
    CharSet.DArrow='v';
    CharSet.RArrow='>';
    CharSet.LArrow='<';
    CharSet.Mark.ch='#';
    CharSet.FalseMark.ch='#';
    CharSet.ShowFalseMark.ch='x';
    CharSet.Unknown.ch='-';
    CharSet.Mine.ch='-';
    CharSet.ShowMine.ch='o';
    CharSet.Space.ch=' ';
    CharSet.Bombed.ch='*';
    CharSet.CursorLeft.ch='<';
    CharSet.CursorRight.ch='>';

    break;

  case 2: // Curses theme that should work on most hardware :P
    SetStockColors();

    CharSet.ch_flag = CH_TYPE;
    CharSet.name = "Curses";
    CharSet.ULCorner=ACS_ULCORNER;
    CharSet.URCorner=ACS_URCORNER;
    CharSet.LLCorner=ACS_LLCORNER;
    CharSet.LRCorner=ACS_LRCORNER;
    CharSet.HLine=ACS_HLINE;
    CharSet.VLine=ACS_VLINE;
    CharSet.UArrow=ACS_UARROW;
    CharSet.DArrow=ACS_DARROW;
    CharSet.RArrow=ACS_RARROW;
    CharSet.LArrow=ACS_LARROW;
    CharSet.Mark.ch=ACS_DIAMOND;
    CharSet.FalseMark.ch=ACS_DIAMOND;
    CharSet.ShowFalseMark.ch='x';
    CharSet.Unknown.ch='-';
    CharSet.Mine.ch='-';
    CharSet.ShowMine.ch='o';
    CharSet.Space.ch=' ';
    CharSet.Bombed.ch='*';
    CharSet.CursorLeft.ch='[';
    CharSet.CursorRight.ch=']';
    break;

  case 3: // The first unicode based theme
    init_pair(CLR_NORMAL,COLOR_WHITE,COLOR_BLACK);
    init_pair(CLR_VAL1,COLOR_BLUE,COLOR_BLACK);
    init_pair(CLR_VAL2,COLOR_CYAN,COLOR_BLACK);
    init_pair(CLR_VAL3,COLOR_GREEN,COLOR_BLACK);
    init_pair(CLR_VAL4,COLOR_MAGENTA,COLOR_BLACK);
    init_pair(CLR_VAL5,COLOR_RED,COLOR_BLACK);
    init_pair(CLR_VAL6,COLOR_RED,COLOR_BLACK);
    init_pair(CLR_VAL7,COLOR_RED,COLOR_BLACK);
    init_pair(CLR_VAL8,COLOR_MAGENTA,COLOR_BLACK);
    init_pair(CLR_UNKNOWN,
              xcolor(165,42,42,COLOR_GREEN),
              COLOR_BLACK);
    init_pair(CLR_MINE,
              xcolor(165,42,42,COLOR_GREEN),
              COLOR_BLACK);
    init_pair(CLR_SHOWMINE,
              xcolor(105,105,105,COLOR_WHITE),
              COLOR_BLACK);

    init_pair(CLR_SPACE,COLOR_WHITE,COLOR_BLACK);
    init_pair(CLR_MARK,
              xcolor(255,255,148,COLOR_WHITE),
              COLOR_BLACK);
    init_pair(CLR_FALSEMARK,
              xcolor(255,255,148,COLOR_WHITE),
              COLOR_BLACK);
    init_pair(CLR_SHOWFALSEMARK,
              xcolor(255,0,0,COLOR_RED),
              COLOR_BLACK);
    init_pair(CLR_DETONATED,COLOR_YELLOW,COLOR_BLACK);

    init_pair(CLR_INFOBAR,
              xcolor(34,34,34,COLOR_BLACK),
              xcolor(173,173,173,COLOR_WHITE));
    init_pair(CLR_STATBAR,
              xcolor(0,0,88,COLOR_BLACK),
              xcolor(173,173,173,COLOR_WHITE));
    init_pair(CLR_MSGBAR,
              xcolor(88,0,0,COLOR_BLACK),
              xcolor(173,173,173,COLOR_WHITE));

    CharSet.ch_flag = UNI_TYPE;
    CharSet.name = "Unicode";
    CharSet.ULCorner=ACS_ULCORNER;
    CharSet.URCorner=ACS_URCORNER;
    CharSet.LLCorner=ACS_LLCORNER;
    CharSet.LRCorner=ACS_LRCORNER;
    CharSet.HLine=ACS_HLINE;
    CharSet.VLine=ACS_VLINE;
    CharSet.UArrow=ACS_UARROW;
    CharSet.DArrow=ACS_DARROW;
    CharSet.RArrow=ACS_RARROW;
    CharSet.LArrow=ACS_LARROW;
    CharSet.Mark.str="⚑";
    CharSet.FalseMark.str="⚑";
    CharSet.ShowFalseMark.str="⚐";
    CharSet.Unknown.str="☁";
    CharSet.Mine.str="☁";
    CharSet.ShowMine.str="Ⳝ";
    CharSet.Space.str=" ";
    CharSet.Bombed.str="*";
    CharSet.CursorLeft.str="⟫";
    CharSet.CursorRight.str="⟪";
    break;

  case 4: // The neon Alien based theme
    init_pair(CLR_NORMAL,COLOR_WHITE,COLOR_BLACK);
    init_pair(CLR_VAL8,
              xcolor(0,128,31,COLOR_BLUE),
              COLOR_BLACK);
    init_pair(CLR_VAL7,
              xcolor(0,160,62,COLOR_BLUE),
              COLOR_BLACK);
    init_pair(CLR_VAL6,
              xcolor(0,176,93,COLOR_BLUE),
              COLOR_BLACK);
    init_pair(CLR_VAL5,
              xcolor(0,192,124,COLOR_BLUE),
              COLOR_BLACK);
    init_pair(CLR_VAL4,
              xcolor(0,208,155,COLOR_BLUE),
              COLOR_BLACK);
    init_pair(CLR_VAL3,
              xcolor(0,224,186,COLOR_BLUE),
              COLOR_BLACK);
    init_pair(CLR_VAL2,
              xcolor(0,240,217,COLOR_BLUE),
              COLOR_BLACK);
    init_pair(CLR_VAL1,
              xcolor(0,255,248,COLOR_BLUE),
              COLOR_BLACK);
    init_pair(CLR_UNKNOWN,
              xcolor(90,90,90,COLOR_WHITE),
              COLOR_BLACK);
    init_pair(CLR_MINE,
              xcolor(90,90,90,COLOR_WHITE),
              COLOR_BLACK);
    init_pair(CLR_SHOWMINE,
              xcolor(105,105,105,COLOR_WHITE),
              COLOR_BLACK);

    init_pair(CLR_SPACE,COLOR_WHITE,COLOR_BLACK);
    init_pair(CLR_MARK,
              xcolor(78,228,78,COLOR_GREEN),
              COLOR_BLACK);
    init_pair(CLR_FALSEMARK,
              xcolor(78,228,78,COLOR_GREEN),
              COLOR_BLACK);
    init_pair(CLR_SHOWFALSEMARK,
              xcolor(255,0,0,COLOR_RED),
              COLOR_BLACK);
    init_pair(CLR_DETONATED,COLOR_YELLOW,COLOR_BLACK);

    init_pair(CLR_INFOBAR,
              xcolor(0,61,0,COLOR_BLACK),
              xcolor(31,199,0,COLOR_GREEN));
    init_pair(CLR_STATBAR,
              COLOR_WHITE,
              xcolor(31,199,0,COLOR_GREEN));
    init_pair(CLR_MSGBAR,
              xcolor(0,61,0,COLOR_BLACK),
              xcolor(31,199,0,COLOR_GREEN));

    CharSet.ch_flag = UNI_TYPE;
    CharSet.name = "Alien";
    CharSet.ULCorner=ACS_ULCORNER;
    CharSet.URCorner=ACS_URCORNER;
    CharSet.LLCorner=ACS_LLCORNER;
    CharSet.LRCorner=ACS_LRCORNER;
    CharSet.HLine=ACS_HLINE;
    CharSet.VLine=ACS_VLINE;
    CharSet.UArrow=ACS_UARROW;
    CharSet.DArrow=ACS_DARROW;
    CharSet.RArrow=ACS_RARROW;
    CharSet.LArrow=ACS_LARROW;
    CharSet.Mark.str="▼";
    CharSet.FalseMark.str="▼";
    CharSet.ShowFalseMark.str="▽";
    CharSet.Unknown.str="⌢";
    CharSet.Mine.str="⌢";
    CharSet.ShowMine.str="Ⳝ";
    CharSet.Space.str=" ";
    CharSet.Bombed.str="*";
    CharSet.CursorLeft.str="⊴";
    CharSet.CursorRight.str="⊵";
    break;

  case 5: // The Dollie or Barbie styled theme
    init_pair(CLR_NORMAL,COLOR_WHITE,COLOR_BLACK);
    init_pair(CLR_VAL8,
              xcolor(128,0,31,COLOR_BLUE),
              COLOR_BLACK);
    init_pair(CLR_VAL7,
              xcolor(160,0,62,COLOR_BLUE),
              COLOR_BLACK);
    init_pair(CLR_VAL6,
              xcolor(176,0,93,COLOR_BLUE),
              COLOR_BLACK);
    init_pair(CLR_VAL5,
              xcolor(192,0,124,COLOR_BLUE),
              COLOR_BLACK);
    init_pair(CLR_VAL4,
              xcolor(208,0,155,COLOR_BLUE),
              COLOR_BLACK);
    init_pair(CLR_VAL3,
              xcolor(224,0,186,COLOR_BLUE),
              COLOR_BLACK);
    init_pair(CLR_VAL2,
              xcolor(240,0,217,COLOR_BLUE),
              COLOR_BLACK);
    init_pair(CLR_VAL1,
              xcolor(255,0,248,COLOR_BLUE),
              COLOR_BLACK);
    init_pair(CLR_UNKNOWN,
              xcolor(255,20,88,COLOR_RED),
              COLOR_BLACK);
    init_pair(CLR_MINE,
              xcolor(255,20,88,COLOR_RED),
              COLOR_BLACK);
    init_pair(CLR_SHOWMINE,
              xcolor(105,105,105,COLOR_WHITE),
              COLOR_BLACK);

    init_pair(CLR_SPACE,COLOR_WHITE,COLOR_BLACK);
    init_pair(CLR_MARK,
              xcolor(255,255,0,COLOR_YELLOW),
              COLOR_BLACK);
    init_pair(CLR_FALSEMARK,
              xcolor(255,255,0,COLOR_YELLOW),
              COLOR_BLACK);
    init_pair(CLR_SHOWFALSEMARK,
              xcolor(255,0,0,COLOR_RED),
              COLOR_BLACK);
    init_pair(CLR_DETONATED,COLOR_YELLOW,COLOR_BLACK);

    init_pair(CLR_INFOBAR,
              xcolor(255,192,203,COLOR_WHITE),
              xcolor(255,20,88,COLOR_RED));
    init_pair(CLR_STATBAR,
              xcolor(88,0,0,COLOR_BLACK),
              xcolor(255,20,88,COLOR_RED));
    init_pair(CLR_MSGBAR,
              xcolor(255,192,203,COLOR_WHITE),
              xcolor(88,0,0,COLOR_BLACK));

    CharSet.ch_flag = UNI_TYPE;
    CharSet.name = "Dollie";
    CharSet.ULCorner=ACS_ULCORNER;
    CharSet.URCorner=ACS_URCORNER;
    CharSet.LLCorner=ACS_LLCORNER;
    CharSet.LRCorner=ACS_LRCORNER;
    CharSet.HLine=ACS_HLINE;
    CharSet.VLine=ACS_VLINE;
    CharSet.UArrow=ACS_UARROW;
    CharSet.DArrow=ACS_DARROW;
    CharSet.RArrow=ACS_RARROW;
    CharSet.LArrow=ACS_LARROW;
    CharSet.Mark.str="★";
    CharSet.FalseMark.str="★";
    CharSet.ShowFalseMark.str="☆";
    CharSet.Unknown.str="✿";
    CharSet.Mine.str="✿";
    CharSet.ShowMine.str="Ⳝ";
    CharSet.Space.str=" ";
    CharSet.Bombed.str="*";
    CharSet.CursorLeft.str="{";
    CharSet.CursorRight.str="}";
    break;

  case 6: // The Undead theme
    init_pair(CLR_NORMAL,COLOR_WHITE,COLOR_BLACK);
    init_pair(CLR_VAL8,
              xcolor(128,0,31,COLOR_BLUE),
              COLOR_BLACK);
    init_pair(CLR_VAL7,
              xcolor(160,0,62,COLOR_BLUE),
              COLOR_BLACK);
    init_pair(CLR_VAL6,
              xcolor(176,0,93,COLOR_BLUE),
              COLOR_BLACK);
    init_pair(CLR_VAL5,
              xcolor(192,0,124,COLOR_BLUE),
              COLOR_BLACK);
    init_pair(CLR_VAL4,
              xcolor(208,0,155,COLOR_BLUE),
              COLOR_BLACK);
    init_pair(CLR_VAL3,
              xcolor(224,0,186,COLOR_BLUE),
              COLOR_BLACK);
    init_pair(CLR_VAL2,
              xcolor(240,0,217,COLOR_BLUE),
              COLOR_BLACK);
    init_pair(CLR_VAL1,
              xcolor(255,0,248,COLOR_BLUE),
              COLOR_BLACK);
    init_pair(CLR_UNKNOWN,
              xcolor(60,60,60,COLOR_WHITE),
              COLOR_BLACK);
    init_pair(CLR_MINE,
              xcolor(60,60,60,COLOR_WHITE),
              COLOR_BLACK);
    init_pair(CLR_SHOWMINE,
              xcolor(105,105,105,COLOR_WHITE),
              COLOR_BLACK);

    init_pair(CLR_SPACE,COLOR_WHITE,COLOR_BLACK);
    init_pair(CLR_MARK,
              xcolor(255,255,0,COLOR_YELLOW),
              COLOR_BLACK);
    init_pair(CLR_FALSEMARK,
              xcolor(255,255,0,COLOR_YELLOW),
              COLOR_BLACK);
    init_pair(CLR_SHOWFALSEMARK,
              xcolor(255,0,0,COLOR_RED),
              COLOR_BLACK);
    init_pair(CLR_DETONATED,COLOR_YELLOW,COLOR_BLACK);

    init_pair(CLR_INFOBAR,
              xcolor(9,107,131,COLOR_WHITE),
              xcolor(22,22,22,COLOR_BLACK));
    init_pair(CLR_STATBAR,
              xcolor(9,107,131,COLOR_BLACK),
              xcolor(22,22,22,COLOR_BLACK));
    init_pair(CLR_MSGBAR,
              COLOR_BLACK,
              xcolor(5,62,76,COLOR_WHITE));

    CharSet.ch_flag = UNI_TYPE;
    CharSet.name = "Undead";
    CharSet.ULCorner=ACS_ULCORNER;
    CharSet.URCorner=ACS_URCORNER;
    CharSet.LLCorner=ACS_LLCORNER;
    CharSet.LRCorner=ACS_LRCORNER;
    CharSet.HLine=ACS_HLINE;
    CharSet.VLine=ACS_VLINE;
    CharSet.UArrow=ACS_UARROW;
    CharSet.DArrow=ACS_DARROW;
    CharSet.RArrow=ACS_RARROW;
    CharSet.LArrow=ACS_LARROW;
    CharSet.Mark.str="☥";
    CharSet.FalseMark.str="☥";
    CharSet.ShowFalseMark.str="⚔";
    CharSet.Unknown.str="◊";
    CharSet.Mine.str="◊";
    CharSet.ShowMine.str="Ⳝ";
    CharSet.Space.str=" ";
    CharSet.Bombed.str="*";
    CharSet.CursorLeft.str="☾";
    CharSet.CursorRight.str="☽";
    break;

  case 7: // The Battleship theme
    init_pair(CLR_NORMAL,COLOR_WHITE,COLOR_BLACK);
    init_pair(CLR_VAL1,COLOR_BLUE,COLOR_BLACK);
    init_pair(CLR_VAL2,COLOR_CYAN,COLOR_BLACK);
    init_pair(CLR_VAL3,COLOR_GREEN,COLOR_BLACK);
    init_pair(CLR_VAL4,COLOR_MAGENTA,COLOR_BLACK);
    init_pair(CLR_VAL5,COLOR_RED,COLOR_BLACK);
    init_pair(CLR_VAL6,COLOR_RED,COLOR_BLACK);
    init_pair(CLR_VAL7,COLOR_RED,COLOR_BLACK);
    init_pair(CLR_VAL8,COLOR_MAGENTA,COLOR_BLACK);

    init_pair(CLR_UNKNOWN,
              xcolor(0,191,255,COLOR_BLUE),
              COLOR_BLACK);
    init_pair(CLR_MINE,
              xcolor(0,191,255,COLOR_BLUE),
              COLOR_BLACK);
    init_pair(CLR_SHOWMINE,
              xcolor(105,105,105,COLOR_WHITE),
              COLOR_BLACK);

    init_pair(CLR_SPACE,COLOR_WHITE,COLOR_BLACK);
    init_pair(CLR_MARK,COLOR_WHITE,COLOR_BLACK);
    init_pair(CLR_FALSEMARK,COLOR_WHITE,COLOR_BLACK);
    init_pair(CLR_SHOWFALSEMARK,COLOR_WHITE,COLOR_BLACK);
    init_pair(CLR_DETONATED,COLOR_YELLOW,COLOR_BLACK);

    init_pair(CLR_INFOBAR,
              COLOR_BLACK,
              xcolor(0,191,255,COLOR_BLUE));
    init_pair(CLR_STATBAR,
              xcolor(78,51,0,COLOR_BLACK),
              xcolor(0,191,255,COLOR_BLUE));
    init_pair(CLR_MSGBAR,
              COLOR_WHITE,
              xcolor(0,0,128,COLOR_WHITE));

    CharSet.ch_flag = UNI_TYPE;
    CharSet.name = "Battleship";
    CharSet.ULCorner=ACS_ULCORNER;
    CharSet.URCorner=ACS_URCORNER;
    CharSet.LLCorner=ACS_LLCORNER;
    CharSet.LRCorner=ACS_LRCORNER;
    CharSet.HLine=ACS_HLINE;
    CharSet.VLine=ACS_VLINE;
    CharSet.UArrow=ACS_UARROW;
    CharSet.DArrow=ACS_DARROW;
    CharSet.RArrow=ACS_RARROW;
    CharSet.LArrow=ACS_LARROW;
    CharSet.Mark.str="⯛";
    CharSet.FalseMark.str="⯛";
    CharSet.ShowFalseMark.str="⯜";
    CharSet.Unknown.str="⌣";
    CharSet.Mine.str="⌣";
    CharSet.ShowMine.str="Ⳝ";
    CharSet.Space.str=" ";
    CharSet.Bombed.str="*";
    CharSet.CursorLeft.str="❨";
    CharSet.CursorRight.str="❩";
    break;

  case 8: // The Slate theme
    init_pair(CLR_NORMAL,COLOR_WHITE,COLOR_BLACK);
    init_pair(CLR_VAL8,
              xcolor(0,128,31,COLOR_BLUE),
              COLOR_BLACK);
    init_pair(CLR_VAL7,
              xcolor(0,160,62,COLOR_BLUE),
              COLOR_BLACK);
    init_pair(CLR_VAL6,
              xcolor(0,176,93,COLOR_BLUE),
              COLOR_BLACK);
    init_pair(CLR_VAL5,
              xcolor(0,192,124,COLOR_BLUE),
              COLOR_BLACK);
    init_pair(CLR_VAL4,
              xcolor(0,208,155,COLOR_BLUE),
              COLOR_BLACK);
    init_pair(CLR_VAL3,
              xcolor(0,224,186,COLOR_BLUE),
              COLOR_BLACK);
    init_pair(CLR_VAL2,
              xcolor(0,240,217,COLOR_BLUE),
              COLOR_BLACK);
    init_pair(CLR_VAL1,
              xcolor(0,255,248,COLOR_BLUE),
              COLOR_BLACK);

    init_pair(CLR_UNKNOWN,
              xcolor(101,115,130,COLOR_BLUE),
              COLOR_BLACK);
    init_pair(CLR_MINE,
              xcolor(101,115,130,COLOR_BLUE),
              COLOR_BLACK);
    init_pair(CLR_SHOWMINE,
              xcolor(105,105,105,COLOR_WHITE),
              COLOR_BLACK);

    init_pair(CLR_SPACE,COLOR_WHITE,COLOR_BLACK);
    init_pair(CLR_MARK,
              xcolor(255,255,173,COLOR_YELLOW),COLOR_BLACK);
    init_pair(CLR_FALSEMARK,
              xcolor(255,255,173,COLOR_YELLOW),COLOR_BLACK);
    init_pair(CLR_SHOWFALSEMARK,COLOR_RED,COLOR_BLACK);
    init_pair(CLR_DETONATED,COLOR_YELLOW,COLOR_BLACK);

    init_pair(CLR_INFOBAR,
              COLOR_BLACK,
              xcolor(112,128,144,COLOR_WHITE));
    init_pair(CLR_STATBAR,
              xcolor(41,41,101,COLOR_BLACK),
              xcolor(112,128,144,COLOR_WHITE));
    init_pair(CLR_MSGBAR,
              COLOR_BLACK,
              xcolor(112,128,144,COLOR_WHITE));

    CharSet.ch_flag = UNI_TYPE;
    CharSet.name = "Slate";
    CharSet.ULCorner=ACS_ULCORNER;
    CharSet.URCorner=ACS_URCORNER;
    CharSet.LLCorner=ACS_LLCORNER;
    CharSet.LRCorner=ACS_LRCORNER;
    CharSet.HLine=ACS_HLINE;
    CharSet.VLine=ACS_VLINE;
    CharSet.UArrow=ACS_UARROW;
    CharSet.DArrow=ACS_DARROW;
    CharSet.RArrow=ACS_RARROW;
    CharSet.LArrow=ACS_LARROW;
    CharSet.Mark.str="◆";
    CharSet.FalseMark.str="◆";
    CharSet.ShowFalseMark.str="◇";
    CharSet.Unknown.str="■";
    CharSet.Mine.str="■";
    CharSet.ShowMine.str="Ⳝ";
    CharSet.Space.str=" ";
    CharSet.Bombed.str="*";
    CharSet.CursorLeft.str="[";
    CharSet.CursorRight.str="]";
    break;

  case 9: // The cheater theme
    init_pair(CLR_NORMAL,COLOR_WHITE,COLOR_BLACK);
    init_pair(CLR_VAL1,COLOR_BLUE,COLOR_BLACK);
    init_pair(CLR_VAL2,COLOR_CYAN,COLOR_BLACK);
    init_pair(CLR_VAL3,COLOR_GREEN,COLOR_BLACK);
    init_pair(CLR_VAL4,COLOR_MAGENTA,COLOR_BLACK);
    init_pair(CLR_VAL5,COLOR_RED,COLOR_BLACK);
    init_pair(CLR_VAL6,COLOR_RED,COLOR_BLACK);
    init_pair(CLR_VAL7,COLOR_RED,COLOR_BLACK);
    init_pair(CLR_VAL8,COLOR_MAGENTA,COLOR_BLACK);
    init_pair(CLR_UNKNOWN,
              xcolor(165,42,42,COLOR_GREEN),
              COLOR_BLACK);
    init_pair(CLR_MINE,
              xcolor(181,46,46,COLOR_GREEN),
              COLOR_BLACK);
    init_pair(CLR_SPACE,COLOR_WHITE,COLOR_BLACK);
    init_pair(CLR_MARK,
              xcolor(255,255,148,COLOR_WHITE),
              COLOR_BLACK);
    init_pair(CLR_FALSEMARK,
              xcolor(255,148,148,COLOR_WHITE),
              COLOR_BLACK);

    init_pair(CLR_INFOBAR,
              xcolor(34,34,34,COLOR_BLACK),
              xcolor(255,0,0,COLOR_RED));
    init_pair(CLR_STATBAR,
              xcolor(0,0,88,COLOR_BLUE),
              xcolor(255,0,0,COLOR_RED));
    init_pair(CLR_MSGBAR,
              xcolor(88,0,0,COLOR_BLACK),
              xcolor(255,0,0,COLOR_RED));

    CharSet.ch_flag = UNI_TYPE;
    CharSet.name = "Cheater";

    CharSet.ULCorner='X';
    CharSet.URCorner='X';
    CharSet.LLCorner='X';
    CharSet.LRCorner='X';
    CharSet.HLine='~';
    CharSet.VLine='!';

    CharSet.UArrow=ACS_UARROW;
    CharSet.DArrow=ACS_DARROW;
    CharSet.RArrow=ACS_RARROW;
    CharSet.LArrow=ACS_LARROW;

    CharSet.Mark.str="↓";
    CharSet.FalseMark.str="X";
    CharSet.Unknown.str="⣀";
    CharSet.Mine.str="☁";
    CharSet.Space.str=" ";
    CharSet.Bombed.str="*";
    CharSet.CursorLeft.str="$";
    CharSet.CursorRight.str="$";
    break;
  }

  //WritePrefsFile(Game);
}
