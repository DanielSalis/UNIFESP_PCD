# Instruções

## Código Original
* [link](https://gist.github.com/amirmasoudabdol/f1efda29760b97f16e0e)

## Executando

* O MPI deve estar instalado na máquina. No Ubuntu o comando é:

```
sudo apt-get install libcr-dev mpich2 mpich2-doc
```

* Para compilar:
```
mpicc read_MPI_group.c -o read_MPI_group.out -O3
```

* Para executar:
```
mpiexec -np <NP> ./read_MPI_group.out
```

NP = número de processos
