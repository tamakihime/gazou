#include <stdio.h>
#include <stdlib.h>
#include <math.h>

double **Cji; //配列サイズ：2*(3*sigma)+1

double gaussian(int y, int x, double sigma);

double **make_array(int ys, int xs) //2次元配列の作成
{
    int y;
    double *tmp = malloc(sizeof(double) * ys * xs);
    double **array = malloc(sizeof(double *) * ys);
    for (y=0;y<ys;y++)
    {
        array[y] = &tmp[y*xs];
    }
    return array;
}

int main(int argc, char* argv[])
{
    int y,x;
    double sigma;
    int w,size;
    
    if ( argc!=2 ) {
        fprintf(stderr, "error,usage:%s sigma(double)\n", argv[0]);
        exit( 1 );
    }
    
    sigma = atof(argv[1]);
    w = ceil(3*sigma); //切り上げ整数
    size = 2*w +1;
    printf("w=%d,size=%d\n",w,size);
    
    Cji = make_array(size, size);
    
    for(y=-w;y<=w;y++){
        for(x=-w;x<=w;x++){
            Cji[y+w][x+w] = gaussian(y,x,sigma);
        }
    }
   
    for(y=-w;y<=w;y++){
        for(x=-w;x<=w;x++){
            printf("%.4f ",Cji[y+w][x+w]);
        }
        printf("\n");
    }
    return 0;
}

double gaussian(int y, int x, double sigma){
    return exp((double)(-x*x-y*y)/(2.0*sigma*sigma))/((double)2.0*M_PI*sigma*sigma);
}
