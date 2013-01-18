
#include "stdio.h"
#include "malloc.h"
#include "math.h"

// define NULL as 0
#define NULL 0

// input int array
void inputIntArray(int * arr, int n, char arrName) {
	int i=0;
	for (i=0;i<n;i++){
		printf("Please input %c(%d): ", arrName, i+1);
		scanf("%d", (arr+i));
	}
}

// intput double array
void inputDoubleArray(double * arr, int n, char arrName) {
	int i=0;
	for (i=0;i<n;i++){
		printf("Please input %c(%d): ", arrName, i+1);
		scanf("%lf", (arr+i));
	}
}

// output int array
void outIntArray(int* arr, int n, char arrName) {
	int i=0;
	for (i=0;i<n;i++)
		printf("%c(%d): %d\n", arrName, i+1, arr[i]);
}

// output double array
void outDoubleArray(double* arr, int n, char arrName) {
	int i=0;
	for (i=0;i<n;i++)
		printf("%c(%d): %1.2lf\n", arrName, i+1, arr[i]);
}

// output formatted p(i) table
void outPiArrays(double **p, int **m, int n, int C) {
	int i=0,j=0;
	for (i=0;i<n;i++) {
		printf("\nn=%d ------------",i+1);
		printf("\nc");
		for (j=1;j<=C;j++)
			if (p[i][j]!=p[i][j-1])
				printf("\t%d", j);
		printf("\np",i+1);
		for (j=1;j<=C;j++)
			if (p[i][j]!=p[i][j-1])
				printf("\t%1.4lf",p[i][j]);
		printf("\nm");
		for (j=1;j<=C;j++)
			if (p[i][j]!=p[i][j-1])
				printf("\t%d",m[i][j]);
	}
}

// generate u(i) table basing on C and c(i)
void calculateUi(int *u, int *c, int C, int n){
	int i=0, sumC=0;
	for (i=0;i<n;i++)
		sumC += c[i];
	// calculate U(i)
	for (i=0;i<n;i++)
		u[i]=(C+c[i]-sumC)/c[i];
}

int main(){
	int C,n,i,j,k,cost;
	int *c=NULL,*u=NULL, **m=NULL, *M=NULL;
	double *r=NULL,**P=NULL, finalP, p;

	puts("Please input the number of modules (n)");
	scanf("%d", &n);

	if (n<0) {
		puts("n CANNOT be EQUAL or LESS then zero!");
		getchar(); getchar();
		return 0;
	}

	// try to allocate memory before program start
	c = (int *)malloc(sizeof(int)*n);	// c(i)
	r = (double *)malloc(sizeof(double)*n);	// r(i)
	u = (int *)malloc(sizeof(int)*n);	// u(i)
	M = (int *)malloc(sizeof(int)*n);	// stores final result of m(i)
	m = (int **)malloc(sizeof(int*)*n);	// stores m(i) for p(i,c)
	P = (double **)malloc(sizeof(double*)*n);	// p(i,c)
	// unable to allocate so many memory...
	if (c==NULL || r==NULL || u==NULL || M==NULL || m==NULL || P==NULL) { 
		puts("Out of memory. Press any key to exit..."); 
		getchar(); getchar();
		return 0;
	}

	inputIntArray(c,n,'c');
	inputDoubleArray(r,n,'r');

	puts("Please input the cost constraint C");
	scanf("%d", &C);

	puts("Your input is :");
	printf("C=%d, n=%d\n", C, n);
	outIntArray(c,n,'c');
	outDoubleArray(r,n,'r');

	puts("Now calculating, please wait..");

	// now calculate u(i)
	calculateUi(u,c,C,n);
	outIntArray(u,n,'u');

	// check ui
	for (i=0;i<n;i++)
		if (u[i]<0) {
			printf("Wrong C or c(i). Causes u(%d) be %d.  Unable to continue. Press any key to exit...\n", i, u[i]); 
			getchar(); getchar();
			return 0;
		}
		
	for (i = 0; i < n; i++) {
		P[i] = (double *)malloc(sizeof(double)*(C+1));
		m[i] = (int *)malloc(sizeof(int)*(C+1));
		for (j = 0; j <= C; j++) {
			P[i][j] = 0;	// init the performance as 0
			m[i][j] = 0;
		}
	}
	
	for (j = 1; j <= C; j++)
		for (i=0; i < n; i++)
			for (k = 0; k < u[i]; k++) {
				cost = c[i] * k;	// current module's cost
				if (cost <= j) {
					p = 1 - pow(1-r[i],k);	// performance
					if (i>0)
						p *= P[i-1][j-cost];	// if this is not the first device, we need to multiply the previous device's performance
					if (p > P[i][j]) {	// get a better answer
						P[i][j] = p;	// update p(i,j)
						m[i][j] = k;	// record m(i,j)
					}
				}
			}
	finalP = P[n-1][C];

	// seek the solution path m(i)
	for (i=n-1, cost=C;i>=0;i--) {
		M[i]= m[i][cost];
		cost -= c[i] * m[i][cost];
	}

	outPiArrays(P,m,n,C);
	printf("\n\nFinal Optimal solution:\n");
	outIntArray(M,n,'m');

	printf("\nMax. System Performance : %lf", finalP);
	
	// free resource
	for (i=0;i<n;i++) {
		free(P[i]);
		free(m[i]);
	}
	free(c); free(r); free(u); free(P); free(m); free(M);
	
	puts("\nPress ENTER to exit the program..");
	getchar(); getchar();

	return 0;
}