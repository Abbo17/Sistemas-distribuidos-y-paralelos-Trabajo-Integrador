#include<stdio.h>
#include<stdlib.h>
#include <sys/time.h>
#include <pthread.h>
#define PRINT 0																	//Definición utilizada para imprimir o no las matrices
#define DBL_MAX 1e+37															//Definición que contiene el maximo valor de un double
double * A,*L,*U,*AA,*AL,*UA,*R;												//Definición de matrices
double MinA = DBL_MAX, MaxA = DBL_MAX * -1, PromA = 0, timetick = 0;			//Definición de variables
//Definición de la variable correspondiente al tamaño de las matrices cuadradas
int N = 0;																		

void imprimirMatriz(double * M)							//Función que tiene como entrada una matriz de NxN e imprime dicha matriz
{
	int i,j;
	for(i=0;i<N;i++){
		printf("\r\n");	
		for(j=0;j<N;j++){
			printf("%4.0f ",M[i+j*N]);
		}
	}
}

void imprimirMatrizTS(double * U)			//Función que tiene como entrada una matriz triangular superior e imprime dicha matriz	
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
	int i,j,k;
	int DimTriangular =0;
	
	if (argc < 2){
		printf("\n Falta un argumento:: N dimension de la matriz");
		return 0;
	}
	N=atoi(argv[1]);																	//Convierto la dimension de N a entero 

	for (i=0;i<N;i++)																	//Calculo la dimension del vector para las matrices triangulares
	{
		DimTriangular += N-i;
	}

//Aloca memoria para las matrices
	A  =(double*)malloc(sizeof(double)*N*N); 											//Matriz A	
	U  =(double*)malloc(sizeof(double)*DimTriangular);									//Matriz triangular superior
	L  =(double*)malloc(sizeof(double)*DimTriangular);									//Matriz triangular inferior
	AA =(double*)malloc(sizeof(double)*N*N);											//Matriz resultante de la multiplicación de AxA
	AL =(double*)malloc(sizeof(double)*N*N);											//Matriz resultante de la multiplicación de AxL
	UA =(double*)malloc(sizeof(double)*N*N);											//Matriz resultante de la multiplicacion de UxL
	R  =(double*)malloc(sizeof(double)*N*N);											//Matriz resultante de realizar el algoritmo completo

for(i=0;i<N;i++){																		//Inicialización de matrices
	for(j=0;j<N;j++){
		A[i*N+j]=1;
		if(i>=j){
			L[j+(i*(i+1))/2]=1;
		} 
		if(j>=i){																		//Comparación realizada para acceder a la matriz triangular inferior o superior
			U[i+(j*(j+1))/2]=1;
		}
		AA[i+j*N]=0;
		AL[i*N+j]=0;
		UA[i*N+j]=0;
		R[i*N+j]=0;
	}
}  

	timetick = dwalltime();																//Inicio para el control de tiempo


  /////////////////////////////////
 /////////////PROCESAR////////////
/////////////////////////////////

//Inicio de procesamiento de A

    for(i=0;i<N;i++){
   		for(j=0;j<N;j++){
			if (A[i+N*j] < MinA){ MinA = A[i+N*j]; }									//Calculo del mínimo de A
			if (A[i+N*j] > MaxA){ MaxA = A[i+N*j]; }									//Calculo del máximo de A
			PromA += A[i+N*j];															//Sumatoria para el calculo del promedio de A
    		for(k=0;k<N;k++){
				AA[i*N+j]= AA[i*N+j] + A[i*N+k]*A[i*N+k];								//En el caso de A*A se realiza el acceso simultaneo que es mejor que cruzado.
			}
		}
	}
    PromA = PromA / (N*N);																//Calculo del promedio de A
//Fin de procesamiento A
  
//Inicio de Procesamiento minA*(AL)
	for(i=0;i<N;i++){
   		for(j=0;j<N;j++){
    		for(k=i;k<N;k++){															//El for este se realiza desde i hasta N debido a que no se tuvo en cuenta los ceros de la matriz triangular
				AL[i*N+j]= AL[i*N+j] + A[i*N+k]*L[k+(i*(i+1))/2];						//Calculo de la multiplicación de A (accediendo por filas) por la matriz triangular inferior(accediendo por columnas)
			}
			AL[i*N+j] *= MinA;
		}
	}
	//Fin de procesamiento minA*(AL)
  

//Inicio de Procesamiento PromA*(UA)
	for(k=0;k<N;k++){
   		for(j=0;j<N;j++){
    		for(i=0;i<=k;i++){															//El for este se realiza desde i= 0 hasta k debido a que no se tuvo en cuenta los ceros de la matriz triangular
				UA[j*N+i]= UA[j*N+i] + A[k*N+i]*U[i+(k*(k+1))/2];						//Calculo de la multiplicación de A (accediendo por filas) por la matriz triangular superior(accediendo por columnas)
			}
			UA[j*N+i] *= PromA;
		}
	}
//Fin de procesamiento PromA*(UA)


//Inicio de Procesamiento R
	for(i=0;i<N;i++){
   		for(j=0;j<N;j++){
			R[i*N+j] = AL[i*N+j] + (AA[i*N+j] * MaxA) + UA[i*N+j];					//Se realiza la suma de los resultados de todas las multiplicaciones anteriormente calculada, junto con la multiplicación del maximo de A por el resultado de AxA
		}
	}
//////////////////////////////
/////FIN PROCESAR////////////
/////////////////////////////

printf("Tiempo en segundos UN BUCLE %f \n", dwalltime() - timetick);					//Se termina de tomar el tiempo y se imprime por pantalla

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


printf("MinA: %f - MaxA: %f - PromA: %f",MinA,MaxA,PromA);								//Se imprime el resultado del mínimo, máximo y promedio de A
if (PRINT){																				//Mediante el valor de la variable PRINT se imprime o no todas las matrices utilizadas
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
 printf("\r\n");																		//Se libera la memoria utilizada por todas las matrices
 free(A);
 free(U);
 free(L);
 free(AL);
 free(AA);
 free(UA);
 free(R);

 return(0);

}
