## Executando testes em C

Dentro da pasta, digite os códigos abaixo:

### Game of Life

1. Para compilar
```
gcc game_of_life_thread.c -fopenmp -o game_of_life_thread.out
```

2. Para executar
```
./game_of_life_thread.out 2048 50 8 > game_of_life_thread.txt
```

### High Life

1. Para compilar
```
gcc high_life_thread.c -fopenmp -o high_life_thread.out
```

2. Para executar
```
./high_life_thread.out 2048 50 8 > high_life_thread.txt
```

Os parâmetros são:
1. O tamanho N da matrix NxN
2. Número de gerações
3. Número de threads