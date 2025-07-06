#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <stdbool.h>

#define MAXBUFFERSIZE 512
#define MAXSYMBOLSIZE 50
#define SECTIONNAMESIZE 30
#define SYMTABSIZE 50
#define RELTEXTSIZE 50
#define RELDATASIZE 50
#define MAXLINESIZE 50
#define MAXDATASECTIONSIZE 100
#define MAXTEXTSECTIONSIZE 100
#define FINALEXECUTABLESIZE 1024

struct symtab{
	char symbol[MAXSYMBOLSIZE];
	int offset;
	char definition;
	char section[SECTIONNAMESIZE];
}typedef symtab;

struct reltext{
	char symbol[MAXSYMBOLSIZE];
	int offset;
}typedef reltext;

struct reldata{
	char symbol[MAXSYMBOLSIZE];
	int offset;
}typedef reldata;

reltext obj1_reltext[RELTEXTSIZE];
reltext obj2_reltext[RELTEXTSIZE];
reltext obj3_reltext[RELTEXTSIZE];

reldata obj1_reldata[RELDATASIZE];
reldata obj2_reldata[RELDATASIZE];
reldata obj3_reldata[RELDATASIZE];

symtab obj1_symtab[SYMTABSIZE];
symtab obj2_symtab[SYMTABSIZE];
symtab obj3_symtab[SYMTABSIZE];

struct Obj_Info{
	int obj1_symcount;
	int obj2_symcount;
	int obj3_symcount;
	int obj1_reldatacount;
	int obj2_reldatacount;
	int obj3_reldatacount;
	int obj1_reltextcount;
	int obj2_reltextcount;
	int obj3_reltextcount;
}typedef Obj_Info;

Obj_Info Obj_Files;
int main_count = 0;
char FinalExecutable[FINALEXECUTABLESIZE];

int SymtabCreation(FILE* fd,int flag){
	char line[MAXLINESIZE];
	memset(line,0,sizeof(line));
	int index = 0;
	fgets(line,sizeof(line),fd);
	while(strstr(line,".section .text") == NULL){
		if(strstr(line,".symtab") != NULL){
			fgets(line,sizeof(line),fd);
			continue;
		}
		else if(strlen(line) != 1){
			char* tmp = line;
			if(strstr(line,"main") != NULL){
				++main_count;
			} 
			if(flag == 1){
				sscanf(line,"%s %c %s %d",obj1_symtab[index].symbol,&obj1_symtab[index].definition, obj1_symtab[index].section,&obj1_symtab[index].offset);
			}
			else if(flag == 2){
				sscanf(line,"%s %c %s %d",obj2_symtab[index].symbol,&obj2_symtab[index].definition, obj2_symtab[index].section,&obj2_symtab[index].offset);
			}
			else{
				sscanf(line,"%s %c %s %d",obj3_symtab[index].symbol,&obj3_symtab[index].definition,obj3_symtab[index].section,&obj3_symtab[index].offset);
			}	
		++index;
		}
		else{
			break;
			}
		fgets(line,sizeof(line),fd);
	}
return index;
}

int ReldataCreation(FILE* fd,int flag){
	char line[MAXLINESIZE];
	memset(line,0,sizeof(line));
	int index = 0;
	fgets(line,sizeof(line),fd);
	while(strstr(line,".section .rel.data") == NULL){
			fgets(line,sizeof(line),fd);
	}
	while(fgets(line,sizeof(line),fd) != NULL && strlen(line) != 1){
		if(strlen(line) <= 1){
			return index;
		}
		char* tmp = line;
		if(flag == 1){
		sscanf(line,"Offset: %d Symbol: %s",&obj1_reldata[index].offset,obj1_reldata[index].symbol);
		}
		else if(flag == 2){
		sscanf(line,"Offset: %d Symbol: %s",&obj2_reldata[index].offset,obj2_reldata[index].symbol);
		}
		else{
		sscanf(line,"Offset: %d Symbol: %s",&obj3_reldata[index].offset,obj3_reldata[index].symbol);
		}
		++index;
		
	}
return index;	
}

int ReltextCreation(FILE* fd, int flag){
	char line[MAXLINESIZE];
	memset(line,0,sizeof(line));
	int index = 0;
	fgets(line,sizeof(line),fd);
	while(strstr(line,".section .rel.text") == NULL){
			fgets(line,sizeof(line),fd);
	}
	while(fgets(line,sizeof(line),fd) != NULL && strlen(line) != 1){
		if(strlen(line) <= 1){
			return index;
		}
		char* tmp = line;
		if(flag == 1){
			sscanf(line,"Offset: %d Symbol: %s",&obj1_reltext[index].offset,obj1_reltext[index].symbol);
		}
		else if(flag == 2){
			sscanf(line,"Offset: %d Symbol: %s",&obj2_reltext[index].offset,obj2_reltext[index].symbol);
		}
		else{
			sscanf(line,"Offset: %d Symbol: %s",&obj3_reltext[index].offset,obj3_reltext[index].symbol);
		}
		++index;
	}
return index;	
}

void SymbolCheck(){
    for (int index = 0; index < Obj_Files.obj1_symcount; ++index){
        if (obj1_symtab[index].definition == 'D') {
            for (int index1 = 0; index1 < Obj_Files.obj2_symcount; ++index1){
                if (strcmp(obj1_symtab[index].symbol, obj2_symtab[index1].symbol) == 0 && obj2_symtab[index1].definition == 'D'){
                    printf("Linek:\nRedefintion of symbol -> %s",obj1_symtab[index].symbol);
					exit(EXIT_FAILURE);
                }
            }
            for (int index1 = 0; index1 < Obj_Files.obj3_symcount; ++index1){
                if (strcmp(obj1_symtab[index].symbol, obj3_symtab[index1].symbol) == 0 && obj3_symtab[index1].definition == 'D'){
                    printf("Linek:\nRedefintion of symbol -> %s",obj1_symtab[index].symbol);
					exit(EXIT_FAILURE);
                }
            }
        }
    }
    for (int index = 0; index < Obj_Files.obj2_symcount; ++index){
        if (obj2_symtab[index].definition == 'D'){
            for (int index1 = 0; index1 < Obj_Files.obj3_symcount; ++index1){
                if (strcmp(obj2_symtab[index].symbol, obj3_symtab[index1].symbol) == 0 && obj3_symtab[index1].definition == 'D') {
					printf("Linek:\nRedefintion of symbol -> %s",obj2_symtab[index].symbol);
					exit(EXIT_FAILURE);
                }
            }
        }
    }
return;
}

void CheckExtern(){
	bool flag = false;
    for (int index = 0; index < Obj_Files.obj1_symcount; ++index){
		flag = false;
        if (obj1_symtab[index].definition == 'U') {
            for(int index1 = 0; index1 < Obj_Files.obj2_symcount; ++index1){
                if (strcmp(obj1_symtab[index].symbol, obj2_symtab[index1].symbol) == 0 && obj2_symtab[index1].definition == 'D'){  // check the externs of 1 obj
						flag = true;
						break;
                }
            }
			if(flag == true){
					continue;
			}
			flag = false;
            for(int index1 = 0; index1 < Obj_Files.obj3_symcount; ++index1){
                if (strcmp(obj1_symtab[index].symbol, obj3_symtab[index1].symbol) == 0 && obj3_symtab[index1].definition == 'D'){
                    flag = true;
					break;
                }
            }
			if(flag == true){
				continue;
			}
			else{
				printf("Linker:\nUndefined reference to -> Extern %s",obj1_symtab[index].symbol);
				exit(EXIT_FAILURE);
			}
        }
    }

    for (int index = 0; index < Obj_Files.obj2_symcount; ++index){
		flag = false;
        if (obj2_symtab[index].definition == 'U') {
            for(int index1 = 0; index1 < Obj_Files.obj1_symcount; ++index1){
                if (strcmp(obj2_symtab[index].symbol, obj1_symtab[index1].symbol) == 0 && obj1_symtab[index1].definition == 'D'){  // check the externs of 2 obj
						flag = true;
						break;
                }
            }
			if(flag == true){
					continue;
			}
            for(int index1 = 0; index1 < Obj_Files.obj3_symcount; ++index1){
                if (strcmp(obj2_symtab[index].symbol, obj3_symtab[index1].symbol) == 0 && obj3_symtab[index1].definition == 'D'){
                    flag = true;
					break;
                }
            }
			if(flag == true){
				continue;
			}
			else{
				printf("Linker:\nUndefined reference to -> Extern %s",obj2_symtab[index].symbol);
				exit(EXIT_FAILURE);
			}
        }
	}
    for (int index = 0; index < Obj_Files.obj3_symcount; ++index){
		flag = false;
        if (obj3_symtab[index].definition == 'U') {
            for(int index1 = 0; index1 < Obj_Files.obj1_symcount; ++index1){
                if (strcmp(obj3_symtab[index].symbol, obj1_symtab[index1].symbol) == 0 && obj1_symtab[index1].definition == 'D'){  // check the externs of 3 obj
						flag = true;
						break;
                }
            }
			if(flag == true){
					continue;
			}
            for(int index1 = 0; index1 < Obj_Files.obj2_symcount; ++index1){
                if (strcmp(obj3_symtab[index].symbol, obj2_symtab[index1].symbol) == 0 && obj2_symtab[index1].definition == 'D'){
                    flag = true;
					break;
                }
            }
			if(flag == true){
				continue;
			}
			else{
				printf("Linker:\nUndefined reference to -> Extern %s",obj3_symtab[index].symbol);
				exit(EXIT_FAILURE);
			}
        }
	}
return;
}

void ConcatSections(FILE* obj1_fd,FILE* obj2_fd, FILE* obj3_fd){
	// symtab concat
	char line[MAXBUFFERSIZE];
	memset(FinalExecutable,0,sizeof(FinalExecutable));
	strcat(FinalExecutable,".segment .symtab\n");
	for(int index = 0; index < Obj_Files.obj1_symcount; ++index){
		if(obj1_symtab[index].definition == 'D'){
			memset(line,0,sizeof(line));
			snprintf(line,sizeof(symtab), "%s %c  %s %d",obj1_symtab[index].symbol,obj1_symtab[index].definition,obj1_symtab[index].section,obj1_symtab[index].offset);
			strcat(FinalExecutable,line);
			strcat(FinalExecutable,"\n");
		}
	}
	for(int index = 0; index < Obj_Files.obj2_symcount; ++index){
		if(obj2_symtab[index].definition == 'D'){
			memset(line,0,sizeof(line));
			snprintf(line,sizeof(symtab), "%s %c  %s %d",obj2_symtab[index].symbol,obj2_symtab[index].definition,obj2_symtab[index].section,obj2_symtab[index].offset);
			strcat(FinalExecutable,line);
			strcat(FinalExecutable,"\n");
		}
	}
	for(int index = 0; index < Obj_Files.obj3_symcount; ++index){
		if(obj3_symtab[index].definition == 'D'){
			memset(line,0,sizeof(line));
			snprintf(line,sizeof(symtab), "%s %c  %s %d",obj3_symtab[index].symbol,obj3_symtab[index].definition,obj3_symtab[index].section,obj3_symtab[index].offset);
			strcat(FinalExecutable,line);
			strcat(FinalExecutable,"\n");
		}
	}

	memset(line,0,sizeof(line));
	fgets(line,sizeof(line),obj1_fd);
	while(strstr(line,".section .data") == NULL){
		fgets(line,sizeof(line),obj1_fd);           //obj1 data concat
	}
	strcat(FinalExecutable,"\n");
	strcat(FinalExecutable,line);
	while(strlen(line) > 1){
		fgets(line,sizeof(line),obj1_fd);
		strcat(FinalExecutable,line);
	}
	
	memset(line,0,sizeof(line));
	fgets(line,sizeof(line),obj2_fd);
	while(strstr(line,".section .data") == NULL){  //obj2 data concat
		fgets(line,sizeof(line),obj2_fd);
	}
	while(strlen(line) > 1){
		fgets(line,sizeof(line),obj2_fd);
		strcat(FinalExecutable,line);
	}
	
	memset(line,0,sizeof(line));
	fgets(line,sizeof(line),obj3_fd);             //obj3 text concat
	while(strstr(line,".section .data") == NULL){
		fgets(line,sizeof(line),obj3_fd);
	}
	while(strlen(line) > 1){
		fgets(line,sizeof(line),obj3_fd);
		strcat(FinalExecutable,line);
	}

	
	fseek(obj1_fd,0,SEEK_SET);
	fseek(obj2_fd,0,SEEK_SET);
	fseek(obj3_fd,0,SEEK_SET);

	memset(line,0,sizeof(line));
	fgets(line,sizeof(line),obj1_fd);
	while(strstr(line,".section .text") == NULL){
		fgets(line,sizeof(line),obj1_fd);           //obj1 text concat
	}
	strcat(FinalExecutable,line);
	while(strlen(line) > 1){
		fgets(line,sizeof(line),obj1_fd);
		strcat(FinalExecutable,line);
	}
	
	memset(line,0,sizeof(line));
	fgets(line,sizeof(line),obj2_fd);
	while(strstr(line,".section .text") == NULL){  //obj2 text concat
		fgets(line,sizeof(line),obj2_fd);
	}
	while(strlen(line) > 1){
		fgets(line,sizeof(line),obj2_fd);
		strcat(FinalExecutable,line);
	}
	
	memset(line,0,sizeof(line));
	fgets(line,sizeof(line),obj3_fd);             //obj3 text concat
	while(strstr(line,".section .text") == NULL){
		fgets(line,sizeof(line),obj3_fd);
	}
	while(strlen(line) > 1){
		fgets(line,sizeof(line),obj3_fd);
		strcat(FinalExecutable,line);
	}

return;
}

void DataSecAddrRelocation(){
		
}

int main(){
	FILE* obj1_fd = fopen("1.obj","r");
	if(obj1_fd == NULL){
		perror("obj1 open failed:\n");
		exit(EXIT_FAILURE);
	}
	FILE* obj2_fd = fopen("2.obj","r");
	if(obj2_fd == NULL){
		perror("obj2 open failed:\n");
		exit(EXIT_FAILURE);
	}
	FILE* obj3_fd = fopen("3.obj","r");
	if(obj3_fd == NULL){
		perror("obj3 open failed:\n");
		exit(EXIT_FAILURE);
	}
//	MemoryAllocation();
	Obj_Files.obj1_symcount = SymtabCreation(obj1_fd,1);			// creating symtab structs
	Obj_Files.obj2_symcount = SymtabCreation(obj2_fd,2);
	Obj_Files.obj3_symcount = SymtabCreation(obj3_fd,3);
	
	fseek(obj1_fd,0,SEEK_SET);
	fseek(obj2_fd,0,SEEK_SET);
	fseek(obj3_fd,0,SEEK_SET);

	Obj_Files.obj1_reldatacount = ReldataCreation(obj1_fd,1);	  // creation reldata
	Obj_Files.obj2_reldatacount = ReldataCreation(obj2_fd,2);
	Obj_Files.obj3_reldatacount = ReldataCreation(obj3_fd,3);

	fseek(obj1_fd,0,SEEK_SET);
	fseek(obj2_fd,0,SEEK_SET);
	fseek(obj3_fd,0,SEEK_SET);

	Obj_Files.obj1_reltextcount = ReltextCreation(obj1_fd,1); // creating reltext
	Obj_Files.obj2_reltextcount = ReltextCreation(obj2_fd,2);
	Obj_Files.obj3_reltextcount = ReltextCreation(obj3_fd,3);

	fseek(obj1_fd,0,SEEK_SET);
	fseek(obj2_fd,0,SEEK_SET);
	fseek(obj3_fd,0,SEEK_SET);

	if(main_count == 0){
		perror("Linker:\nUndefined referance to _main:\n");    // main check
		exit(EXIT_FAILURE);
	}
	else if(main_count > 1){
		perror("Linker:\nRedefinition of _main:\n");
		exit(EXIT_FAILURE);
	}else;

	SymbolCheck(); //This fuction checks the redefinition of symbols	
	CheckExtern(); //This function checks all extern variables
																			//resolution ends
																			//start relocation

	ConcatSections(obj1_fd,obj2_fd,obj3_fd);
	printf("Final executable is:\n%s\n",FinalExecutable);
	
	fclose(obj1_fd);
	fclose(obj2_fd);
	fclose(obj3_fd);
/*printf("                The 1 obj file :\n");
printf("symtabis:\n");
	for(int index = 0; index < Obj_Files.obj1_symcount; ++index){
		printf("The symbol %d is:\n",index);
		printf("%s\n",obj1_symtab[index].symbol);	
		printf("%c\n",obj1_symtab[index].definition);
		printf("%s\n",obj1_symtab[index].section);
		printf("%d\n",obj1_symtab[index].offset);
		printf("\n");
	}
	printf("Reldata is:\n");
	for(int index = 0; index < Obj_Files.obj1_reldatacount; ++index){
		printf("The symbol %d is:\n",index);
		printf("%s\n",obj1_reldata[index].symbol);	
		printf("%d\n",obj1_reldata[index].offset);
		printf("\n");
	}
	printf("reltextis:\n");
	for(int index = 0; index < Obj_Files.obj1_reltextcount; ++index){
		printf("The symbol %d is:\n",index);
		printf("%s\n",obj1_reltext[index].symbol);	
		printf("%d\n",obj1_reltext[index].offset);
		printf("\n");
	}
	printf("Textsection :\n");
	for(int index = 0 ;index < Obj_Files.obj1_textsectioncount; ++index){
		printf("The symbol %d is:\n",index);
		printf("%d\n",obj1_textsection[index].textsection_offset);
		printf("%s\n",obj1_textsection[index].symbol);
		printf("%s\n",obj1_textsection[index].instruction);
	}
	printf("Datasection \n");
	for(int index = 0; index < Obj_Files.obj1_datasectioncount; ++index){
		printf("The symbol %d is:\n",index);
		printf("%d\n",obj1_datasection[index].datasection_offset);
		printf("%d\n",obj1_datasection[index].file_offset);
		printf("%s",obj1_datasection[index].symbol);
	}
	
printf("\n\n\n\n              The 2 obj file :\n");
printf("symtabis:\n");
	for(int index = 0; index < Obj_Files.obj2_symcount; ++index){
		printf("The symbol %d is:\n",index);
		printf("%s\n",obj2_symtab[index].symbol);	
		printf("%c\n",obj2_symtab[index].definition);
		printf("%s\n",obj2_symtab[index].section);
		printf("%d\n",obj2_symtab[index].offset);
		printf("\n");
	}
	printf("Reldatais:\n");
	for(int index = 0; index < Obj_Files.obj2_reldatacount; ++index){
		printf("The symbol %d is:\n",index);
		printf("%s\n",obj2_reldata[index].symbol);	
		printf("%d\n",obj2_reldata[index].offset);
		printf("\n");
	}
	printf("reltextis:\n");
	for(int index = 0; index < Obj_Files.obj2_reltextcount; ++index){
		printf("The symbol %d is:\n",index);
		printf("%s\n",obj2_reltext[index].symbol);	
		printf("%d\n",obj2_reltext[index].offset);
		printf("\n");
	}
	printf("Textsectionis :\n");
	for(int index = 0 ;index < Obj_Files.obj2_textsectioncount; ++index){
		printf("The symbol %d is:\n",index);
		printf("%d\n",obj2_textsection[index].textsection_offset);
		printf("%s\n",obj2_textsection[index].symbol);
		printf("%s\n",obj2_textsection[index].instruction);
	}
	printf("Datasectionis \n");
	for(int index = 0; index < Obj_Files.obj2_datasectioncount; ++index){
		printf("The symbol %d is:\n",index);
		printf("%d\n",obj2_datasection[index].datasection_offset);
		printf("%d\n",obj2_datasection[index].file_offset);
		printf("%s",obj2_datasection[index].symbol);
	}	


printf("\n\n\n\n                The 3 obj file :\n");
printf("symtabis:\n");
	for(int index = 0; index < Obj_Files.obj3_symcount; ++index){
		printf("The symbol %d is:\n",index);
		printf("%s\n",obj3_symtab[index].symbol);	
		printf("%c\n",obj3_symtab[index].definition);
		printf("%s\n",obj3_symtab[index].section);
		printf("%d\n",obj3_symtab[index].offset);
		printf("\n");
	}
	printf("Reldatais:\n");
	for(int index = 0; index < Obj_Files.obj3_reldatacount; ++index){
		printf("The symbol %d is:\n",index);
		printf("%s\n",obj3_reldata[index].symbol);	
		printf("%d\n",obj3_reldata[index].offset);
		printf("\n");
	}
	printf("reltextis:\n");
	for(int index = 0; index < Obj_Files.obj3_reltextcount; ++index){
		printf("The symbol %d is:\n",index);
		printf("%s\n",obj3_reltext[index].symbol);	
		printf("%d\n",obj3_reltext[index].offset);
		printf("\n");
	}
	printf("Textsectionis :\n");
	for(int index = 0 ;index < Obj_Files.obj3_textsectioncount; ++index){
		printf("The symbol %d is:\n",index);
		printf("%d\n",obj3_textsection[index].textsection_offset);
		printf("%s\n",obj3_textsection[index].symbol);
		printf("%s\n",obj3_textsection[index].instruction);
	}
	printf("Datasectionis \n");
	for(int index = 0; index < Obj_Files.obj3_datasectioncount; ++index){
		printf("The symbol %d is:\n",index);
		printf("%d\n",obj1_datasection[index].datasection_offset);
		printf("%d\n",obj1_datasection[index].file_offset);
		printf("%s",obj1_datasection[index].symbol);
	}*/	
}


