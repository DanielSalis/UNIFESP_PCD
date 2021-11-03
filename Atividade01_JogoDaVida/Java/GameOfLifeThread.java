package Java;

import java.util.ArrayList;

public class GameOfLifeThread {

    public static void main(String[] args) {
        long startProgramTime = System.nanoTime();
        int N = 50;
        int generationCounter = 0;

        int[][] grid = initialGrid(N);
        runGame(grid, N, generationCounter);
        long endProgramTime = System.nanoTime();

        long totalTime = endProgramTime - startProgramTime;
        System.out.println("Tempo:" + totalTime);
    }

    static int getNeighbors(int[][] grid, int i, int j, int N) {
        int edge = N - 1;
        int count = 0;

        int a, b;
        for (a = i - 1; a <= i + 1; a++) {
            for (b = j - 1; b <= j + 1; b++) {
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

    static void printCurrentGeneration(int[][] newGeneration, int N, int generationCounter) {
        int aliveCellsCounter = 0;
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                if (newGeneration[i][j] == 0) {
                    continue;
                } else {
                    aliveCellsCounter++;
                }
            }
        }
        System.out.println("Geração" + generationCounter + ":" + aliveCellsCounter);
    }

    static int[][] newGrid(int grid[][], int N, int generationCounter) {
        int[][] newGeneration = new int[N][N];
        ArrayList<Thread> threads = new ArrayList<>();

        long startLoopTime = System.nanoTime();
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                Integer thread_local_row = i;
                Integer thread_local_column = j;
                Thread t = new Thread(() -> {
                    int aliveNeighbours = getNeighbors(grid, thread_local_row, thread_local_column, N);

                    if ((grid[thread_local_row][thread_local_column] == 1) && (aliveNeighbours < 2))
                        newGeneration[thread_local_row][thread_local_column] = 0;

                    else if ((grid[thread_local_row][thread_local_column] == 1) && (aliveNeighbours > 3))
                        newGeneration[thread_local_row][thread_local_column] = 0;

                    else if ((grid[thread_local_row][thread_local_column] == 0) && (aliveNeighbours == 3))
                        newGeneration[thread_local_row][thread_local_column] = 1;

                    else
                        newGeneration[thread_local_row][thread_local_column] = grid[thread_local_row][thread_local_column];
                    printCurrentGeneration(grid, N, generationCounter);
                });
                t.start();
                threads.add(t);
                long endLoopTime = System.nanoTime();

                long totalLoopTime = endLoopTime - startLoopTime;
                System.out.println("LoopTime:" + totalLoopTime);
            }

            for (Thread t : threads)
                try {
                    t.join();
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
        }

        return newGeneration;
    }

    static void runGame(int[][] grid, int N, int generationCounter) {
        for (generationCounter = 1; generationCounter <= 5; generationCounter++) {
            grid = newGrid(grid, N, generationCounter);
        }
    }

    static int[][] initialGrid(int N) {
        int aliveCellsCounter = 0;
        int[][] grid = new int[N][N];

        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                grid[i][j] = 0;
            }
        }

        // GLIDER
        int lin = 1, col = 1;
        grid[lin][col + 1] = 1;
        grid[lin + 1][col + 2] = 1;
        grid[lin + 2][col] = 1;
        grid[lin + 2][col + 1] = 1;
        grid[lin + 2][col + 2] = 1;

        lin = 10;
        col = 30;
        grid[lin][col + 1] = 1;
        grid[lin][col + 2] = 1;
        grid[lin + 1][col] = 1;
        grid[lin + 1][col + 1] = 1;
        grid[lin + 2][col + 1] = 1;
        aliveCellsCounter = 10;

        System.out.println("Condição Inicial:" + aliveCellsCounter);

        return grid;
    }
}