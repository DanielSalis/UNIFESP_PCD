package Java;

public class HighLife {

    int size = 2048;
    int SUBSIZE = 50;
    int[][] grid, newGrid;

    public void freeGrids() {
        grid = new int[size][size];
        newGrid = new int[size][size];
    }

    public void print() {
        int i = 0, j = 0;
        for (i = 0; i < SUBSIZE; i++) {
            for (j = 0; j < SUBSIZE; j++) {
                System.out.println(grid[i][j]);
            }
            System.out.println();
        }
        System.out.println();
        System.out.println();
    }

    public int neighborsAlive(int i, int j) {
        int edge = size - 1;
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

    public int totalAlive() {
        int i, j, count = 0;

        for (i = 0; i < size; i++) {
            for (j = 0; j < size; j++) {
                count += grid[i][j];
            }
        }

        return count;
    }

    public int execute() {
        int i, j, totalAlive = 0;

        // #pragma omp parallel for reduction(+:totalAlive) private(i, j)
        for (i = 0; i < size; i++) {
            for (j = 0; j < size; j++) {
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

    public void createGrids() {
        int i, j;

        grid = new int[size][size];
        newGrid = new int[size][size];

        for (i = 0; i < size; i++) {
            for (j = 0; j < size; j++) {
                grid[i][j] = 0;
                newGrid[i][j] = 0;
            }
        }
    }

    public void copyNewGridToGrid() {
        int i, j;

        for (i = 0; i < size; i++) {
            for (j = 0; j < size; j++) {
                grid[i][j] = newGrid[i][j];
            }
        }
    }

    public void initialize(int[][] matriz) {
        // GLIDER
        int lin = 1, col = 1;
        matriz[lin][col + 1] = 1;
        matriz[lin + 1][col + 2] = 1;
        matriz[lin + 2][col] = 1;
        matriz[lin + 2][col + 1] = 1;
        matriz[lin + 2][col + 2] = 1;

        // R-pentomino
        lin = 10;
        col = 30;
        matriz[lin][col + 1] = 1;
        matriz[lin][col + 2] = 1;
        matriz[lin + 1][col] = 1;
        matriz[lin + 1][col + 1] = 1;
        matriz[lin + 2][col + 1] = 1;
    }

    public static void main(String[] args) {

        HighLife G = new HighLife();

        int generations, n_threads;

        generations = 2000;
        n_threads = 2;

        long startProgramTime = System.nanoTime();

        System.out.println("\n**Game of Life**");
        System.out.println("Matriz de tamanho " + G.size);
        System.out.println("Número de gerações: " + generations);
        System.out.println("Número de threads: " + n_threads);

        G.createGrids();

        G.initialize(G.grid);
        G.initialize(G.newGrid);

        int n = G.totalAlive();
        System.out.println("Condição inicial: " + n);
        System.out.println();

        int i=0;
        for (i = 0; i < generations; i++) {
            long startLoopTime = System.nanoTime();

            n = G.execute();
            G.copyNewGridToGrid();

            long endLoopTime = System.nanoTime();

            long totalLoopTime = endLoopTime - startLoopTime;

            System.out.println("Geração "+ i + ":" + n + " | Tempo de execução: x seg "+ totalLoopTime +" \n");

            if (i < 5)
                G.print();
        }

        System.out.println("\nÚltima geração (" + generations+ " iterações): " + n + "células vivas\n");

        G.freeGrids();

        long endProgramTime = System.nanoTime();

        long totalProgramTime = endProgramTime - startProgramTime;
        System.out.println("\nTempo total de execução: "+ totalProgramTime +" min\n");
    }
}