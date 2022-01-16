#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

#define SEC(tv) (tv.tv_sec + tv.tv_usec/1e6)

void read_csv(int row, int col, char *filename, double **data){
	FILE *file;
	file = fopen(filename, "r");
	float menor = 100000.00;
	float maior = 0.00;
	float media = 0;
	float soma  = 0;

	int i = 0;
    char line[4098];
	while (fgets(line, 4098, file) && (i < row))
    {
    	// double row[ssParams->nreal + 1];
        char* tmp = strdup(line);

	    int j = 0;
	    const char* tok;
	    for (tok = strtok(line, ","); tok && *tok; j++, tok = strtok(NULL, ","))
	    {
	        data[i][j] = atof(tok);
			if(j == 2 && i > 0 && data[i][j] < menor){
				menor = data[i][j]; 
			}
			if(j == 2 && i > 0 && data[i][j] > maior){
				maior = data[i][j]; 
			}
			if(j == 2 && i > 0){
				soma = data[i][j] + soma;
			}
	        printf("%f\t", data[i][j]);
	    }
	    printf("\n");

        free(tmp);
        i++;
    }
	media = soma / (row - 1);
	printf("Media: %f", media);
	printf("\n");
	printf("O MENOR: %f", menor);
	printf("\n");
	printf("O MAIOR: %f", maior);
	printf("\n");
}

int main(int argc, char const *argv[])
{
	struct timeval p_start, p_end, p_time;
	/* code */
	if (argc < 3){
		printf("Please specify the CSV file as an input.\n");
		exit(0);
	}

	int row     = atoi(argv[1]);
	int col     = atoi(argv[2]);
	char fname[256];	strcpy(fname, argv[3]);

	double **data;
	data = (double **)malloc(row * sizeof(double *));
	for (int i = 0; i < row; ++i){
		data[i] = (double *)malloc(col * sizeof(double));
	}

	gettimeofday(&p_start, NULL);
	read_csv(row, col, fname, data);
	gettimeofday(&p_end, NULL);
    timersub(&p_end, &p_start, &p_time);
	 printf("\nTempo total de execução: %.3f s\n", SEC(p_time));

	return 0;
}

