#include <stdio.h>
#include <stdlib.h>

#include "sweep.h"

volatile unsigned int g_tick = 0;

void sighandler(int signo)
{
	switch(signo)
	{
		case SIGALRM:
			g_tick++;
			SweepMessage("%u", g_tick);
			signal(SIGALRM, sighandler);
			alarm(1);
			return;
			break;
		default:
			break;
	}
	signal(SIGALRM, sighandler);
}
