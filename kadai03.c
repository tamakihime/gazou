#include<stdio.h>
#include<math.h>
#include"pgmlib.h"

long int hist[256]; //ヒストグラム用１次元配列

void make_histgram( int n );                //画像nのヒストグラム作成
void make_histgram_image( int h, int n );   //ヒストグラムを画像化(幅256×高さh画素)して、画像nに記録する
void binarize( int n1, int n2, int bin_n ); //画像n1を二値化する
int get_fmin( int n );
int get_fmax( int n );
int get_threshold_median( int n );         //中央値
int get_threshold_otsu( int n );           //大津

int main( int argc, char* argv[] )
{
    int bin_n;
    if ( argc!=5 ) {
        fprintf(stderr, "usage:%s input.pgm histgram.pgm binary.pgm n\n", argv[0]);
        exit( 1 );
    }
    load_image( 0, argv[1] );      //ファイル読み込み → 画像0
    make_histgram( 0 );            //画像nのヒストグラム作成
    make_histgram_image( 256, 2 ); //ヒストグラムの画像化
    save_image( 2, argv[2] );      //ヒストグラム保存
    bin_n = atoi(argv[4]);         //1:中央値   2:大津
    binarize( 0, 1, bin_n );       //画像0を二値化して，画像1へ
    save_image( 1, argv[3] );      //画像1をファイル保存
    return 0;
}

int get_fmin( int n ) //最小画素値
{
    int x,y;
    int fmin=image[n][0][0];
    for(y=0;y<height[n];y++){
        for(x=0;x<width[n];x++){
            int g = image[n][y][x];
            if(fmin>g) fmin=g;
        }
    }

    return fmin;
}

int get_fmax( int n ) //最大画素値
{
    int x,y;
    int fmax=image[n][0][0];
    for(y=0;y<height[n];y++){
        for(x=0;x<width[n];x++){
            int g = image[n][y][x];
            if(fmax<g) fmax=g;
        }
    }
    return fmax;
}

void binarize( int n1, int n2, int bin_n ) //画像n1を二値化して 画像n2へ
{
    int t;  //二値化のしきい値 
    int x,y;
    
    switch(bin_n){
        case 1: t = get_threshold_median(n1); //中央値
                break;
        case 2: t = get_threshold_otsu(n1);   //大津
                break;
        default:break;
    }
    printf("しきい値=%d\n", t);
    
    width[n2]  = width[n1];
    height[n2] = height[n1];
    for(y=0;y<height[n1];y++){
        for(x=0;x<width[n1];x++){
            image[n2][y][x] = ( image[n1][y][x] < t ) ? 255 : 0 ;
                                                      //黒 else 白
        }
    }
}

int get_threshold_median( int n )
{
    int t; //しきい値
    int f; //画素値（ヒストグラムx軸）
    
    for(t=1;t<256;t++){      //tはクラス2に含めるため，t=0を除く
        int n1;              //クラス1の画素数
        int n2;              //クラス2の画素数
        
        n1 = 0;
        for(f=0;f<t;f++){    //クラス1：f=[0..t-1]の範囲
            n1 += hist[f];
        }
        
        n2 = 0;
        for(f=t;f<256;f++){   //クラス2：f=[t..255]の範囲
            n2 += hist[f];
        }
        
        printf("%d: %d, %d\n",t,n1,n2);      //確認用：必要なければ，コメントアウトする
        
        if ( n1 > (width[n]*height[n]/2) ) { //クラス1が「総画素数の半分」を超えたら、forループを抜ける
            break;                           //中央値はクラス1に含まれる
        }
    }
    
    return t-1;                              //中央値をクラス2に含めるため、しきい値は1減算
}

double calc_a( int t ) //式aの計算
{
    int f;             //画素値（ヒストグラムx軸）
    double n1 = 0.0;   //クラス1の画素数          ※割り算のためdoubleにする
    double n2 = 0.0;   //クラス2の画素数
    double m1 = 0.0;   //クラス1の平均画素値
    double m2 = 0.0;   //クラス2の平均画素値
    
    //n1,m1
    for(f = 0; f < t; f++){
        n1+=hist[f];
        m1+=hist[f]*f;
    }
    m1= m1/n1;
    
    //n2,m2
    for(f = t; f < 256; f++){
        n2 +=hist[f];
        m2 +=hist[f]*f;
    }
    m2=m2/ n2;
    
    return n1*n2*(m1-m2)*(m1-m2);           //式a
}

int get_threshold_otsu( int n ){
    int    t;             //しきい値
    int    max_t = 0;     //aが最大の時のしきい値
    double max_a = 0.0;   //aの最大値
    
    for(t = 1; t < 256; t++){
        double a = calc_a(t);
        if(a > max_a){
            max_a=a;           //max_aの更新
            max_t=t;           //max_tの更新
        }
    }
    
    return max_t;
}

void make_histgram( int n ) //画像nのヒストグラム作成
{
    int f,x,y;
    
    for(f=0;f<256;f++)            //画素値(ヒストグラムx軸)
        hist[f] = 0;            //ヒストグラムの初期化
    
    for(y=0;y<height[n];y++){
        for(x=0;x<width[n];x++){
            hist[image[n][y][x]]++;        //画素値image[n][y][x]の度数を１増やす
        }
    }
}

void make_histgram_image( int h, int n ) //ヒストグラムを画像化(幅256×高さh画素)して、画像nに記録する
{
    int i,x,y;
    long int max; //最大頻度
    int takasa;   //各画素値の度数の高さ（棒の画素数）
    
    width[n]=256;
    height[n]=h;
    init_image(n, (unsigned char)255);   //白255の背景
    
    //ヒストグラムの最大度数maxを求める
    max=hist[0];
    for(i=0;i<256;i++){
        if ( hist[i] > max )
            max = hist[i];
    }
    
    //棒グラフの高さをh画素に正規化しながら一本ずつ下から黒点で線を引く
    for(x=0;x<width[n];x++){
        takasa = (int)( (double)hist[x] * h / max ); //棒の高さ
        if ( takasa > h )
            takasa = h;
        for(y=0;y<takasa;y++){
            image[n][h-1-y][x] = 0;  //棒を下から黒0で線を引く
        }
    }
}
