#include<stdio.h>
#include<math.h>
#include"pgmlib.h"

unsigned char get_fmax( int n );        //最大の画素値を求める
unsigned char get_fmin( int n );        //最小の画素値を求める
void trans( int n1, int n2, int trans_n ); //画像n1をn2に階調変換する

char* trans_list[] = {
/* 0 */    "",
/* 1 */    "f & 0xc0",
/* 2 */    "f & 0xc0 + 0x20",
/* 3 */    "f*2",
/* 4 */    "f/2",
/* 5 */    "f + 32",
/* 6 */    "255*(f-fmin)/(fmax-fmin)",
/* 7 */    "255-f",
/* 8 */    "gamma=2.0",
/* 9 */    "kadai02a",
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
        printf("階調変換%d: %s\n", trans_n, trans_list[trans_n]);
    else{
        fprintf(stderr, "error: trans_number=1..%lu\n", (sizeof(trans_list)/sizeof(trans_list[0]))-1 );
        exit(1);
    }
    
    load_image( 0, argv[1] );
    
    trans( 0, 1, trans_n);  //画像0を1に階調変換
    
    save_image( 1, argv[2] );
    
    return 0;
}

unsigned char get_fmax( int n ) //最大の画素値を求める
{
    int x,y;
    unsigned char fmax=image[n][0][0]; //仮の最大値（初期値），fmax=0でもよい（取り得る最小値）
    
    
    
    
    
    
    return fmax;
}

unsigned char get_fmin( int n ) //最小の画素値を求める
{
    int x,y;
    unsigned char fmin=image[n][0][0]; //仮の最小値（初期値）, fmin=255でもよい（取り得る最大値）
    for(y=0;y<height[n];y++){
        for(x=0;x<width[n];x++){
            unsigned char g = image[n][y][x];
            if(fmin>g) fmin=g;
        }
    }
    return fmin;
}

void trans( int n1, int n2, int trans_n ) //画像n1をn2に階調変換する
{
    int x,y;
    
    int fmin = get_fmin(n1);
    int fmax = get_fmax(n1);
    printf("fmin=%d, fmax=%d\n", fmin, fmax);
    
    width[n2]  = width[n1];
    height[n2] = height[n1];
    
    for(y=0;y<height[n1];y++){
        for(x=0;x<width[n1];x++){
            int f = image[n1][y][x];
            int g;//image[n2][y][x];
            switch(trans_n){
                case 1:
                    g = f & 0xc0; //4階調+32, (f>>6)<<6, (f/64)*64
                    break;
                case 2:
                    g = (f & 0xc0) + 0x20; //4階調+32, (f/64)*64+32
                    break;
                case 3:
                    g = f*2; //2倍
                    break;
                case 4:
                    g = f/2; //0.5倍
                    break;
                case 5:
                    g = f+32;                        //シフト補正
                    break;
                case 6:
                    g = (255*(f-fmin))/(fmax-fmin);    //[fmin..fmax]を[0..255]に線形変換
                    break;
                case 7:
                    g = 255 - f;                     //濃淡反転
                    break;
                case 8:{
                    double gamma=2.0;
                    g = 255.0*pow(f/255.0, 1/gamma); //ガンマ補正
                    break;
                }
                case 9:                              //kadai02a
                    if (f<64)       g =2*f;
                    else if (f<192) g =f+64;
                    else            g =255;
                    break;
                default:
                    ;
            }
            
            // 画素値gを[0..255]に収める処理
            if ( g<0 )        g =0;
            else if ( g>255 ) g = 255;
            image[n2][y][x] = g;
        }
    }
}
