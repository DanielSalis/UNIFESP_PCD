## Readme com instruções das atividades

* O MPI deve estar instalado na máquina. No Ubuntu o comando é esse:

```
sudo apt-get install libcr-dev mpich2 mpich2-doc
```

* Para compilar:
```
mpicc game_of_life_MPI.c -o game_of_life_MPI -O3
```

* Para executar:
```
mpiexec -np <NP> ./game_of_life_MPI
```

NP = número de processos
