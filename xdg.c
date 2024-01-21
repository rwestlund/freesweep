/**********************************************************************
 *  This source code is copyright 2024 by Ron R Wills		      *
 *  It may be distributed under the terms of the GNU General Purpose  *
 *  License, version 2 or above; see the file COPYING for more	      *
 *  information.						      *
 *								      *
 **********************************************************************/

#include "sweep.h"
#include <string.h>

char *XDGConfigHome()
{
  char *home = getenv("HOME");
  char *home_config = getenv("XDG_CONFIG_HOME");

  // $HOME is not set, yikes!
  if (home == NULL)
  {
    perror("XDGConfigHome::getenv");
    return NULL;
  }

  if (home_config == NULL)
  {
    /* XDG_CONFIG_HOME hasn't been set so use the default location and make
     * sure it exists if possible.
     */
    char *result = (char*)xmalloc(MAX_LINE);

    // Make sure $HOME/.config exists
#if defined(HAVE_SNPRINTF)
    snprintf(result, MAX_LINE, "%s/.config", home);
#else
    sprintf(result, "%s/.config", home);
#endif
    if (!xexists(result))
    {
      if (mkdir(result, 0750) == -1)
      {
        perror("Unable to create the directory $HOME/.config");
        return NULL;
      }
    }

    // Make sure $HOME/.config/freesweep exists
    strncat(result, "/freesweep", MAX_LINE);
    if (!xexists(result))
    {
      if (mkdir(result, 0750) == -1)
      {
        perror("Unable to create the directory $HOME/.config/freesweep");
        return NULL;
      }
    }

    return result;
  }
  else
  {
    /* XDG_CONFIG_HOME has been set. I beleive it is up to the user to ensure
     * this directory exists we only attempt to create the freesweep directory
     * within it.
     */
    char *result = (char*)xmalloc(MAX_LINE);

#if defined(HAVE_SNPRINTF)
    snprintf(result, MAX_LINE, "%s/freesweep", home_config);
#else
    sprintf(result, "%s/freesweep", home_config);
#endif

    if (!xexists(result))
    {
      if (mkdir(result, 0750) == -1)
      {
        perror("Unable to create the directory $XDG_CONFIG_HOME/freesweep");
        return NULL;
      }
    }

    return result;
  }
}

char *XDGDataHome() {
  char *home = getenv("HOME");
  char *home_data = getenv("XDG_DATA_HOME");

  if (home == NULL)
  {
    perror("XDGDataHome::getenv");
    return NULL;
  }

  if (home_data == NULL)
  {
    /* XDG_DATA_HOME hasn't been set so use the default location and make
     * sure it exists if possible.
     */
    char *result = (char*)xmalloc(MAX_LINE);

    // Make sure $HOME/.local exists
#if defined(HAVE_SNPRINTF)
    snprintf(result, MAX_LINE, "%s/.local", home);
#else
    sprintf(result, "%s/.local", home);
#endif
    if (!xexists(result))
    {
      if (mkdir(result, 0750) == -1)
      {
        perror("Unable to create the directory $HOME/.local");
        return NULL;
      }
    }

    // Make sure $HOME/.local/share exists
    strncat(result, "/share", MAX_LINE);
    if (!xexists(result))
    {
      if (mkdir(result, 0750) == -1)
      {
        perror("Unable to create the directory $HOME/.local/share");
        return NULL;
      }
    }

    // Make sure $HOME/.local/share/freesweep exists
    strncat(result, "/freesweep", MAX_LINE);
    if (!xexists(result))
    {
      if (mkdir(result, 0750) == -1)
      {
        perror("Unable to create the directory $HOME/.local/share/freesweep");
        return NULL;
      }
    }

    return result;
  } else {
    char *result = (char*)xmalloc(MAX_LINE);

#if defined(HAVE_SNPRINTF)
    snprintf(result, MAX_LINE, "%s/freesweep", home_data);
#else
    sprintf(result, "%s/freesweep", home_data);
#endif

    if (!xexists(result))
    {
      if (mkdir(result, 0750) == -1)
      {
        perror("Unable to create the directory $XDG_DATA_HOME/freesweep");
        return NULL;
      }
    }

    return result;
  }
}
