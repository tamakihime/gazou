#include<stdio.h>
#include<math.h>
#include<stdlib.h>  //qsort関数
#include<float.h>   //DBL_MIN, DBL_MAX定数
#include"pgmlib.h"

double d_image[512][512];  //計算用double画像配列
double Cji[32][32];        //フィルタ係数（配列は大きめに用意）

void filtering( int filter_num, int n1, int n2 );
double sekiwa( int n, int y, int x, int w);
double sort_3x3( int n, int y, int x, int k);
void padding_border(int n1, int n2, int w);          //外周部の埋め合わせ処理

void set_Cji_sobel_v(int *w); //フィルタ係数をCjiに設定、内部でフィルタ幅(2*w+1)設定
void set_Cji_sobel_h(int *w);
void set_Cji_laplacian(int *w);
void set_Cji_sharpen(int *w);
void set_Cji_gaussian(double sigma, int *w);
void set_Cji_blur_v(int *w);
void set_Cji_blur_h(int *w);

int main(int argc, char* argv[])
{
    int filter_num;

    if ( argc!=4 ) {
        fprintf(stderr, "usage:%s input.pgm output.pgm filter_num\n", argv[0]);
        fprintf(stderr, "1: sobel_v\n");
        fprintf(stderr, "2: sobel_h\n");
        fprintf(stderr, "3: laplacian\n");
        fprintf(stderr, "4: sharpen\n");
        fprintf(stderr, "5: gaussian sigma=1.0\n");
        fprintf(stderr, "6: gaussian sigma=3.0\n");
        fprintf(stderr, "7: blur_v\n");
        fprintf(stderr, "8: blur_h\n");
        fprintf(stderr, "9: median\n");
        fprintf(stderr, "10: min\n");
        fprintf(stderr, "11: max\n");
        exit( 1 );
    }
    filter_num = atoi(argv[3]);
    load_image( 0, argv[1] );       //No.0に画像ファイル読み込み
    filtering( filter_num, 0, 1 );  //No.0をフィルタリングしてNo.1に
    save_image( 1, argv[2] );       //No.1を画像ファイルに書き出し
    return 0;
}

void filtering(int filter_num, int n1, int n2 )
{
    int y,x;
    double min, max;

    int w;
    int k;
    switch(filter_num) {
        case 1: set_Cji_sobel_v(&w); //フィルタ係数の設定、w=1
            break;
        case 2: set_Cji_sobel_h(&w); //w=1
            break;
        case 3: set_Cji_laplacian(&w); //w=1
            break;
        case 4: set_Cji_sharpen(&w); //w=1
            break;
        case 5: set_Cji_gaussian(1.0, &w); //w=3
            break;
        case 6: set_Cji_gaussian(3.0, &w); //w=9
            break;
        case 7: set_Cji_blur_v(&w); //w=5
            break;
        case 8: set_Cji_blur_h(&w); //w=5
            break;
        case 9: w=1;
            k=4; //median_3x3:[0,1,2,3,4,5,6,7,8]
            break;
        case 10: w=1;
            k=0; //min_3x3:[0,1,2,3,4,5,6,7,8]
            break;
        case 11: w=1;
            k=8; //max_3x3:[0,1,2,3,4,5,6,7,8]
            break;
    }

    //画像No.n2の画像サイズを入力
    width[n2]=width[n1];
    height[n2]=height[n1];

    //フィルタ窓が画像をはみ出さないように「w画素内側」の範囲を計算する（外周部を除く）
    //同時に、最小値・最大値を求め、正規化に使用する
    min = DBL_MAX; //double型の最大値
    max = DBL_MIN; //double型の最小値
    for(y=w;y<height[n2]-w;y++){
        for(x=w;x<width[n2]-w;x++){
            double v = (filter_num < 9) ? sekiwa( n1, y, x, w) : sort_3x3(n1, y, x, k);
            d_image[y][x] = v;
            if ( v < min ) min=v;
            if ( v > max ) max=v;
        }
    }
    printf("min=%f, max=%f\n", min, max);

    for(y=w;y<height[n2]-w;y++){
        for(x=w;x<width[n2]-w;x++){
            int g = 255*(d_image[y][x]-min)/(max-min); //正規化
            if ( g>255 ) g=255;
            else if ( g<0 ) g=0;
            image[n2][y][x] = g;
        }
    }

    padding_border(n1, n2, w); //
}

int uchar_compare(const void *a, const void *b)
{
    if(*(unsigned char *)a < *(unsigned char *)b) return -1;
    else if(*(unsigned char *)a > *(unsigned char *)b) return 1;
    else return 0;
}

double sort_3x3( int n, int y, int x, int k)
{
    unsigned char p[9];
    p[0] = image[n][y-1][x-1];
    p[1] = image[n][y-1][x  ];
    p[2] = image[n][y-1][x+1];
    p[3] = image[n][y  ][x-1];
    p[4] = image[n][y  ][x  ];
    p[5] = image[n][y  ][x+1];
    p[6] = image[n][y+1][x-1];
    p[7] = image[n][y+1][x  ];
    p[8] = image[n][y+1][x+1];

    qsort(p, 9, sizeof(unsigned char), uchar_compare);  //quick_sort関数

    return (double)p[k];  //k:min(0), median(4), max(8)
}

double sekiwa( int n, int y, int x, int w)
{
//画像No.nの(y,x)を中心にしてフィルタ係数との積和演算
    int i,j;
    double value=0;

    //フィルタ係数の中心(w,w)と画像の(y,x)を基準にする場合
    for(j=-w;j<=w;j++){     // yを中心にして-w..w
        for(i=-w;i<=w;i++){ // xを中心にして-w..w
            value += Cji[j+w][i+w] * image[n][y+j][x+i];
        }
    }
    //フィルタ係数をC[j][i]とした場合：左上(0,0)を基準にする
    //int size=2*w+1;
    //for(j=0;j<size;j++){
    //    for(i=0;i<size;i++){
    //        value += Cji[j][i] * image[n][y+j-w][x+i-w];
    //    }
    //}
    return value;
}

void padding_border(int n1, int n2, int w)
{
//外周部（w画素）の埋め合わせ：内側の画素値をコピー
    int y,x;

    for(y=w-1;y>=0;y--){                                     // 上端
        for(x=w;x<width[n2]-w;x++){                          // 左上と右上の角を除く
            image[n2][y][x] = image[n2][w][x];
        }
    }

    for(y=height[n2]-w-1;y<height[n2];y++){                 // 下端
        for(x=w;x<width[n2]-w;x++){                         // 左下と右下の角を除く
            image[n2][y][x] = image[n2][height[n2]-w-1][x];
        }
    }

    for(x=w-1;x>=0;x--){                                   // 左端
        for(y=0;y<height[n2]+1;y++){                         // 左上と左下の角を含める
            image[n2][y][x] = image[n2][y][w];
        }
    }

    for(x=width[n2]-w-1;x<width[n2];x++){                  // 右端
        for(y=0;y<height[n2]+1;y++){                         // 右上と右下の角を含める
            image[n2][y][x] = image[n2][y][width[n2]-w-1];
        }
    }
}

void set_Cji_sobel_v(int *w)
{
    double Cji_sobel_v[3][3] =
            {
                    {-1, 0, 1},
                    {-2, 0, 2},
                    {-1, 0, 1}
            };
    int j,i;
    (*w) = 1;
    int size=2*(*w)+1;
    for(j=0;j<size;j++){
        for(i=0;i<size;i++){
            Cji[j][i] = Cji_sobel_v[j][i];
        }
    }
}

void set_Cji_sobel_h(int *w)
{
    double Cji_sobel_h[3][3] =
            {
                    {-1,-2,-1},
                    {0, 0 , 0},
                    {1 ,2 , 1}
            };
    int j,i;
    (*w) = 1;
    int size=2*(*w)+1;
    for(j=0;j<size;j++){
        for(i=0;i<size;i++){
            Cji[j][i] = Cji_sobel_h[j][i];
        }
    }
}

void set_Cji_laplacian(int *w)
{
    double Cji_laplacian[3][3] =
            {
                    {0,-1,0},
                    {-1,4,-1},
                    {0,-1,0}
            };
    int j,i;
    (*w) = 1;
    int size=2*(*w)+1;
    for(j=0;j<size;j++){
        for(i=0;i<size;i++){
            Cji[j][i] = Cji_laplacian[j][i];
        }
    }
}

void set_Cji_sharpen(int *w)
{
    double Cji_sharpen[3][3] =
            {
                    {0,-1,0},
                    {-1,5,-1},
                    {0,-1,0}
            };
    int j,i;
    (*w) = 1;
    int size=2*(*w)+1;
    for(j=0;j<size;j++){
        for(i=0;i<size;i++){
            Cji[j][i] = Cji_sharpen[j][i];
        }
    }
}

void set_Cji_gaussian(double sigma, int *w)
{
    int j,i;
    (*w) = ceil(3*sigma); //切り上げ整数
    int size = 2*(*w)+1;
    for(j=-(*w);j<=(*w);j++){
        for(i=-(*w);i<=(*w);i++){
            Cji[j+(*w)][i+(*w)] = exp((double)(-1*(j*j+i*i)/(2*sigma*sigma)))/((double)2.0*M_PI*sigma*sigma);
        }
    }
}

void set_Cji_blur_v(int *w)
{
    double Cji_blur_v[9][9] =
            {
                    {0, 0, 0, 0, 1, 0, 0, 0, 0},
                    {0, 0, 0, 0, 1, 0, 0, 0, 0},
                    {0, 0, 0, 0, 1, 0, 0, 0, 0},
                    {0, 0, 0, 0, 1, 0, 0, 0, 0},
                    {0, 0, 0, 0, 1, 0, 0, 0, 0},
                    {0, 0, 0, 0, 1, 0, 0, 0, 0},
                    {0, 0, 0, 0, 1, 0, 0, 0, 0},
                    {0, 0, 0, 0, 1, 0, 0, 0, 0},
                    {0, 0, 0, 0, 1, 0, 0, 0, 0},
            };
    int j,i;
    (*w) = 4;
    int size=2*(*w)+1;
    for(j=0;j<size;j++){
        for(i=0;i<size;i++){
            Cji[j][i] = Cji_blur_v[j][i];
        }
    }
}

void set_Cji_blur_h(int *w)
{
    double Cji_blur_h[9][9] =
            {
                    {0, 0, 0, 0, 0, 0, 0, 0, 0},
                    {0, 0, 0, 0, 0, 0, 0, 0, 0},
                    {0, 0, 0, 0, 0, 0, 0, 0, 0},
                    {0, 0, 0, 0, 0, 0, 0, 0, 0},
                    {1, 1, 1, 1, 1, 1, 1, 1, 1},
                    {0, 0, 0, 0, 0, 0, 0, 0, 0},
                    {0, 0, 0, 0, 0, 0, 0, 0, 0},
                    {0, 0, 0, 0, 0, 0, 0, 0, 0},
                    {0, 0, 0, 0, 0, 0, 0, 0, 0},
            };
    int j,i;
    (*w) = 4;
    int size=2*(*w)+1;
    for(j=0;j<size;j++){
        for(i=0;i<size;i++){
            Cji[j][i] = Cji_blur_h[j][i];
        }
    }
}
