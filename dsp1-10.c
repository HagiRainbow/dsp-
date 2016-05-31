//＊＊＊H27年度・DSP1-10＊＊＊
/*初期設定*/
#define _USE_MATH_DEFINES
#define Yb 10.0e-7
#define A 1.0
#define maxS 128/*多めに確保*/
#define maxN 20000/*多めに確保*/
#define E_Change 0//自主課題用(1にするとON)
/*ヘッダ類*/
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<time.h>
//ファイル読み込み(返り値は件数)
int fread2(double x[],char fn[maxS])
{
	FILE *fp;
	int i=0;
	if((fp=fopen(fn,"r"))==NULL){
		printf("[%s]を開けませんでした.\n",fn);
		exit(1);
	}
	while((fscanf(fp,"%lf\n",&x[i]))!=EOF){
		i++;
	}
	fclose(fp);
	return i;
}
//ノイズ関連
double WG(void){
	double r1=0.0,r2=0.0,ave=0.0,sig=1.0;
	//srand((unsigned)time(NULL));//乱数初期化?
	while(r1==0.0){
		r1=(double)rand()/(RAND_MAX);
	}
	r2=(double)rand()/(RAND_MAX);
	return (sig*sqrt(-2.0*log(r1))*cos(2.0*M_PI*r2)+ave);
}

double CG(void){
	double color;
	static double old_color=0.0;
	color=WG()+0.95*old_color;
	old_color=color;
	return color;
}

//メイン
int main()
{
	double *xn;			//入力信号ベクトル
	double *hn;			//推定システム
	double wn[maxN],x[maxN];//未知システム,入力信号
	char finn[maxS],fout[maxS],frn[maxS],fen[maxS],fdn[maxS];
	FILE *fr,*fw,*fe,*fin,*fd;
	double d,e,y,xx,prev_e,buf;
	int i,sample,n,nx,j,mode=0,modei=0;
	srand((unsigned)time(NULL));
	printf("H27年度・DSP1-10・番号26\n");
	printf("説明:ノイズキャンセルプログラムです．\n");
	printf("モード1:入力ファイルにインパルス応答(ノイズ)を掛けた際の出力誤差の2乗(e)から出した収束特性のみ出力。\n");
	printf("モード2:入力ファイルにインパルス応答(ノイズ)を掛けた際の出力データ(y),出力誤差の2乗(e)を、観測信号(d)を出力。\n");
	printf("それぞれの入出力ファイルを入力すると計算が行われ、指定したファイルに結果が出力されます。\n");
	printf("モード選択：1->出力誤差の２乗のみ出力,それ以外->y,e,dをそのまま出力：");
	scanf("%d",&mode);
	printf("入力ファイル名:");
	scanf("%s",&frn);
	printf("インパルス応答ファイル名:");
	scanf("%s",&finn);
	if(mode!=1){
		printf("出力ファイル名:");
		scanf("%s",&fout);
		printf("dの保存ファイル名:");
		scanf("%s",&fdn);
	}
	printf("eの保存ファイル名:");
	scanf("%s",&fen);
	//インパルス応答をファイルから読み込む．数もカウントnとする
	n=fread2(wn,finn);
	//配列の動的確保
	if((xn=(double*)calloc(n,sizeof(double)))==NULL){
		printf("メモリが確保できません\n");
		exit(1);
	}
	if((hn=(double*)calloc(n,sizeof(double)))==NULL){
		printf("メモリが確保できません\n");
		exit(1);
	}
	//入力データが入ったファイルをオープン
	//入力データをファイルから読み込む場合はそのファイルをオープン．
	nx=fread2(x,frn);
	//保存用ファイルのオープン
	if(mode!=1){
		if((fw=fopen(fout,"w"))==NULL){
			printf("[%s]を開けませんでした.(1)\n",fout);
			exit(1);
		}
		if((fd=fopen(fdn,"w"))==NULL){
			printf("[%s]を開けませんでした.(2)\n",fdn);
			exit(1);
		}
	}
	//保存用ファイルのオープン
	if((fe=fopen(fen,"w"))==NULL){
		printf("[%s]を開けませんでした.(3)\n",fen);
		exit(1);
	}
	//ここからグラフの横軸に相当する，サンプル回の繰り返し
	for(i=0;i<(nx);i++){
		//xnの更新
		for(j=n-1;j>0;j--){
			xn[j]=xn[j-1];//入力ベクトルを生成
		}
		//xn[0]=WG();
		if(i<nx){
			xn[0]=x[i];
		}else{
			xn[0]=0.0;
		}
		//ファイルから読むものについては，1サンプルごとに読み込み
		//所望信号dの計算（firフィルタによるフィルタリング） d = xnとwnの内積
		d=0.0;
		//Wnを変更(環境変化を想定)：自主課題に使用
		if(E_Change==1){
			if(i%((int)(nx/10))==0){
				buf=wn[0];
				for(j=1;j<n;j++){
					wn[j-1]=wn[j];
				}
				wn[n-1]=buf;
			}
		}
		for(j=0;j<n;j++){
			d+=xn[j]*wn[j];
		}
		//フィルタからの出力信号yの計算（方法はdと同様）
		y=0.0;
		for(j=0;j<n;j++){
			y+=xn[j]*hn[j];
		}
		e=d-y;//誤差信号eの計算
		//入力信号ベクトルxnのノルムの二乗||xn||^2の計算(xn同士の内積)
		xx=0.0;
		for(j=0;j<n;j++){
			xx+=xn[j]*xn[j];
		}
		for(j=0;j<n;j++){
			hn[j]+=(A*xn[j]*e)/(xx+Yb);//フィルタ係数のhnの計算
		}
		//アルゴリズムの評価に使うeの2乗を計算
		//評価に使うデータ(eなど)をファイルへ1つずつ保存
		if(mode!=1){
			fprintf(fw,"%d\n",(int)y);
			fprintf(fe,"%lf\n",e*e);
			fprintf(fd,"%lf\n",d);
		}else{
			if(e!=0.0){
				prev_e=10.0*log10(e*e);
			}
			fprintf(fe,"%lf\n",prev_e);
		}
	}
	//サンプルループここまで
	fclose(fe);
	if(mode!=1){
		fclose(fw);
		fclose(fd);
	}
	printf("出力完了\n");
	getchar();
	getchar();
	return 0;
}