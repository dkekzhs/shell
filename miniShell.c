#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h> 
#include <sys/types.h> //파일 타입 확인하는거
#include <dirent.h>
#include <time.h> 
#include <grp.h>  //gid
#include <pwd.h>  //uid
#include <fcntl.h> //ps
#include <ctype.h> // ps

#define dirBufSize 200
#define hisMax 100

const char *prompt = "Shell : " ;

typedef struct strs{
	char strs[64];
}strs;

int modeGetChar(struct stat sb, char * szPer){
	int i=0;
	static short permition[9] = {0400,0200,0100,0040,0020,0010,0004,0002,0001};
	char RW[] = "rwxrwxrwx";
	
	if((sb.st_mode & S_IFMT)  ==S_IFDIR ){
		szPer[0] ='d';
		for(i=0;i<9;i++){
			if(sb.st_mode & permition[i])
				szPer[i+1] = RW[i];
			else szPer[i+1] = '-';
	}
	}
	else{
		szPer[0] ='-';
		for(i=0;i<9;i++){

		if(sb.st_mode & permition[i])
		       	szPer[i] =RW[i];
		else szPer[i] ='-';
		}
	}
	printf("%s",szPer);
	return 0;
}
int printFile(char *name){
	char buf[256];
	FILE *fp;
	int lineNumber =1;
	fp = fopen(name , "r");
	if(fp == NULL){
		printf("히스토리 없음\n");
	}
	while(fgets(buf,256,fp) != NULL){
		printf("%d  %s",lineNumber++,buf);
	}
	fclose(fp);

}
int countLine(char *name){
	FILE *fp;
	char c;
	int line =0;
	fp = fopen(name,"r");
	if(fp == NULL){
		printf("히스토리 생성\n");
	}
	else{
	while((c=fgetc(fp))!=EOF)
		if(c=='\n')line++;
	fclose(fp);
	}
	return(line);
}

int main (int argc, char * argv[]) {
	char cut[5] = " \n";
	char dirname[dirBufSize];
	char * pathHome;
	char history[64] ,* historyPath;
	char historyCntChar[5];
	FILE *fp, *copy;
	char *filename = NULL;
	
	int chkOneWrite =1;

	int aliasCnt= 0;
	char * alias[6], aliasChar[64], *aliasCom[6][3];
	strs STRING[6] ,STRING2[6];

	DIR * dir = NULL;
	struct dirent * entry =NULL;
	struct stat sb; 
	int historyCnt = 0;
	
	int i,j,historyMax;
	
	char *tty,tty_self[32],ppath[1024],statpath[1024],name[64]; //tty >32 메모리 침범 hispath
	int psid;
	DIR *psdir;
	FILE *file;
	int fd,fdSelf, chk;

	pathHome = getenv("HOME");
	
	strcpy(history , pathHome);
	strcat(history , "/history.txt");
	historyPath = history;

	
	while(1){	
		char str[64];
		chkOneWrite =1;
		fputs(prompt, stdout);
		fgets(str,sizeof(str) -1 ,stdin);
		
		if(strstr(str , "alias")){
			strcpy(aliasChar,str);
		}

		historyMax = countLine(historyPath);
		
		
		if(strstr(str,"!")){			
			argc =0;
			char command[64];
			argv[argc] = strtok(str,"!");
			int lineNumber=0;
			char hisBuf[256];
			if(argv[0] == NULL ){
				continue;
			}
			strcpy(historyCntChar, argv[argc]);
			if(atoi(historyCntChar) == 0 ){ //atoi 오류 반환 0
				printf("문자열 입력\n");
				continue;
			}
			else{ 
				historyCnt = atoi(historyCntChar);
			}
			if(historyCnt > historyMax){
				printf("히스토리 라인넘긴 숫자 \n");
				continue;
			}
			else{
				fp = fopen(historyPath,"r");
				while(fgets(hisBuf,256,fp) !=NULL){
					lineNumber++;
					if(historyCnt == lineNumber){
						printf("%d번째 줄 : %s\n",historyCnt , hisBuf);
						strcpy(command, hisBuf);
						if(strstr(command,"alias")){
						strcpy(aliasChar,command);
						}
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
			if(aliasCnt>0 && chkOneWrite ==1){
				char aliascopy[64];
				for(i=0;i<aliasCnt;i++){
					if(!strcmp(aliasCom[i][0],command)){
						strcpy(aliascopy , aliasCom[i][1]);
						argc =0;
						argv[argc++] = strtok(aliascopy,cut);
						while(argv[argc] = strtok(NULL,cut))
							argc++;
					
						 if(argv[0] != NULL && historyMax<= hisMax ){ 
							fp = fopen(historyPath,"a+");
							fprintf(fp, "%s\n", aliasCom[i][0]);
							fclose(fp);
							chkOneWrite =0;
						 }
					}
				}
			}

			 if(argv[0] != NULL && historyMax<= hisMax &&chkOneWrite ==1){ 
				fp = fopen(historyPath ,"a+");
				for(i=0;i<argc;i++){
					fprintf(fp,"%s " , argv[i]);
				}
				fprintf(fp,"\n");
				fclose(fp);
				chkOneWrite =0;
			}
			}
		}		
		else{
			argc =0;
			argv[argc++] = strtok(str,cut);
			if(argv[0] == NULL)
				continue;

			while(argv[argc] = strtok(NULL, cut))
				argc++;
			if(aliasCnt >0 && chkOneWrite ==1){	
				char aliascopy[64];
				for(i=0;i<aliasCnt;i++){
					if(!strcmp(aliasCom[i][0],str)){
						strcpy(aliascopy , aliasCom[i][1]);
						argc =0;
						argv[argc++] = strtok(aliascopy,cut);
						while(argv[argc] = strtok(NULL,cut))
							argc++;

						if(argv[0] != NULL && historyMax<= hisMax ){ 
						fp = fopen( historyPath ,"a+");
						fprintf(fp,"%s\n" , aliasCom[i][0]);
						fclose(fp);
						chkOneWrite =0;
						}
					}
				}
					
			}
			if(argv[0] != NULL && historyMax<= hisMax && chkOneWrite ==1){ 
				fp = fopen(historyPath ,"a+");
				for(i=0;i<argc;i++){
					fprintf(fp,"%s " , argv[i]);
				}
				fprintf(fp,"\n");
				fclose(fp);
				chkOneWrite =0;
			}
		}
		if(!strcmp(argv[0], "exit"))		
			return 0;


		if(!strcmp(argv[0],"pwd")){
			if(!getcwd(dirname,dirBufSize)){
				printf("파일 위치 모름\n");
				continue;
			}
			else{
				printf("%s\n",dirname);

			}
		}
		else if(!strcmp(argv[0] ,"history")){
			printFile(historyPath);
		}
		else if(!strcmp(argv[0] ,"cd")){
			if(argc <=1){
				if(chdir(pathHome) == 0){
					printf("cd  : %s\n",pathHome);
				}
				else{
					printf("cd 에러\n");
					continue;
				}
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
				printf("mkdir 폴더로 입력\n" );
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
				continue;
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
						printf(" %s ",buf);
						}
						cnt++;
						printf("\n");
					}
				}	
			}

		}
		else if(!strcmp(argv[0] , "ls")){
			if(argc ==1){
				if((dir = opendir(".")) ==NULL  ){
					printf("디렉토리 오류\n ");
				continue;
				}
			}
			else if(argc ==2){
				if((dir = opendir(argv[1])) == NULL){
					printf("그런 디렉토리 없어\n");
					continue;
				}
			}
			else{
				printf("ls 인자 오류\n");
				continue;
			}	
			while( (entry  =readdir(dir)) !=NULL){
				memset(&sb,0,sizeof(struct stat));
				lstat(entry -> d_name , &sb);
				if(S_ISDIR(sb.st_mode)){
					if(strncmp(".",entry -> d_name,1) ==0 || strncmp("..",entry -> d_name, 2)==0)
						continue;
					printf("%s   ",entry -> d_name);
				}
				else{
					if(strncmp(".",entry -> d_name,1) ==0 || strncmp("..",entry -> d_name, 2)==0)
						continue;	
					printf("%s   " ,entry-> d_name);
									
				}
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
				continue;
			}
			else if(access(argv[2] ,F_OK)!=0){
				printf("chmod 할 파일존재없음\n");
				continue;
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
				printf("권한 바꾸끼 성공\n");
			}
		
		}
		else if(!strcmp(argv[0] , "cp")){
			if(argc != 3){
				printf("cp 인자 오류 \n");
				continue;
			}
			char cpBuf[2048];
			int ret;
			
			dir = opendir(argv[1]);
			if(dir !=NULL){
				printf("폴더는 복사가 안됨\n");
			}
			else{
				int fdin,fout;
				fdin = open(argv[1] , O_RDONLY);
				if(fdin == -1){		
					printf("파일 열기 에러 \n");
					continue;
				}
				stat(argv[1] , &sb);
				fout = open(argv[2] , O_WRONLY | O_CREAT | O_TRUNC, sb.st_mode);
				if(fout ==-1){
					printf("복사파일 에러\n");
					continue;
				}	
				while((ret = read(fdin,cpBuf,2048)) >0){
						write(fout,cpBuf,ret);
					}
			
			close(fdin);
			close(fout);
			}
			closedir(dir);
		}
		else if(!strcmp(argv[0] , "alias")){
			if(argc ==1){
				printf("alias Command \n");
				for(i=0;i<aliasCnt;i++){
					printf("%s" , alias[i]);
				}
			}
			else{	int i=1,chkOneAlias = 1;
				strcpy(STRING[aliasCnt].strs,aliasChar);
				if(strstr(STRING[aliasCnt].strs ,"='") && strstr(STRING[aliasCnt].strs,"'\0") ){
					strcpy(STRING2[aliasCnt].strs,STRING[aliasCnt].strs);
					i=1;
					aliasCom[aliasCnt][0] = strtok(STRING2[aliasCnt].strs , "='");
					if(aliasCnt == 0){	
					while(aliasCom[aliasCnt][i] = strtok(NULL, "'\n"))
						i++;
					aliasCom[aliasCnt][0] = (STRING2[aliasCnt].strs+6);
						if(strstr(aliasCom[aliasCnt][0]," ") !=NULL || aliasCom[aliasCnt][1] == NULL ){
							continue;
						}
					alias[aliasCnt] = STRING[aliasCnt].strs;
					aliasCnt++;
					}

					else{ //처음 아닐 때 
 					for(j=0;j<aliasCnt;j++){ //이미 있는 단축어인지 확인
						if(!strcmp(aliasCom[j][0] , (STRING2[aliasCnt].strs+6))){ //들어온단축어와 원래있는게 있으면
							while(aliasCom[j][i] = strtok(NULL, "'\n")) //단축어 자리에 명령어만 바꿈
								i++;
							
							if(strstr(aliasCom[j][0]," ") !=NULL || aliasCom[j][1] == NULL ){
								continue;
							}
							strcpy(STRING[j].strs,aliasChar);
							chkOneAlias =0;
						}
							
						
					}
					if(chkOneAlias == 1){
						i=1;							
						while(aliasCom[aliasCnt][i] = strtok(NULL, "'\n"))
							i++;
						aliasCom[aliasCnt][0] = (STRING2[aliasCnt].strs+6);
						if(strstr(aliasCom[aliasCnt][0]," ") !=NULL || aliasCom[aliasCnt][1] == NULL ){
							continue;
						}
						alias[aliasCnt] = STRING[aliasCnt].strs;
						if(aliasCnt <5){
							aliasCnt++;
						}
						else{
							continue;
						}
					}
				
					}
				
				}
			}			
		}
		
		else if(!strcmp(argv[0] ,"ln")){
			if(argc != 3){
				printf("%s [파일],[뉴파일]으로 입력\n",argv[0]);
				continue;
			}
			else if(access(argv[1], F_OK)!=0){
				printf("%s 파일 없음\n",argv[1]);
				continue;
			}
			else if(link(argv[1],argv[2])<0){
				printf("링크 에러\n");
			}
			else{
				printf("링크 성공\n");
			}
		}
		else if(!strcmp(argv[0] ,"stat")){
			if(argc != 2 ){
				printf("Usage : %s [파일이름]\n",argv[0]);
				continue;
			}
			if(lstat(argv[1] , &sb) ==-1){
				printf("파일 오류\n");
				continue;
			}
			printf("File : %s \n", argv[1]);
			printf("size : %lld     ", (long long)sb.st_size);
			printf("Blocks : %lld      " , (long long ) sb.st_blocks);
			printf("I/O block : %ld    ", (long)sb.st_blksize);
			switch(sb.st_mode & S_IFMT){
				case S_IFBLK : printf("bloack device\n"); break;
				case S_IFCHR : printf("character device\n"); break;
				case S_IFDIR : printf("directiory\n"); break;
				case S_IFIFO : printf("FIFO/pipe\n"); break;
				case S_IFLNK : printf("symlink\n"); break;
				case S_IFREG : printf("regular file\n"); break;
				case S_IFSOCK : printf("socket\n"); break;
				default : printf("unknown?\n"); break;
			}
			char modeChar[30];
			memset(modeChar,0x00,sizeof(modeChar));
			modeGetChar(sb,modeChar);
			printf("Device : %ld     " ,(long)sb.st_dev);	
			printf("I-node : %ld     ", (long) sb.st_ino);
			printf("Link : %ld\n     ", (long) sb.st_nlink);
			printf("Access : %lo", (unsigned long) sb.st_mode);
			printf("Uid : ( %s, %d)", getpwuid(sb.st_uid) -> pw_name,sb.st_uid);
			printf("Gid : ( %s, %d)\n" ,(getgrgid(sb.st_gid)) -> gr_name,sb.st_gid);
			printf("Last status change : %s" ,ctime(&sb.st_ctime));
			printf("Last file accress : %s" ,ctime(&sb.st_atime));
			printf("Last file modification  : %s" ,ctime(&sb.st_mtime));
		}
		else if(!strcmp(argv[0] ,"ps")){
			if(argc >=2 ){
				printf("ps 인자 오류 \n");
			}
			else if(argc == 1){
			printf("PID   TTY   CMD\n");
			psdir = opendir("/proc");
			fdSelf = open("/proc/self/fd/0",O_RDONLY);
			sprintf(tty_self,"%s",ttyname(fdSelf));
			close(fdSelf);
			while((entry = readdir(psdir)) != NULL){
				chk=1;
				for(i=0;entry->d_name[i];i++){
					if(!isdigit(entry->d_name[i])){ // 프로세스가 아닐때 
						chk =0;
						break;
					}
				}
				 if(chk ==1){ 
					sprintf(ppath,"/proc/%s/fd/0" ,entry ->d_name);
					fd = open(ppath,O_RDONLY);
					tty =ttyname(fd);
					if(tty !=NULL && strcmp(tty,tty_self) == 0){
						sprintf(statpath,"/proc/%s/stat",entry ->d_name);
						file = fopen(statpath,"r");	
						if(file == NULL){
							printf("프로세스 오픈 오류\n");
						}
						else{	
							fscanf(file,"%d %s",&psid , name);
							name[strlen(name)-1] = '\0';
							printf("%d  %s  %s\n",psid,tty,name+1);
						
						}

							fclose(file);
						close(fd);
					}		

				 }
				
			}
			closedir(psdir);
			}
		}
	}	
}





