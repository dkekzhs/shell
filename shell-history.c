#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h> 
#include <sys/types.h> //파일 타입 확인하는거
#include <dirent.h>

#define bufSize 100

const char *prompt = "Shell : " ;

int countLine(char *name){
FILE * fp;
char c;
int line =0;
fp = fopen(name, "r");
while((c=fgetc(fp))!=EOF)
	if(c=='\n')line++;
fclose(fp);
return(line);
}
int main (int argc, char * argv[]) {
	char cut[] = " \n";
	char dirname[bufSize];
	char * pathHome ,*his ="/history.txt";
	char history[30]; 
	
	FILE *fp;
	char *filename = NULL;
	char buf[255];

	DIR * dir = NULL;
	struct dirent * entry =NULL;
	int historyCnt = 0;
	int i;
	
	pathHome = getenv("HOME");
	strcpy(history , pathHome);
	strcat(history , his);
	while(1){
		char str[64];


		fputs(prompt, stdout);
		fgets(str,sizeof(str) -1 ,stdin);

		argc =0;
		argv[argc++] = strtok(str,cut);

		if(argv[0] == NULL)
			continue;

		while(argv[argc] = strtok(NULL, cut))
			argc++;
		if(argv[0] != NULL){
			fp = fopen( history ,"a+");
				historyCnt = countLine(history);		
				fprintf(fp, "%d " , historyCnt);
				for(i=0;i<argc;i++){
				fprintf(fp," %s " , argv[i]);
				}
			fprintf(fp,"\n");
			historyCnt++;
			fclose(fp);
		}
		if(!strcmp(argv[0], "exit"))		
			return 0;


		if(!strcmp(argv[0],"pwd")){
			if(!getcwd(dirname,bufSize))	
				printf("파일 위치 모름\n");

			else
				printf("%s\n",dirname);

		}
		else if(!strcmp(argv[0] ,"history")){
			fp= fopen(history,"r");
			while(fgets(buf,255,fp)!=NULL){
				printf("%s",buf);
			}
			fclose(fp);
		
		}
		else if(!strcmp(argv[0] ,"cd")){
			if(argc <=1){
				chdir(pathHome);
			}
			else if(argc ==2){
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

			else if(argc ==2){	
				if(mkdir(argv[1],0755))
					printf("폴더 생성 오류 발생 \n");
				else
					printf("폴더 생성 성공 \n");
			}

		}
		else if(!strcmp(argv[0],"rm")){
			if(argc ==1){
				printf("빈칸 입력\n");

			}

		}
		else if(!strcmp(argv[0], "cat")){
			int line_number =0;
			int cnt =1;

			if(argc <=1){
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
		else if(!strcmp(argv[0] , "ls")){
			int cnt =1;

			getcwd(dirname,bufSize);
			if((dir = opendir(dirname)) ==NULL  ){
				printf("디렉토리 오류 ");
			}
			while( (entry  =readdir(dir)) !=NULL){
				if(entry ->d_ino !=0)
					printf("%s     ", entry -> d_name);
			}

			closedir(dir);
			printf("\n");
		}



	}
}
