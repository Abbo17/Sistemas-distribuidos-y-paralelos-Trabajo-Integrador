#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <omp.h>																		//Se incluye la libreria openMP	
#define PRINT 0																			//Definición utilizada para imprimir o no las matrices
#define DBL_MAX 1e+37																	//Definición que contiene el maximo valor de un double
double * A,*L,*U,*AA,*AL,*UA,*R;														//Definición de matrices
double MinA = DBL_MAX, MaxA = DBL_MAX * -1, PromA = 0, timetick = 0;					//Definición de mínimo, máximo , promedio y timetick para el calculo del tiempo
int N = 0;																				//Definición de la variable que contiene el tamaño de las matrices

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


int main(int argc,char*argv[]){

	int i,j,k,NUM_THREADS;																//Definición de indices y numero de hilos
	int DimTriangular =0;																//Definición de la variable correspondiente a la dimension de las matrices triangulares
	
	if (argc < 3){
		printf("\n Falta un argumento:: N dimension de la matriz, n es la cantidad de hilos \n");
		return 0;
	}

	N=atoi(argv[1]);																	//Convierto la dimension de N a entero 
	NUM_THREADS = atoi(argv[2]);
	if (N < NUM_THREADS && N % NUM_THREADS == 0)										
	{
		printf("\n N debe ser Mayor o igual a %d \n",NUM_THREADS);
		return 0;
	}
	omp_set_num_threads(NUM_THREADS);													//Definimos el numero de threads		
//Conviert la dimension de N a entero o
	N=atoi(argv[1]);
//Calculo la dimension del vector para las matrices triangulares
	for (i=0;i<N;i++)
	{
		DimTriangular += N-i;
	}

//Aloca memoria para las matrices
	A  =(double*)malloc(sizeof(double)*N*N);
	U  =(double*)malloc(sizeof(double)*DimTriangular);
	L  =(double*)malloc(sizeof(double)*DimTriangular);
	AA =(double*)malloc(sizeof(double)*N*N);
	AL =(double*)malloc(sizeof(double)*N*N);
	UA =(double*)malloc(sizeof(double)*N*N);
	R  =(double*)malloc(sizeof(double)*N*N);
//Inicializar las matrices
for(i=0;i<N;i++){
	for(j=0;j<N;j++){
		A[i*N+j]=1;
		if(i>=j){
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
//Control de tiempo
	timetick = dwalltime();


  /////////////////////////////////
 /////////////PROCESAR////////////
/////////////////////////////////

//Inicio de procesamiento de A x A
	
    for(i=0;i<N;i++){
    	#pragma omp parallel for private (k) reduction(+:PromA) reduction(min:MinA) reduction(max:MaxA)	 			//Defino variables internas de indice como privada, y variables minimo, maximo y promedio como compartidas									
   		for(j=0;j<N;j++){
			if (A[i+N*j] < MinA){ MinA = A[i+N*j]; }
			if (A[i+N*j] > MaxA){ MaxA = A[i+N*j]; }
			PromA += A[i+N*j];
    		for(k=0;k<N;k++){
				//En el caso de A*A se realiza el acceso simultaneo que es mejor que cruzado.
				AA[i*N+j]= AA[i*N+j] + A[i*N+k]*A[i*N+k];
			}
		}
	}
    PromA = PromA / (N*N);
//Fin de procesamiento A
  
//Inicio de Procesamiento minA*(AL)
	
	for(i=0;i<N;i++){
		#pragma omp parallel for private (k)
   		for(j=0;j<N;j++){
    		for(k=i;k<N;k++){
				//Se realiza la multiplicacion de A (accediendo por filas) por la matriz inferior (accediendo por columnas)
				AL[i*N+j]= AL[i*N+j] + A[i*N+k]*L[k+(i*(i+1))/2];
			}
			//AL[i*N+j] *= MinA;
		}
	}
	//Fin de procesamiento minA*(AL)
  
//Inicio de Procesamiento PromA*(UA)

	
	for(k=0;k<N;k++){
		#pragma omp parallel for private (i)
   		for(j=0;j<N;j++){
    		for(i=0;i<=k;i++){
				//Se realiza la multiplicacion de A (accediendo por filas) por la matriz superior (accediendo por columnas)
				UA[j*N+i]= UA[j*N+i] + A[k*N+i]*U[i+(k*(k+1))/2];
			}
			//UA[j*N+i] *= PromA;
		}
	}
//Fin de procesamiento PromA*(UA)
  
//Inicio de Procesamiento R

	#pragma omp parallel for private (j)
	for(i=0;i<N;i++){
   		for(j=0;j<N;j++){
			R[i*N+j] = (AL[i*N+j] * MinA) + (AA[i*N+j] * MaxA) + (UA[i*N+j]*PromA);
		}
	}
//////////////////////////////
/////FIN PROCESAR////////////
/////////////////////////////
  printf("Tiempo en segundos UN BUCLE %f \n", dwalltime() - timetick);

int check = 1;
 //Verifica el resultado
  for(i=0;i<N;i++){
   for(j=0;j<N;j++){
	check=check&&(R[i*N+j]==3*N-i-j);
   }
  }   

  if(check){
   printf("Multiplicacion de matrices resultado correcto\n");
  }else{
   printf("Multiplicacion de matrices resultado erroneo\n");
  }


printf("MinA: %f - MaxA: %f - PromA: %f",MinA,MaxA,PromA);
if (PRINT){												//Se imprime las matrices si PRINT == 1
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
 free(A);													//Se liberan las memorias de las matrices
 free(U);	
 free(L);
 free(AL);
 free(AA);
 free(UA);
 free(R);

 return(0);

}
