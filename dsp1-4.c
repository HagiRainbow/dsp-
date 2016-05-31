//＊＊＊H27年度・DSP1-4＊＊＊
#define N 701 /*多めに確保*/
#include <stdio.h>
#include <stdlib.h> /*関数exitのために必要*/
#include <math.h> /*関数sqrtのために必要*/
#include <string.h> /*関数strcatのために必要*/

void read(double d[],char fn[128])
{
	FILE *fp;
	int i=0;
	if((fp=fopen(fn,"r"))==NULL){
		printf("can't open file[%s].\n",fn);
		exit(1);
	}
	while(fscanf(fp,"%lf",&d[i])!=EOF){
		i++;
	}
	fclose(fp);
}

double average(double u[],int x)
{
	int i;
	double res=0;
	for(i=0;i<x;i++){
		res+=u[i];
	}
	res=res/(double)x;
	return res;
}

double Snaiseki(double u1[],double u2[],int y,int o)
{
	int i;
	double res=0;
	for(i=0;i<y;i++){
		if(i+o<702){
			res+=u1[i]*u2[i+o];
		}
	}
	return res;
}

double Ssoukann(double n,double z)
{
	double res=0;
	res=n/z;
	return res;
}

int main()
{
	char fon[128];
	double idb1[N],idb2[N];//元の配列
	double a1,a2;//平均
	double n12;//内積
	double s12;//相関係数
	int max=0;
	int i,j;
	printf("H27年度・DSP1-4・番号26\n");
	printf("使い方の説明:\n実行ファイル内での入力操作は要りません．\n相互相関(wdata1とwdata2)の結果は[cc.txt]に，\n自己相関(data3)の結果は[ac.txt]に出力されます．\n結果の一部はこの下にも表示されます．\n\n");
	printf("実行結果\n");
	for(j=0;j<2;j++){
		FILE *fp;
		if(j==0){
		//データを入れよう．
			read(idb1,"wdata1.txt");
			read(idb2,"wdata2.txt");
			fp=fopen("cc.txt","w");
			max=701;
			printf("相互相関係数\n");
		}else{
			read(idb1,"data3.txt");
			read(idb2,"data3.txt");
			fp=fopen("ac.txt","w");
			max=71;
			printf("自己相関係数\n");
		}
		//とりあえずFILE型に入れよう．
		if(fp==NULL){
			printf("can't open file.\n");
			exit(1);
		}
		//Average(無意味?)
		a1=average(idb1,max);
		a2=average(idb2,max);
		for(i=0;i<max;i++){
			//Naiseki
			n12=Snaiseki(idb1,idb2,max,i);
			//soukann
			s12=Ssoukann(n12,max);
			fprintf(fp,"%lf\n",s12);
			if(j==0){
				if(i%100==0){
					printf("Rxy(%3d)=%lf\n",i,s12);
				}
			}else{
				if(i%10==0){
					printf("Rxx(%2d)=%lf\n",i,s12);
				}
			}
		}
		//後処理
		fclose(fp);
	}
	return 0;
}