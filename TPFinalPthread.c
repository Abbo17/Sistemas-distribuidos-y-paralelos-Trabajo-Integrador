#include<stdio.h>
#include<stdlib.h>
#include <sys/time.h>
#include <pthread.h>																	//Se incluye la libreria pthread
#define PRINT 0																			//Definición utilizada para imprimir o no las matrices
#define DBL_MAX 1e+37																	//Definición que contiene el maximo valor de un double
double * A,*L,*U,*AA,*AL,*UA,*R;
double MinA = DBL_MAX, MaxA = DBL_MAX * -1, PromA = 0, timetick = 0;					//Definición de mínimo, máximo , promedio global y timetick para el calculo del tiempo
int N = 0;																				//Definición de la variable que contiene el tamaño de las matrices
int NUM_THREADS = 0;																	//Definición de la variable que contiene el numero de hilos
pthread_mutex_t PromA_mutex = PTHREAD_MUTEX_INITIALIZER;								//Definición e inicialización del mutex
pthread_barrier_t barrera;																//Definición de barrera
void imprimirMatriz(double * M)															//Función que tiene como entrada una matriz de NxN e imprime dicha matriz
{
	int i,j;
	for(i=0;i<N;i++){
		printf("\r\n");	
		for(j=0;j<N;j++){
			printf("%4.0f ",M[i+j*N]);
		}
	}
}

void imprimirMatrizTS(double * U)														//Función que tiene como entrada una matriz triangular superior e imprime dicha matriz	
{
		int i,j;
		for(i=0;i<N;i++){
		printf("\r\n");	
		for(j=0;j<N;j++){
			if(i > j)
			{
				printf("%4.0f ",0.0);
			}
			else
			{
				printf("%4.0f ",U[j+(i*(i+1))/2]);
			}
		}
	}
}
void imprimirMatrizTI(double * L)														//Función que tiene como entrada una matriz triangular inferior e imprime dicha matriz
{
		int i,j;
		for(i=0;i<N;i++){
		printf("\r\n");	
		for(j=0;j<N;j++){
			if(i >= j)
			{
				
				printf("%4.0f ",L[j+(i*(i+1))/2]);
								
			}
			else
			{
				printf("%4.0f ",0.0);
			}
		}
	}

}


double dwalltime()																		//Función que se encarga de calcular el tiempo de ejecución																							
{
	double sec;
	struct timeval tv;

	gettimeofday(&tv,NULL);
	sec = tv.tv_sec + tv.tv_usec/1000000.0;
	return sec;
}

////////////////////////////////////////////////////////////////////////////////////////
 void ProcesarAL(long tid)															//Función que se encarga de realizar la multiplicacion de la matriz A con triangular inferior y el Mínimo valor de A
 {
	int i,j,k;
  	for(i=0;i<N;i++){
   		for(j=tid*(N/NUM_THREADS);j<(tid+1)*(N/NUM_THREADS);j++){						//Se realiza la división del for en partes iguales a los threads
    		for(k=i;k<N;k++){
				AL[i*N+j]= AL[i*N+j] + A[i*N+k]*L[k+(i*(i+1))/2];						//Calculo de la multiplicación de A (accediendo por filas) por la matriz triangular superior(accediendo por columnas)			
			}
			//AL[i*N+j] *= MinA;															//Multiplicación del resultado por el minimo global
		}
	}
 }
 ///////////////////////////////////////////////////////////////////////////////






 ///////////////////////////////////////////////////////////////////////////////
 void ProcesarUA(long tid)															//Función que se encarga de realizar la multiplicacion de la matriz A con triangular superior y el promedio valor de A
 {
	int i,j,k;
	for(k=0;k<N;k++){
   		for(j=tid*(N/NUM_THREADS);j<(tid+1)*(N/NUM_THREADS);j++){						//Se realiza la división del for en partes iguales a los threads
    		for(i=0;i<=k;i++){
				UA[j*N+i]= UA[j*N+i] + A[k*N+i]*U[i+(k*(k+1))/2];						//Calculo de la multiplicación de A (accediendo por filas) por la matriz triangular superior(accediendo por columnas)										
			}
			//UA[j*N+i] *= PromA;															//Multiplicación del resultado por el promedio global
		}
	}
 }

 ///////////////////////////////////////////////////////////////////////////////





 ///////////////////////////////////////////////////////////////////////////////
 void ProcesarA(long tid)
 {
	double lMinA,lMaxA,lPromA;															//Definición de las variables correspondientes al mínimo, máximo y sumatoria local
	int i,j,k;																			//Definición de indices
	lMinA = DBL_MAX;																	//Inicializo el Mínimo local
	lMaxA = DBL_MAX * -1;																		//Inicializo el Máximo local
	for(i=0;i<N;i++){
   		for(j=tid*(N/NUM_THREADS);j<(tid+1)*(N/NUM_THREADS);j++){						//Se realiza la división del for en partes iguales a los threads
			if (A[i+N*j] < lMinA){ lMinA = A[i+N*j]; }									//Se calcula el mínimo local de cada proceso
			if (A[i+N*j] > lMaxA){ lMaxA = A[i+N*j]; }									//Se calcula el máximo local de cada proceso
			lPromA += A[i+N*j];															//Se reaiza la sumatoria local de cada valor de la matriz por proceso
    		for(k=0;k<N;k++){
				AA[i*N+j]= AA[i*N+j] + A[i*N+k]*A[i*N+k];								//En el caso de A*A se realiza el acceso simultaneo que es mejor que cruzado.
			}
		}
	}
	lPromA /= (N*N);																	//Se calcula el promedio local
	pthread_mutex_lock(&PromA_mutex);													//Se entra a la sección critica para actualizar el Mínimo y Máximo global si fuera necesario
	/**/if (lMinA < MinA){ MinA = lMinA; }												//Se compara y actualiza el mínimo global si fuera necesario
	/**/if (lMaxA > MaxA){ MaxA = lMaxA; }												//Se compara y actualiza el máximo global si fuera necesario
	/**/PromA += lPromA;																//Se suma los valores calculados localmente para el calculo del promedio
	pthread_mutex_unlock(&PromA_mutex);													//Se sale de la sección critica
 }

/////////////////////////////////////////////////////////


////////////////////////////////////////////////////////


 void ProcesarR(long tid)
 {
	int i,j;
	for(i=0;i<N;i++){
   		for(j=tid*(N/NUM_THREADS);j<(tid+1)*(N/NUM_THREADS);j++){							//Se realiza la división del for en partes iguales a los threads
			R[i*N+j] = (AL[i*N+j]*MinA) + (AA[i*N+j] * MaxA) + (UA[i*N+j]*PromA);							//Se realiza la suma de todas las matrices resultantes de los procesos anteriores, multiplicando ademas el maximo global por el resultado de AxA
		}
	}
 }

//////////////////////////////////////////////////////


////////////////////////////////////////////////////

void *ProcesarParalelo(void * threadid)
{
	long tid;
    tid = (long)threadid;																	//id correspondiente a cada thread
	ProcesarA(tid);																			//Función donde se realiza la multiplicacion de la matriz A por si misma
	ProcesarAL(tid);																	//Función donde se realiza la multiplicacion de la matriz A por la matriz triangular inferior
	ProcesarUA(tid);																	//Función donde se realiza la multiplicacion de la matriz A por la matriz triangular superior
	pthread_barrier_wait(&barrera);															//Segunda barrera donde esperan todos los hilos para continuar su ejecución
	ProcesarR(tid);
    pthread_exit(NULL);
}

//////////////////////////////////////////////////////

/////////////////////////////////////////////////////
int main(int argc,char*argv[]){
	int i,j,k;
	int DimTriangular =0;																	//Variable para la dimension de las matrices triangulares
	
	if (argc < 3){
		printf("\n Falta un argumento:: N dimension de la matriz, n es la cantidad de hilos \n");
		return 0;
	}

	N=atoi(argv[1]);																		//Convierto la dimension de N a entero 
	NUM_THREADS = atoi(argv[2]);
	if (N < NUM_THREADS && N % NUM_THREADS == 0)											//Se comprueba que el numero de hilos sea multiplo de N y menor a este
	{
		printf("\n N debe ser Mayor o igual a %d \n",NUM_THREADS);
		return 0;
	}

	for (i=0;i<N;i++)																		//Calculo la dimension del vector para las matrices triangulares
	{
		DimTriangular += N-i;
	}

																							//Aloca memoria para las matrices
	A  =(double*)malloc(sizeof(double)*N*N); 												//Matriz A	
	U  =(double*)malloc(sizeof(double)*DimTriangular);										//Matriz triangular superior
	L  =(double*)malloc(sizeof(double)*DimTriangular);										//Matriz triangular inferior
	AA =(double*)malloc(sizeof(double)*N*N);												//Matriz resultante de la multiplicación de AxA
	AL =(double*)malloc(sizeof(double)*N*N);												//Matriz resultante de la multiplicación de AxL
	UA =(double*)malloc(sizeof(double)*N*N);												//Matriz resultante de la multiplicacion de UxL
	R  =(double*)malloc(sizeof(double)*N*N);												//Matriz resultante de realizar el algoritmo complet
																							//Inicializar las matrices
for(i=0;i<N;i++){
	for(j=0;j<N;j++){
		A[i*N+j]=1;
		if(i>=j){																			//Condición para incializar la matriz inferior o superior dado que no se consideran los ceros
			L[j+(i*(i+1))/2]=1;
		} 
		if(j>=i){
			U[i+(j*(j+1))/2]=1;
		}
		AA[i+j*N]=0;
		AL[i*N+j]=0;
		UA[i*N+j]=0;
		R[i*N+j]=0;
	}
}  

	timetick = dwalltime();																	//Control de tiempo


  /////////////////////////////////
 /////////////PROCESAR////////////
/////////////////////////////////
//Declaracion de variables
   	pthread_t threads[NUM_THREADS];															//Arreglo de Threads												
    int rc;	
    int t;
    long threads_ids[NUM_THREADS];															//Arreglo de ids de los Threads

//Inicio de procesamiento de A
	pthread_barrier_init(&barrera, NULL, NUM_THREADS);										//Creamos la barrera para los hilos
    for(t=0; t<NUM_THREADS; t++){
    	threads_ids[t]=t;																	//Establecemos los ids de los hilos
      	rc = pthread_create(&threads[t], NULL, ProcesarParalelo, (void *)threads_ids[t]);	//Creamos los hilos, donde le pasamos la función ProcesarParalelo y sus ids como parametros
       	if (rc){
          printf("ERROR; return code from pthread_create() is %d\n", rc);
          exit(-1);
       	}
    }
    for(t=0; t<NUM_THREADS; t++){															//Se espera a que todos los hilos finalicen
		pthread_join(threads[t],NULL);
	}
	pthread_mutex_destroy(&PromA_mutex);													//Eliminamos el mutex
	pthread_barrier_destroy(&barrera);														//Eliminamos la barrera
//Fin de procesamiento A
  	printf("Tiempo en segundos UN BUCLE %f \n", dwalltime() - timetick);
//////////////////////////////
/////FIN PROCESAR////////////
/////////////////////////////

int check = 1;
 //Verifica el resultado
  for(i=0;i<N;i++){
   for(j=0;j<N;j++){
	check=check&&(R[i*N+j]==3*N-i-j);														//Se realiza el chequeo individual de valor en la matriz resultante
   }
  }   

  if(check){
   printf("Multiplicacion de matrices resultado correcto\n");
  }else{
   printf("Multiplicacion de matrices resultado erroneo\n");
  }


printf("MinA: %f - MaxA: %f - PromA: %f",MinA,MaxA,PromA);									//Se imprime el máximo, mínimo y promedio global
if (PRINT){																					//Imprimimos las matrices si Print == 1
//PRINT
 printf("\r\nImprimiendo Matriz A:\r\n");
imprimirMatriz(A);
 printf("\r\nImprimiendo Matriz L:\r\n");
imprimirMatrizTI(L);
 printf("\r\nImprimiendo Matriz U:\r\n");
imprimirMatrizTS(U);
 printf("\r\nImprimiendo Matriz AL:\r\n");
imprimirMatriz(AL);
 printf("\r\nImprimiendo Matriz AA:\r\n");
imprimirMatriz(AA);
 printf("\r\nImprimiendo Matriz UA:\r\n");
imprimirMatriz(UA);
 printf("\r\nImprimiendo Matriz R:\r\n");

imprimirMatriz(R);
}
 printf("\r\n");
 free(A);																				//Liberamos la memoria utilizadas por las matrices
 free(U);
 free(L);
 free(AL);
 free(AA);
 free(UA);
 free(R);

 return(0);

}
