#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define GENERATIONS 2000
#define SIZE 2048

int **grid, **newGrid;

void freeGrids(int size)
{
    int i;
    for (i = 0; i < size + 2; i++)
    {
        free(grid[i]);
        free(newGrid[i]);
    }

    free(grid);
    free(newGrid);
}

int neighborsAlive(int i, int j, int size)
{
    int edge_x = SIZE - 1;
    int edge_y = size - 1;
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
                pos_i = edge_x;
            else if (a > edge_x)
                pos_i = 0;

            if (b < 0)
                pos_j = edge_y;
            else if (b > edge_y)
                pos_j = 0;

            count += grid[pos_i][pos_j];
        }
    }

    return count;
}

int execute(int proc_id, int size)
{
    int i, j, count = 0;

    for (i = 1; i <= size; i++)
    {
        for (j = 0; j < SIZE; j++)
        {
            int result = neighborsAlive(i, j, size);

            if (grid[i][j] == 0 && result == 3)
                newGrid[i][j] = 1;

            else if (grid[i][j] == 1 && (result < 2 || result > 3))
                newGrid[i][j] = 0;

            count += newGrid[i][j];
        }
    }

    return count;
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

void createGrids(int size)
{
    int i, j;

    grid = (int**) malloc((size + 2) * sizeof(int *));
    newGrid = (int**) malloc((size + 2) * sizeof(int *));

    for (i = 0; i < size + 2; i++)
    {
        grid[i] = (int*) malloc(SIZE * sizeof(int));
        newGrid[i] = (int*) malloc(SIZE * sizeof(int));
        for (j = 0; j < SIZE; j++)
        {
            grid[i][j] = 0;
            newGrid[i][j] = 0;
        }
    }
}

void copyNewGridToGrid(int proc_id, int size)
{
    int i, j;

    for (i = 0; i < size + 2; i++)
    {
        for (j = 0; j < SIZE; j++)
        {
            grid[i][j] = newGrid[i][j];
        }
    }
}

void update_borders(int proc_id, int prev, int next, int size)
{
    MPI_Request r1, r2, r3, r4;

    MPI_Isend(grid[size], SIZE, MPI_INT, next, 11, MPI_COMM_WORLD, &r1);

    MPI_Isend(grid[1], SIZE, MPI_INT, prev, 10, MPI_COMM_WORLD, &r2);

    MPI_Irecv(grid[0], SIZE, MPI_INT, prev, 11, MPI_COMM_WORLD, &r3);

    MPI_Irecv(grid[size + 1], SIZE, MPI_INT, next, 10, MPI_COMM_WORLD, &r4);
}

int main(int argc, char *argv[])
{
    int proc_id, num_procs, ierr, ferr, subsize;
    double start, finish;

    ierr = MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
    MPI_Comm_rank(MPI_COMM_WORLD, &proc_id);

    subsize = SIZE / num_procs;
    int rest = SIZE % num_procs;

    if (proc_id == num_procs - 1)
    {
        subsize += rest;
    }

    createGrids(subsize);

    MPI_Barrier(MPI_COMM_WORLD);

    if (proc_id == 0)
    {
        start = MPI_Wtime();

        initialize(grid);
        initialize(newGrid);
    }

    int prev_proc = (proc_id + num_procs - 1) % num_procs;
    int next_proc = (proc_id + 1) % num_procs;

    int i, count = 0, total_sum = 0;

    for (i = 0; i < GENERATIONS; i++)
    {
        update_borders(proc_id, prev_proc, next_proc, subsize);

        MPI_Barrier(MPI_COMM_WORLD);

        count = execute(proc_id, subsize);

        copyNewGridToGrid(proc_id, subsize);

        MPI_Barrier(MPI_COMM_WORLD);

        MPI_Reduce(&count, &total_sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

        if (proc_id == 0)
            printf("Células vivas na geração %d: %d\n", i + 1, total_sum);

        MPI_Barrier(MPI_COMM_WORLD);
        fflush(stdout);
    }

    if (proc_id == 0)
    {
        freeGrids(subsize);

        finish = MPI_Wtime();
        printf("\nTempo total de execução: %.4f seg\n", finish - start);
    }

    ferr = MPI_Finalize();
    return 1;
}