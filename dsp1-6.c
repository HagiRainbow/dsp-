//2015年度・dsp1-6
#include<stdio.h>
#include<string.h>

/////////////////////////////////////////////////////
// RIFFチャンクおよびWAVEフォームタイプ構造体
/////////////////////////////////////////////////////
typedef struct{
	char id[4];			// "RIFF"
	unsigned long size;		// ファイルサイズ-8
	char form[4];			// "WAVE"
} riff_chunk;
/////////////////////////////////////////////////////
// fmtチャンク構造体
/////////////////////////////////////////////////////
typedef struct{
	char id[4];			// "fmt " スペースも含まれるので注意
	unsigned long size;		// fmt領域のサイズ
	unsigned short format_id;	// フォーマットID (PCM:1)
	unsigned short channel;		// チャネル数 (モノラル:1 ステレオ:2)
	unsigned long  fs;		// サンプリング周波数
	unsigned long  byte_sec;	// １秒あたりのバイト数（fs×byte_samp)
	unsigned short byte_samp;	// １要素のバイト数（channel×(bit/8)）
	unsigned short bit;		// 量子化ビット数（8 or 16）
} fmt_chunk;

typedef struct{
	char id[4];
	unsigned long byte;		// data領域のbyte数
} data_chunk;
	riff_chunk riff;		//riff_chunk型で変数riffを宣言
	fmt_chunk fmt;
	data_chunk data;

int main(){
	char fnamei[256],fnameo[256];
	char textbuffer[16]; /*テキストを読み込む作業用文字列*/
	unsigned long dsize,dn=0;
	unsigned short pcm_data;
	double wtime;
	int i,mode,spd=1;
	FILE *fin;
	FILE *fout;
	printf("2015年度・dsp1-6・4J26\n");
	printf("「WAVE←→TEXT」の双方変換に対応しています。\n");
	printf("wav-->txt:1 txt-->wav:2 -->");
	scanf("%d",&mode);
	if(mode==1){
		printf("読み込むファイル名を入力(拡張子含む)-->");
		scanf("%s",fnamei);
		if((fin=fopen(fnamei,"rb"))==NULL){
			exit(-1);
		}
		printf("出力ファイル名を入力(拡張子含む)-->");
		scanf("%s",fnameo);
		if((fout=fopen(fnameo,"w"))==NULL){
			exit(-1);
		}
		fseek(fin,0,SEEK_SET);	//ファイル読み込み位置をファイルの先頭バイトへ
		fread(&riff.id[0],sizeof(char),1,fin);	//'RIFF'を読み込む
		fread(&riff.id[1],sizeof(char),1,fin);	//'RIFF'を読み込む
		fread(&riff.id[2],sizeof(char),1,fin);	//'RIFF'を読み込む
		fread(&riff.id[3],sizeof(char),1,fin);	//'RIFF'を読み込む
		printf("%s\n",riff.id);
		fread(&riff.size, sizeof(unsigned long),1,fin);
		printf("サイズ:%d\n",riff.size);
		fread(&riff.form[0],sizeof(char),1,fin);	//'FORM'を読み込む
		fread(&riff.form[1],sizeof(char),1,fin);	
		fread(&riff.form[2],sizeof(char),1,fin);	
		fread(&riff.form[3],sizeof(char),1,fin);	
		printf("フォーマット:%s\n",riff.form);
		fread(&fmt.id[0],sizeof(char),1,fin);	//' fmt'を読み込む
		fread(&fmt.id[1],sizeof(char),1,fin);	
		fread(&fmt.id[2],sizeof(char),1,fin);	
		fread(&fmt.id[3],sizeof(char),1,fin);	
		printf("%s\n",fmt.id);
		fread(&fmt.size, sizeof(unsigned long),1,fin);
		printf("ファイルサイズ(RIFF):%d[Byte]\n",fmt.size);
		fread(&fmt.format_id, sizeof(unsigned short),1,fin);
		printf("FormID:%d\n",fmt.format_id);
		fread(&fmt.channel, sizeof(unsigned short),1,fin);
		printf("チャネル数:%d[ch]\n",fmt.channel);
		fread(&fmt.fs, sizeof(unsigned long),1,fin);
		printf("サンプリング周波数:%d[Hz]\n",fmt.fs);
		fread(&fmt.byte_sec, sizeof(unsigned long),1,fin);
		printf("平均データ速度:%d\n",fmt.byte_sec);
		fread(&fmt.byte_samp, sizeof(unsigned short),1,fin);
		printf("ブロックサイズ:%d\n",fmt.byte_samp);
		fread(&fmt.bit, sizeof(unsigned short),1,fin);
		printf("量子化bit数:%d[bit]\n",fmt.bit);
		fread(&data.id[0],sizeof(char),1,fin);	//'data'を読み込む
		fread(&data.id[1],sizeof(char),1,fin);	
		fread(&data.id[2],sizeof(char),1,fin);	
		fread(&data.id[3],sizeof(char),1,fin);	
		printf("%s\n",data.id);
		fread(&data.byte, sizeof(unsigned long),1,fin);
		dsize=data.byte/2;
		printf("データ数:%d[Sample]\n",dsize);
		wtime=(double)dsize/fmt.fs;
		printf("録音時間:%lf[s]\n",wtime);
		//データ書き出しループ
		for(i=0;i<dsize;i++){
			fread(&pcm_data,sizeof(short),1,fin);
			fprintf(fout,"%hd\n",pcm_data);
		}
		printf("text書き出し完了%d\n",i);
	}else{
		printf("WAVにしたいファイル名を入力(拡張子含む)-->");
		scanf("%s",fnamei);
		if((fin=fopen(fnamei,"r"))==NULL){
			exit(-1);
		}
		printf("出力ファイル名を入力(拡張子含む)-->");
		scanf("%s",fnameo);
		if((fout=fopen(fnameo,"wb"))==NULL){
			exit(-1);
		}
		printf("n倍速-->");
		scanf("%d",&spd);
		//ファイル内の要素はいくつあるの?
		while (fgets(textbuffer,16,fin)!=NULL) { /*ファイル読み込みエラーでない限り*/
			dn++;//データ数カウンタ
		}
		printf("データ数:%d個\n",dn);
		fseek(fin,0,SEEK_SET);	//ファイル読み込み位置をファイルの先頭バイトへ
		fseek(fout,0,SEEK_SET);	//ファイル書き出し位置をファイルの先頭バイトへ
		//書き込み処理
		strcpy(riff.id,"RIFF");
		fwrite(&riff.id[0],sizeof(char),1,fout);//'RIFF'を読み込む
		fwrite(&riff.id[1],sizeof(char),1,fout);//'RIFF'を読み込む
		fwrite(&riff.id[2],sizeof(char),1,fout);//'RIFF'を読み込む
		fwrite(&riff.id[3],sizeof(char),1,fout);//'RIFF'を読み込む
		printf("%s\n",riff.id);
		riff.size=dn*2+36;
		printf("data.byte:%d\n",data.byte);
		printf("データ数:%d個\n",riff.size);
		fwrite(&riff.size,sizeof(unsigned long),1,fout);
		strcpy(riff.form,"WAVE");
		fwrite(&riff.form[0],sizeof(char),1,fout);//'FORM'を読み込む
		fwrite(&riff.form[1],sizeof(char),1,fout);
		fwrite(&riff.form[2],sizeof(char),1,fout);
		fwrite(&riff.form[3],sizeof(char),1,fout);
		strcpy(fmt.id,"fmt ");
		fwrite(&fmt.id[0],sizeof(char),1,fout);	//' fmt'を読み込む
		fwrite(&fmt.id[1],sizeof(char),1,fout);	
		fwrite(&fmt.id[2],sizeof(char),1,fout);	
		fwrite(&fmt.id[3],sizeof(char),1,fout);	
		fmt.size=16;
		fwrite(&fmt.size,sizeof(unsigned long),1,fout);
		fmt.format_id=1;
		fwrite(&fmt.format_id,sizeof(unsigned short),1,fout);
		fmt.channel=1;
		fwrite(&fmt.channel,sizeof(unsigned short),1,fout);
		fmt.fs=11025*spd;//サンプリング周波数
		fwrite(&fmt.fs,sizeof(unsigned long),1,fout);
		fmt.byte_sec=22050*spd;//平均データ速度
		fwrite(&fmt.byte_sec,sizeof(unsigned long),1,fout);
		fmt.byte_samp=2;
		fwrite(&fmt.byte_samp,sizeof(unsigned short),1,fout);
		fmt.bit=16;
		fwrite(&fmt.bit,sizeof(unsigned short),1,fout);
		strcpy(data.id,"data");
		fwrite(&data.id[0],sizeof(char),1,fout);//'data'を読み込む
		fwrite(&data.id[1],sizeof(char),1,fout);
		fwrite(&data.id[2],sizeof(char),1,fout);
		fwrite(&data.id[3],sizeof(char),1,fout);
		data.byte=dn*2;
		printf("data.byte:%d\n",data.byte);
		fwrite(&data.byte,sizeof(unsigned long),1,fout);//データ数(1データで2サンプル)
		wtime=(double)dn/fmt.fs;
		printf("推定録音時間:%lf[s]\n",wtime);
		//データ書き出しループ
		for(i=0;i<(int)dn;i++){
			fscanf(fin,"%d",&pcm_data);
			fwrite(&pcm_data,sizeof(unsigned short),1,fout);
		}
		printf("wave書き出し完了%d\n",i);
	}
	fclose(fin);
	fclose(fout);
	getchar();
	getchar();
	return 0;
}
