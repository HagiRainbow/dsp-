//＊＊＊H27年度・DSP1-5＊＊＊
#define _USE_MATH_DEFINES/*M_PI有効化*/
#define maxN 1000 /*多めに確保*/
#define maxS 128 /*多めに確保*/
#include <stdio.h> /*言うまでもないやつ*/
#include <stdlib.h> /*関数exitのために必要*/
#include <math.h> /*M_PIと関数sqrt,sin,cos,atan2のために必要*/
//returnだけの関数はdefineのほうがいいよね．
#define Freq(a,b) 20*log10(sqrt((a)*(a)+(b)*(b)))
#define Pha(a,b) (180*atan2((b),(a)))/M_PI

/*構造体定義*/
typedef struct {
	double re; /*実*/
	double im; /*虚*/
} rid;

void RIread(rid d[],char fn[128],int N,int dft)
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

void DftIdft(rid d[],rid o[],int N,int dft,int a,int b){
	int i,j;
	for(i=0;i<N;i++){
		o[i].re=0; o[i].im=0;
		for(j=0;j<N;j++){
			o[i].re+=d[j].re*cos(((2*M_PI)/N)*i*j)+a*d[j].im*sin(((2*M_PI)/N)*i*j);
			o[i].im+=d[j].im*cos(((2*M_PI)/N)*i*j)-a*d[j].re*sin(((2*M_PI)/N)*i*j);
		}
		o[i].re/=b;
		o[i].im/=b;
	}
}

void RIwrite(rid d[],char fn[128],int N,int dft,int rt)
{
	FILE *fp;
	int i=0;
	if((fp=fopen(fn,"w"))==NULL){
		printf("[%s]を開けませんでした.\n",fn);
		exit(1);
	}
	for(i=0;i<N;i++){
		if(dft!=1){
			fprintf(fp,"%lf\n",d[i].re);
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

void window(rid d[],int N)
{
	int i;
	for(i=0;i<N;i++){
		d[i].re*=(0.54-0.46*cos((2*M_PI*i)/N));
	}
}

int main()
{
	char fin[maxS],fout[maxS],hout[maxS];
	double d[maxN];
	int dft,rt=0,N,a,b,win;
	rid xn[maxN],Xk[maxN];
	//最初の質問
	printf("H27年度・DSP1-5・番号26\n");
	printf("説明:textデータをDFTもしくはIDFTします．\n");
	printf("使い方:textデータをDFTもしくはIDFTします．\n");
	printf("入力ファイル:DFTは一列のデータ，IDFTは二列のデータ[実部(Space of Tab)虚部]\n");
	printf("出力ファイル:DFTは二列のデータ[実部(Tab)虚部],IDFTは一列のデータ\n\n");
	printf("DFT:1 or IDFT:other\n");
	scanf("%d",&dft);
	printf("点数N:");
	scanf("%d",&N);
	if(dft==1){
		printf("DFT結果(実部，虚部の値)をそのまま出力:1\n振幅スペクトルと位相スペクトルを計算し出力:other\n");
		scanf("%d",&rt);
		printf("ハミング窓を使う:1 使わない:other\n");
		scanf("%d",&win);
		a=1; b=1;
	}else{
		a=-1; b=N;
	}
	printf("入力ファイル名:");
	scanf("%s",&fin);
	printf("出力ファイル名:");
	scanf("%s",&fout);
	printf("処理中...\n");
	//データ読み取り
	RIread(xn,fin,N,dft);
	//ハミング窓処理
	if(win==1){
		printf("窓関数適応後の元データ出力ファイル名:");
		scanf("%s",&hout);
		window(xn,N);
		RIwrite(xn,hout,N,0,0);
	}
	//計算
	DftIdft(xn,Xk,N,dft,a,b);
	//書き込み
	RIwrite(Xk,fout,N,dft,rt);
	printf("処理終了\n");
	printf("結果は「%s」に出力されています．\n",fout);
	getchar();
	return 0;
}