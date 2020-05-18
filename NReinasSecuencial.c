#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#define NMax 16

//Variable global utilizada para el tamaño del tablero
unsigned char N;
//Variable global utilizada para la cantidad de diagonales
unsigned char cDiagonales;

//Funcion para obtener el tiempo
double dwalltime()
{
	double sec;
	struct timeval tv;
	gettimeofday(&tv,NULL);
	sec = tv.tv_sec + tv.tv_usec/1000000.0;
	return sec;
}
//Estructura del tablero
typedef struct Tablero{
    //Arreglo de dimension N para indicar por cada fila en que columna hay una reina
     char reinas[NMax];
     char diagonal[(2 * NMax)-1]; //Representa la cantidad de reinas en la i-esima diagonal
     char diagonalI[(2 * NMax)-1];//Representa la cantidad de reinas en la i-esima diagonal Inversa.
} Tablero;
//Quita una reina de un tablero
void quitarReina(Tablero * t,unsigned char x, unsigned char y)
{
    t->reinas[x]   = -1;
    //Decrementa en uno la columnas y las diagonales correspondientes
    t->diagonal[y -x +(cDiagonales-2)/2] -= 1;
    t->diagonalI[N -1 -y -x +(cDiagonales-2)/2] -= 1;
}
unsigned char verificarParcial (Tablero *t, unsigned char i)
{
    //Verifica en la nueva reina en el tablero si en su columna o diagonales hay mas de una reina.
    char resultado = 1;
    //Obtengo los indices
    char iDiagonal =  t->reinas[i] - i +(cDiagonales-2)/2 ;
    char iDiagonalI =  N -1 -t->reinas[i] -i +(cDiagonales-2)/2;
    //Si es valido
    if (iDiagonal > -1){
        //Veifico
        resultado &= !(t->diagonal[iDiagonal] > 1);
    }
    //Si es valido
    if (iDiagonalI > -1){
        //Verifico
        resultado &= !(t->diagonalI[iDiagonalI] > 1);
    }
    return resultado;
}
//Inicializa el tablero vacio, con todas las columnas y diagonales en 0
void reInicializarTablero(Tablero * t)
{
    unsigned char i;
    for (i = 0; i<cDiagonales; i++)
    {
        if (i < N)
        {
                 t->reinas[i] = -1;
        }
        t->diagonal[i] = 0;
        t->diagonalI[i] = 0;
    }
}
//Prueba ubicar reina si puede retorna 1, en caso contrario 0
char ubicarReina(Tablero * t,unsigned char x, unsigned char y)
{
    
    if (t->reinas[x] == -1)
    {//Si la columna está disponible
        //Coloco la reina.
        t->reinas[x]   = y;
        //Incremento en uno las diagonales ocupadas.
        t->diagonal[y -x +(cDiagonales-2)/2] += 1;
        t->diagonalI[N -1 -y -x +(cDiagonales-2)/2] += 1;
        //Verifico si el tablero es valido
        if (verificarParcial(t,x))
        {   //Si el tablero resultante es valido retorno 1
            return 1;
        }
        else
        {   //Si no es valido quito la reina recien colocada y retorno 0
            quitarReina(t,x,y);
            return 0;
        }
    }
    else
    {//Si la columna está ocupada retorno 0
        return 0;
    }

}
// Imprime el vector de reinas, solo se usó durante la depuracion
void imprimirReinas(Tablero * t)
{
    int i;
    printf("\nReinas:\t\t");
    for (i = 0;i<N;i++)
    {
        printf("%d\t",t->reinas[i]);
    }
}

// Imprime el tablero completo, solo se usó durante la depuracion
void imprimirTablero(Tablero * t,unsigned char * f)
{
    int i = 0;
    printf("\nReinas:\t\t");
    for (i = 0;i<N;i++)
    {
        printf("%d\t",t->reinas[i]);
    }
    printf("\n\nDiagonal:\t");
    for (i = 0;i<cDiagonales;i++)
    {
        printf("%d\t",t->diagonal[i]);
    }
    printf("\n\nDiagonalI:\t");
    for (i = 0;i<cDiagonales;i++)
    {
        printf("%d\t",t->diagonalI[i]);
    }
        printf("\n");
        printf("Vector Filas:\t");
    for (i = 0;i<N;i++)
    {
        printf("%d\t",f[i]);
    }
}
//Retorna 1 si hay una reina en la posicion dada, y 0 en caso contrario
unsigned char hayReina (Tablero * t,unsigned char x, unsigned char y){
    return t->reinas[x] == y;
}

int main(int argc, char const *argv[])
{
    //Variable para los indices que recorren el tablero
    //i representa a las columnas y j a las filas
    unsigned char i = 0, j = 0;
    //Variable donde se acumularán las soluciones encontradas
    int soluciones = 0;
    if (argc < 2){
        // Si no se recibió el argumento del tamaño del tablero
        //Muestro mensaje de error y finalizo la ejecucion.
		printf("\n Falta un argumento:: N dimension del Tablero \n");
		return 0;
	};
    //Convierte a un valor entero el parametro recibido
    N=atoi(argv[1]);
    //Creo un vector para saber a que columna debe volver para una dada fila
    unsigned char filas[N];
    
    cDiagonales = (2*N)-1;
    //Crea una variable para un tablero
    Tablero t;
    //Muestra los mensajes de que tablero se va a procesar
    printf("Tablero de %d * %d\n",N,N);
    printf("Con %d Diagonales\n",cDiagonales);
    //Inicializa el tablero
    reInicializarTablero(&t);
    //Toma un valor de tiempo
    double timetick = dwalltime();
        while(i < N)
        {
            while (j < N)
            {//Mientras no se desborde el tablero.....
                //Almacena la columna procesada para la fila actual
                filas[j] = i;
                if (!hayReina(&t,i,j))
                {//Si no hay reina, Prueba ubicar
                    if (ubicarReina(&t,i,j))
                    {//Si pude ubicar y es la ultima fila
                        if (j == N-1)
                        {//Encontró solución
                            soluciones++;
                            quitarReina(&t,i,j);
                            j--;
                            i = filas[j];

                        }
                    //Si pude ubicar y NO es en la ultima fila
                        else
                        {//Mueve el indice a la columna 0
                            i = 0;
                         //Corta la ejecucion para salga del while actual e incremente el indice de las filas
                            break;
                        }
                    }
                    else
                    {// Si no pude ubicar y es la ultima columna
                        if (i == N-1)
                        {//Vuelve a donde estaba en la fila anterior
                            j--;
                            i = filas[j];
                        }
                    //Si no pude ubicar y NO es la ultima columna
                        else
                        {//Avanza a la columna siguiente
                            i++;
                        }
                    }
                }
                else
                {//Si hay reina en la posicion actual y es la ultima columna
                    if (i == N-1)
                    {//Quita la reina
                        quitarReina(&t,i,j);
                    //Decrementa J, que en caso de tener que tomar el valor -1 al estar declarada como unsigned char toma el valor 255
                        j--;
                    //Si j es mayor que la dimension del tablero --> Fin del tablero
                        if (j > N){
                            //Modifica los indices de forma que corten la ejecucion
                            i = N + 1;
                            j = N + 1;
                        }
                        else
                        {//Si no se termino el tablero, vuelve a la columna correspondiente en la fila anterior
                            i = filas[j];
                        }
                    }
                //Si hay reina y NO es la ultima columna
                    else
                    {//Quita la reina  
                        quitarReina(&t,i,j);
                    //Avanza a la siguiente columna
                        i++;
                    }

                }
            }//while (j < N)
            //Avanza a la siguiente fila
            j++;
        }//while (i < N)
    //Toma un nuevo valor de tiempo y muestra la diferencia con respecto al inicio
    printf("\nTiempo en segundos: %f \n", dwalltime() - timetick);
    //Muestra la cantidad de soluciones encontradas.
    printf("\nSoluciones:%d\n",soluciones);
    //Libera la memoria utilizada por el tablero
    return 0;
}
