// kadai05-dct.c
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<float.h>
#include"pgmlib.h"

#define DCT_MAX 1024
#define M 8
#define N 8

void dct_encode( int n );
void dct_decode( int n );
void create_dct_image( int n );
void DCT(void);
void IDCT(void);

double pdata[DCT_MAX][DCT_MAX];
double f[N][M];
double g[N][M];
int dct_filter[N][M] = {
{1,1,0,0,0,0,0,0},
{1,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0}
};

int main( int argc, char* argv[] )
{

    load_image( 0, argv[1] );
    dct_encode( 0 );

    height[1]=height[0];
    width[1]=width[0];
    create_dct_image( 1 );
    save_image( 1, argv[2] );

    height[2]=height[0];
    width[2]=width[0];
    dct_decode( 2 );
    save_image( 2, argv[3] );

    return 0;
}

void dct_encode(int n)
{
    int ys,xs;
    int y,x;

    for(ys=0; ys<height[n]; ys+=N)
        for(xs=0; xs<width[n]; xs+=M){
            for(y=0; y<N; y++)
                for(x=0; x<M; x++)
                    f[y][x]=(double)image[n][ys+y][xs+x];
            DCT();
            for(y=0; y<N; y++)
                for(x=0; x<M; x++)
                    pdata[ys+y][xs+x]=g[y][x];
        }
}

void dct_decode(int n)
{
    int ys,xs;
    int y,x;

    for(ys=0; ys<height[n]; ys+=N)
        for(xs=0; xs<width[n]; xs+=M){
            for(y=0; y<N; y++)
                for(x=0; x<M; x++)
                    g[y][x]=pdata[ys+y][xs+x];
            IDCT();
            for(y=0; y<N; y++)
                for(x=0; x<M; x++){
                    int g = f[y][x];
                    if ( g>255 ) g=255;
                    else if ( g<0 ) g=0;
                    image[n][ys+y][xs+x] = (unsigned char)g;
                }
        }
}


void DCT()
{
    int x,y,u,v;
    double cv, cu, dd;

    for(v=0; v<N; v++){
        cv = (v==0) ? ... : ... ;
        for(u=0; u<M; u++){
            cu = (u==0) ? ... : ... ;
            dd=0;
            for(y=0; y<N; y++)
                for(x=0; x<M; x++)
                    dd += f[y][x] * cos((2*x+1) * u * M_PI/ ... ) * cos((2*y+1) * v * M_PI/ ... );
            g[v][u]=dd*cu*cv;
        }
    }
}

void IDCT()
{
    int x,y,u,v;
    double cv, cu, dd;

    for(y=0; y<N; y++)
        for(x=0; x<M; x++){
            dd=0;
            for(v=0; v<N; v++){
                cv = (v==0) ? ... : ... ;
                for(u=0; u<M; u++){
                    cu = (u==0) ? ... : ... ;
                    dd += g[v][u] * cos( ... ) * cos( ... ) * cu * cv * dct_filter[v][u];
                }
            }
            f[y][x]=dd;
        }
}


void create_dct_image( int n )
{
    int i,j,x,y;
    double fmin,fmax;

    printf("DCTの画像を作成します．\n");

    fmin=DBL_MAX; fmax=-DBL_MAX;

    for(y=0; y<height[n]; y++){
        for(x=0; x<width[n]; x++){
            if ( pdata[y][x] < fmin )
                fmin = pdata[y][x];
            if ( pdata[y][x] > fmax )
                fmax = pdata[y][x];
        }
    }

    printf("fmin=%f, fmax=%f\n", fmin, fmax);

    for(y=0; y<height[n]; y++){
        for(x=0; x<width[n]; x++){
            int g = (int)( 255.0 * (pdata[y][x] - fmin ) / ( fmax - fmin ) );
            if ( g < 0 ) g = 0;
            if ( g > 255 ) g = 255;
            image[n][y][x] = g;
        }
    }
}
