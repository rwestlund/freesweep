/*********************************************************************
* $Id: bests.c,v 1.2 1999-02-10 23:49:24 hartmann Exp $
*********************************************************************/

#include "sweep.h"
/* CompNodeAttribs() takes a pair of nodes, and compares their attributes,
   first numerically by height, then numerically by width, then numerically
   by number of mines. If they are equal in all these respect, 0 is returned.
   Return values less than zero means the second node was greater, and greater
   than zero indicates Node1 was greater.
*/
int CompNodeAttribs(BestTimeNode* Node1, BestTimeNode* Node2)
{
	int Value1=0, Value2=0;

	if ((Node1 == NULL) && (Node2 == NULL))
	{
		return 0;
	}
	else if (Node1 == NULL)
	{
		return -1;
	}
	else if (Node2 == NULL)
	{
		return 1;
	}

	if (strcasecmp(Node1->Attributes,Node2->Attributes)==0)
	{
		return 0;
	}
	else
	{
		Value1=atoi(index(Node1->Attributes,'h')+1);
		Value2=atoi(index(Node2->Attributes,'h')+1);
		if (Value1!=Value2)
		{
			return (Value1-Value2);
		}

		Value1=atoi(index(Node1->Attributes,'w')+1);
		Value2=atoi(index(Node2->Attributes,'w')+1);
		if (Value1!=Value2)
		{
			return (Value1-Value2);
		}

		Value1=atoi(index(Node1->Attributes,'m')+1);
		Value2=atoi(index(Node2->Attributes,'m')+1);

		if (Value1!=Value2)
		{
			return (Value1-Value2);
		}
		/* If all the values are the same, it should have already returned 0 */
		return 0;
	}
}


/* CompNodeTImes() returns the difference of the times in Node1 and Node2.
   A return value of 0 would indicate equality.
  */
int CompNodeTimes(BestTimeNode* Node1, BestTimeNode* Node2)
{
	return (Node1->Time-Node2->Time);
}

/* MigrateBestTimes() converts the previous file format of best times files
   into the current format, which is superior in all respects. Returns non-zero
   on error, zero on successful completion.
   As a part of the conversion process, MigrateBestTimes() eliminates all but
   the lowest time for a given set of node attributes.
*/
int MigrateBestTimes(FILE* OldFile)
{
	char FileBuffer[MAX_LINE], PathBuffer[MAX_LINE];
	char *AttribBuffer, *HomePath;
	unsigned char* ValueBuffer;
	BestTimeNode *Head, *Current, *Compare, *Trash;
	FILE* NewFile;
	struct stat ExistCheck;
	int Low=0, Written=0, BestTimeLen=0;

	/* Once Again, change the path! */
	if ((HomePath=getenv("HOME"))==NULL)
	{
		perror("MigrateBestTimes::getenv(HOME)");
		return 1;
	}
	
	strcpy(PathBuffer,HomePath);
	strcat(PathBuffer,"/");
	strcat(PathBuffer,DFL_BESTS_FILE);

	if (stat(PathBuffer,&ExistCheck)==-1)
	{
		if (errno!=ENOENT)
		{
			perror("MigrateBestTimes::CheckBestTimesFile");
#ifdef DEBUG_LOG
			fprintf(DebugLog,"Prefs file %s doesn't seem to exist.\n",PathBuffer);
			fflush(DebugLog);
#endif /* DEBUG_LOG */
			return 1;
		}
	}
	else
	{
		fprintf(stderr,"BestTime file $HOME/%s exists.\n",DFL_BESTS_FILE);
		return 1;
	}

	Current=Head=NULL;
	while (fgets(FileBuffer,MAX_LINE,OldFile)!=0)
	{
		/* Split the line into component parts. */
		if (((AttribBuffer=strtok(FileBuffer,"tT\n\t"))==NULL)||((ValueBuffer=strtok(NULL,"tT\n\t"))==NULL))
		{
			fprintf(stderr,"Bad entry in BestTimes file: %s\n",FileBuffer);
			break;
		}
		else
		{
			/* Allocate a node to keep the components in. */
			if (Head==NULL)
			{
				if ((Head=malloc(sizeof(BestTimeNode)))==NULL)
				{
					perror("MigrateBestTimes::AllocNode");
					return 1;
				}
				Current=Head;
			}
			else
			{
				if ((Current->Next=malloc(sizeof(BestTimeNode)))==NULL)
				{
					perror("MigrateBestTimes::AllocNode");
					return 1;
				}
				Current=Current->Next;
			}

			/* Try to duplicate the string into the node. */
			if ((Current->Attributes=strdup(AttribBuffer))==NULL)
			{
				perror("MigrateBestTimes::DupBuffer");
				return 1;
			}
			else if (ValueBuffer!=NULL)
			{
				Current->Time=atoi(ValueBuffer);
			}
			else
			{
				/* I think I should do something here, but I don't know what. */
			}
		}
	}
	Current=NULL;

	/* Eliminate nodes with identical attributes, saving the low times. */
	Current=Head;
	while (Current!=NULL)
	{
		Compare=Current;
		Low=Current->Time;
		while (Compare->Next!=NULL)
		{
			if ((CompNodeAttribs(Current,Compare->Next))==0)
			{
				Trash=Compare->Next;
				if (Low>Compare->Next->Time)
				{
					Low=Compare->Next->Time;
				}
				Compare->Next=Trash->Next;
				Trash->Next=NULL;
				FreeNode(Trash);
			}
			else
			{
				Compare=Compare->Next;
			}
		}
		Current->Time=Low;
		Current=Current->Next;
	}

	/* Sort the remaining nodes. */
	SortNodeList(&Head);

	/* Make the home pathname again! Fuck! */
	
	if ((HomePath=getenv("HOME"))==NULL)
	{
		perror("MigrateBestTimes::Getenv $HOME");
		return 1;
	}
	strcpy(PathBuffer,HomePath);
	strcat(PathBuffer,"/");
	strcat(PathBuffer,DFL_BESTS_FILE);

	if ((NewFile=fopen(PathBuffer,"w"))==NULL)
	{
		perror("MigrateBestTimes::OpenFile");
		return 1;
	}

	/* Write the remaining times into the file. */
	Current=Head;
	while (Current!=NULL)
	{
		/* Get the username, since it wasn't logged before. */
		if ((ValueBuffer=getenv("USER"))==NULL)
		{
			fprintf(stderr,"MigrateBestTimes::SetUser:unable to get $USER.\n");
			return 1;
		}
		
		/* Put all the data into a nice string. */
		BestTimeLen=(strlen(ValueBuffer)+(strlen(Current->Attributes))+(log10(Current->Time)+1)+11);
		if ((AttribBuffer=malloc(BestTimeLen+1))==0)
		{
			perror("MigrateBestTimes::AllocBuffer");
			return 1;
		}
		/* sprintf(AttribBuffer,"%s\t%st%d\tUnknown\n",ValueBuffer,Current->Attributes,Current->Time); */
		snprintf(AttribBuffer, BestTimeLen, "%s\t%st%d\tUnknown\n",ValueBuffer,Current->Attributes,Current->Time);

		/* Convert it. Technically insecure, but it'll keep people from fucking with their times. */
		ValueBuffer=AttribBuffer;
		while (*ValueBuffer!='\n')
		{
			*ValueBuffer=*ValueBuffer^MAGIC_NUMBER;
			ValueBuffer++;
		}

		/* Do the writing. */
		ValueBuffer=AttribBuffer;
		Low=strlen(ValueBuffer);
		Written=0;
		while (Written<Low)
		{
			Written+=fwrite(ValueBuffer,1,Low-Written,NewFile);
			ValueBuffer+=Written;
		}
		Current=Current->Next;
	}

	if (fclose(NewFile)!=0)
	{
		perror("MigrateBestTimes::CloseFile");
		return 1;
	}

	Current=Head;
	while (Current!=NULL)
	{
		Trash=Current;
		Current=Current->Next;
		free(Trash->Attributes);
		free(Trash);
	}
	return 0;
}

/* Sorts a linked list of BestTimeNode. Uses a quicksort with a randomly
   selected pivot on large lists, and an insert sort on smaller lists. */
void SortNodeList(BestTimeNode** Head)
{
	int NumNodes=0;
	BestTimeNode *Current;

	Current=*Head;
	while (Current!=NULL)
	{
		NumNodes++;
		Current=Current->Next;
	}
	srand((int)*Head);
	QSortNodeList(Head,NULL,NumNodes);
#ifdef DEBUG_LOG
		DumpNodeList(*Head,DebugLog);
#endif /* DEBUG_LOG */
	return;
}

/* QSortNodeList() does a quick sort on a list of nodes until any given
sub-list is nine items or less, whereupon it does an insert sort. */
/* FOO - the quick sort is completely useless. Totally fucked. */
void QSortNodeList(BestTimeNode** Head, BestTimeNode* Tail, int NumNodes)
{
	int LeftCount=0,RightCount=0,NPivot=0;
	BestTimeNode **LeftWalk,**RightWalk,*Pivot,*Old,*Right;

	while (NumNodes>1)
	{
		/* For small lists, use an insertion sort. Otherwise, a quick sort. */
		if (NumNodes<=9)
		{
			/* Fuck it. */
			/*ISortNodeList(Head,Tail);*/
			ISortNodeList(Head,NULL);
			break;
		}
		NPivot=(abs(rand())%(NumNodes-4))+1;

		Old=*Head;
		while (NPivot-->0)
		{
			Old=Old->Next;
		}
		Pivot=Old->Next;
		Old->Next=Pivot->Next;

		LeftWalk=Head;
		RightWalk=&Right;
		LeftCount=RightCount=0;

		for (Old=*Head;Old!=NULL;Old=Old->Next)
		{
			if (CompNodeAttribs(Old,Pivot)>0)
			{
				LeftCount++;
				*LeftWalk=Old;
				LeftWalk=&(Old->Next);
			}
			else
			{
				RightCount++;
				*RightWalk=Old;
				RightWalk=&(Old->Next);
			}
		}
		*RightWalk=NULL;
		*LeftWalk=Pivot;
		Pivot->Next=Right;
		
		if (LeftCount>RightCount)
		{
			QSortNodeList(&(Pivot->Next),Tail,RightCount);
			Tail=Pivot;
			NumNodes=LeftCount;
		}
		else
		{
			QSortNodeList(Head,Pivot,LeftCount);
			Head=&(Pivot->Next);
			NumNodes=RightCount;
		}
	}
	return;
}

void ISortNodeList(BestTimeNode** Head, BestTimeNode* Tail)
{
	BestTimeNode *Current=NULL, *NewList=NULL, *Save=NULL, **pNewLink;

	NewList=Tail;
	Current=*Head;

	for (;Current!=Tail;Current=Save)
	{
		for (pNewLink=&NewList;(*pNewLink!=Tail)&&(CompNodeAttribs(Current,*pNewLink)<=0);pNewLink=&((*pNewLink)->Next));
		Save=Current->Next;
		Current->Next=*pNewLink;
		*pNewLink=Current;
	}
	*Head=NewList;
	return;
}

/* LoadNodeList() takes a filename as an argument, and returns a pointer
   to the head of a linked list of BestTimeNodes. If the file does not
   exist, cannot be opened, or does not contain valid data, NULL is
   returned. If the file cannot be completely read, or is corrupted,
   all valid nodes previously loaded will be returned.
*/
BestTimeNode* LoadNodeList(char* Filename)
{
	FILE* Nodefile;
	BestTimeNode *Head, *Tail, *Temp;
	char Buffer[MAX_LINE];
	unsigned char *Decoder;
	char *ValBuffer;
	if (Filename==NULL)
	{
		if (((ValBuffer=malloc(MAX_LINE))==NULL) || ((Decoder=getenv("HOME"))==NULL))
		{
			return NULL;
		}
		strcpy(ValBuffer,Decoder);
		strcat(ValBuffer,"/");
		strcat(ValBuffer,DFL_BESTS_FILE);

		if ((Nodefile=fopen(ValBuffer,"r"))==NULL)
		{
			return NULL;
		}
	}
	else if ((Nodefile=fopen(Filename,"r"))==NULL)
	{
		return NULL;
	}

	Tail=Head=NULL;

	while (fgets(Buffer,MAX_LINE,Nodefile)!=NULL)
	{
		if ((Temp=malloc(sizeof(BestTimeNode)))==NULL)
		{
			return Head;
		}
	
		/* Decode the string */
		Decoder=Buffer;
		while (*Decoder!='\n')
		{
			*Decoder=*Decoder^MAGIC_NUMBER;
			Decoder++;
		}

		/* Break it into component parts. */
		/* First the username/email address */
		if ((ValBuffer=strtok(Buffer,"\t"))==NULL)
		{
			return Head;
		}
		else
		{
			strncpy(Temp->Username,ValBuffer,MAX_NAME);
		}
		/* Now the Attributes */
		if ((ValBuffer=strtok(NULL,"Tt"))==NULL)
		{
			return Head;
		}
		else
		{
			if ((Temp->Attributes=malloc(strlen(ValBuffer)+1))==NULL)
			{
				return Head;
			}
			strcpy(Temp->Attributes,ValBuffer);
		}
		/* Now the Time */
		if ((ValBuffer=strtok(NULL,"\t"))==NULL)
		{
			return Head;
		}
		else
		{
			Temp->Time=atoi(ValBuffer);
		}
		/* Now the Date */
		if ((ValBuffer=strtok(NULL,"\t\n"))==NULL)
		{
			return Head;
		}
		else
		{
			strncpy(Temp->Date,ValBuffer,MAX_DATE);
		}

		/* Now to put it at the end of the list. */
		Temp->Next=NULL;

		if (Head==NULL)
		{
			Head=Temp;
		}
		else
		{
			Tail->Next=Temp;
		}
		Tail=Temp;
	}
	return Head;
}

void DumpNodeFile(char* Filename)
{
	BestTimeNode *Current, *Head;

	Head=Current=LoadNodeList(Filename);
	DumpNodeList(Head,stdout);
	FreeNode(Head);
	return;
}

void DumpNodeList(BestTimeNode* Head, FILE* Dump)
{
	BestTimeNode *Current;

	Current=Head;
	while (Current!=NULL)
	{
		fprintf(Dump,"%s\t%s\t%d\t%s\n",Current->Username,Current->Attributes,Current->Time,Current->Date);
		fflush(Dump);
		Current=Current->Next;
	}
	return;
}

void FreeNode(BestTimeNode* Node)
{
	if (Node == NULL)
	{
		return;
	}

	if (Node->Next != NULL)
	{
		FreeNode(Node->Next);
	}
	free(Node->Attributes);
	return;
}

int AddNodeToFile(char* Filename, BestTimeNode* NewNode)
{
	BestTimeNode* Head, *Current, *Trash;
	int AddedNode=0, FoundMatch=0;

	if (NewNode==NULL)
	{
#ifdef DEBUG_LOG
		fprintf(DebugLog,"Null node not added to the list.\n");
		fflush(DebugLog);
#endif /* DEBUG_LOG */
		return 1;
	}

	Head=Current=LoadNodeList(Filename);

	if (Head==NULL)
	{
#ifdef DEBUG_LOG
		fprintf(DebugLog,"No nodes loaded - writing NewNode to file.\n");
		fflush(DebugLog);
#endif /* DEBUG_LOG */
		if (WriteNodeList(Filename,NewNode) != 0)
		{
			perror("AddNodeToFile::Write file");
			return 1;
		}
		return 0;
	}

	/* FOO - redo all this crap. */
	/* Don't bother sorting, just walk the list with a big brother */
	/* serach until a node "less" than the new one is found. Then */
	/* just do a simple insert to the list and then write it to a file. */


	/* Walk the list, looking for a node with the same attributes. */
	/* Head is a special case... */

	if ((CompNodeAttribs(Head,NewNode))==0)
	{
		FoundMatch=1;
		/* Compare the times on the two nodes. */
		if ((CompNodeTimes(Head,NewNode))>0)
		{
#ifdef DEBUG_LOG
		fprintf(DebugLog,"Adding new node to the HEAD of the list.\n");
		fflush(DebugLog);
#endif /* DEBUG_LOG */
			NewNode->Next=Head->Next;
			Trash=Head;
			Head=NewNode;
			FreeNode(Trash);
			AddedNode=1;
		}
	}
	else
	{
		Current=Head;
		while (Current->Next!=NULL)
		{
			if ((CompNodeAttribs(Current->Next,NewNode))==0)
			{
				FoundMatch=1;
				if ((CompNodeTimes(Current->Next,NewNode))>0)
				{
#ifdef DEBUG_LOG
		fprintf(DebugLog,"Adding new node to the MIDDLE of the list.\n");
		fflush(DebugLog);
#endif /* DEBUG_LOG */
					NewNode->Next=Current->Next->Next;
					Trash=Current->Next;
					Current->Next=NewNode;
					FreeNode(Trash);
					AddedNode=1;
				}
			}
			Current=Current->Next;
		}
	}

	if (FoundMatch==0)
	{
#ifdef DEBUG_LOG
		fprintf(DebugLog,"Adding new node to the END of the list.\n");
		fflush(DebugLog);
#endif /* DEBUG_LOG */
		Current=Head;
		while (Current->Next!=NULL)
		{
			Current=Current->Next;
		}
		Current->Next=NewNode;
		NewNode->Next=NULL;
		AddedNode=1;
	}

	if (AddedNode==0)
	{
		/* No point in writing the same file we just read. */
		return 0;
	}

	/* Now sort the list, to see if the new node ranks. */
	SortNodeList(&Head);


	if (WriteNodeList(Filename,Head) != 0)
	{
		perror("AddNodeToFile::WriteFile");
	}

	return AddedNode;
}

BestTimeNode* InitNode(GameStats* Game)
{
	BestTimeNode *NewNode;
	char *ValueBuffer;
	unsigned char *NewLine;
	time_t Now;
	int Mines;

	if ((NewNode=malloc(sizeof(BestTimeNode)))==NULL)
	{
		fprintf(stderr,"AddNewNode::AllocNewNode:Unable to allocate new node");
		return NULL;
	}
	NewNode->Next=NULL;

	/* Get the username. */
	if ((ValueBuffer=getenv("USER"))==NULL)
	{
		fprintf(stderr,"AddNewNode::GetUser:unable to get $USER.\n");
		return NULL;
	}
	else
	{
		strncpy(NewNode->Username,ValueBuffer,MAX_NAME);
	}

	if (Game->Percent!=0)
	{
		Mines=((Game->Percent)*(Game->Width)*(Game->Height))/100;
	}
	else
	{
		Mines=Game->NumMines;
	}

	/* Set the attributes. */
	if ((NewNode->Attributes=malloc((2*MAX_H)+(2*MAX_W)+4))==NULL)
	{
		perror("AddNewNode::SetAttributes");
		return NULL;
	}
/*	else if (sprintf(NewNode->Attributes,"w%dh%dm%d",Game->Width,Game->Height,Mines) < 0)*/
	else if (snprintf(NewNode->Attributes, ((2*MAX_H)+(2*MAX_W)+4), "w%dh%dm%d",Game->Width,Game->Height,Mines) < 0)
	{
		perror("AddNewNode::SetAtributes");
		return NULL;
	}

	/* Set the time. */
/*	NewNode->Time=Game->Time;*/
	NewNode->Time=rand()%4000;

	/* Set the date. */
	time(&Now);
	if ((ValueBuffer=ctime(&Now))==NULL)
	{
		strcpy(NewNode->Date,"Unknown");
	}
	else
	{
		/* Strip the newline that ctime puts on the string. */
		if ((NewLine=rindex(ValueBuffer,'\n'))!=NULL)
		{
			*NewLine=0;
		}
		strcpy(NewNode->Date,ValueBuffer);
	}

	return NewNode;
}

int WriteNodeList(char* Filename, BestTimeNode* Head)
{
	int LeftToWrite=0, Written=0, Fwritten=0;
	char ValueBuffer[MAX_LINE];
	char WriteBuffer[((2*L_MAX_W)+(2*L_MAX_H)+MAX_DATE+MAX_NAME+24)];
	BestTimeNode *Tail,*Current;
	FILE* NodeFile;
	unsigned char* Decoder;

	if (Filename==NULL)
	{
		if ((Decoder=getenv("HOME"))==NULL)
		{
			perror("WriteNodeFile::Get Home Directory");
			return 1;
		}

		strcpy(ValueBuffer,Decoder);
		strcat(ValueBuffer,"/");
		strcat(ValueBuffer,DFL_BESTS_FILE);

		if ((NodeFile=fopen(ValueBuffer,"w"))==NULL)
		{
			perror("WriteNodeFile::Open Default file");
			return 1;
		}
	}
	else if ((NodeFile=fopen(Filename,"w"))==NULL)
	{
		perror("WriteNodeFile::Open Named file");
		return 1;
	}

	Current=Tail=Head;

	while (Current!=NULL)
	{
/*		if ((sprintf(WriteBuffer,"%s\t%st%d\t%s\n",Current->Username,Current->Attributes,Current->Time,Current->Date)) < 0)*/
		if ((snprintf(WriteBuffer,((2*L_MAX_W)+(2*L_MAX_H)+MAX_DATE+MAX_NAME+24),"%s\t%st%d\t%s\n",Current->Username,Current->Attributes,Current->Time,Current->Date)) < 0)
		{
			perror("WriteNodeList::Write to buffer");
			/* Handle the fail gracefully here. */


			return 1;
		}

		/* Convert it. Technically insecure, but it'll keep people from fucking with their times. */
		Decoder=WriteBuffer;
		while (*Decoder!='\n')
		{
			*Decoder=*Decoder^MAGIC_NUMBER;
			Decoder++;
		}

		/* Do the writing. */
		Decoder=WriteBuffer;
		LeftToWrite=strlen(Decoder);
		Written=0;
		while (Written<LeftToWrite)
		{
			Fwritten+=fwrite(Decoder,1,(LeftToWrite-Written),NodeFile);
			if ((Fwritten==0)&&ferror(NodeFile))
			{
				perror("WriteNodeFile::Write encrypted string");
				return 1;
			}
			else
			{
				Written+=Fwritten;
			}
			
			Decoder+=Written;
		}
		/* Advance the current node. */
		Current=Current->Next;
	}
	
	if (fclose(NodeFile)!=0)
	{
		perror("WriteNodeFile::CloseFile");
		return 1;
	}

	return 0;
}

BestTimeNode* GenerateFakeNode()
{
	BestTimeNode *NewNode;
	char *ValueBuffer;
	unsigned char *NewLine;
	time_t Now;

	if ((NewNode=malloc(sizeof(BestTimeNode)))==NULL)
	{
		fprintf(stderr,"AddNewNode::AllocNewNode:Unable to allocate new node");
#ifdef DEBUG_LOG
		fprintf(DebugLog,"The new node was NULL!\n");
		fflush(DebugLog);
#endif /* DEBUG_LOG */
		return NULL;
	}
	NewNode->Next=NULL;

	/* Get the username. */
	if ((ValueBuffer=getenv("USER"))==NULL)
	{
		fprintf(stderr,"AddNewNode::GetUser:unable to get $USER.\n");
#ifdef DEBUG_LOG
		fprintf(DebugLog,"The new nodew was NULL!\n");
		fflush(DebugLog);
#endif /* DEBUG_LOG */
		return NULL;
	}
	else
	{
		strncpy(NewNode->Username,ValueBuffer,MAX_NAME);
	}


	/* Set the attributes. */
	if ((NewNode->Attributes=malloc((2*MAX_H)+(2*MAX_W)+4))==NULL)
	{
		perror("AddNewNode::SetAttributes");
#ifdef DEBUG_LOG
		fprintf(DebugLog,"The new nodew was NULL!\n");
		fflush(DebugLog);
#endif /* DEBUG_LOG */
		return NULL;
	}
/*	else if (sprintf(NewNode->Attributes,"w%dh%dm%d",rand()%50,rand()%50,rand()%250) < 0)*/
	else if (snprintf(NewNode->Attributes,((2*MAX_H)+(2*MAX_W)+4),"w%dh%dm%d",rand()%50,rand()%50,rand()%250) < 0)
	{
		perror("AddNewNode::SetAtributes");
#ifdef DEBUG_LOG
		fprintf(DebugLog,"The new nodew was NULL!\n");
		fflush(DebugLog);
#endif /* DEBUG_LOG */

		return NULL;
	}

	/* Debugging shit for generating fake scores. */
	NewNode->Time=rand()%100;

	/* Set the date. */
	time(&Now);
	if ((ValueBuffer=ctime(&Now))==NULL)
	{
		strcpy(NewNode->Date,"Unknown");
	}
	else
	{
		/* Strip the newline that ctime puts on the string. */
		if ((NewLine=rindex(ValueBuffer,'\n'))!=NULL)
		{
			*NewLine=0;
		}
		strcpy(NewNode->Date,ValueBuffer);
	}

	return NewNode;
}
