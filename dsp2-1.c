//＊＊＊H28年度・DSP2-1＊＊＊
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
#define Max(a,b) ((a)>(b)?(a):(b))
/*構造体定義*/
typedef struct {
	double re;/*実*/
	double im;/*虚*/
} rid;
clock_t start,end;/*時間計測用*/
/*グローバル変数等*/
rid xn[maxN],xn2[maxN],rtr[maxN],Rxx[maxN];//xn:計算前xk:計算後rtr:回転子
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
int read(rid d[],char fn[maxS])
{
	FILE *fp;
	int i=0;
	if((fp=fopen(fn,"r"))==NULL){
		printf("[%s]を開けませんでした.\n",fn);
		exit(1);
	}
	while(fscanf(fp,"%lf",&d[i].re)!=EOF){
		i++;
	}
	fclose(fp);
	return i;
}

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
			fprintf(fp,"%lf\n",d[i].re/(N-rt));
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
//ファイル書き出し2
void Rwrite(rid d[],char fn[maxS],int N,int m)
{
	FILE *fp;
	int i=0;
	if((fp=fopen(fn,"w"))==NULL){
		printf("[%s]を開けませんでした.\n",fn);
		exit(1);
	}
	for(i=0;i<m+1;i++){
		fprintf(fp,"%lf\n",(d[i].re/N));
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
//窓関数類
void hamming(rid d[],int N)
{
	int i;
	for(i=0;i<N;i++){
		d[i].re*=(0.54-0.46*cos((2*M_PI*i)/N));
	}
}

void hanning(rid d[],int N)
{
	int i;
	for(i=0;i<N;i++){
		d[i].re*=(0.50-0.50*cos((2*M_PI*i)/N));
	}
}

void black(rid d[],int N)
{
	int i;
	for(i=0;i<N;i++){
		d[i].re*=(0.42-0.50*cos((2*M_PI*i)/N)+0.08*cos((4*M_PI*i)/N));
	}
}
//構造体の定数倍
void rid_mult(rid d[],double n,int N)
{
	int i;
	for(i=0;i<N;i++){
		d[i].re*=n;
	}
}

int main()
{
	char fin[maxS],fin2[maxS],fout[maxS];
	int rt=0,N,win,i,n=0,mode=1,in1N=0,in2N=0,M,m,n2;
	//最初の質問
	printf("H28年度・DSP2-1・番号25\n");
	printf("説明:textデータからFFTもしくはIFFTを用いて自己、相互相関関数を求めます。\n");
	printf("入力ファイル:2つのデータファイル(自己相関は同じ、相互相関は違うファイル)\n");
	printf("出力ファイル:上記のいずれも1つのデータファイル\n\n");
	//中身をすべて0にする
	for(i=0;i<maxN;i++){
		xn[i].re=0.0;
		xn2[i].re=0.0;
		xn[i].im=0.0;
		xn2[i].im=0.0;
	}
	printf("入力ファイル名(1):");
	scanf("%s",&fin);
	in1N=read(xn,fin);
	printf("入力ファイル名(2):");
	scanf("%s",&fin);
	in2N=read(xn2,fin);
	printf("出力ファイル名:");
	scanf("%s",&fout);
	printf("処理中...\n");
	N=Max(in1N,in2N);
	//データ読み取り
	printf("ファイル(1):%d個 ファイル(2):%d個 ∴N=%d\n",in1N,in2N,N);
	printf("最大の時間差(0～%d)：",N-1);
	scanf("%d",&m);
	//点数を求める
	M=1;
	while(M<(N+m)){
		M*=2;
	}
	printf("M=%d\n",M);
	wid(rtr,M,1);//回転子計算
	printf("回転子計算完了\n");
	n=rrid(xn,M);//bitリハーサル,返り値はfftに渡す物。
	n2=rrid(xn2,M);//bitリハーサル,返り値はfftに渡す物。
	printf("ビットリハ完了\n");
	//FFT本体関数呼び出し(この中にバタフライ演算も実装)
	FFTIFFT(xn,rtr,M,n);
	FFTIFFT(xn2,rtr,M,n2);
	printf("FFT完了\n");
	//1個目を複素共役にしたうえで2個目と乗算
	for(i=0;i<M;i++){
		Rxx[i]=proR(ccR(xn[i]),xn2[i]);
	}
	wid(rtr,M,0);//回転子計算
	printf("回転子計算完了\n");
	n=rrid(Rxx,M);//bitリハーサル,返り値はfftに渡す物。
	printf("ビットリハ完了\n");
	//FFT本体関数呼び出し(この中にバタフライ演算も実装)
	FFTIFFT(Rxx,rtr,M,n);
	printf("IFFT完了\n");
	//データ書き出し
	Rwrite(Rxx,fout,M*N,m);
	printf("処理終了\n");
	printf("掛け算数:%d 所要時間:%lf\n",mcnt,((double)(end-start)/CLOCKS_PER_SEC));
	printf("結果は「%s」に出力されています．\n",fout);
	getchar();
	getchar();
	return 0;
}
