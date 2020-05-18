# Sistemas distribuidos y paralelos - Trabajo sobre memoria compartida y memoria distribuida.

Dicho proyecto es un Trabajo Integrador realizado para la materia Sistemas distribuidos y paralelos correspondiente al plan de estudio de la carrera Ingeniería en
computación de la Universdad Nacional de la Plata.

## Objetivo

El trabajo consiste en la resolución de dos algoritmos diferentes realizando su implementación
secuencial y a su vez su paralelización mediante el uso de las librerias Pthread, OpenMP y MPI. En ambos ejercicios el algoritmo secuencial debe ser lo más
óptimo posible. Por otra parte también se deben realizar cálculos de tiempos de ejecución, junto con su Speedup y
Eficiencia.

## Optimización de algoritmos secuenciales y modelo de memoria compartida
En la primera parte del trabajo integrador se nos pide trabajar con el uso de memoria compartida
mediante las librerías de Pthread y OpenMP, para la resolución de la siguiente expresión:
*  A es una matriz de N x N.
* L y U son matrices de NxN triangulares inferior y superior, respectivamente.
*  Los escalares minA y maxA son el mínimo y el máximo valor de los elementos de la matriz
A, respectivamente.
* El escalar promA es el valor promedio de los elementos de la matriz A.

Para la ejecución del código paralelo se nos pide realizarlo con matrices de tamaño igual a 512, 1024,
2048 y mediante el uso de 2 y 4 hilos.

## N-Reinas - Programa paralelo

Resolver con MPI el problema de N-Reinas por demanda (modelo Master-Worker). El juego de las N-Reinas consiste en ubicar sobre un tablero de ajedrez N reinas sin que estas se amenacen entre ellas. Una reina amenaza a aquellas reinas que se encuentren en su misma fila, columna o diagonal. La solución al problema de las N-Reinas consiste en encontrar todas
las posibles soluciones para un tablero de tamaño NxN. En una estrategia Master-Worker, el proceso Master realiza cierto cálculo inicial y luego entrega una cantidad de trabajo específica a los workers cuando estos lo requieren. Cada proceso Worker realiza cómputo y entrega al proceso Master los resultados. Dependiendo de la aplicación, el proceso Mater
podría o no trabajar asumiendo transitoriamente el rol de Worker.
Evaluar para N entre 5 y 15 utilizando 2 máquinas:
* 4 procesos (2 en cada máquina)
* 8 procesos (4 en cada máquina)
Se debe obtener la cantidad total de soluciones encontradas NO los tableros con las soluciones.

### Informe del Proyecto

Para mas información acerca de este Proyecto puede acceder al Informe donde se detalla el mismo: [Informe Final.](https://github.com/Abbo17/Sistemas-distribuidos-y-paralelos-Trabajo-Integrador/blob/master/Informe%20Final.pdf) 

