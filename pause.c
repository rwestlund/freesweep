/*                                                                    -*- c -*-
 * Pause screen for the game.
 * Copyright (C) 2024  Ron Wills <ron@digitalcombine.ca>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 51
 * Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "sweep.h"
#include <pwd.h>

static WINDOW* pause_win = NULL;
static WINDOW* scroll_win = NULL;

static char* help_mesgs[] = {
  "Playing the Game",
  "",
  "  The purpose of the game is to flag all the mines on the",
  " game board with out exposing a mine.",
  "  To start the game you can set the size of the game board",
  " and the amount of mines found in the board. The amount of",
  " mines is set as a percentage of the total number of cells",
  " of the board.",
  "  When a cell is exposed, that doesn't have a bomb, it can",
  " be blank or have a number 1 thru 8. This is the clues to",
  " where the hidden bombs are. A blank cell has no bombs",
  " adjacent to it. The number 1 thru 8 is the number of bombs",
  " found beside the cell.",
  "  It\'s treacherous work! Good luck!",
  "",
  "Basic Controls",
  "",
  "  The arrow keys will move you around in every screen.",
  " During game play it\'ll move the cursor to the adjacent",
  " cell.",
  "  The Page Up and Page Down keys will move the cursor to",
  " the top and the bottom of the board.",
  "  The Home and End keys will move the cursor the the left",
  " most side and the right most side of the board",
  "  The \'f\' key will flag or unflag the cell at the cursor.",
  " When all the mines are flagged correctly the game is won.",
  "  The space bar or enter key will expose the cell at the",
  " cursor. If the cell contains a mine the game is over.",
  "  The \'a\' or ecaspe key will end and abort the current game.",
  "  The \'q\' key will save the current game and exit",
  " freesweep. When the game is started again the saved game",
  " will be loaded and continued.",
  "",
  "Vi Like Key Bindings",
  "",
  " \'h\'  moves the curser to the left.",
  " \'j\'  moves the curser to the down.",
  " \'k\'  moves the curser to the up.",
  " \'l\'  moves the curser to the right.",
  " \'0\'  moves the cursor to the left side of the board.",
  " \'$\'  moves the cursor to the right side of the board.",
  " \'gg\' moves the cursor to the top of the board.",
  " \'G\'  moves the cursor to the bottom of the board.",
  "",
  "Emacs Like Key Bindings",
  ""
  " ctrl-\'l\' redraws the screen.",
  " ctrl-\'a\' moves the cursor to the left side of the board.",
  " ctrl-\'e\' moves the cursor to the right side of the board.",
  NULL
};

static char* credit_mesgs[] = {
  "Original Developers",
  "",
  "  Gus Hartmann <hartmann@cs.wisc.edu>",
  "  Pete \"Psilord\" Keller <psilord@cs.wisc.edu>",
  "",
  "Current Maintainers/Developers",
  "",
  "  Randy Westlund <rwestlun@gmail.com>",
  "  Ron Wills <ron@digitalcombine.ca>",
  "",
  "Color conversion code borrowed from the tmux project",
  "",
  "  Nicholas Marriott <nicholas.marriott@gmail.com>",
  "  Avi Halachmi <avihpit@yahoo.com>",
  "",
  " Special thanks to Pete Keller for lots of help.",
  " Thanks to the FSF for the license and certain chunks of",
  "code.",
  " Thanks to the Undergraduate Project Lab at the University",
  "of Wisconsin",
  NULL
};

static char* gpl_mesg[] = {
  "              GNU GENERAL PUBLIC LICENSE",
  "                 Version 2, June 1991",
  "",
  "Copyright (C) 1989, 1991 Free Software Foundation, Inc.",
  "51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA",
  "Everyone is permitted to copy and distribute verbatim",
  "copies of this license document, but changing it is not",
  "allowed.",
  "",
  "                      Preamble",
  "",
  "  The licenses for most software are designed to take away",
  "your freedom to share and change it.  By contrast, the GNU",
  "General Public License is intended to guarantee your freedom",
  "to share and change free software--to make sure the software",
  "is free for all its users. This General Public License",
  "applies to most of the Free Software Foundation's software",
  "and to any other program whose authors commit to using it.",
  "(Some other Free Software Foundation software is covered by",
  "the GNU Library General Public License instead.)  You can",
  "apply it to your programs, too.",
  "",
  "  When we speak of free software, we are referring to",
  "freedom, not price.  Our General Public Licenses are designed",
  "to make sure that you have the freedom to distribute copies",
  "of free software (and charge for this service if you wish),",
  "that you receive source code or can get it if you want it,",
  "that you can change the software or use pieces of it in new",
  "free programs; and that you know you can do these things.",
  "",
  "  To protect your rights, we need to make restrictions that",
  "forbid anyone to deny you these rights or to ask you to",
  "surrender the rights. These restrictions translate to certain",
  "responsibilities for you if you distribute copies of the",
  "software, or if you modify it.",
  "",
  "  For example, if you distribute copies of such a program,",
  "whether gratis or for a fee, you must give the recipients all",
  "the rights that you have. You must make sure that they, too,",
  "receive or can get the source code. And you must show them",
  "these terms so they know their rights.",
  "",
  "  We protect your rights with two steps: (1) copyright the",
  "software, and (2) offer you this license which gives you legal",
  "permission to copy, distribute and/or modify the software.",
  "",
  "  Also, for each author's protection and ours, we want to make",
  "certain that everyone understands that there is no warranty",
  "for this free software. If the software is modified by someone",
  "else and passed on, we want its recipients to know that what",
  "they have is not the original, so that any problems introduced",
  "by others will not reflect on the original authors' reputations.",
  "",
  "  Finally, any free program is threatened constantly by software",
  "patents.  We wish to avoid the danger that redistributors of a",
  "free program will individually obtain patent licenses, in effect",
  "making the program proprietary.  To prevent this, we have made",
  "it clear that any patent must be licensed for everyone's free",
  "use or not licensed at all.",
  "",
  "  The precise terms and conditions for copying, distribution and",
  "modification follow.",
  "",
  "                 GNU GENERAL PUBLIC LICENSE",
  "TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION",
  "",
  "  0. This License applies to any program or other work which",
  "contains a notice placed by the copyright holder saying it may",
  "be distributed under the terms of this General Public License.",
  "The \"Program\", below, refers to any such program or work, and",
  "a \"work based on the Program\" means either the Program or any",
  "derivative work under copyright law:",
  "that is to say, a work containing the Program or a portion of",
  "it, either verbatim or with modifications and/or translated into",
  "another language.  (Hereinafter, translation is included without",
  "limitation in the term \"modification\".)  Each licensee is",
  "addressed as \"you\".",
  "",
  "Activities other than copying, distribution and modification are",
  "not covered by this License; they are outside its scope.  The act",
  "of running the Program is not restricted, and the output from the",
  "Program is covered only if its contents constitute a work based",
  "on the Program (independent of having been made by running the",
  "Program). Whether that is true depends on what the Program does.",
  "",
  "  1. You may copy and distribute verbatim copies of the Program's",
  "source code as you receive it, in any medium, provided that you",
  "conspicuously and appropriately publish on each copy an",
  "appropriate copyright notice and disclaimer of warranty; keep",
  "intact all the notices that refer to this License and to the",
  "absence of any warranty; and give any other recipients of the",
  "Program a copy of this License along with the Program.",
  "",
  "You may charge a fee for the physical act of transferring a copy,",
  "and you may at your option offer warranty protection in exchange",
  "for a fee.",
  "",
  "  2. You may modify your copy or copies of the Program or any",
  "portion of it, thus forming a work based on the Program, and",
  "copy and distribute such modifications or work under the terms",
  "of Section 1 above, provided that you also meet all of these",
  "conditions:",
  "",
  "    a) You must cause the modified files to carry prominent",
  "    notices stating that you changed the files and the date of",
  "    any change.",
  "",
  "    b) You must cause any work that you distribute or publish,",
  "    that in whole or in part contains or is derived from the",
  "    Program or any part thereof, to be licensed as a whole at no",
  "    charge to all third parties under the terms of this License.",
  "",
  "    c) If the modified program normally reads commands",
  "    interactively when run, you must cause it, when started",
  "    running for such interactive use in the most ordinary way,",
  "    to print or display an announcement including an appropriate",
  "    copyright notice and a notice that there is no warranty (or",
  "    else, saying that you provide a warranty) and that users may",
  "    redistribute the program under these conditions, and telling",
  "    the user how to view a copy of this License. (Exception: if",
  "    the Program itself is interactive but does not normally print",
  "    such an announcement, your work based on the Program is not",
  "    required to print an announcement.)",
  "",
  "These requirements apply to the modified work as a whole.  If",
  "identifiable sections of that work are not derived from the Program,",
  "and can be reasonably considered independent and separate works in",
  "themselves, then this License, and its terms, do not apply to those",
  "sections when you distribute them as separate works.  But when you",
  "distribute the same sections as part of a whole which is a work based",
  "on the Program, the distribution of the whole must be on the terms of",
  "this License, whose permissions for other licensees extend to the",
  "entire whole, and thus to each and every part regardless of who",
  "wrote it.",
  "",
  "Thus, it is not the intent of this section to claim rights or contest",
  "your rights to work written entirely by you; rather, the intent is to",
  "exercise the right to control the distribution of derivative or",
  "collective works based on the Program.",
  "",
  "In addition, mere aggregation of another work not based on the Program",
  "with the Program (or with a work based on the Program) on a volume of",
  "a storage or distribution medium does not bring the other work under",
  "the scope of this License.",
  "",
  "  3. You may copy and distribute the Program (or a work based on it,",
  "under Section 2) in object code or executable form under the terms of",
  "Sections 1 and 2 above provided that you also do one of the following:",
  "",
  "    a) Accompany it with the complete corresponding machine-readable",
  "    source code, which must be distributed under the terms of Sections",
  "    1 and 2 above on a medium customarily used for software interchange; or,",
  "",
  "    b) Accompany it with a written offer, valid for at least three",
  "    years, to give any third party, for a charge no more than your",
  "    cost of physically performing source distribution, a complete",
  "    machine-readable copy of the corresponding source code, to be",
  "    distributed under the terms of Sections 1 and 2 above on a medium",
  "    customarily used for software interchange; or,",
  "",
  "    c) Accompany it with the information you received as to the offer",
  "    to distribute corresponding source code.  (This alternative is",
  "    allowed only for noncommercial distribution and only if you",
  "    received the program in object code or executable form with such",
  "    an offer, in accord with Subsection b above.)",
  "",
  "The source code for a work means the preferred form of the work for",
  "making modifications to it.  For an executable work, complete source",
  "code means all the source code for all modules it contains, plus any",
  "associated interface definition files, plus the scripts used to",
  "control compilation and installation of the executable.  However, as a",
  "special exception, the source code distributed need not include",
  "anything that is normally distributed (in either source or binary",
  "form) with the major components (compiler, kernel, and so on) of the",
  "operating system on which the executable runs, unless that component",
  "itself accompanies the executable.",
  "",
  "If distribution of executable or object code is made by offering",
  "access to copy from a designated place, then offering equivalent",
  "access to copy the source code from the same place counts as",
  "distribution of the source code, even though third parties are not",
  "compelled to copy the source along with the object code.",
  "",
  "  4. You may not copy, modify, sublicense, or distribute the Program",
  "except as expressly provided under this License.  Any attempt",
  "otherwise to copy, modify, sublicense or distribute the Program is",
  "void, and will automatically terminate your rights under this License.",
  "However, parties who have received copies, or rights, from you under",
  "this License will not have their licenses terminated so long as such",
  "parties remain in full compliance.",
  "",
  "  5. You are not required to accept this License, since you have not",
  "signed it.  However, nothing else grants you permission to modify or",
  "distribute the Program or its derivative works.  These actions are",
  "prohibited by law if you do not accept this License.  Therefore, by",
  "modifying or distributing the Program (or any work based on the",
  "Program), you indicate your acceptance of this License to do so, and",
  "all its terms and conditions for copying, distributing or modifying",
  "the Program or works based on it.",
  "",
  "  6. Each time you redistribute the Program (or any work based on the",
  "Program), the recipient automatically receives a license from the",
  "original licensor to copy, distribute or modify the Program subject to",
  "these terms and conditions.  You may not impose any further",
  "restrictions on the recipients' exercise of the rights granted herein.",
  "You are not responsible for enforcing compliance by third parties to",
  "this License.",
  "",
  "  7. If, as a consequence of a court judgment or allegation of patent",
  "infringement or for any other reason (not limited to patent issues),",
  "conditions are imposed on you (whether by court order, agreement or",
  "otherwise) that contradict the conditions of this License, they do not",
  "excuse you from the conditions of this License.  If you cannot",
  "distribute so as to satisfy simultaneously your obligations under this",
  "License and any other pertinent obligations, then as a consequence you",
  "may not distribute the Program at all.  For example, if a patent",
  "license would not permit royalty-free redistribution of the Program by",
  "all those who receive copies directly or indirectly through you, then",
  "the only way you could satisfy both it and this License would be to",
  "refrain entirely from distribution of the Program.",
  "",
  "If any portion of this section is held invalid or unenforceable under",
  "any particular circumstance, the balance of the section is intended to",
  "apply and the section as a whole is intended to apply in other",
  "circumstances.",
  "",
  "It is not the purpose of this section to induce you to infringe any",
  "patents or other property right claims or to contest validity of any",
  "such claims; this section has the sole purpose of protecting the",
  "integrity of the free software distribution system, which is",
  "implemented by public license practices.  Many people have made",
  "generous contributions to the wide range of software distributed",
  "through that system in reliance on consistent application of that",
  "system; it is up to the author/donor to decide if he or she is willing",
  "to distribute software through any other system and a licensee cannot",
  "impose that choice.",
  "",
  "This section is intended to make thoroughly clear what is believed to",
  "be a consequence of the rest of this License.",
  "",
  "  8. If the distribution and/or use of the Program is restricted in",
  "certain countries either by patents or by copyrighted interfaces, the",
  "original copyright holder who places the Program under this License",
  "may add an explicit geographical distribution limitation excluding",
  "those countries, so that distribution is permitted only in or among",
  "countries not thus excluded.  In such case, this License incorporates",
  "the limitation as if written in the body of this License.",
  "",
  "  9. The Free Software Foundation may publish revised and/or new versions",
  "of the General Public License from time to time.  Such new versions will",
  "be similar in spirit to the present version, but may differ in detail to",
  "address new problems or concerns.",
  "",
  "Each version is given a distinguishing version number.  If the Program",
  "specifies a version number of this License which applies to it and \"any",
  "later version\", you have the option of following the terms and conditions",
  "either of that version or of any later version published by the Free",
  "Software Foundation.  If the Program does not specify a version number of",
  "this License, you may choose any version ever published by the Free Software",
  "Foundation.",
  "",
  "  10. If you wish to incorporate parts of the Program into other free",
  "programs whose distribution conditions are different, write to the author",
  "to ask for permission.  For software which is copyrighted by the Free",
  "Software Foundation, write to the Free Software Foundation; we sometimes",
  "make exceptions for this.  Our decision will be guided by the two goals",
  "of preserving the free status of all derivatives of our free software and",
  "of promoting the sharing and reuse of software generally.",
  "",
  "                           NO WARRANTY",
  "",
  "  11. BECAUSE THE PROGRAM IS LICENSED FREE OF CHARGE, THERE IS NO WARRANTY",
  "FOR THE PROGRAM, TO THE EXTENT PERMITTED BY APPLICABLE LAW.  EXCEPT WHEN",
  "OTHERWISE STATED IN WRITING THE COPYRIGHT HOLDERS AND/OR OTHER PARTIES",
  "PROVIDE THE PROGRAM \"AS IS\" WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED",
  "OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF",
  "MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  THE ENTIRE RISK AS",
  "TO THE QUALITY AND PERFORMANCE OF THE PROGRAM IS WITH YOU.  SHOULD THE",
  "PROGRAM PROVE DEFECTIVE, YOU ASSUME THE COST OF ALL NECESSARY SERVICING,",
  "REPAIR OR CORRECTION.",
  "",
  "  12. IN NO EVENT UNLESS REQUIRED BY APPLICABLE LAW OR AGREED TO IN WRITING",
  "WILL ANY COPYRIGHT HOLDER, OR ANY OTHER PARTY WHO MAY MODIFY AND/OR",
  "REDISTRIBUTE THE PROGRAM AS PERMITTED ABOVE, BE LIABLE TO YOU FOR DAMAGES,",
  "INCLUDING ANY GENERAL, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES ARISING",
  "OUT OF THE USE OR INABILITY TO USE THE PROGRAM (INCLUDING BUT NOT LIMITED",
  "TO LOSS OF DATA OR DATA BEING RENDERED INACCURATE OR LOSSES SUSTAINED BY",
  "YOU OR THIRD PARTIES OR A FAILURE OF THE PROGRAM TO OPERATE WITH ANY OTHER",
  "PROGRAMS), EVEN IF SUCH HOLDER OR OTHER PARTY HAS BEEN ADVISED OF THE",
  "POSSIBILITY OF SUCH DAMAGES.",
  "",
  "                    END OF TERMS AND CONDITIONS",
  "",
  "           How to Apply These Terms to Your New Programs",
  "",
  "  If you develop a new program, and you want it to be of the greatest",
  "possible use to the public, the best way to achieve this is to make it",
  "free software which everyone can redistribute and change under these terms.",
  "",
  "  To do so, attach the following notices to the program.  It is safest",
  "to attach them to the start of each source file to most effectively",
  "convey the exclusion of warranty; and each file should have at least",
  "the \"copyright\" line and a pointer to where the full notice is found.",
  "",
  "    <one line to give the program's name and a brief idea of what it does.>",
  "    Copyright (C) 19yy  <name of author>",
  "",
  "    This program is free software; you can redistribute it and/or modify",
  "    it under the terms of the GNU General Public License as published by",
  "    the Free Software Foundation; either version 2 of the License, or",
  "    (at your option) any later version.",
  "",
  "    This program is distributed in the hope that it will be useful,",
  "    but WITHOUT ANY WARRANTY; without even the implied warranty of",
  "    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the",
  "    GNU General Public License for more details.",
  "",
  "    You should have received a copy of the GNU General Public License",
  "    along with this program; if not, write to the Free Software",
  "    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,",
  "    MA 02110-1301 USA.",
  "",
  "",
  "Also add information on how to contact you by electronic and paper mail.",
  "",
  "If the program is interactive, make it output a short notice like this",
  "when it starts in an interactive mode:",
  "",
  "    Gnomovision version 69, Copyright (C) 19yy name of author",
  "    Gnomovision comes with ABSOLUTELY NO WARRANTY; for details type `show w'.",
  "    This is free software, and you are welcome to redistribute it",
  "    under certain conditions; type `show c' for details.",
  "",
  "The hypothetical commands `show w' and `show c' should show the appropriate",
  "parts of the General Public License.  Of course, the commands you use may",
  "be called something other than `show w' and `show c'; they could even be",
  "mouse-clicks or menu items--whatever suits your program.",
  "",
  "You should also get your employer (if you work as a programmer) or your",
  "school, if any, to sign a \"copyright disclaimer\" for the program, if",
  "necessary.  Here is a sample; alter the names:",
  "",
  "  Yoyodyne, Inc., hereby disclaims all copyright interest in the program",
  "  `Gnomovision' (which makes passes at compilers) written by James Hacker.",
  "",
  "  <signature of Ty Coon>, 1 April 1989",
  "  Ty Coon, President of Vice",
  "",
  "This General Public License does not permit incorporating your program into",
  "proprietary programs.  If your program is a subroutine library, you may",
  "consider it more useful to permit linking proprietary applications with the",
  "library.  If this is what you want to do, use the GNU Library General",
  "Public License instead of this License.",
  NULL
};

/****************
 * scroll_mesgs *
 ****************/

static int scroll_mesgs(const char *mesgs[]) {
  /* Manages a scrollable display for mesgs.
   * Returns:
   *  0 for quit.
   *  1 for tab.
   */
  int start_y = 0, i, input;
  int start_x = 0, length;
  int width, height, count;

  getmaxyx(scroll_win, height, width);
  height--;
  for (count = 0; mesgs[count] != NULL; count++);

  while (1) {
    wclear(scroll_win);

    // Display the message.
    length = 0;
    for (i = 0; i < height && mesgs[start_y + i] != NULL; i++) {
      int slen = strlen(mesgs[start_y + i]);

      if (start_x <= slen) {
        mvwprintw(scroll_win, i, 1, "%s",
                  xtrunc(width - 2, &mesgs[start_y + i][start_x]));
      }

      length = xmax(length, slen);
    }

    // Vertical scroll indicator.
    if (start_y > 0 || mesgs[start_y + i] != NULL) {
      mvwvline(scroll_win, 1, width - 1, CharSet.VLine, height - 2);
      if (mesgs[start_y + i] != NULL)
        mvwaddch(scroll_win, height - 1, width - 1, CharSet.DArrow);
      if (start_y > 0)
        mvwaddch(scroll_win, 0, width - 1, CharSet.UArrow);
    }

    // Horizontal scroll indicator
    if (start_x > 0 || length > width) {
      mvwhline(scroll_win, height, 2, CharSet.HLine, width - 3);
      if (length - start_x > width - 2)
        mvwaddch(scroll_win, height, width - 1, CharSet.RArrow);
      if (start_x > 0)
        mvwaddch(scroll_win, height, 1, CharSet.LArrow);
    }

    // Refresh the terminal.
    wrefresh(scroll_win);

    input = getch();
    switch (input) {
    case KEY_PPAGE:
      start_y -= (height / 2);
      if (start_y < 0) start_y = 0;
      break;
    case KEY_NPAGE:
      start_y += (height / 2);
      if (start_y + height > count)
        start_y = count - height;
      break;
    case KEY_HOME:
      start_x = 0;
      break;
    case KEY_END:
      start_x = length - (width - 2);
      break;
    case KEY_UP:
      if (start_y > 0) start_y--;
      break;
    case KEY_DOWN:
      if (mesgs[start_y + i] != NULL) start_y++;
      break;
    case KEY_LEFT:
      if (start_x > 0) start_x--;
      break;
    case KEY_RIGHT:
      if (length - start_x > width - 2) start_x++;
      break;
    case '\t':
      wclear(scroll_win);
      wrefresh(scroll_win);
      return 1;
    case '\e': // Escape key.
    case 'q':
      wclear(scroll_win);
      wrefresh(scroll_win);
      return 0;
    }
  }
  return 0;
}

/*****************
 * scroll_scores *
 *****************/

static int scroll_scores() {
  int height, width;
  bests_table_t* bfd = NULL;
  int i, input;
  time_t enttime;

  getmaxyx(scroll_win, height, width);

  mvwprintw(scroll_win, 0, 4,  "Player");
  mvwhline(scroll_win,  1, 1,  CharSet.HLine, 12);
  mvwprintw(scroll_win, 0, 16, "Area");
  mvwhline(scroll_win,  1, 14, CharSet.HLine, 8);
  mvwprintw(scroll_win, 0, 24, "Mines");
  mvwhline(scroll_win,  1, 23, CharSet.HLine, 7);
  mvwprintw(scroll_win, 0, 33, "Time");
  mvwhline(scroll_win,  1, 31, CharSet.HLine, 8);
  mvwprintw(scroll_win, 0, 50, "Date");
  mvwhline(scroll_win,  1, 40, CharSet.HLine, 24);

  bfd = bests_load();
  for (i = 0; i < bfd->numents && i < height - 6; i++) {
    const struct passwd *pw = getpwuid(bfd->entries[i].user);
    mvwprintw(scroll_win, 2 + i, 1, "%s", pw->pw_name);
    mvwprintw(scroll_win, 2 + i, 14, "%u", bfd->entries[i].area);
    mvwprintw(scroll_win, 2 + i, 23, "%u", bfd->entries[i].mines);
    enttime = bfd->entries[i].time;
    if (enttime >= 86400) {
      mvwprintw(scroll_win, 2 + i, 31,
                "%ld:%02ld:%02ld:%02lds",
                enttime / 86400, (enttime % 86400) / 3600,
                (enttime % 3600) / 60 , enttime % 60);
    } else if (enttime >= 3600) {
      mvwprintw(scroll_win, 2 + i, 31, "%ld:%02ld:%02lds",
                enttime / 3600, (enttime % 3600) / 60,
                enttime % 60);
    } else if (enttime >= 60) {
      mvwprintw(scroll_win, 2 + i, 31, "%ld:%02lds",
                enttime / 60, enttime % 60);
    } else {
      mvwprintw(scroll_win, 2 + i, 31, "%lds", enttime);
    }
    mvwprintw(scroll_win, 2 + i, 40, "%s",
              xtrunc(width - 40, ctime(&bfd->entries[i].date)));
  }
  bests_free(bfd);
  wrefresh(scroll_win);

  while (1) {
    input = getch();
    switch (input) {
    case '\t':
      wclear(scroll_win);
      wrefresh(scroll_win);
      return 1;
    case 'q':
    case '\e': // Escape key
      wclear(scroll_win);
      wrefresh(scroll_win);
      return 0;
    }
  }
}

/***************
 * scroll_logs *
 ***************/

static int scroll_logs() {
  log_t* mesgs = logs();
  int start_y = 0, i, input;
  int width, height;

  getmaxyx(scroll_win, height, width);

  while (1) {
    wclear(scroll_win);

    for (i = start_y, mesgs = logs(); i > 0 && mesgs != NULL;
         i--, mesgs = mesgs->next);
    for (i = 0; i < height && mesgs != NULL; i++, mesgs = mesgs->next) {
      mvwprintw(scroll_win, i, 1, "%s", xtrunc(width - 2, mesgs->line));
    }
    if (start_y > 0 || mesgs != NULL) {
      // Show scroll here
    }
    wrefresh(scroll_win);

    input = getch();
    switch (input) {
    case KEY_UP:
      if (start_y > 0) start_y--;
      break;
    case KEY_DOWN:
      if (mesgs != NULL) start_y++;
      break;
    case '\t':
      wclear(scroll_win);
      wrefresh(scroll_win);
      return 1;
    case 'q': // Include ESC Key.
      wclear(scroll_win);
      wrefresh(scroll_win);
      return 0;
    }
  }
  return 0;
}

/*****************
 * pause_display *
 *****************/

void pause_display() {
  int tab = 1;
  char cont = 1;

  if ((pause_win = newwin(LINES - 1, COLS - INFO_W, 0, 0)) == NULL) {
    log_message("Unable to create the pause window");
    return;
  }

  if ((scroll_win = derwin(pause_win, LINES - 4, COLS - INFO_W,
                           3, 0)) == NULL) {
    log_message("Unable to create pause the scrolled window");
    delwin(pause_win);
    pause_win = NULL;
    return;
  }

  log_status("Pausing game...");

  scrollok(scroll_win, FALSE);
  wclear(pause_win);

  while (cont) {
    mvwprintw(pause_win, 0, 1,
              "Paused                                          ");
    mvwprintw(pause_win, 1, 8, " Help    Scores  Credits Log     GPL     ");
    mvwhline(pause_win, 2, 1, CharSet.HLine, COLS - INFO_W - 2);
    mvwhline(pause_win, 0, 9 + (8 * (tab - 1)), CharSet.HLine, 7);
    mvwhline(pause_win, 2, 9 + (8 * (tab - 1)), ' ', 7);
    mvwaddch(pause_win, 0, 8 + (8 * (tab - 1)), CharSet.ULCorner);
    mvwaddch(pause_win, 0, 16 + (8 * (tab - 1)), CharSet.URCorner);
    mvwaddch(pause_win, 1, 8 + (8 * (tab - 1)), CharSet.VLine);
    mvwaddch(pause_win, 1, 16 + (8 * (tab - 1)), CharSet.VLine);
    mvwaddch(pause_win, 2, 8 + (8 * (tab - 1)), CharSet.LRCorner);
    mvwaddch(pause_win, 2, 16 + (8 * (tab - 1)), CharSet.LLCorner);
    wrefresh(pause_win);

    switch (tab) {
    case 1:
      if (scroll_mesgs((const char **)help_mesgs)) tab = 2;
      else cont = 0;
      break;
    case 2:
      if (scroll_scores()) tab = 3;
      else cont = 0;
      break;
    case 3:
      if (scroll_mesgs((const char **)credit_mesgs)) tab = 4;
      else cont = 0;
      break;
    case 4:
      if (scroll_logs()) tab = 5;
      else cont = 0;
      break;
    case 5:
      if (scroll_mesgs((const char **)gpl_mesg)) tab = 1;
      else cont = 0;
      break;
    }
  }

  delwin(pause_win);
  delwin(scroll_win);
  pause_win = NULL;
  scroll_win = NULL;

  log_status("");
}
