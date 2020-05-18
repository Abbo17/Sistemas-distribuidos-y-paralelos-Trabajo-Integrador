#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <mpi.h>

#define TAG_PEDIDO      1
#define TAG_RESULTADO   2
#define TAG_DATOS       3
#define TAG_FIN_DATOS   4

#define NMax 16

//Variable global utilizada para el tamaño del tablero
unsigned char N;
//Variable global utilizada para la cantidad de diagonales
unsigned char cDiagonales;
//Variable global utilizada para la cantidad de procesos en ejecucion
int cantidadDeProcesos;


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
     char diagonal[(2 * NMax) -1]; //Representa la cantidad de reinas en la i-esima diagonal
     char diagonalI[(2 * NMax) -1];//Representa la cantidad de reinas en la i-esima diagonal Inversa.
     char filas[NMax];    //Representa a que columna debe volver para una dada fila
} Tablero;

//Quita una reina de un tablero
void quitarReina(Tablero * t,unsigned char x, unsigned char y)
{
    t->reinas[x]   = -1;
    //Decrementa las diagonales correspondientes
    t->diagonal[y -x +(cDiagonales-2)/2] -= 1;
    t->diagonalI[N -1 -y -x +(cDiagonales-2)/2] -= 1;

}
unsigned char verificarParcial (Tablero *t, unsigned char i)
{
    //Verifica por cada reina en el tablero si en su columna o diagonales hay mas de una reina.
    char resultado = 1;
    //Obtengo los indices
    char iDiagonal =  t->reinas[i] - i +(cDiagonales-2)/2 ;
    char iDiagonalI =  N -1 -t->reinas[i] -i +(cDiagonales-2)/2;
    //Si es valido 
    if (iDiagonal > -1){
        //Verifico
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
//A partir de un tablero que solo tiene las posiciones de las reinas,
//Genera las estructuras auxiliares utilizadas para el procesamiento
void generarTablero(Tablero * t)
{
	int i;
	for (i = 0; i<N; i++)
	{
		if (t->reinas[i] != -1)
		{
			t->diagonal[t->reinas[i] -i +(cDiagonales-2)/2] += 1;
			t->diagonalI[N -1 -t->reinas[i] -i +(cDiagonales-2)/2] += 1;
            t->filas[t->reinas[i]] = i;
		}
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
void imprimirTablero(Tablero * t)
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
        printf("\nVector Filas:\t");
    for (i = 0;i<N;i++)
    {
        printf("%d\t",t->filas[i]);
    }
    printf("\n");
}

//Retorna 1 si hay una reina en la posicion dada, y 0 en caso contrario
unsigned char hayReina (Tablero * t,unsigned char x, unsigned char y){
    return t->reinas[x] == y;
}

int Procesar(Tablero * t)
{
    int j = 2,i = 0;
    int iback = t->filas[1] + 1;
	int soluciones = 0;
        while(i < N)
        {
            while (j < N)
            {
                t->filas[j] = i;
                if (!hayReina(t,i,j))
                {
                    if (ubicarReina(t,i,j))
                    {
                        if (j == N-1)
                        {
                            soluciones++;
                            quitarReina(t,i,j);
                            j--;
                            i = t->filas[j];

                        }
                        else
                        {
                            i = 0;
                            break;
                        }
                    }
                    else
                    {
                        if (i == N-1)
                        {
                            j--;
                            i = t->filas[j];
                        }
                        else
                        {
                            i++;
                        }
                    }
                }
                else
                {
                        if (i == N-1 && j != 1)
                        {
                            quitarReina(t,i,j);
                            j--;
                            i = t->filas[j];
                        }
                        else
                        {
                            quitarReina(t,i,j);
                            i++;
                        }
                        if (j == 1 && i == iback){
                                i = N + 1;
                                j = N + 1;
                        }

                }
            }
            j++;
        }
	return soluciones;
}

void GenerarSemillas(Tablero * tableros)
{
	Tablero tablero;
	Tablero * t = &tablero;
	reInicializarTablero(t);
	int soluciones = 0;
	unsigned char i=0,j=0,k;
    while (j < 2)
        {
        while(i < N)
            {
                t->filas[j] = i;
                if (!hayReina(t,i,j))
                {
                    if (ubicarReina(t,i,j))
                    {
                        if (j == 1)
                        {
                        	for (k=0;k<N;k++)
                        	{
                        		tableros[soluciones].reinas[k] = t->reinas[k];
                        	}
                        	soluciones++;
                            quitarReina(t,i,j);
                            if (i == N-1)
                            {
                                j--;
                                i = t->filas[j];
                            }else
                            {
                                i++;

                            }

                        }
                        else
                        {
                            i = 0;
                            break;
                        }
                    }
                    else
                    {
                        if (i == N-1)
                        {
                            j--;
                            i = t->filas[j];
                        }
                        else
                        {
                            i++;
                        }
                    }
                }
                else
                {
                        if (i == N-1)
                        {
                            quitarReina(t,i,j);
                            j--;
                            if (j > 2){
                                i = N + 1;
                                j = N + 1;
                            }
                            else
                            {
                                i = t->filas[j];
                            }
                        }
                        else
                        {
                            quitarReina(t,i,j);
                            i++;
                        }

                }
            }
            j++;
        }
}





void Master()
{
    //Muestra Mensajes Identificandose y sobre que tablero va a procesar
	printf("Hello, I'm Master.\n");
    printf("Tablero de %d * %d\n",N,N);
    printf("Con %d Diagonales\n",cDiagonales);
    //Crea una varible para el estado de la comunicacion
    MPI_Status status;
    //Crea variable para el flag que indica si hay un mensaje pendiente
    int flag;
    //Crea variable dato para recibir lo que envien los Workers
    //Crea variable soluciones para acumular las soluciones
    int dato, soluciones = 0;
    //Crea variable indice de varios usos
    int k;
    //Crea variable semillas inicializada en la cantidad de semillas que debe generar para dos tener las dos primeras filas completas
    int semillas = (N-1)*(N-2);
    //Crea un arreglo de tableros de tamaño cantidad de semillas
    Tablero tableros[semillas];
    //Toma el tiempo actual y lo almacena en una variable
    double timetick = dwalltime();
    for (k=0; k<semillas; k++)
    {
        //Inicializa los tableros
    	reInicializarTablero(&tableros[k]);
    }
    //Hace llamado a la funcion que crea cada una de las semillas
    GenerarSemillas(tableros);
    printf("\nTiempo Procesamiento Secuencial en segundos: %f \n", dwalltime() - timetick);
    timetick = dwalltime();
    k = 0;
    //Mientras no haya entregado todas las semillas....
    while(k <semillas){
        //Lee si hay mensajes pendientes de recibir
        MPI_Iprobe(MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD,&flag,&status);
        if (flag)
        {  //Si hay mensajes lo recibe
            MPI_Recv(&dato,1,MPI_INT,MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD,&status);
            if (status.MPI_TAG == TAG_PEDIDO)
            {// Si era un pedido le manda un tablero para trabajar
                MPI_Ssend(tableros[k].reinas,N,MPI_CHAR,status.MPI_SOURCE,TAG_DATOS,MPI_COMM_WORLD);
                k++;
            }else if(status.MPI_TAG == TAG_RESULTADO){
                //Si era un resultado, lo almacena y le manda mas trabajo
                soluciones += dato;
                MPI_Ssend(tableros[k].reinas,N,MPI_CHAR,status.MPI_SOURCE,TAG_DATOS,MPI_COMM_WORLD);
                k++;
            }
        }
    }
    k = 0;
    //Mientras no se le informe a todos los workers que no hay mas datos....
    while(k < cantidadDeProcesos - 1)
    {
        //Lee si hay mensajes pendientes de recibir
        MPI_Iprobe(MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD,&flag,&status);
        if (flag)
        {   //Si hay mensajes lo recibe
            MPI_Recv(&dato,1,MPI_INT,MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD,&status);
            if (status.MPI_TAG == TAG_RESULTADO)
            { //Si era un resultado lo almacena
                soluciones += dato;
            }
            //Indistintamente si es pedido o resultado le envia mensaje de que no hay mas datos
            MPI_Ssend(tableros[k].reinas,N,MPI_CHAR,status.MPI_SOURCE,TAG_FIN_DATOS,MPI_COMM_WORLD);
            k++;
        }
    }
	printf("\nTiempo Procesamiento Paralelo en segundos: %f \n", dwalltime() - timetick);
    printf("\nSoluciones:%d\n",soluciones);
}

void Worker(int id)
{
    //Variable de corte de la ejecucion
    unsigned char corte = 0;
    //Crea una variable para el dato que le tiene que enviar al master
    int send = -1;
    //Crea una variable para almacenar las soluciones locales encontradas
    int soluciones;
	//Crea una variable para recibir el tablero parcial
    char dato[N];
	//Crea una varible para el estado de la comunicacion
    MPI_Status status;
    //Inicializa un tablero
	Tablero wTablero;
    //Envia un pedido al master
    MPI_Ssend(&send,1,MPI_INT,0,TAG_PEDIDO,MPI_COMM_WORLD);
    //Mientras el master no indique fin de datos...
    while (!corte){
        reInicializarTablero(&wTablero);
        //Hace una recepcion bloqueante
        MPI_Recv(wTablero.reinas,N,MPI_CHAR,0,MPI_ANY_TAG,MPI_COMM_WORLD,&status);
        //Si recibio datos...
        if (status.MPI_TAG == TAG_DATOS)
        {
            soluciones = 0;
            //Genera los datos faltantes
            generarTablero(&wTablero);
            //Busca las soluciones
            soluciones = Procesar(&wTablero);
        	//Envia las soluciones al master
            MPI_Ssend(&soluciones,1,MPI_INT,0,TAG_RESULTADO,MPI_COMM_WORLD);
        }
        else if(status.MPI_TAG == TAG_FIN_DATOS)
        {
            //Si recibe fin de datos corta la ejecucion
            corte = 1;
        }
        else
        {
            return;
        }
    }
}


int main(int argc, char *argv[])
{
 
	if (argc < 2){
	    // Si no se recibió el argumento del tamaño del tablero
        //Muestro mensaje de error y finalizo la ejecucion.
    	printf("\n Falta un argumento:: N dimension del Tablero \n");
		return 0;
	};
    //Convierte a un valor entero el parametro recibido
	N=atoi(argv[1]);
    //Calcula la cantidad de diagonales que tiene un tablero
	cDiagonales = (2*N)-1;
    
    int miID;
    //Inicializa MPI
    MPI_Init(&argc, &argv);
    //Obtiene su ID
    MPI_Comm_rank(MPI_COMM_WORLD,&miID);
    //Obtiene la cantidad de procesos
    MPI_Comm_size(MPI_COMM_WORLD,&cantidadDeProcesos);
    //Toma su rol en funcion de su ID
    if(miID == 0){
        Master();
        }
    else {
        Worker(miID);
        }
    MPI_Finalize();
    return 0;
}
