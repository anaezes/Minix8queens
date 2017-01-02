#ifndef __ALGORITHM_H
#define __ALGORITHM_H

/** @defgroup algorithm
 * @{
 *
 * Algorithm to validate game plays
 *
 */


/**
 * Board size
 **/
#define SIZE 8


/**
 * @brief Verify if a given queen position is valid.
 * @param game matrix
 * @param queen x coordinate
 * @param queen y coordinate
 * @return 1 if is valid
 **/
int is_valid(int matrix[SIZE][SIZE], int x, int y);


/**
 * @brief Verify the left diagonal of a given queen position.
 * @param game matrix
 * @param queen x coordinate
 * @param queen y coordinate
 * @return 1 if is valid
 **/
int valid_left_diag(int matrix[SIZE][SIZE], int x, int y);


/**
 * @brief Verify the right diagonal of a given queen position.
 * @param game matrix
 * @param queen x coordinate
 * @param queen y coordinate
 * @return 1 if is valid
 **/
int valid_right_diag(int matrix[SIZE][SIZE], int x, int y);


/** @} end of algorithm */
#endif /* __ALGORITHM_H */
