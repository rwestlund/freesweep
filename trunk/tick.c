#include <stdio.h>
#include <stdlib.h>

#include "sweep.h"

volatile unsigned int g_tick = 0;

RETSIGTYPE sighandler(int signo)
{
	switch(signo)
	{
		case SIGALRM:
			g_tick++;
			signal(SIGALRM, sighandler);
			alarm(1);
			return;
			break;
		case SIGSEGV:
            clear();
            refresh();
            endwin();
			printf("\n\t***Segmentation Fault detected. Cleaning up....\n\n");
            exit(EXIT_FAILURE);
			break;
		case SIGBUS:
            clear();
            refresh();
            endwin();
			printf("\n\t***Bus Error detected. Cleaning up....\n\n");
            exit(EXIT_FAILURE);
			break;
		case SIGILL:
            clear();
            refresh();
            endwin();
			printf("\n\t***Illegal Instruction detected. Cleaning up....\n\n");
            exit(EXIT_FAILURE);
			break;
		default:
			break;
	}
	signal(SIGALRM, sighandler);
}
