/*********************************************************************
* $Id: hash.c,v 1.1.1.1 1998-11-23 03:57:08 hartmann Exp $
*********************************************************************/

#include "sweep.h"

CoordPair* GenerateCoordPair(int ThisX, int ThisY)
{
	CoordPair* NewPair;
	if ((NewPair=malloc(sizeof(CoordPair)))<=0)
	{
		perror("GenerateCoordPair::AllocCoordPair");
	}

	NewPair->CoordX=ThisX;
	NewPair->CoordY=ThisY;

	return NewPair;
}
		
