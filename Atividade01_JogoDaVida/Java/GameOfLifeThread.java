package Java;

import java.util.ArrayList;

public class GameOfLifeThread {
    final static int aliveCellsCounter = 0;
    public static long currentTime;
    public static int numberOfThreads = 8;
    public static int N = 2048;
    

    public static void main(String[] args) {
        long startProgramTime = System.nanoTime();
        int generationCounter = 0;
        int maxGenerations = 2000;

        int[][] grid = initialGrid(N);
        runGame(grid, N, generationCounter, maxGenerations);
        long endProgramTime = System.nanoTime();

        long totalTime = endProgramTime - startProgramTime;
        System.out.println("Tempo total de execução:" + totalTime + "ns");
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
        System.out.println("Geração" + generationCounter + ":" + aliveCellsCounter +  "| Tempo: "+currentTime+ "ns");
    }

    public static void evaluateCell(int[][] grid, int[][] newGeneration, int i, int j, int aliveNeighbours){
        if ((grid[i][j] == 1) && (aliveNeighbours < 2))
        newGeneration[i][j] = 0;

    else if ((grid[i][j] == 1) && (aliveNeighbours > 3))
        newGeneration[i][j] = 0;

    else if ((grid[i][j] == 0) && (aliveNeighbours == 3))
        newGeneration[i][j] = 1;

    else
        newGeneration[i][j] = grid[i][j];
    }

    static int[][] newGrid(int grid[][], int N, int generation) {
        int[][] newGeneration = new int[N][N];
        ArrayList<Thread> threads = new ArrayList<>();

        long startLoopTime = System.nanoTime();

        for (int t = 0; t < numberOfThreads; t++) {
            Integer thread_local_row_start = t * N / numberOfThreads;
            Integer thread_local_row_end = (t + 1) * N / numberOfThreads;
            Thread tr = new Thread(new Runnable() {
                public void run(){
                    for (int i = thread_local_row_start; i < thread_local_row_end; i++) {
                        for (int j = 0; j < N; j++) {
                            int aliveNeighbours = getNeighbors(grid, i, j, N);
                            evaluateCell(grid, newGeneration, i, j, aliveNeighbours);
                        }
                    }
                }
            });
            tr.start();
            threads.add(tr);
        }
        for (Thread t : threads) {
            try {
                t.join();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }

        long endLoopTime = System.nanoTime();

        currentTime = endLoopTime - startLoopTime;
        return newGeneration;
    }

    static void runGame(int[][] grid, int N, int generationCounter, int maxGenerations) {
        for (generationCounter = 1; generationCounter <= maxGenerations; generationCounter++) {
            grid = newGrid(grid, N, generationCounter);
            printCurrentGeneration(grid, N, generationCounter);
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