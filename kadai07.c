// kadai07.c
#include<stdio.h>
#include<math.h>
#include"pgmlib.h"

#define HAIKEI (255)        //背景：白255
#define ZUKEI (0)           //図形：黒0

int chain_code[8][2] = {  //Freeman の Chain code
    {  0,  1 }, //0 増分{dy,dx}
    { -1,  1 }, //1
    { -1,  0 }, //2
    { -1, -1 }, //3
    {  0, -1 }, //4
    {  1, -1 }, //5
    {  1,  0 }, //6
    {  1,  1 }, //7
    };
int next_code_no[8] = { 7, 7, 1, 1, 3, 3, 5, 5 };
int start_code_no = 5;

#define CODE_LENGTH_MAX 100
char code_str[CODE_LENGTH_MAX]; //輪郭チェーンコード

void detect_contour( int n );
int inside( int x, int y, int n );
int calc_chain_code( int n );

double calc_perimeter();
double calc_area( int n );
double calc_m00( int n );

int main(int argc, char* argv[])
{
    printf("二値画像（背景：白255，図形：黒0）を読み込みます\n");
    load_image( 0, argv[1] );
    detect_contour( 0 );
    return 0;
}

void detect_contour( int n )
{
    int code_length = calc_chain_code( n );   //輪郭チェーンコードをcode_str[]に求め，長さを返す
    double perimeter = calc_perimeter();      //code_str[]から周囲長を計算
    double area = calc_area( n );             //面積の算出
    double circularity = 4*M_PI*area/(perimeter*perimeter);   // 円形度=4*M_PI*面積/(周囲長*周囲長)

    printf("chaincode_length=%d\n", code_length);
    printf("perimeter=%.2f\n", perimeter);
    printf("area=%.2f\n", area);
    printf("circularity=%.2f\n", circularity);
}

int inside( int y, int x, int n )
// 画像座標の内外部判定
{
    if ( x >= 0 && x < width[n] && y >= 0 && y < height[n] )
        return 1;
    else
        return 0;
}

int calc_chain_code( int n )
{
    int x,y,start_x,start_y,xx,yy;
    int code_no,found,code_i;

    // 画像配列をラスタ走査して、最初の画素(図形の左上）を検出
    for(y=0;y<height[n];y++)
        for(x=0;x<width[n];x++)
            if(image[n][y][x] == ZUKEI) goto LABEL_START;  // 2重ループを一気に抜けるgoto

LABEL_START:
    start_x=x; start_y=y;  // 輪郭線の探索開始点
    code_no=start_code_no; // ラスタ走査後の初期探索コード
    code_i=0;              // 配列code_str[]のインデックス
    do{
        found=0;           // 反時計回りに、輪郭が見つかったかどうかのフラグ
        do{
            xx = x + chain_code[code_no][1]; // 次の輪郭座標x
            yy = y + chain_code[code_no][0]; // 次の輪郭座標y
            if ( inside(yy,xx,n) && image[n][yy][xx]==ZUKEI )
                found=1;        //次の輪郭画素が見つかったらdo-whileループを抜ける
            else {
                code_no++;      //次の探索コードは反時計回りに調べる
                if ( code_no > 7 ) code_no=0; //7の次は0
            }
        }while( found == 0 );

        // チェーンコードをcode_str[]文字列に追加
        sprintf(&code_str[code_i], "%d", code_no);
        code_i++;

        x = xx;  y = yy;                   // 次の画素に移動
        code_no = next_code_no[code_no];   // 次の探索コードを代入
    }while( !(x==start_x && y==start_y) );

    code_str[code_i]='\0';
    printf("(%d,%d) %s\n",start_x,start_y,code_str);

    return code_i; //==code_length
}

double calc_perimeter()
{
    int i;
    int code_length = strlen(code_str);
    double perimeter=0;

    for(i=0;i<code_length;i++){
        int num = code_str[i]-'0';  //数字'0'を数値0に変換 code_str[i]-'0'
        if(num%2==0)
            perimeter +=1;          //偶数コードなら +1
        else
            perimeter += sqrt(2.0); //奇数コードなら +sqrt(2.0)
    }

    return perimeter;
}

double calc_area( int n )// 図形（黒0）の画素を数える
{
    int x,y;
    int area=0;
    for(y=0;y<height[n];y++)
        for(x=0;x<width[n];x++)
            if(image[n][y][x]==ZUKEI)   // 図形（ZUKEI:黒0）の画素を数える
                area++;
    return (double)area;
}


double calc_m00( int n)
{
    int x,y;
    double m00=0;
    for(y=0;y<height[n];y++)
        for(x=0;x<width[n];x++)
            if(image[n][y][x]==ZUKEI)   // 図形（ZUKEI:黒0）の画素を数える
                m00 += 1*1;
                // x^0*y^0
                // power((double)x, 0.0) * power((double)y, 0.0)
    return (double)m00;
}
