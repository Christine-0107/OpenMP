#include <iostream>
#include <time.h>
#include <sys/time.h>
#include <omp.h>
using namespace std;

const int NUM_THREADS=4;

void serialGauss(float** matrix,int N)
{
    for (int k = 0; k < N; k++)
    {
        float  pivot = matrix[k][k];
        for (int j = k; j < N; j++)
        {
            matrix[k][j] = matrix[k][j] / pivot;
        }
        for (int i = k + 1; i < N; i++)
        {
            float temp = matrix[i][k];
            for (int j = k + 1; j < N; j++)
            {
                matrix[i][j] = matrix[i][j] - temp * matrix[k][j];
            }
            matrix[i][k] = 0;
        }
    }
}

void normalOmpGauss(float** matrix,int N)
{
    #pragma omp parallel num_threads(NUM_THREADS)
    for(int k=0;k<N;k++)
    {
        #pragma omp single
        {
            for(int j=k;j<N;j++)
            {
                matrix[k][j]=matrix[k][j]/matrix[k][k];
            }
        }
        #pragma omp for
        for(int i=k+1;i<N;i++)
        {
            for(int j=k+1;j<N;j++)
            {
                matrix[i][j]=matrix[i][j]-matrix[i][k]*matrix[k][j];
            }
            matrix[i][k]=0.0;
        }
    }
}

void staticOmpGauss(float** matrix,int N)
{
    #pragma omp parallel num_threads(NUM_THREADS)
    for(int k=0;k<N;k++)
    {
        #pragma omp single
        {
                for(int j=k;j<N;j++)
                {
                        matrix[k][j]=matrix[k][j]/matrix[k][k];
                }
        }
        #pragma omp for schedule(static,1)
        for(int i=k+1;i<N;i++)
        {
                for(int j=k+1;j<N;j++)
                {
                        matrix[i][j]=matrix[i][j]-matrix[i][k]*matrix[k][j];
                }
                matrix[i][k]=0.0;
        }
    }
}

void dynamicOmpGauss(float** matrix,int N)
{
    #pragma omp parallel num_threads(NUM_THREADS)
    for(int k=0;k<N;k++)
    {
        #pragma omp single
        {
                for(int j=k;j<N;j++)
                {
                        matrix[k][j]=matrix[k][j]/matrix[k][k];
                }
        }
        #pragma omp for schedule(dynamic,(N-k-1)/NUM_THREADS)
        for(int i=k+1;i<N;i++)
        {
                for(int j=k+1;j<N;j++)
                {
                        matrix[i][j]=matrix[i][j]-matrix[i][k]*matrix[k][j];
                }
                matrix[i][k]=0.0;
        }
    }
}

void guidedOmpGauss(float** matrix,int N)
{
    #pragma omp parallel num_threads(NUM_THREADS)
    for(int k=0;k<N;k++)
    {
        #pragma omp single
        {
                for(int j=k;j<N;j++)
                {
                        matrix[k][j]=matrix[k][j]/matrix[k][k];
                }
        }
        #pragma omp for schedule(guided,1)
        for(int i=k+1;i<N;i++)
        {
                for(int j=k+1;j<N;j++)
                {
                        matrix[i][j]=matrix[i][j]-matrix[i][k]*matrix[k][j];
                }
                matrix[i][k]=0.0;
        }
    }
}

void columnOmpGauss(float** matrix,int N)
{
    #pragma omp parallel num_threads(NUM_THREADS)
    for(int k=0;k<N;k++)
    {
        #pragma omp single
        {
            for(int j=k;j<N;j++)
            {
                matrix[k][j]=matrix[k][j]/matrix[k][k];
            }
        }
        
        for(int i=k+1;i<N;i++)
        {
            #pragma omp for
            for(int j=k+1;j<N;j++)
            {
                matrix[i][j]=matrix[i][j]-matrix[i][k]*matrix[k][j];
            }
            matrix[i][k]=0.0;
        }
    }
}


void setMatrix(float** matrix,int N)
{
    srand((unsigned)time(0));
    for(int i=0;i<N;i++){
        for(int j=0;j<i;j++){
            matrix[i][j]=0.0;
        }
        matrix[i][i]=1.0;
        for(int j=i+1;j<N;j++){
            matrix[i][j]=rand()%100;
        }
    }
}

int main()
{
    int N[10]={8,32,128,256,512,1024,2048,3000,4096,5200};
    for(int p=0;p<10;p++)
    {
    	float** matrix = new float* [N[p]];
        for (int i = 0; i < N[p]; i++)
        {
            matrix[i] = new float[N[p]];
        }
        
        struct timeval start;
        struct timeval end;
        unsigned long diff;

        setMatrix(matrix,N[p]);
        gettimeofday(&start,NULL);
        for(int count=0;count<3;count++)
        {
            serialGauss(matrix,N[p]);
        }
        gettimeofday(&end,NULL);
        diff=1000000*(end.tv_sec-start.tv_sec)+end.tv_usec-start.tv_usec;
        cout<<"??????"<<endl;
        cout<<"N: "<<N[p]<<" time: "<<diff/3<<"us"<<endl;

        setMatrix(matrix,N[p]);
        gettimeofday(&start,NULL);
        for(int count=0;count<3;count++)
        {
            normalOmpGauss(matrix,N[p]);
        }
        gettimeofday(&end,NULL);
        diff=1000000*(end.tv_sec-start.tv_sec)+end.tv_usec-start.tv_usec;
        cout<<"????????????omp???"<<endl;
        cout<<"NUM_THREADS: "<<NUM_THREADS<<" N: "<<N[p]<<" time: "<<diff/3<<"us"<<endl;

        setMatrix(matrix,N[p]);
        gettimeofday(&start,NULL);
        for(int count=0;count<3;count++)
        {
            staticOmpGauss(matrix,N[p]);
        }
        gettimeofday(&end,NULL);
        diff=1000000*(end.tv_sec-start.tv_sec)+end.tv_usec-start.tv_usec;
        cout<<"??????????????????omp???"<<endl;
        cout<<"NUM_THREADS: "<<NUM_THREADS<<" N: "<<N[p]<<" time: "<<diff/3<<"us"<<endl;
        
        setMatrix(matrix,N[p]);
        gettimeofday(&start,NULL);
        for(int count=0;count<3;count++)
        {
            dynamicOmpGauss(matrix,N[p]);
        }
        gettimeofday(&end,NULL);
        diff=1000000*(end.tv_sec-start.tv_sec)+end.tv_usec-start.tv_usec;
        cout<<"??????omp???"<<endl;
        cout<<"NUM_THREADS: "<<NUM_THREADS<<" N: "<<N[p]<<" time: "<<diff/3<<"us"<<endl;
        
        /*setMatrix(matrix,N[p]);
        gettimeofday(&start,NULL);
        for(int count=0;count<3;count++)
        {
            columnOmpGauss(matrix,N[p]);
        }
        gettimeofday(&end,NULL);
        diff=1000000*(end.tv_sec-start.tv_sec)+end.tv_usec-start.tv_usec;
        cout<<"column_omp???"<<endl;
        cout<<"NUM_THREADS: "<<NUM_THREADS<<" N: "<<N[p]<<" time: "<<diff/3<<"us"<<endl;*/
        
        
        setMatrix(matrix,N[p]);
        gettimeofday(&start,NULL);
        for(int count=0;count<3;count++)
        {
            guidedOmpGauss(matrix,N[p]);
        }
        gettimeofday(&end,NULL);
        diff=1000000*(end.tv_sec-start.tv_sec)+end.tv_usec-start.tv_usec;
        cout<<"Guided_omp???"<<endl;
        cout<<"NUM_THREADS: "<<NUM_THREADS<<" N: "<<N[p]<<" time: "<<diff/3<<"us"<<endl;
        cout<<endl;
        
        
    }
    return 0;
}
