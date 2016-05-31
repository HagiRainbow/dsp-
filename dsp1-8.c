//＊＊＊H27年度・DSP1-8＊＊＊
/*初期設定*/
#define maxN 50000/*多めに確保*/
#define maxS 128/*多めに確保*/
/*ヘッダ類*/
#include<stdio.h>

//ファイル読み込み(返り値は件数)
int fread1(int x[],char fn[maxS])
{
	FILE *fp;
	int i=0;
	if((fp=fopen(fn,"r"))==NULL){
		printf("[%s]を開けませんでした.\n",fn);
		exit(1);
	}
	while((fscanf(fp,"%d\n",&x[i]))!=EOF){
		i++;
	}
	fclose(fp);
	return i;
}
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
//ファイル書き出し
void fwrite1(int x[],char fn[maxS],int n)
{
	FILE *fp;
	int i=0;
	if((fp=fopen(fn,"w"))==NULL){
		printf("[%s]を開けませんでした.\n",fn);
		exit(1);
	}
	for(i=0;i<n;i++){
		fprintf(fp,"%d\n",x[i]);
	}
	fclose(fp);
}
//本体
int main(){
	FILE *fr,*fw;
	int x[maxN],xb[maxN],n,nx,i,j;
	double h[maxN],y;
	char fin[maxS],fout[maxS],fir[maxS];
	//最初の質問
	printf("H27年度・DSP1-8・番号26\n");
	printf("説明:textデータにFIRフィルタを掛けます．\n");
	printf("入力ファイル及び出力ファイル:一列のテキストデータ\n");
	printf("フィルタ係数データ:一列のテキストデータ\n");
	printf("入力ファイル名:");
	scanf("%s",&fin);
	printf("出力ファイル名:");
	scanf("%s",&fout);
	printf("フィルタファイル名:");
	scanf("%s",&fir);
	//データ読み取り
	n=fread2(h,fir);
	printf("フィルタのデータ数:%5d\n",n);
	nx=fread1(x,fin);
	printf("入力データ数      :%5d\n",nx);

	if((fw=fopen(fout,"w"))==NULL){
		printf("[%s]を開けませんでした.\n",fout);
		exit(1);
	}
	//この中に処理を実装しよう。
	for(i=0;i<n;i++){
		xb[i]=0;
	}
	for(i=0;i<(nx+n);i++){
		for(j=n-1;j>0;j--){
			xb[j]=xb[j-1];//a）入力信号xの配列の中身を一つずつシフト
		}
		//b）ファイルから1行（1サンプル分）読み込む
		//c）入力信号ｘの配列の先頭[0]へb)を代入
		//c+)以下のようにすることで抜けきってないデータも入れることが出来る
		if(i<nx){
			xb[0]=x[i];
		}else{
			xb[0]=0;
		}
		y=0.0;
		for(j=0;j<n;j++){
			y+=(double)xb[j]*h[j];//d)内積と同様の計算（hとｘの関和演算）で出力ｙを算出
		}
		fprintf(fw,"%d\n",(int)y);//e）yをファイルに1行（1サンプル）ずつ保存
	}
	fclose(fw);
	printf("出力データ数      :%5d\n",(nx+n));
	printf("処理終了\n");
	getchar();
	getchar();
	return 0;
}