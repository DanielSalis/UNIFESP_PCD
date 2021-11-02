package Java;

public class GameOfLife {

    public static void main(String[] args) {
        int N = 50;
        int generationCounter = 0;

        int[][] grid = initialGrid(N);
        runGame(grid, N, generationCounter);
    }

    static int getNeighbors(int[][] grid, int l, int m) {
        int neighbours = 0;
        for (int i = -1; i <= 1; i++)
            for (int j = -1; j <= 1; j++)
                neighbours += grid[l + i][m + j];

        return neighbours;
    }

    static void printCurrentGeneration(int[][] newGeneration, int N, int generationCounter) {
        int aliveCellsCounter = 0;
        // System.out.println("Generation" + generationCounter);
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                if (newGeneration[i][j] == 0)
                    // System.out.print(".");
                    continue;
                else {
                    aliveCellsCounter++;
                    // System.out.print("*");
                }
            }
            // System.out.println();
        }
        System.out.println("Geração" + generationCounter + ":" + aliveCellsCounter);
    }

    static int[][] newGrid(int grid[][], int N) {
        int[][] newGeneration = new int[N][N];

        // Loop through every cell
        for (int i = 1; i < N - 1; i++) {
            for (int j = 1; j < N - 1; j++) {
                int aliveNeighbours = getNeighbors(grid, i, j);
                aliveNeighbours -= grid[i][j];

                if ((grid[i][j] == 1) && (aliveNeighbours < 2))
                    newGeneration[i][j] = 0;

                else if ((grid[i][j] == 1) && (aliveNeighbours > 3))
                    newGeneration[i][j] = 0;

                else if ((grid[i][j] == 0) && (aliveNeighbours == 3))
                    newGeneration[i][j] = 1;

                else
                    newGeneration[i][j] = grid[i][j];
            }
        }

        return newGeneration;
    }

    static void runGame(int[][] grid, int N, int generationCounter) {
        for (generationCounter = 1; generationCounter <= 5; generationCounter++) {
            grid = newGrid(grid, N);
            printCurrentGeneration(grid, N, generationCounter);
        }
    }

    static int[][] initialGrid(int N) {
        // @formatter:off
        // int[][] grid = {
        //     { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, 
        //     { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        //     { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, 
        //     { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, 
        //     { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        //     { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, 
        //     { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, 
        //     { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        //     { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, 
        //     { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
        // };
        // @formatter:on

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

        System.out.println("Condição Inicial");
        int aliveCellsCounter = 0;
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                if (grid[i][j] == 0)
                    System.out.print(".");
                else{
                    aliveCellsCounter++;
                    System.out.print("*");
                }
            }
            System.out.println();
        }
        System.out.println("Condição Inicial:" + aliveCellsCounter);

        return grid;
    }
}