//＊＊＊H27年度・DSP1-3＊＊＊
#include <stdio.h>
#include <stdlib.h> /*関数exitのために必要*/
#include <math.h> /*関数sqrtのために必要*/

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

double naiseki(double u1[],double u2[],int y)
{
	int i;
	double res=0;
	for(i=0;i<y;i++){
		res+=u1[i]*u2[i];
	}
	return res;
}

double seiki(double u[],int y)
{
	double res=0;
	int i=0;
	for(i=0;i<y;i++){
		res+=u[i]*u[i];
	}
	res=sqrt(res);
	return res;
}

double soukann(double n,double z1,double z2)
{
	double res=0;
	res=n/(z1*z2);
	return res;
}

void rprint(double s12,double s13,double s14)
{
	printf("H16男性・女性=%1.3lf\n",s12);
	printf("H16男性・総人口=%1.3lf\n",s13);
	printf("H16男性・S24=%1.3lf\n",s14);
}

int main()
{
	FILE *f1,*f2,*f3,*f4;
	double idb1[86],idb2[86],idb3[86],idb4[86];//元の配列
	double id1[86],id2[86],id3[86],id4[86];//直流除去後
	double a1,a2,a3,a4;//平均
	double n12,n13,n14;//内積
	double s12,s13,s14;//相関係数
	int i=0;
	//とりあえずFILE型に入れよう．
	if (((f1=fopen("rdata1.txt","r"))==NULL)||((f2=fopen("rdata2.txt","r"))==NULL)
	  ||((f3=fopen("rdata3.txt","r"))==NULL)||((f4=fopen("rdata4.txt","r"))==NULL)) {
		printf("can't open file.\n");
		exit(1);/*ファイルオープンエラーで緊急停止*/
	}
	//データを入れよう．
	while(fscanf(f1,"%lf",&idb1[i])!=EOF){
		i++;
	}
	i=0;
	while(fscanf(f2,"%lf",&idb2[i])!=EOF){
		i++;
	}
	i=0;
	while(fscanf(f3,"%lf",&idb3[i])!=EOF){
		i++;
	}
	i=0;
	while(fscanf(f4,"%lf",&idb4[i])!=EOF){
		i++;
	}
	i=0;
	printf("H27年度・DSP1-3・番号26\n");
	printf("使い方の説明:\n実行ファイル内での入力操作は要りません．\nこの説明の下に結果が表示されます．\n\n");
	//Average
	a1=average(idb1,86);
	a2=average(idb2,86);
	a3=average(idb3,86);
	a4=average(idb4,86);
//	printf("a1=%lf,a2=%lf,a3=%lf,a4=%lf\n",a1,a2,a3,a4);
	for(i=0;i<86;i++){
		id1[i]=idb1[i]-a1;
		id2[i]=idb2[i]-a2;
		id3[i]=idb3[i]-a3;
		id4[i]=idb4[i]-a4;
	}
	//Naiseki
	n12=naiseki(id1,id2,86);
	n13=naiseki(id1,id3,86);
	n14=naiseki(id1,id4,86);
//	printf("n12=%lf,n13=%lf,n14=%lf\n",n12,n13,n14);
	//soukann
	s12=soukann(n12,seiki(id1,86),seiki(id2,86));
	s13=soukann(n13,seiki(id1,86),seiki(id3,86));
	s14=soukann(n14,seiki(id1,86),seiki(id4,86));
	printf("相関係数結果:\n");
	rprint(s12,s13,s14);
	//Naiseki
	n12=naiseki(idb1,idb2,86);
	n13=naiseki(idb1,idb3,86);
	n14=naiseki(idb1,idb4,86);
//	printf("n12=%lf,n13=%lf,n14=%lf\n",n12,n13,n14);
	//soukann
	s12=soukann(n12,seiki(idb1,86),seiki(idb2,86));
	s13=soukann(n13,seiki(idb1,86),seiki(idb3,86));
	s14=soukann(n14,seiki(idb1,86),seiki(idb4,86));
	printf("直流成分未除去の場合:\n");
	rprint(s12,s13,s14);
	//後処理
	fclose(f1);
	fclose(f2);
	fclose(f3);
	fclose(f4);
	return 0;
}