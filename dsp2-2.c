//＊＊＊H28年度・DSP2-2＊＊＊
/*初期設定*/
#define _USE_MATH_DEFINES/*M_PIとか有効化*/
#define maxN 200/*多めに確保*/
#define maxS 128/*多めに確保*/
#define Limit 0//これ以下の値は0にする
//bmp書き込み用の設定
#define Hsize 54
#define Csize 3
#define DIsize 40
/*ヘッダ類*/
#include <stdio.h>/*言うまでもないやつ*/
#include <stdlib.h>/*関数exitのために必要*/
#include <math.h>/*M_PIと関数sqrt,sin,cos,atan2,log2のために必要*/
/*returnだけの関数をdifineのマクロで実装*/

/////////////////////////////////////////////////////
// 構造体 BITMAPFILEHEADER
/////////////////////////////////////////////////////
#pragma pack(1)
typedef struct TagBITMAPFILEHEADER{//14byte
	unsigned short bfType;			// ファイルタイプ("BM"：0x4D42が入る)
	unsigned long  bfSize;			// ファイルサイズ
	unsigned short bfReserved1;		// 予約領域（0）
	unsigned short bfReserved2;		// 予約領域（0）
	unsigned long  bfOffBits;		// ファイル先頭からみたデータの位置（byte）
} BITMAPFILEHEADER;
#pragma pack()
/////////////////////////////////////////////////////
// 構造体 BITMAPINFOHEADER
/////////////////////////////////////////////////////
typedef struct{//40byte
	unsigned long biSize;			// BITMAPINFOHEADERのサイズ（40[byte]）
	         long biWidth;			// 幅（ピクセル）
	         long biHeight;			// 高さ（ピクセル）
	unsigned short biPlanes;		// カラープレーン数（1）
	unsigned short biBitCount;		// ビクセル当たりビット数（1,4,8,24,32bit）
	unsigned long biCompression;		// 圧縮（0：なし）
	unsigned long biSizeimage;		// 画像データサイズ（[byte]：0でよい）
	         long biXPixPerMeter;		// 水平解像度：m当たり画素数（ピクセル：0でよい）
	         long biYPixPerMeter;		// 垂直解像度：m当たり画素数（ピクセル：0でよい）
	unsigned long biClrUsed;		// カラーパレット数:色数（0でよい）
	unsigned long biClrImportant;		// 重要なカラーインデックス数（0でよい）
} BITMAPINFOHEADER;
/////////////////////////////////////////////////////
// 構造体 カラーパレット　RGBQuad
/////////////////////////////////////////////////////
typedef struct{//1,4,8のときのみそれぞれ2,16,256組の配列を確保
	unsigned char blue;			// Bule値
	unsigned char green;			// Green値
	unsigned char red;			// Red値
	unsigned char re;			// Reserve値
} RGBQUAD;
/////////////////////////////////////////////////////
// 構造体 データ保存　RGB
/////////////////////////////////////////////////////
typedef struct{
	unsigned char blue;			// Bule値
	unsigned char green;			// Green値
	unsigned char red;			// Red値
} RGB_DATA;

BITMAPFILEHEADER bmp_file;
BITMAPINFOHEADER bmp_info;
RGB_DATA *rgb;

double **xn,**xk;

int bmprgb(char fnamei[maxS],char fnameo[maxS])
{
	int dbyte,dsize,i;
	FILE *fin;
	FILE *fout;
	if((fin=fopen(fnamei,"rb"))==NULL){
		exit(-1);
	}
	if((fout=fopen(fnameo,"w"))==NULL){
		exit(-1);
	}
	//bmp_file
	fseek(fin,0,SEEK_SET);	//ファイル読み込み位置をファイルの先頭バイトへ
	fread(&bmp_file.bfType, sizeof(bmp_file.bfType),1,fin);//ヘッダ読み込み例
	printf("BM:%d\n",bmp_file.bfType);
	fread(&bmp_file.bfSize, sizeof(bmp_file.bfSize),1,fin);
	printf("ファイルサイズ:%d[Byte]\n",bmp_file.bfSize);
	fread(&bmp_file.bfReserved1, sizeof(bmp_file.bfReserved1),1,fin);//0
	fread(&bmp_file.bfReserved2, sizeof(bmp_file.bfReserved2),1,fin);//0
	fread(&bmp_file.bfOffBits, sizeof(bmp_file.bfOffBits),1,fin);
	printf("データ開始位置:%d[Byte]\n",bmp_file.bfOffBits);
	//bmp_info
	fread(&bmp_info.biSize, sizeof(bmp_info.biSize),1,fin);//ヘッダ読み込み例
	printf("info部サイズ:%d[Byte]\n",bmp_info.biSize);
	fread(&bmp_info.biWidth, sizeof(bmp_info.biWidth),1,fin);
	fread(&bmp_info.biHeight, sizeof(bmp_info.biHeight),1,fin);
	printf("画素数:%d×%d\n",bmp_info.biWidth,bmp_info.biHeight);
	fread(&bmp_info.biPlanes, sizeof(bmp_info.biPlanes),1,fin);//1
	fread(&bmp_info.biBitCount, sizeof(bmp_info.biBitCount),1,fin);
	dbyte=bmp_info.biBitCount/8;
	printf("bit/ピクセル:%d[bit](=%d[byte])\n",bmp_info.biBitCount,dbyte);
	fread(&bmp_info.biCompression, sizeof(bmp_info.biCompression),1,fin);//0以外は圧縮有?
	printf("圧縮有無,フォーマット:%d\n",bmp_info.biCompression);
	fread(&bmp_info.biSizeimage, sizeof(bmp_info.biSizeimage),1,fin);
	printf("データ部サイズ?:%d[Byte]\n",bmp_info.biSizeimage);
	fread(&bmp_info.biXPixPerMeter, sizeof(bmp_info.biXPixPerMeter),1,fin);
	fread(&bmp_info.biYPixPerMeter, sizeof(bmp_info.biYPixPerMeter),1,fin);
	printf("解像度:%d×%d\n",bmp_info.biXPixPerMeter,bmp_info.biYPixPerMeter);
	fread(&bmp_info.biClrUsed, sizeof(bmp_info.biClrUsed),1,fin);
	printf("カラーパレット数:%d\n",bmp_info.biClrUsed);
	fread(&bmp_info.biClrImportant, sizeof(bmp_info.biClrImportant),1,fin);
	printf("カラーインデックス数:%d\n",bmp_info.biClrImportant);
	//ここからデータ
	dsize=(bmp_file.bfSize-bmp_file.bfOffBits)/dbyte;
	printf("画素数:%d\n",dsize);
	rgb=(RGB_DATA*)malloc(dsize*sizeof(RGB_DATA));
	for(i=0;i<dsize;i++){
		fread(&rgb[i].blue, sizeof(rgb[i].blue),1,fin);
		//fprintf(fout,"%d,",rgb[i].blue);
		fread(&rgb[i].green, sizeof(rgb[i].green),1,fin);
		//fprintf(fout,"%d,",rgb[i].green);
		fread(&rgb[i].red, sizeof(rgb[i].red),1,fin);
		//fprintf(fout,"%d,\n",rgb[i].red);
	}
	//printf("text書き出し完了\n");
	fclose(fin);
	fclose(fout);
	return bmp_info.biWidth;
}

//ファイル書き出し2
void R2write(double **d,char fn[maxS],int N)
{
	FILE *fp;
	int i,j;
	if((fp=fopen(fn,"w"))==NULL){
		printf("[%s]を開けませんでした.\n",fn);
		exit(1);
	}
	for(j=0;j<N;j++){
		for(i=0;i<N;i++){
			fprintf(fp,"%lf\t",d[i][j]);
		}
		fprintf(fp,"\n");
	}
	fclose(fp);
}

//行列を計算する
void Mul_Matp(int N,double **d1,double **d2,double **res)
{
	int i,j,k;
	for(j=0;j<N;j++){
		for(i=0;i<N;i++){
			res[i][j]=0.0;
			for(k=0;k<N;k++){
				res[i][j]+=d1[i][k]*d2[k][j];
			}
		}
	}
}

void Trans_Matp(int N,double **d,double **res)
{
	int i,j;
	for(i=0;i<N;i++){
		for(j=0;j<N;j++){
			res[i][j]=d[j][i];
		}
	}
}

double CnkN(double n,double k,double N)
{
	if(k==0){
		return 1/sqrt(N);
	}else{
		return sqrt(2/N)*cos(((2*n+1)*k*M_PI)/(2*N));
	}
}

//DCT処理
void DCTIDCT2(int N,int mode)
{
	int k,n,i;
	//double c[maxN][maxN],buf[maxN][maxN],ct[maxN][maxN];
	double **c,**buf,**ct;
	c=(double **)calloc(N,sizeof(double *));//doubleポインタ型×N個を確保
	for(i=0;i<N;i++)c[i]=(double *)calloc(N,sizeof(double));//N個にdouble×Nのメモリ配列を確保
	buf=(double **)calloc(N,sizeof(double *));//doubleポインタ型×N個を確保
	for(i=0;i<N;i++)buf[i]=(double *)calloc(N,sizeof(double));//N個にdouble×Nのメモリ配列を確保
	ct=(double **)calloc(N,sizeof(double *));//doubleポインタ型×N個を確保
	for(i=0;i<N;i++)ct[i]=(double *)calloc(N,sizeof(double));//N個にdouble×Nのメモリ配列を確保
	for(k=0;k<N;k++){
		for(n=0;n<N;n++){
			c[n][k]=CnkN(n,k,N);
			//printf("%d,%d,%lf\n",n,k,c[n][k]);
		}
	}
	Trans_Matp(N,c,ct);
	if(mode==2){
		Mul_Matp(N,c,xn,buf);
		Mul_Matp(N,buf,ct,xk);
	}else{
		Mul_Matp(N,ct,xn,buf);
		Mul_Matp(N,buf,c,xk);
	}
	for(i=0;i<N;i++)free(c[i]);free(c);
	for(i=0;i<N;i++)free(buf[i]);free(buf);
	for(i=0;i<N;i++)free(ct[i]);free(ct);
}

int main()
{
	int mode=0,N,i,j;
	char fnamei[maxS],fnameo[maxS];
	printf("2016年度・dsp2-2・5J25\n");
	printf("読み取ったbmpをYCC->DCTします。\n");
	printf("読み込むファイル名を入力(拡張子含む)-->");
	scanf("%s",fnamei);
	printf("出力ファイル名を入力(拡張子含む)-->");
	scanf("%s",fnameo);
	N=bmprgb(fnamei,fnameo);
	printf("動的確保\n");
	xn=(double **)calloc(N,sizeof(double *));//doubleポインタ型×N個を確保
	for(i=0;i<N;i++)xn[i]=(double *)calloc(N,sizeof(double));//N個にdouble×Nのメモリ配列を確保
	xk=(double **)calloc(N,sizeof(double *));//doubleポインタ型×N個を確保
	for(i=0;i<N;i++)xk[i]=(double *)calloc(N,sizeof(double));//N個にdouble×Nのメモリ配列を確保
	for(j=0;j<N;j++){
		for(i=0;i<N;i++){
			xn[i][j]=rgb[i+j*120].red*0.299+rgb[i+j*120].green*0.587+rgb[i+j*120].blue*0.114;
		}
	}
	printf("YCC変換完了\n");
	DCTIDCT2(N,1);
	for(j=0;j<N;j++){
		for(i=0;i<N;i++){
			if(xk[i][j]<Limit){
				xk[i][j]=0;
			}
		}
	}
	R2write(xk,fnameo,N);
	for(i=0;i<N;i++)free(xn[i]);free(xn);
	for(i=0;i<N;i++)free(xk[i]);free(xk);
	getchar();
	getchar();
	return 0;
}