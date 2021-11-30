## Readme com instruções das atividades

### Exercício 01

* Para compilar: ```gcc  -fopenmp -03  exercicio01.c -o exercicio01.out```

* Para executar ```./exercicio01.out <number_of_threads>```

### Exercício 02

* Para compilar, ex:
```
gcc game_of_life_reduction.c -fopenmp 03 -o game_of_life_reduction.out
```

* Para executar, ex:
```
./game_of_life_thread.out 6000 50 4
```

Os parâmetros são:
1. O tamanho N da matrix NxN
2. Número de gerações
3. Número de threads