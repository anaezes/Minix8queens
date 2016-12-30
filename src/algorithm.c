#include <minix/syslib.h>
#include <minix/drivers.h>
#include <machine/int86.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "algorithm.h"

int valid_left_diag(int matrix[SIZE][SIZE], int x, int y);

int valid_right_diag(int matrix[SIZE][SIZE], int x, int y);

int is_valid(int matrix[SIZE][SIZE], int x, int y)
{
	int n_queens = 0;
	int i, j;

	//LINE
	for(i = 0; i < SIZE; i++)
	{
		if(matrix[y][i] == 1)
			n_queens++;

		if(n_queens > 1)
			return 0;
	}


	//COLLUMN
	n_queens = 0;
	for(j = 0; j < SIZE; j++)
	{
		if(matrix[j][x] == 1)
			n_queens++;

		if(n_queens > 1)
			return 0;
	}

	//DIAGONAL
	if(!valid_left_diag(matrix, x, y))
		return 0;
	if(!valid_right_diag(matrix, x, y))
		return 0;

	return 1;
}

int valid_left_diag(int matrix[SIZE][SIZE], int x, int y)
{
	int j, i;
	int n_queens = 0;
	if(x > y)
	{
		i = 0;
		j = x-y;
	}
	else
	{
		i = y-x;
		j = 0;
	}

	while(i < SIZE && j < SIZE)
	{
		if(matrix[i][j] == 1)
			n_queens++;

		if(n_queens > 1)
			return 0;

		i++;
		j++;
	}

	return 1;
}

int valid_right_diag(int matrix[SIZE][SIZE], int x, int y)
{
	int i, j;
	int n_queens = 0;
	if(x + y > 6)
	{
		i = abs(x+y-(SIZE-1));
		j = SIZE-1;
	}
	else
	{
		i = 0;
		j = x+y;
	}

	while(i < SIZE && j >= 0)
	{
		if(matrix[i][j] == 1)
			n_queens++;

		if(n_queens > 1)
			return 0;

		i++;
		j--;
	}

	return 1;
}
