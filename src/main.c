#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define MAX_INPUT 1024
#define MAX_ARGS 16
#define SHELL_NAME "shell"
#define PROMPT SHELL_NAME "# "
void ashela_loop();
char **parse_input(char *input); 
void free_args(char **args);

int main(){
	ashela_loop();
	return 0;
} 
int pwd(char **args);
int cd(char **args);
int ls(char **args);
int rm(char **args);
int mv(char **args);
int mkdir(char **args);
int fetchbox(char **args); 

int is_builtin(char *cmd);
int run_builtin(char *cmd,char **args);
int builtin_count();

void ashela_loop(){
	char input[MAX_INPUT];
	char **args;

	while (1) {
		printf("%s",PROMPT);
		if (!fgets(input, MAX_INPUT, stdin)) {
			printf("\n");
			break;
		}
		input[strcspn(input,"\n")]=0;
		if(strlen(input)==0)continue;
		args=parse_input(input);
		if(!args || !args[0]){ continue;}
		if(strcmp(args[0],"exit")==0) { return; }
		if(strcmp(args[0],"clear")==0) 
		{system("clear"); continue;}
		if(is_builtin(args[0])){
			run_builtin(args[0],args);
			continue;
		}
		else { printf("%sCommand not found: %s\n",PROMPT,args[0]);}
	} 
}
char **parse_input(char *input){

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
	return args;

} 

void free_args(char **args){
	if(!args)return;

for(int i=0;args[i];i++){
	free(args[i]);

}
free(args);
}

typedef struct {
char *name;
int (*func)(char **args);
}builtin_t;


builtin_t builtins[]={
	{"pwd",pwd},
//	{"cd",cd},
//	{"ls",ls},
//	{"rm",rm},
//	{"mv",mv},
	{"fetchbox",fetchbox}
};
int builtin_count() {
    return sizeof(builtins) / sizeof(builtin_t);
}

int is_builtin(char *cmd){
	for(int i = 0; i < builtin_count();i++)
	{
		if(strcmp(cmd,builtins[i].name)==0){ 
			return 1;}
	}
	return 0;
}
int run_builtin(char *cmd , char **args){
	for(int i = 0; i < builtin_count();i++)
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
	return 1;
}

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


