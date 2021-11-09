#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <sys/types.h>

const char *prompt = "Shell : " ;


int main (int argc, char * argv[]) {
	char cut[] = " \n";
	char dirname[100];
	char * pathvar;
	FILE *fp;
	char *filename = NULL;
	char buf[255];

	while(1){
	char str[64];
	int i=0;

	fputs(prompt, stdout);
	fgets(str,200,stdin);

	if(strlen(str) ==1)
		continue;
	argc =0;
	argv[argc++] = strtok(str,cut);
	while(argv[argc] = strtok(NULL,cut))
		argc++;

	if(!strcmp(argv[0], "exit"))
		return 0;
	
	if(!strcmp(argv[0],"pwd")){
		if(!getcwd(dirname,100))	
			printf("파일 위치 모름\n");
		else
			printf("%s\n",dirname);
	
	}
	else if(!strcmp(argv[0] ,"cd")){
		if(argc ==1)
		{
			chdir(getenv("HOME"));
			getcwd(dirname,40);
			printf("현재 %s에 있습니다. \n ",dirname);
		}
		else if(argc ==2)
		{
			if(chdir(argv[1])){
				printf("%s",argv[1]);
				printf("디렉토리 없음\n" );
			}
				else 
				printf("이동완료 \n");
		}



	}
	else if(!strcmp(argv[0] , "mkdir")){
		if(argc ==1){
		printf("mkdir 빈칸\n" );
		}
		
		else if(argc ==2)
		{	
		bool ret;
		ret = mkdir(argv[1],0755);
		if(ret == 0)
			printf(" susses \n");
		else
			printf("error \n");
		}
	
	}
	else if(!strcmp(argv[0],"rm")){
		if(argc ==1){
		printf("빈칸 입력\n");
		}
	
	}
	else if(!strcmp(argv[0], "cat")){
	int line_number =0;
	int cnt =2;
	
	if(argc ==1){
	printf("켓 빈칸 \n");
	}
	else{
		while(cnt <argc){
			filename =argv[cnt];
			
			fp= fopen(filename,"r");
		if(fp==NULL){
		printf("Can't open %s\n" ,filename);
		cnt++;
		}
		else{	
		while(fgets(buf,255,fp)!=NULL){
			printf("%d%s",line_number++,buf);
		}
		fclose(fp);
		cnt++;
		}
		}	
	}
	}
	}
}
