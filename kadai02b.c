#include<stdio.h>
#include<math.h>
#include"pgmlib.h"

void trans( int n1, int n2, int trans_n ); //画像n1をn2に解像度変換する

void half_w( int n1, int n2); //解像度1/2(幅)
void half_h( int n1, int n2); //解像度1/2(高さ)
void half_wh( int n1, int n2); //解像度1/2(幅・高さ)
void double_w( int n1, int n2); //解像度2(幅)
void double_h( int n1, int n2); //解像度2(高さ)
void double_wh( int n1, int n2); //解像度2(幅・高さ)

char* trans_list[] = {
/* 0 */    "",
/* 1 */    "half_w",
/* 2 */    "half_h",
/* 3 */    "half_wh",
/* 4 */    "double_w",
/* 5 */    "double_h",
/* 6 */    "double_wh",
};

int main(int argc, char* argv[])
{
    int trans_n;
    
    if (argc != 4) {
        int i;
        fprintf(stderr, "usage: %s input.pgm output.pgm trans_number\n", argv[0]);
        for(i = 1; i<sizeof(trans_list)/sizeof(trans_list[0]); i++){
            fprintf(stderr, "%d: %s\n",i,trans_list[i]);
        }
        exit(1);
    }
    
    trans_n = atoi(argv[3]);
    if(trans_n<sizeof(trans_list)/sizeof(trans_list[0]))
        printf("解像度変換%d: %s\n", trans_n, trans_list[trans_n]);
    else{
        fprintf(stderr, "error: trans_number=1..%lu\n", (sizeof(trans_list)/sizeof(trans_list[0]))-1 );
        exit(1);
    }
    
    load_image( 0, argv[1] );
    
    trans( 0, 1, trans_n);  //画像0を1に解像度変換
    
    save_image( 1, argv[2] );
    
    return 0;
}

void trans( int n1, int n2, int trans_n ) //画像n1をn2に解像度変換する
{
    switch(trans_n){
        case 1:
            half_w(n1, n2);
            break;
        case 2:
            half_h(n1, n2);
            break;
        case 3:
            half_wh(n1, n2);
            break;
        case 4:
            double_w(n1, n2);
            break;
        case 5:
            double_h(n1, n2);
            break;
        case 6:
            double_wh(n1, n2);
            break;
    }
}

void half_w( int n1, int n2)
{
    int x,y;
    width[n2] = width[n1]/2;
    height[n2] = height[n1];
    
    for(y=0;y<height[n2];y++){
        for(x=0;x<width[n2];x++){
            image[n2][y][x] = (image[n1][y][x*2]+image[n1][y][x*2+1])/2;
        }
    }
}

void half_h( int n1, int n2)
{
    int x,y;
    width[n2] = width[n1];
    height[n2] = height[n1]/2;
    
    for(y=0;y<height[n2];y++){
        for(x=0;x<width[n2];x++){
            image[n2][y][x] = (image[n1][y*2][x]+image[n1][y*2+1][x])/2;
        }
    }
}

void half_wh( int n1, int n2)
{
    int x,y;
    width[n2] = width[n1]/2;
    height[n2] = height[n1]/2;
    
    for(y=0;y<height[n2];y++){
        for(x=0;x<width[n2];x++){
            image[n2][y][x] = 
             ( image[n1][2*y][x*2]+image[n1][2*y][x*2+1]
             + image[n1][y*2+1][2*x+1]+image[n1][y*2+1][2*x])/4; //
        }
    }
}

void double_w( int n1, int n2)
{
    int x,y;
    width[n2] = width[n1]*2;
    height[n2] = height[n1];
    
    for(y=0;y<height[n2];y++){
        for(x=0;x<width[n2];x++){
            image[n2][y][x] = image[n1][y][x/2];
        }
    }
}

void double_h( int n1, int n2)
{
    int x,y;
    width[n2] = width[n1];
    height[n2] = height[n1]*2;
    
    for(y=0;y<height[n2];y++){
        for(x=0;x<width[n2];x++){
            image[n2][y][x] = image[n1][y/2][x];
        }
    }
}

void double_wh( int n1, int n2)
{
    int x,y;
    width[n2] = width[n1]*2;
    height[n2] = height[n1]*2;
    
    for(y=0;y<height[n2];y++){
        for(x=0;x<width[n2];x++){
            image[n2][y][x] =image[n1][y/2][x/2]; //
        }
    }
}
