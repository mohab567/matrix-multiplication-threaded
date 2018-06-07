#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/time.h>
#define min(a,b) (((a)<(b))?(a):(b))
/*
written by Mohab CSED19
multithreaded matrix multiplier
x * y = z
z1 is from method 1
z2 is from method 2
colx/y,rowx/y are sizes for x,y
a is path of x file
b is path of y file
c1 is out file from method 1
c2 is out file from method 2
*/
int x[1024][1024],y[1024][1024],z1[1024][1024],z2[1024][1024],z3[1024][1024],z4[1024][1024];
int colx,rowx,coly;
int rowy;
char *a,*b,c1[200],c2[200];
int sumA [1024];
/*
to parse file path and save it into arr
*/
void delay(unsigned int mseconds)
{
    clock_t goal = mseconds + clock();
    while (goal > clock());
}
int* parse(char * path, int arr[1024][1024]) {
        static int size[2];
        FILE *fp;
        char str[10240];
        char *e;
        /* opening file for reading */
        fp = fopen(path, "r");
        if(fp == NULL)
        {
            perror("Error opening file");
            exit(0);
        }
        fgets (str,10240, fp);
        if (strcmp(strtok(str," ="),"row")){
            printf("error in input files");
            exit(0);
            }
        size[0]=strtol(strtok(NULL," ="),&e,10);
        if (strcmp(strtok(NULL," ="),"col")){
            printf("error in input files");
            exit(0);}
        size[1]=strtol(strtok(NULL," ="),&e,10);
        if(size[0]>1000||size[0]>1000)
        {
            printf("error max number of col and rows is 1000");
            exit(0);
        }
        if(size[0]<=0||size[0]<=0)
        {
            printf("error least size of matrix is 1x1");
            exit(0);
        }
        int j=0;
        while( fgets (str,10240, fp)!=NULL && j<size[0] )
        {
            int i=0;
            arr[j][i]=strtol(strtok(str,"\t"),&e,10);
            for(i=1;i<size[1];i++){
                arr[j][i]=strtol(strtok(NULL,"\t"),&e,10);
            }
            j++;
        }
        fclose(fp);
    return size;
}
int main(int argc, char *argv[]){
    if(argc==4){
        a=argv[1];
        b=argv[2];
        strcpy(c1,argv[3]);
        strcat(c1,"_1");
        strcpy(c2,argv[3]);
        strcat(c2,"_2");
    }
    else if(argc==1){
        a="a.txt";
        b="b.txt";
        strcpy(c1,"c_1.out");
        strcpy(c2,"c_2.out");
    }else{
    printf("error wrong number of arguments");
    return 0;}
  /*  int *t;
    t=parse(a,x);
    rowx=*t;
    colx=*(t+1);
    t=parse(b,y);
    rowy=*t;
    coly=*(t+1);
    */
    srand(time(NULL));   // should only be called once
    int m,n,t=100;
    colx=t;
    coly=t;
    rowx=t;
    rowy=t;

    for(m=0;m<t;m++){
        for(n=0;n<t;n++){
          x[m][n]= (int)rand();
           y[m][n]= (int)rand();
    }
    }
    //calculate time
    struct timeval tvalBefore, tvalAfter;
    gettimeofday (&tvalBefore, NULL);
   // method1(x,y,rowx,colx,rowy,coly,z1);
    gettimeofday (&tvalAfter, NULL);
    printf("Microseconds taken by method one: %ld\n", ((tvalAfter.tv_sec - tvalBefore.tv_sec)*1000000L
           +tvalAfter.tv_usec) - tvalBefore.tv_usec);
   // delay(500);
    /*
    gettimeofday(&start, NULL);
    method2(x,y,rowx,colx,rowy,coly,z2);
    gettimeofday(&stop, NULL);
    printf("Microseconds taken by method two: %lu\n", stop.tv_usec - start.tv_usec);
    */
    int blockSize = 4 ;
   gettimeofday (&tvalBefore, NULL);
    calBlocking(x,y,rowx,blockSize,rowy,coly,z3);
    gettimeofday (&tvalAfter, NULL);
    printf("Microseconds taken by enhanced: %ld\n", ((tvalAfter.tv_sec - tvalBefore.tv_sec)*1000000L
           +tvalAfter.tv_usec) - tvalBefore.tv_usec);

    gettimeofday (&tvalBefore, NULL);
   // calen(x,y,rowx,colx,rowy,coly,z3);
    gettimeofday (&tvalAfter, NULL);
    printf("Microseconds taken by enhanced: %ld\n", ((tvalAfter.tv_sec - tvalBefore.tv_sec)*1000000L
           +tvalAfter.tv_usec) - tvalBefore.tv_usec);

    gettimeofday (&tvalBefore, NULL);
    //calrowwise(x,y,rowx,colx,rowy,coly,z4);
    gettimeofday (&tvalAfter, NULL);
    printf("Microseconds taken by normal: %ld\n", ((tvalAfter.tv_sec - tvalBefore.tv_sec)*1000000L
           +tvalAfter.tv_usec) - tvalBefore.tv_usec);


    writeOut(z1,c1);
    writeOut(z2,c2);
    return 0;
}
/*
to calculate a row pargs in first method
*/
void *calculateRow(void *pargs){
int j,k;
int i=(int)pargs;
for(j=0;j<coly;j++){
        for(k=0;k<colx;k++){
            z1[i][j]+=x[i][k]*y[k][j];
        }
    }
}
/*
method 1
x*y = z
rowx/y,colx,y are sizes of x,y
*/
void method1(int x[1024][1024],int y[1024][1024],int rowx,int colx,int rowy,int coly,int z[1024][1024]){
pthread_t threads[1024];
if(colx!=rowy){
    printf("error the two matrixes cannot be multiplied");
    exit(0);}
int i;
for(i=0;i<rowx;i++){
    if(pthread_create(&threads[i], NULL, &calculateRow, (void *)i)) {
        printf("Error creating thread maybe due to big number of threads");
        exit(0);}}
printf("Number of threads created by method one = %d\n",i);
int s=0;
while(s<rowx){
if(pthread_join(threads[s], NULL)) {
fprintf(stderr, "Error joining thread\n");
exit(0);}
s++;}
}
/*
to calculate element
whose place in z[pargs/1000][pargs%1000]
*/
void *calculateElement(void *pargs){
int k;
int t=(int)pargs;
int i=t/1000,j=t%1000;
for(k=0;k<colx;k++){
            z2[i][j]+=x[i][k]*y[k][j];
        }
}
/*
method 2
x*y = z
rowx/y,colx,y are sizes of x,y
*/
void method2(int x[1024][1024],int y[1024][1024],int rowx,int colx,int rowy,int coly,int z[1024][1024]){
pthread_t threads[1001][1001];
if(colx!=rowy){
    printf("error the two matrixes cannot be multiplied");
    exit(0);
}
int i,j;
for(i=0;i<rowx;i++){
    for(j=0;j<coly;j++){
        int t=i*1000+j;
        if(pthread_create(&threads[i][j], NULL, &calculateElement, (void *)t)) {
printf("Error creating thread maybe due to big number of threads");
exit(0);
}
    }
}
printf("Number of threads created by method two = %d\n",i*j);
int m=0,n=0;
for(m=0;m<rowx;m++){
    for(n=0;n<coly;n++){
      if(pthread_join(threads[m][n], NULL)) {
fprintf(stderr, "Error joining thread\n");
exit(0);
}
    }
}
}
void calrowwise (int x[1024][1024],int y[1024][1024],int rowx,int colx,int rowy,int coly,int z[1024][1024]){
   int sum =0,c,d,k;
    for (c = 0; c < rowx; c++) {
      for (d = 0; d < coly; d++) {
        for (k = 0; k < rowy; k++) {
          sum += x[c][k]*y[k][d];
        }

        z[c][d] = sum;
        sum = 0;
      }
    }

}
void calen (int x[1024][1024],int y[1024][1024],int rowx,int colx,int rowy,int coly,int z[1024][1024]){
   int c,d,k,s=0,i;
    for (c = 0; c < rowx; c++) {
      for (d = 0; d < coly; d++) {
        for (k = 0; k < rowy; k++) {
          sumA[k] = x[c][k]*y[k][d];
        }
 for(i=0;i<colx;i++)
    s+=sumA[i];
        z[c][d] = s;
        s = 0;
      }
    }

}
void calBlocking (int a[1024][1024],int b[1024][1024],int n,int BlockSize,int rowy,int coly,int c[1024][1024]){
	c[0][0]=0;
	for( int i1=0;i1<(n/BlockSize);++i1)
	{
		for(int j1=0;j1<(n/BlockSize);++j1)
		{
			for(int k1=0;k1<(n/BlockSize);++k1)
			{
				for(int i=i1=0;i<min(i1+BlockSize-1,n);++i)
				{
					for(int j=j1=0;j<min(j1+BlockSize-1,n);++j)
					{
						for(int k=k1;k<min(k1+BlockSize-1,n);++k)
						{
							c[i][j] = c[i][j] + a[i][k] * b[k][j] ;
						}
					}
				}
			}
		}
           }
}
/*
to print z[][] in file its path is c
*/
void writeOut(int z[1024][1024],char *c){
    FILE *f = fopen(c, "w");
    if (f == NULL)
    {
        printf("Error opening file!\n");
        exit(1);
    }
int m=0,n=0;
for(m=0;m<rowx;m++){
    for(n=0;n<coly;n++){
      fprintf(f, "%d\t",z[m][n]);
    }
    fprintf(f, "\n");
}
    fclose(f);
}
