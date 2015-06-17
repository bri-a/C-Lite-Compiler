/* life */

/* it is a 10x10 board */
int gBoard[200]={1,1};
int gCurrentBoard = 0;
int MAXBOARDSIZE = 10;
int BOARDSIZE = MAXBOARDSIZE;

int getSquare( i, x, y)
int i, x, y;
{
	return gBoard[i*MAXBOARDSIZE*MAXBOARDSIZE+y*MAXBOARDSIZE+x];
}
int setSquare( i, x, y,v)
int i, x, y, v;
{
	gBoard[i*MAXBOARDSIZE*MAXBOARDSIZE+y*MAXBOARDSIZE+x]=v;
}

int random = 1;
int MAXINT = 32000;
int a = 2113, c = 53, m=1000000;
int DEAD=0;
int LIVE=1;

int srand(seed)
int seed;
{
	random = (a * seed + c ) % m;
	random = random  % MAXINT;
	return random;
}

int rand(seed)
int seed;
{
	random = (a * random + c ) % m;
	random = random  % MAXINT;

	return random;
}


int randomFill(board)
int board;
{
	int x,y;
	int cell;
	for(x=0;x<MAXBOARDSIZE;x++)
	{
		for(y=0;y<MAXBOARDSIZE;y++)
		{
			setSquare(0, x, y, -1);
			setSquare(1, x, y, -1);

		}
	}
	for(x=0;x<BOARDSIZE;x++)
	{
		for(y=0;y<BOARDSIZE;y++)
		{
			if( rand(0) % 10 <2 )
			{
				cell = LIVE;
			}
			else
			{
				cell = DEAD;
			}
			setSquare(board, x, y, cell);
		}
	}
}

int nextGen(board, x,  y)
int board; int x; int y;
{
	int startX, endX;
	int startY, endY;
	int liveNeighbors = 0;
	int i,j;

	startX=0;
	if (x -1  >=0 )
	{
		startX = x-1;
	}
	endX = x;
	if( x+1 < BOARDSIZE )
	{
		endX = x +1;
	}
	startY = 0;

	if( (y-1) >=0 )
		startY = y-1;

	endY = y;


	if ( (y+1) <BOARDSIZE )
		endY =  y+1 ;

	for(i=startX;i<=endX;i++)
	{
		for(j=startY;j<=endY;j++)
		{
			if(getSquare(board, i, j) == LIVE )
			{
				liveNeighbors++;
			}
		}
	}

	/* remove the center*/
	liveNeighbors =  liveNeighbors - getSquare(board,x,y);

	setSquare((board+1)%2,x,y, getSquare(board,x,y));

	if( getSquare(board,x,y) == LIVE)
	{
		if(liveNeighbors < 2 || liveNeighbors > 3)
		{
			setSquare((board+1)%2,x,y, DEAD);
		}
	}
	else if(getSquare(board,x,y) == DEAD)
	{
		if(liveNeighbors == 3)
		{
			setSquare((board+1)%2,x,y, LIVE);
		}
	}
}

int runOneGeneration( board)
int board;
{
	int x,y;
	for(x=0;x<BOARDSIZE;x++)
	{
		for(y=0;y<BOARDSIZE;y++)
		{
			nextGen(board,x,y);
		}
	}
}

int runOneGenerationProcess( board,  myColumn,  processes)
int board; int myColumn; int processes;
{
	int x,y;
	for(x=0;x<BOARDSIZE;x++)
	{
		for(y=myColumn; y<BOARDSIZE; y = y+processes)
		{
			nextGen(board,x,y);
		}
	}
}

int printBoard( board)
int board;
{
	int x,y=0;
	output(-1);
	for(x=0;x<BOARDSIZE;x++)
	{
		output(getSquare(board, x, y),
				getSquare(board, x, y+1),
				getSquare(board, x, y+2),
				getSquare(board, x, y+3),
				getSquare(board, x, y+4),
				getSquare(board, x, y+5),
				getSquare(board, x, y+6),
				getSquare(board, x, y+7),
				getSquare(board, x, y+8),
				getSquare(board, x, y+9));
	}
	output(-1);
}


main()
{
	int generations,seed;

	input(seed); /* 12312 */
	srand(seed);
	
	input(BOARDSIZE);
	for(1;BOARDSIZE<1 || BOARDSIZE > MAXBOARDSIZE; 1)
	{
		input(BOARDSIZE);
	}
	
	randomFill(gCurrentBoard);

	input(generations);


	for( printBoard(gCurrentBoard);  generations >0 ;generations --)
	{
		runOneGeneration(gCurrentBoard);
		gCurrentBoard =(gCurrentBoard +1) %2;
		printBoard(gCurrentBoard);
	}
}
