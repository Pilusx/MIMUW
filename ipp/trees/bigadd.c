#include <string.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

void gen(int k){
	char t[] = "add_test";
	char err[] = ".err";
	char out[] = ".out";
	char in[] = ".in";
	char number[100] = "";
	char fin[100] = "";
	char fout[100] = "";
	char ferr[100] = "";
	
	sprintf(number,"%d",k);	
	strcat(fin,t);
	strcat(fin,number);
	strcat(fout,fin);
	strcat(ferr,fout);
	strcat(ferr,err);
	strcat(fin,in);
	strcat(fout,out);
	
	printf("%s %s %s \n",fin,fout,ferr);

	FILE *fi = fopen(fin,"w");
	FILE *fo = fopen(fout,"w");
	FILE *fe = fopen(ferr,"w");;

	int i = 1;
	while (i <= k){
		fprintf(fi,"ADD_NODE %d\n",(i%6 == 0 ? i/2 : i-1));
		fprintf(fo,"OK\n");
		fprintf(fe,"NODES: %d\n",i+1);
		i++;
	}	
	
	fclose(fi);
	fclose(fo);
	fclose(fe);
}




int main(){
	gen(10000);
	gen(250000);
	gen(700000);
	gen(1000000);	
	
	return 0;
}
