package Java;

import java.util.ArrayList;

class GameThread extends Thread {

    public int gridColumnSize;
    public int gridLineSize;
    public int start;
    public int end;
    public int[][] prevMatrix;
    public int[][] nextMatrix;
    public int geracao;

    public GameThread(int gcs, int gls, int s, int e, int[][] pM, int[][] nM, int g) {
        this.start = s;
        this.end = e;
        this.prevMatrix = pM;
        this.nextMatrix = nM;
        this.geracao = g;
        this.gridColumnSize = gcs;
        this.gridLineSize = gls;
    }

    public void run() {
        this.generateNewGrid(this.prevMatrix, this.nextMatrix, this.geracao);
    }

    private void generateNewGrid(int[][] oldGrid, int[][] newGrid, int geracao_counter) {
        int aliveCellsCounter;

        Boolean geracaoPar = geracao_counter % 2 == 0 ? true : false;
        if (geracaoPar) {
            for (int i = this.start; i < this.end; i++) {
                for (int j = 0; j < this.gridColumnSize; j++) {
                    aliveCellsCounter = this.getNeighbors(i, j, newGrid, this.gridColumnSize);

                    if (newGrid[i][j] == 1 && (aliveCellsCounter == 2 || aliveCellsCounter == 3)) {
                        oldGrid[i][j] = 1;
                    }

                    else if (newGrid[i][j] == 0 && aliveCellsCounter == 3) {
                        oldGrid[i][j] = 1;
                    }

                    else {
                        oldGrid[i][j] = 0;
                    }
                }
            }
        } else {
            for (int i = this.start; i < this.end; i++) {
                for (int j = 0; j < this.gridColumnSize; j++) {
                    aliveCellsCounter = this.getNeighbors(i, j, oldGrid, this.gridColumnSize);

                    if (oldGrid[i][j] == 1 && (aliveCellsCounter == 2 || aliveCellsCounter == 3)) {
                        newGrid[i][j] = 1;
                    } else if (oldGrid[i][j] == 0 && aliveCellsCounter == 3) {
                        newGrid[i][j] = 1;
                    } else {
                        newGrid[i][j] = 0;
                    }
                }
            }
        }

    }

    private int getNeighbors(int i, int j, int[][] matriz, int lines) {
        int edge = lines - 1;
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

                count += matriz[pos_i][pos_j];
            }
        }

        return count;
    }
}

public class JogoDaVida {

    ArrayList<GameThread> threads = new ArrayList<GameThread>();
    int maxThreads; // 1, 2, 4, 8
    int column;
    int line;
    int generations;
    int[][] prevMatrix;
    int[][] nextMatrix;

    public JogoDaVida(int mT, int c, int l, int g) {
        this.maxThreads = mT; // 1, 2, 4, 8
        this.column = c;
        this.line = l;
        this.generations = g;
        this.prevMatrix = new int[l][c];
        this.nextMatrix = new int[l][c];
    }

    public static void inicializa(int[][] matriz1, int[][] matriz2, int N) {
        int aliveCellsCounter = 0;

        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                matriz1[i][j] = 0;
                matriz2[i][j] = 0;
            }
        }

        // GLIDER
        int lin = 1, col = 1;
        matriz1[lin][col + 1] = 1;
        matriz1[lin + 1][col + 2] = 1;
        matriz1[lin + 2][col] = 1;
        matriz1[lin + 2][col + 1] = 1;
        matriz1[lin + 2][col + 2] = 1;

        lin = 10;
        col = 30;
        matriz1[lin][col + 1] = 1;
        matriz1[lin][col + 2] = 1;
        matriz1[lin + 1][col] = 1;
        matriz1[lin + 1][col + 1] = 1;
        matriz1[lin + 2][col + 1] = 1;
        aliveCellsCounter = 10;

        System.out.println("Condição Inicial:" + aliveCellsCounter);
    }

    public static void preencheMatriz(int[][] matriz) {
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

    public static int getVivos(int[][] matriz, int lines, int columns) {
        int vivos = 0;

        for (int i = 0; i < lines; i++) {
            for (int j = 0; j < columns; j++) {
                vivos += matriz[i][j];
            }
        }
        return vivos;
    }

    public static void main(String[] args) throws InterruptedException {
        JogoDaVida jogo = new JogoDaVida(8, 2048, 2048, 2000);

        inicializa(jogo.prevMatrix, jogo.nextMatrix, jogo.line);

        long startTime = System.currentTimeMillis();

        for (int i = 1; i <= jogo.generations; i++) {
            long startLoopTime = System.nanoTime();

            for (int j = 0; j < jogo.maxThreads; j++) {
                int s = (jogo.line / jogo.maxThreads) * j;
                int e = (jogo.column / jogo.maxThreads) * (j + 1);
                GameThread J = new GameThread(jogo.line, jogo.column, s, e, jogo.prevMatrix, jogo.nextMatrix, i);
                J.start();
                jogo.threads.add(J);
            }
            for (Thread t : jogo.threads) {
                try {
                    t.join();
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
            int vivosAtuais = getVivos(jogo.prevMatrix, jogo.line, jogo.column);
            long total_loop_time = (System.nanoTime() - startLoopTime) / 1000000;
            System.out.println("Geração " + i + ": " + vivosAtuais + " | Tempo de execução: " + total_loop_time + "ms");
        }

        int vivos = getVivos(jogo.prevMatrix, jogo.line, jogo.column);
        System.out.println("Última geração (" + jogo.generations + " iterações): " + vivos + " células vivas");

        long total_time = (System.currentTimeMillis() - startTime) / 1000;
        System.out.println("Tempo total de execução: " + total_time + "s");
    }
}