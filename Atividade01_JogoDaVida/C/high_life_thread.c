#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>
#include <sys/time.h>

#define SEC(tv) (tv.tv_sec + tv.tv_usec/1e6)

#define SUBSIZE 50

/*
(0,0)  (0,1)  (0,2)........
(1,0)
(2,0)
...
.................(2047,2047)
*/

int size;
int **grid, **newGrid;

void freeGrids()
{
    int i;
    for (i = 0; i < size; i++)
    {
        free(grid[i]);
        free(newGrid[i]);
    }

    free(grid);
    free(newGrid);
}

void print()
{
    int i = 0, j = 0;
    for (i = 0; i < SUBSIZE; i++)
    {
        for (j = 0; j < SUBSIZE; j++)
        {
            printf("%d ", grid[i][j]);
        }
        printf("\n");
    }
    printf("\n\n");
}

int neighborsAlive(int i, int j)
{
    int edge = size - 1;
    int count = 0;

    int a, b;
    for (a = i - 1; a <= i + 1; a++)
    {
        for (b = j - 1; b <= j + 1; b++)
        {
            if (a == i && b == j)
                continue;

            int pos_i = a, pos_j = b;

            if (a < 0)
                pos_i = edge;
            else if (a > edge)
                pos_i = 0;

            if (b < 0)
                pos_j = edge;
            else if (b > edge)
                pos_j = 0;

            count += grid[pos_i][pos_j];
        }
    }

    return count;
}

int totalAlive()
{
    int i, j, count = 0;

    for (i = 0; i < size; i++)
    {
        for (j = 0; j < size; j++)
        {
            count += grid[i][j];
        }
    }

    return count;
}

int execute()
{
    int i, j, totalAlive = 0;;

    for (i = 0; i < size; i++)
    {
        for (j = 0; j < size; j++)
        {
            int result = neighborsAlive(i, j);

            if (grid[i][j] == 0 && (result == 3 || result == 6))
                newGrid[i][j] = 1;

            else if (grid[i][j] == 1 && (result < 2 || result > 3))
                newGrid[i][j] = 0;

            totalAlive += newGrid[i][j];
        }
    }

    return totalAlive;
}

void createGrids()
{
    int i, j;

    grid = (int **)malloc(sizeof(int *) * size);
    newGrid = (int **)malloc(sizeof(int *) * size);

    for (i = 0; i < size; i++)
    {
        grid[i] = (int *)malloc(sizeof(int) * size);
        newGrid[i] = (int *)malloc(sizeof(int) * size);

        for (j = 0; j < size; j++)
        {
            grid[i][j] = 0;
            newGrid[i][j] = 0;
        }
    }
}

void copyNewGridToGrid()
{
    int i, j;

    for (i = 0; i < size; i++)
    {
        for (j = 0; j < size; j++)
        {
            grid[i][j] = newGrid[i][j];
        }
    }
}

void initialize(int **matriz)
{
    //GLIDER
    int lin = 1, col = 1;
    matriz[lin][col + 1] = 1;
    matriz[lin + 1][col + 2] = 1;
    matriz[lin + 2][col] = 1;
    matriz[lin + 2][col + 1] = 1;
    matriz[lin + 2][col + 2] = 1;

    //R-pentomino
    lin = 10;
    col = 30;
    matriz[lin][col + 1] = 1;
    matriz[lin][col + 2] = 1;
    matriz[lin + 1][col] = 1;
    matriz[lin + 1][col + 1] = 1;
    matriz[lin + 2][col + 1] = 1;
}

int main(int argc, char **argv)
{
    if (argc != 4)
    {
        printf("Uso comando: <tamanho N da matrix nXn> <qtd de gerações> <qtd de threads> \n");
        return 0;
    }

    int generations, n_threads;

    size = atoi(argv[1]);
    generations = atoi(argv[2]);
    n_threads = atoi(argv[3]);

    omp_set_num_threads(n_threads);

    struct timeval p_start, p_end, p_time, loop_start, loop_end, loop_time;
    gettimeofday(&p_start, NULL);

    printf("\n**HighLife**\n");
    printf("Matriz de tamanho %d\n", size);
    printf("Número de gerações: %d\n", generations);
    printf("Número de threads: %d\n", n_threads);

    createGrids();

    initialize(grid);
    initialize(newGrid);

    int n = totalAlive();
    printf("Condição inicial: %d\n\n", n);

    int i;
    for (i = 0; i < generations; i++)
    {
        gettimeofday(&loop_start, NULL);

        n = execute();
        copyNewGridToGrid();

        gettimeofday(&loop_end, NULL);
        timersub(&loop_end, &loop_start, &loop_time);

        printf("Geração %d: %d | Tempo de execução:  %.3f seg \n", i + 1, n, SEC(loop_time));

        if (i < 5) print();
    }

    printf("\nÚltima geração (%d iterações): %d células vivas\n", generations, n);

    freeGrids();

    gettimeofday(&p_end, NULL);
    timersub(&p_end, &p_start, &p_time);
    printf("\nTempo total de execução: %.3f min\n", SEC(p_time)/60);

    return 1;
}