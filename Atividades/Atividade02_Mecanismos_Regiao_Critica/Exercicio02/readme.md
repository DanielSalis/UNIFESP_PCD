## Executando testes em C

Dentro da pasta, digite os códigos abaixo:

### Game of Life

1. Para compilar, ex:
```
gcc game_of_life_reduction.c -fopenmp 03 -o game_of_life_reduction.out
```

2. Para executar, ex:
```
./game_of_life_thread.out 6000 50 4 
```

Os parâmetros são:
1. O tamanho N da matrix NxN
2. Número de gerações
3. Número de threads