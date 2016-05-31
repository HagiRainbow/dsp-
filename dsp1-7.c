//＊＊＊H27年度・DSP1-7＊＊＊
/*初期設定*/
#define _USE_MATH_DEFINES/*M_PIとか有効化*/
#define maxN 20000/*多めに確保*/
#define maxS 128/*多めに確保*/
/*ヘッダ類*/
#include <stdio.h>/*言うまでもないやつ*/
#include <stdlib.h>/*関数exitのために必要*/
#include <math.h>/*M_PIと関数sqrt,sin,cos,atan2,log2のために必要*/
#include <time.h> /*時間計測用*/
/*returnだけの関数をdifineのマクロで実装*/
#define Abs2R(a,b) (a)*(a)+(b)*(b)//絶対値^2
#define AbsR(a,b) sqrt(Abs2R((a),(b)))//絶対値
#define Freq(a,b) 20*log10(AbsR((a),(b)))//振幅スペクトル
#define Pha(a,b) (180*atan2((b),(a)))/M_PI//位相スペクトル
/*構造体定義*/
typedef struct {
	double re;/*実*/
	double im;/*虚*/
} rid;
clock_t start,end;/*時間計測用*/
/*グローバル変数等*/
rid xn[maxN],rtr[maxN];//xn:計算前xk:計算後rtr:回転子
int mcnt=0;//掛け算数カウンタ

/*構造体加算*/
rid sumR(rid r1,rid r2){
	rid res;
	res.re=r1.re+r2.re;
	res.im=r1.im+r2.im;
	return res;
}
/*構造体減算*/
rid difR(rid r1,rid r2){
	rid res;
	res.re=r1.re-r2.re;
	res.im=r1.im-r2.im;
	return res;
}
/*構造体乗算*/
rid proR(rid r1,rid r2){
	rid res;
	res.re=r1.re*r2.re-r1.im*r2.im;
	res.im=r1.im*r2.re+r1.re*r2.im;
	return res;
}
/*共役複素数*/
rid ccR(rid r1){
	rid res;
	res.re=r1.re;
	res.im=-r1.im;
	return res;
}
/*構造体除算*/
rid quoR(rid r1,rid r2){
	rid res;
	double abs;
	abs=AbsR(r2.re,r2.im);
	printf("%lf\n",abs);
	res.re=(r1.re*r2.re+r1.im*r2.im)/abs;
	res.im=(r1.im*r2.re-r1.re*r2.im)/abs;
	return res;
}
/*回転子計算(*rtr:回転子,N:数,fft:FFTorIFFT)*/
void wid(rid *rtr,int N,int fft)
{
	int i,n;
	n=N/2;
	for(i=0;i<n;i++){
		rtr[i].re=cos(((2*M_PI)/N)*i);
		rtr[i].im=sin(((2*M_PI)/N)*i);
		if(fft!=1){
			rtr[i]=ccR(rtr[i]);//複素共役にする
		}
	}
}
//ファイル読み込み
void RIread(rid d[],char fn[maxS],int N,int dft)
{
	FILE *fp;
	int i=0;
	if((fp=fopen(fn,"r"))==NULL){
		printf("[%s]を開けませんでした.\n",fn);
		exit(1);
	}
	for(i=0;i<N;i++){
		if(dft==1){
			if(fscanf(fp,"%lf\n",&d[i].re)==EOF){
				printf("データが不足しています．\n");
				exit(1);
			}
		}else{
			if(fscanf(fp,"%lf %lf\n",&d[i].re,&d[i].im)==EOF){
				printf("データが不足しています．\n");
				exit(1);
			}
		}
	}
	fclose(fp);
}
//ファイル書き出し
void RIwrite(rid d[],char fn[maxS],int N,int dft,int rt)
{
	FILE *fp;
	int i=0;
	if((fp=fopen(fn,"w"))==NULL){
		printf("[%s]を開けませんでした.\n",fn);
		exit(1);
	}
	for(i=0;i<N;i++){
		if(dft!=1){
			fprintf(fp,"%lf\n",d[i].re/N);
		}else{
			if(rt==1){
				fprintf(fp,"%lf\t%lf\n",d[i].re,d[i].im);
			}else{
				fprintf(fp,"%lf\t%lf\n",Freq(d[i].re,d[i].im),Pha(d[i].re,d[i].im));
			}
		}
	}
	fclose(fp);
}
/*ビットリハーサル(*xn:計算前データ,N:数)*/
int bitr(int *bit,int N)
{
	int n,i,j;
	n=(int)(log10(N)/log10(2));//底の変換公式利用
	//計算の本体
	for(i=0;i<N;i++){
		bit[i]=0;//初期化
		for(j=0;j<n;j++){
			bit[i]+=((i>>j)&1)<<(n-1-j);//bitシフト方式
		}
	}
	return n;
}
/*ビットリハーサルによる入れ替え関数*/
int rrid(rid *xn,int N)
{
	int bit[maxN],i,n=0;
	rid buf[maxN];
	n=bitr(bit,N);//ビットリハーサル
	//計算前データを待避
	for(i=0;i<N;i++){
		buf[i]=xn[i];
	}
	//計算前データを並び替え
	for(i=0;i<N;i++){
		//printf("%d\n",bit[i]);
		xn[bit[i]]=buf[i];
	}
	return n;
}
/*バタフライ
a-----------+---
       -   -    
        - -     
         -      
        - -     
       -   -    
b-*rtr--*-1-+---
このグループがj、グループ内での計算数がk
*/
/*FFTIFFT本体*/
void FFTIFFT(rid *xn,rid *rtr,int N,int n)
{
	int i,j,k,bg=N/2,bc=1,ih,il,rn;
	rid buf;//回転子かけた後に保管するもの
	start=clock();
	for(i=0;i<n;i++){
		//バタフライ演算するだけ
		for(j=0;j<bg;j++){
			//蝶グループのmaxはどんどん減っていく
			for(k=0;k<bc;k++){
				//蝶内での計算順のmaxはどんどん増えていく(bg*bc=2/N)
				ih=j*bc*2+k;//一段目は一つの蝶の中でk個ずれる。蝶ごとに2^i*2個ずつづれていく
				il=ih+bc;//二段目は一段目より2^iだけ多いはず。
				rn=k*bg;//回転子の番号。
				buf=proR(xn[il],rtr[rn]);//二段目×回転子
				xn[il]=difR(xn[ih],buf);//二段目
				xn[ih]=sumR(xn[ih],buf);//一段目
				mcnt++;
			}
		}
		bg/=2;//蝶グループはどんどん/2になる。
		bc*=2;//計算順はどんどん2倍になる。(=2^i)
	}
	end=clock();
}

int main()
{
	char fin[maxS],fout[maxS];
	int fft,rt=0,N,win,i,n=0;
	rid a={1,2},b={3,4},res;//試験用
	//最初の質問
	printf("H27年度・DSP1-7・番号26\n");
	printf("説明:textデータをFFTもしくはIFFTします．\n");
	printf("入力ファイル:FFTは一列のデータ，IFFTは二列のデータ[実部(Space of Tab)虚部]\n");
	printf("出力ファイル:FFTは二列のデータ[実部(Tab)虚部],IFFTは一列のデータ\n\n");
	printf("FFT:1 or IFFT:other\n");
	scanf("%d",&fft);
	printf("点数N(2^n):");
	scanf("%d",&N);
	if(fft==1){
		printf("FFT結果(実部，虚部の値)をそのまま出力:1\n振幅スペクトルと位相スペクトルを計算し出力:other\n");
		scanf("%d",&rt);
	}
	printf("入力ファイル名:");
	scanf("%s",&fin);
	printf("出力ファイル名:");
	scanf("%s",&fout);
	printf("処理中...\n");
	//データ読み取り
	RIread(xn,fin,N,fft);
	printf("読み取り完了\n");
	wid(rtr,N,fft);//回転子計算
	printf("回転子計算完了\n");
	n=rrid(xn,N);//bitリハーサル,返り値はfftに渡す物。
	printf("ビットリハ完了\n");
	//FFT本体関数呼び出し(この中にバタフライ演算も実装)
	FFTIFFT(xn,rtr,N,n);
	printf("FFT完了\n");
	//データ書き出し
	RIwrite(xn,fout,N,fft,rt);
	printf("処理終了\n");
	printf("  掛け算数:%d 所要時間:%lf\n",mcnt,((double)(end-start)/CLOCKS_PER_SEC));
	printf("結果は「%s」に出力されています．\n",fout);
	getchar();
	getchar();
	return 0;
}
