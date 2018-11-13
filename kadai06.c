// kadai06.c
#include<stdio.h>
#include<math.h>
#include"pgmlib.h"
#include"labeling.h"

int chain_code[8][2] = {  // Freeman の Chain code
    {  0,  1 }, //0 増分{dy,dx}
    { -1,  1 }, //1
    { -1, 0  }, //2
    { -1, -1 }, //3
    { 0 , -1 }, //4
    { 1 , -1 }, //5
    { 1 ,  0 }, //6
    {  1,  1 }, //7
    };
int next_code_no[8] = { 7, 0, 1, 2, 3 , 4 , 5, 6  };

void detect_contour( int n );
int inside( int x, int y );
int calc_chain_code( int label_no, int n );

#define CODE_LENGTH_MAX 100
int label_number;
int label_w, label_h;
int labeling( int n );

int main(int argc, char* argv[])
{
    printf("２値画像（白：255，黒：0）を読み込みます\n");
    load_image( 0, argv[1] );      // 画像No.0(原画像)

    label_number = labeling( 0 );  // ラベリング結果を配列label[x][y]に作る
    label_w = width[0];            // ラベル配列の幅
    label_h = height[0];           // ラベル配列の高さ
    printf("Number of labeling: %d\n",label_number);
    
    width[1] = width[0];
    height[1] = height[0];
    init_image( 1, 255 );
    detect_contour( 1 );
    save_image( 1, argv[2] );
    
    return 0;
}

int inside( int y, int x )
// 画像座標の内外部判定
{
    if ( x >= 0 && x < label_w && y >= 0 && y < label_h )
    return 1;
    else  return 0;
}

void detect_contour( int n )
// ラベル配列label[x][y]のラベルごとに輪郭線を求めてチェーンコードを表示する
{
    int i;

    for(i=1;i<=label_number;i++){
        int l = calc_chain_code( i, n );   // 戻り値はチェーンコードの長さ
        printf("No.%d, chaincode_length=%d\n", i, l);
    }
}

int calc_chain_code( int label_no, int n )
{
    int x,y,start_x,start_y,xx,yy;
    int code_no,found,code_length;
    char code_str[CODE_LENGTH_MAX];
    
    // ラベル配列をラスタスキャンして、図形要素label_no の最初の画素(図形の左上）を検出
    for(y=0;y<label_h;y++)
        for(x=0;x<label_w;x++)
            if(label[y][x] == label_no) goto LABEL_FOUND;
    
LABEL_FOUND:     // ラベルが見つかったら、２重ループをgoto文で抜ける
    start_x=x; start_y=y;  // 輪郭線の探索開始点
    code_no=5;             // 初期探索方向
    code_length=0;         // 輪郭線のコード長
    do{
        found=0;           // 反時計回りに、輪郭が見つかったかどうかのフラグ
        do{
            xx = x + chain_code[code_no][1]; // 次の輪郭座標x
            yy = y + chain_code[code_no][0]; // 次の輪郭座標y
            if ( inside(yy,xx) && label[yy][xx]==label_no )
                found=1;
            else { code_no++;   if ( code_no > 7 ) code_no=0; }
                   // 輪郭の探索方向は反時計回りに調べる。7の次は0
        }while( found == 0 );
        // チェーンコードをcode_str[]文字列に追加
        sprintf(&code_str[code_length], "%d", code_no );
        code_length++;                     // コード長
        x = xx;  y = yy;                   // 次の点に移動
        image[n][y][x]=0;
        code_no = next_code_no[code_no];   // 次の探索方向を代入
    }while( !(x==start_x && y==start_y) );
    printf("(%d,%d) %s\n",start_x,start_y,code_str);
    
    return code_length;
}

