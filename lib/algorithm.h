#ifndef __ALGORITHM_H
#define __ALGORITHM_H


#define SIZE 8

void putQueen(int matrix[SIZE][SIZE], int x, int y);

int is_valid(int matrix[SIZE][SIZE], int x, int y);

int valid_left_diag(int matrix[SIZE][SIZE], int x, int y);

int valid_right_diag(int matrix[SIZE][SIZE], int x, int y);

void removeQueen(int matrix[SIZE][SIZE], int x, int y);


void printBoard(int matrix[SIZE][SIZE]);

#endif
