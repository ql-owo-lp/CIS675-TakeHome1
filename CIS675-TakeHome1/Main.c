
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
void outIntArray(int* arr, int n, char arrName){
	int i=0;
	for (i=0;i<n;i++)
		printf("%c(%d): %d\n", arrName, i+1, arr[i]);
}

// output double array
void outDoubleArray(double* arr, int n, char arrName){
	int i=0;
	for (i=0;i<n;i++)
		printf("%c(%d): %1.2lf\n", arrName, i+1, arr[i]);
}

// output formatted p(i) table
void outPiArrays(double** p, int ** _c, int n, int *u) {
	int i=0,j=0;
	for (i=0;i<n;i++) {
		printf("\nm(%d)=", i+1);	// print header
		for (j=0;j<u[i];j++)
			printf("\t%d",j+1);
		printf("\np(%d)=", i+1);	// print p(i)
		for (j=0;j<u[i];j++)
			printf("\t%1.4lf", p[i][j]);
		printf("\nc(%d)=", i+1);	// print _c(i) (c prime)
		for (j=0;j<u[i];j++)
			printf("\t-%d", _c[i][j]);
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

// generate p(i) performance table
void calculatePi(double **p, int** _c, int *u, int *c, double *r, int n){
	int i=0,j=0;
	for (i=0;i<n;i++){	// "i" is i
		for (j=0;j<u[i];j++){	// "j+1" is m(i)
			p[i][j] = 1 - pow(1-r[i],j+1);
			_c[i][j] = c[i]*(j+1);
		}
	}
}

// pick the highest performance from the table
double pickPerformance(double **p, int **c, int *u, int n, int i, double performance, int cost){
	int j=0;
	double finalP = 0;
	double *_p = (double *)malloc(sizeof(double)*u[i]);		// current performance (possibility) list
	if (i<n-1){	// i is level
		for (j=0;j<u[i];j++)
			if (cost - c[i][j] >=0)	// if we still have money, down to the next level
				_p[j]=pickPerformance(p, c, u, n, i+1, performance*p[i][j], cost-c[i][j]);
			else	// if we can't afford it, as m(i)>=1, changed performance to 0 to indicate this path is closed
				_p[j]=0;
	}
	else {	// i==n, we will / should never get to i>n
		for (j=0;j<u[i];j++)
			if (cost - c[i][j] >=0)
				_p[j]=p[i][j]*performance;
			else
				_p[j]=0;
	}
	j = pickMaxValue(_p,u[i]);	// pick the biggest one
	finalP = _p[j];
	free(_p);
	return finalP;
}

// pick up the max value from a dynamic array, return the index
int pickMaxValue(double *p, int n){
	int i=0, max = 0;	// *p should has at least one value
	for (i=1;i<n;i++)
		if (p[i]>p[max]) max=i;
	return max;
}

int main(){
	int C=0,n=0,i=0;
	int *c=NULL,*m=NULL, *u=NULL, **_c=NULL;
	double *r=NULL,**p=NULL, finalP=0;

	puts("Please input the number of modules (n)");
	scanf("%d", &n);

	if (n<0) {
		puts("n CANNOT be EQUAL or LESS then zero!");
		getchar(); getchar();
		return 0;
	}

	// try to allocate memory before program start
	c = (int *)malloc(sizeof(int)*n); 
	_c = (int **)malloc(sizeof(int*)*n); 
	r = (double *)malloc(sizeof(double)*n); 
	m = (int *)malloc(sizeof(int)*n); 
	u = (int *)malloc(sizeof(int)*n);
	p = (double **)malloc(sizeof(double*)*n); 
	// unable to allocate so many memory...
	if (c==NULL || r==NULL || m==NULL || u==NULL || _c==NULL || p==NULL) { 
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

	// now calculate U(i)
	calculateUi(u,c,C,n);
	outIntArray(u,n,'u');

	// check ui
	for (i=0;i<n;i++)
		if (u[i]<0) {
			printf("Wrong C or c(i). Causes u(%d) be %d.  Unable to continue. Press any key to exit...\n", i, u[i]); 
			getchar(); getchar();
			return 0;
		}

	// allocate p* and _c*
	for (i=0;i<n;i++){
		p[i] = (double *)malloc(u[i] * sizeof(double));
		_c[i] = (int *)malloc(u[i] * sizeof(int));
	}

	calculatePi(p, _c, u, c, r, n);
	outPiArrays(p, _c, n, u);

	finalP = pickPerformance(p, _c, u, n, 0, 1, C);
	printf("\nMax Performance is : %lf", finalP);

	puts("\nPress ENTER to exit the program..");
	getchar(); getchar();
	
	// free resource
	for (i=0;i<n;i++){
		free(p[i]);
		free(_c[i]);
	}
	free(c); free(r); free(m); free(u);
	return 0;
}