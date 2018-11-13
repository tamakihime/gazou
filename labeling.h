// labeling.h  ラベリング処理用ヘッダファイル
int label[MAX_IMAGESIZE][MAX_IMAGESIZE]; //ラベル用配列

#define UNLABEL 255         //未処理ラベル：ラベルの最大値
#define MAX_LABEL 256       //最大ラベル個数
#define ZUKEI (0)           //図形：黒
#define HAIKEI (255)        //背景：白

int label_lut[MAX_LABEL];   //ラベルLUT
int label_lut2[MAX_LABEL];  //ラベルLUT（空き番号を詰める）

int labeling( int n );
int min_neighbors_label( int x, int y, int n, int *num ); //8近傍の最小ラベル
void lookup_table(int label, int min_label, int num);     //LUT更新
    
int min(int n1, int n2)
{
    return (n1<n2) ? n1 : n2;
}

int max(int n1, int n2)
{
    return (n1>n2) ? n1 : n2;
}

void lookup_table(int label, int min_label, int num)
{
    int i;
    int old_label, new_label;
    
    if(label==UNLABEL) return;
    if(label_lut[label]==min_label) return;
    
    new_label=min(label_lut[label], min_label);
    old_label=max(label_lut[label], min_label);
    
    for(i=1;i<=num;i++)
        if(label_lut[i]==old_label) label_lut[i]=new_label;
}

int min_neighbors_label( int y, int x, int n, int *num )
{
    int min_label;
    int label1=UNLABEL;  //初期値
    int label2=UNLABEL;
    int label3=UNLABEL;
    int label4=UNLABEL;
    
    if(y-1>=0 && x-1>=0      ) label1=label[y-1][x-1]; //左上のラベル
    if(y-1>=0                ) label2=label[y-1][x];   //　上のラベル
    if(y-1>=0 && x+1<width[n]) label3=label[y-1][x+1]; //右上のラベル
    if(x-1>=0                ) label4=label[y][x-1];   //　左のラベル
    
    min_label = min(min(min(label1, label2), label3), label4); //最小値ラベル
    
    if (min_label==UNLABEL){    //新しいラベル
        min_label = ++(*num);   //1増やした値を代入
    }
    else {                                     //LUTの更新
        lookup_table(label1, min_label, *num); //label1とmin_labelが同じ連結成分
        lookup_table(label2, min_label, *num);
        lookup_table(label3, min_label, *num);
        lookup_table(label4, min_label, *num);
    }
    return min_label;
}

int pack_label( int num )
{
    int i;
    int old_label_num = 1;
    int new_label_num = 0;
    for(i=1; i<=num; i++){
        if(label_lut[i] != i){
            label_lut2[i] = label_lut2[label_lut[i]];
        }
        else label_lut2[i] = ++new_label_num;
    }
    return new_label_num;
}

int labeling( int n ) // 画像nの図形にラベリング label[y][x]，戻り値はラベル個数
{
    int x,y,num,i;
    int label_num=0;    // ラベル個数
    int pack_label_num; // 空き番号を詰めたラベル個数
    
    // ラベルを初期化
    for(y=0;y<height[n];y++)
        for(x=0;x<width[n];x++)
            label[y][x]=UNLABEL; // 未処理のラベル
    
    for(i=1;i<MAX_LABEL;i++)
        label_lut[i]=i;
    
    // 画像nをラスタ走査
    for(y=0;y<height[n];y++)
        for(x=0;x<width[n];x++){
            if ( image[n][y][x] == ZUKEI ){
                label[y][x] = min_neighbors_label(y,x,n, &label_num);
            }
        }
    
    //for(i=1; i<=label_num; i++)
    //    printf("label_lut[%d]=%d\n",i,label_lut[i]);
    
    pack_label_num = pack_label(label_num); //LUTの空き番号を詰める label_lut >> label_lut2
    
    //for(i=1; i<=label_num; i++)
    //    printf("label_lut2[%d]=%d\n",i,label_lut2[i]);
    
    if ( label_num > 0 ){
        for(y=0;y<height[n];y++)
            for(x=0;x<width[n];x++)
                label[y][x]=label_lut2[label[y][x]];  //LUTを参照してラベルの付け直し
        
        return pack_label_num;
    }
    else return 0;
}
