#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdbool.h>
#define MAX_INPUT 1024
#define MAX_ARGS 16
#define SHELL_NAME "shell"
#define PROMPT SHELL_NAME "# "
void ashela_loop();
char **parse_input(char *input, int *argv); 
void free_args(char **args);
int argv=0;
int main(){
	ashela_loop();
	return 0;
} 
int pwd(char **args);
int cd(char **args);
int ls(char **args);
int rm(char **args);
int mv(char **args);
int fetchbox(char **args); 
int touch(char **args);
int do_mkdir(char **args);
int is_builtin(char *cmd);
int run_builtin(char *cmd,char **args);
int builtin_count();

void ashela_loop(){
	char input[MAX_INPUT];
	char **args;
	int arvgv=0;

	while (1) {
		printf("%s",PROMPT);
		if (!fgets(input, MAX_INPUT, stdin)) {
			printf("\n");
			break;
		}
		input[strcspn(input,"\n")]=0;
		if(strlen(input)==0)continue;
		args=parse_input(input,&argv);
		if(!args || !args[0]){ continue;}
		if(strcmp(args[0],"exit")==0) { return; }
		if(strcmp(args[0],"clear")==0) 
		{system("clear"); continue;}
		//builtin fuser
		if(is_builtin(args[0])){
			run_builtin(args[0],args);
			continue;
		}
		else { printf("Command not found: %s\n",args[0]);}
	} 
}
char **parse_input(char *input, int *argv){

	char **args = malloc(MAX_ARGS *  sizeof(char*));
	char *token;
	if(args==NULL)return NULL;
	int i = 0;
	token = strtok(input," ");
	while(token!=NULL && i < MAX_ARGS-1)
	{
		args[i++]=strdup(token);
		token = strtok(NULL," ");
	}
	args[i]=NULL;
	*argv = i;
	return args;

} 

void free_args(char **args){
	if(!args)return;

for(int i=0;args[i];i++){
	free(args[i]);

}
free(args);
}

typedef struct { //we create a struct definition called by builtin_t
char *name;
int (*func)(char **args);
}builtin_t;


builtin_t builtins[]={
	{"pwd",pwd},
	{"cd",cd},
	{"ls",ls},
	{"touch",touch},
//	{"rm",rm},
	{"mkdir",do_mkdir},
//	{"mv",mv},
//	{"cp",cp}
	{"fetchbox",fetchbox}
};
int builtin_count() {
    return sizeof(builtins) / sizeof(builtin_t);
}

int is_builtin(char *cmd){//returns  1 when true 
	for(int i = 0; i < builtin_count();i++)
	{
		if(strcmp(cmd,builtins[i].name)==0){ 
			return 1;}
	}
	return 0;
}
int run_builtin(char *cmd , char **args){
	for(int i = 0; i < builtin_count();i++)//this step probably not neccessary you can run directly
	{
		if(strcmp(cmd,builtins[i].name)==0){ 
		return builtins[i].func(args);
		}
	}
	return 0;
}

//BUILTIN FUNCTIONS 

int pwd(char **args){
	char cwd[1024];
	char *env_PWD = getenv("PWD");
	if(args[1]==NULL || strcmp(args[1],"-p") == 0 || strcmp(args[1],"-P") == 0)
	{
		if(getcwd(cwd,sizeof(cwd))!=NULL)
		{
			printf("%s\n",cwd);
			return 0;
		}
		else { perror( "pwd");}
	}

	if(strcmp(args[1],"-l") == 0 || strcmp(args[1],"-L") == 0)
	{
		if(env_PWD!=NULL)
		{ 	
			printf("%s\n",env_PWD); 
			return 0; 
		}
		else if(getcwd(cwd,sizeof(cwd))!=NULL) 
		{
			printf("%s\n",cwd);
			return 0;
		}
		else { perror("pwd");}
	}
	fprintf(stderr,"pwd: invalid option %s\n",args[1]);
	return 0;
}
/* ls list files .
 * no args ignore .files
 * -l print with author . Long list format
 *  -a print all, dont ignore  .files 
 *  */
#include<dirent.h>
int ls(char **args){ 
	char cwd[1024];
	if(getcwd(cwd,sizeof(cwd))==NULL){perror("Problem accuared by getting working directory");return 1;}
	DIR *dir = opendir(cwd);
	struct dirent *entry;
	//ls no args
	if(args[1]==NULL)
	{
		while( (entry = readdir(dir))!=NULL)
		{
			if( entry->d_name[0]=='.'){continue;} 
			printf("%s  ",entry->d_name);
		 }
	}
	
	//ls -a 
	else if(strcmp(args[1],"-a")==0){
		while( (entry = readdir(dir))!=NULL){
		
			printf("%s  ",entry->d_name);
		}}
	printf("\n");
	return 0;}

	int cd(char **args){
		if(args[1]==NULL){return 0;}
		const char *path = args[1];
		if(chdir(path)!=0)
		{
			perror("An error occured. cd: \n");
			return 1;
		}

		return 0;}
//is_file_exist function is checking file name is exist.
int is_file_exist(char *filename) 
{
	FILE *fptr;
	if((fptr = fopen (filename,"r")))
	{fclose(fptr);
		return 0;//File is exist

	}
	return 1;//File not exist
}
int touch(char **args)
{


	if(argv==1){return 0;}
	FILE *fptr;
	for(int i=1;i<argv;i++)
	{
		if(!is_file_exist(args[i])){printf("%s%s\n",args[i]," is already exist");continue;}
		fptr = fopen(args[i],"w");
	}
	return 0;}


	bool is_folder_exist(const char *name) {
		struct stat st;
		return stat(name, &st) == 0;
	}
int do_mkdir(char **args)
{
	if(args[1]==NULL){printf("%s\n","mkdir: missing argumant");return 1;}
	for(int i=1;i<argv;i++)
	{
	           if(is_folder_exist(args[i])){printf("%s%s\n",args[i]," is already exist");continue;}
		   if(mkdir(args[i],0755)==-1){perror("an error occured while creating folder");}
		
	}
return 0;}












//I will separate fetchbox into another file in the future

#include <sys/utsname.h>
#include <unistd.h>
#include <sys/sysinfo.h>
int fetchbox(char** args)
{
	struct utsname uts;
	struct sysinfo info;
	char hostname[256];
	FILE *cpuinfo;
	char cpu_model[128];

	char s_TotalRam[64];
	char s_FreeRam[64];
	char s_UsageRam[64];
	char s_TotalSwap[64];
	char s_Uptime[128];
	

	unsigned long total_ram;
	unsigned long free_ram;
	unsigned long ram_usage;
	unsigned long total_swap;
	unsigned long uptime;

	//Hostname
	gethostname(hostname,sizeof(hostname));
	//Kernel info
	uname(&uts);
	sysinfo(&info);
	total_ram=info.totalram/(1024*1024);
	free_ram=info.freeram/(1024*1024);
	ram_usage=total_ram-free_ram;
	total_swap=info.totalswap/(1024*1024);
	uptime=info.uptime/60;

	snprintf(s_TotalRam,sizeof(s_TotalRam),"%lu MB",total_ram);
	snprintf(s_FreeRam,sizeof(s_FreeRam),"%lu MB",free_ram);
	snprintf(s_UsageRam,sizeof(s_UsageRam),"%lu MB",ram_usage);
	snprintf(s_TotalSwap,sizeof(s_TotalSwap),"%lu Swap",total_swap);
	snprintf(s_Uptime,sizeof(s_Uptime),"%lu Min",uptime);


	cpuinfo = fopen("/proc/cpuinfo","r");	
	if(cpuinfo)
	{
		char line[256];
		while(fgets(line,sizeof(line),cpuinfo))
		{
			if(strncmp(line,"model name",10) == 0)
			{
				char *colon = strchr(line,':');
				if(colon)
				{
					strncpy(cpu_model,colon+2,(sizeof(cpu_model)-1));
					cpu_model[strcspn(cpu_model,"\n")]=0;
					break;
				}

			}

		}
	}

	fclose(cpuinfo);

 const char *template =
    "\033[1;36m     // \\\\      \033[0m   \033[1;32m%s\033[0m\n"
    "\033[1;36m    //   \\\\     \033[0m   Kernel : %s\n"
    "\033[1;36m   //     \\\\    \033[0m   CPU    : %s\n"
    "\033[1;36m    \\o/  \\o/    \033[0m   Memory    : %s/%s\n"
    "\033[1;36m   _/   ^  \\\\_  \033[0m   Swap    :%s\n"
    "\033[1;36m  | \\  '-'  / | \033[0m   Uptime    :%s\n";

	char output[2048];
 snprintf(output, sizeof(output), template,
		 hostname,
		 uts.release,
		 cpu_model[0] ? cpu_model : "Unknown",
             s_UsageRam,s_TotalRam,s_TotalSwap,s_Uptime);
	if(args[1]==NULL)
	{
		printf("%s",output);
		return 0;
	}
	if(strcmp(args[1],"--no-art")==0)
	{
		printf("%s",output);
		return 0;
	}
	fprintf(stderr,"fetchbox: this is not an option:%s\n",args[1]);
	return 1;
}

