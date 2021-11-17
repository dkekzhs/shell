#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h> 
#include <sys/types.h> //파일 타입 확인하는거
#include <dirent.h>

#define bufSize 100

const char *prompt = "Shell : " ;
int printFile(char *name){
	char buf[255];
	FILE *fp;
	int lineNumber =0;
	fp = fopen(name , "r");
	while(fgets(buf,255,fp) != NULL){
		printf("%s ",buf);
	}
	fclose(fp);

}
int countLine(char *name){
	FILE *fp;
	char c;
	int line =0;
	fp = fopen(name,"r");
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
	char historyCntChar[5];
	FILE *fp, *copy;
	char *filename = NULL;

	DIR * dir = NULL;
	struct dirent * entry =NULL;
	struct stat st; 
	int historyCnt = 0;
	
	int i,historyMax;
	
	pathHome = getenv("HOME");
	strcpy(history , pathHome);
	strcat(history , his);
	while(1){
		char str[64];

		fputs(prompt, stdout);
		fgets(str,sizeof(str) -1 ,stdin);
		
		historyMax = countLine(history);
		if(strchr(str,'!')){			
			argc =0;
			char command[64];
			argv[argc] = strtok(str," !");
			int lineNumber=0;
			char buf[255];
			if(argv[0] == NULL)
				continue;
			strcpy(historyCntChar, argv[argc]);
			historyCnt = atoi(historyCntChar);
			if(historyCnt==0){ //atoi 오류 반환 0
				continue;
				}
			if(historyCnt >= historyMax){
				printf("히스토리 라인넘긴 숫자 \n");
				continue;
				}
			fp = fopen(history,"r");
			while(fgets(buf,255,fp) !=NULL){
				lineNumber++;
				if(historyCnt == lineNumber){
					printf("%d번째 줄 : %s\n",historyCnt , buf);
					strcpy(command, buf);
					break;
				}	
			}	
			fclose(fp);	

			argc =0;
			argv[argc++] = strtok(command,cut);
			
			if(argv[0] == NULL)
				continue;

			while(argv[argc] = strtok(NULL, cut))
				argc++;
		
			if(argv[0] != NULL && historyMax<=100){ 
				fp = fopen( history ,"a+");
				for(i=0;i<argc;i++){
					fprintf(fp,"%s " , argv[i]);
				}
				fprintf(fp,"\n");
				fclose(fp);
			}
		}		
		else{
			argc =0;
			argv[argc++] = strtok(str,cut);
			
			if(argv[0] == NULL)
				continue;

			while(argv[argc] = strtok(NULL, cut))
				argc++;
			
		
			if(argv[0] != NULL && historyMax<=100){ 
				fp = fopen( history ,"a+");
				for(i=0;i<argc;i++){
					fprintf(fp,"%s " , argv[i]);
				}
				fprintf(fp,"\n");
				fclose(fp);
			}
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
			printFile(history);
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
			int result =0;
			filename = argv[1];
			result = remove(filename);
			if(result == 0 ){
				printf("파일 삭제 성공 \n");
			}
			else{
				 printf("폴더가 비어있지않음\n");
			}

		}
		else if(!strcmp(argv[0], "cat")){
			int lineNumber =0;
			int cnt =1;
			char buf[255];
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
						while(fgets(buf,255,fp) != NULL){
						printf("%d %s ", lineNumber++,buf);
						}
						cnt++;
					}
				}	
			}

		}
		else if(!strcmp(argv[0] , "ls")){

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
		else if(!strcmp(argv[0] , "chmod")){
			char *end;
			char mode[2]  = "0";
			int modeOct =0;
			
			if(argc != 3){
				printf("chmod 파일이름 명령어8진수 순서로  입력\n");
			}
			else if(access(argv[2] ,F_OK)!=0){
				printf("chmod 할 파일존재없음\n");
			}
			
			if(strchr(argv[1],'8') || strchr(argv[1],'9')){
				printf("8진수 잘못된 숫자 입력\n");
				continue;
			}
			else{
				strcat(mode,argv[1]);
				modeOct =strtol(mode,&end,8);//오류시 변환 0
			}

			if(modeOct == 0 || modeOct >777){
				printf(" 권한은 숫자로 입력 또한 큰 숫자 입력 X\n");
				continue;	
			}

			if(chmod(argv[2],modeOct)!=0){
				printf("권한 바꾸기  실패\n");
			}
			else{
				printf("권한 바꾸끼 성공 ^^\n");
			}
		
		}
		else if(!strcmp(argv[0] , "cp")){
			if(argc != 3){
				printf("cp 인자 오류 \n");
				continue;
			}
			char buffer[1024];
			int ret;
			
			dir = opendir(argv[1]);
			if(dir !=NULL){
				printf("폴더는 복사가 안됨\n");
			}
			else{
				fp = fopen(argv[1] , "r");
				if(fp == NULL){		
					printf("파일이 없음 \n");
					continue;
				}
				copy = fopen(argv[2] , "w");
					while(ret = fread(buffer, 1 , sizeof(buffer), fp)){
						fwrite(buffer,1,ret,copy);
					}
			
			fclose(fp);
			fclose(copy);
			}
			closedir(dir);
		}
		else if(!strcmp(argv[0] , "alias")){
			printf("%s ",argv[1]);

		}	
	}
}




