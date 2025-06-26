#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>


#define LSH_RL_BUFFSIZE 1024
#define LSH_TOK_BUFFSIZE 64
#define LSH_TOK_DELIM " \t\r\n\a"


void lsh_loop(void);
char *lsh_read_line(void);
char **lsh_split_line(char *);
int lsh_launch(char **args);
int lsh_cd(char **args);
int lsh_help(char **args);
int lsh_exit(char **args);
int lsh_execute(char **args);


char *Builtin_str[] = {
  "cd",
  "help",
  "exit"
};

int (*builtin_func[]) (char **) = {
  &lsh_cd,
  &lsh_help,
  &lsh_exit
};

int lsh_num_builtins(){
  return sizeof(Builtin_str)/sizeof(char *);
}




int main(int *argc, char *argv)
{
  //run config(if any)

  lsh_loop();

  //run the shutdown commands

  return EXIT_SUCCESS;

}

void lsh_loop(void)
{
  char *line;
  char **args;
  int status;

  do{
    printf("/> ");
    line = lsh_read_line();
    args = lsh_split_line(line);
    status = lsh_execute(args);

    free(line);
    free(args);
  }while(status);
}

char *lsh_read_line(void){
  int buffsize = LSH_RL_BUFFSIZE;
  int position = 0;
  char *buffer = (char *)malloc(sizeof(char)*buffsize);
  int c;

  if(!buffer){
    fprintf(stderr,"lsh:allocation error!\n");
    exit(EXIT_FAILURE);
  }

  while(1){
    c = getchar();

    if(c== EOF || c == '\n'){
      buffer[position] = '\0';
      return buffer;
    }else{
      buffer[position] = c;
    }
    position++;

    if(position>=buffsize){
      buffsize += LSH_RL_BUFFSIZE;
      buffer = realloc(buffer, buffsize);
      if(!buffer){
        fprintf(stderr,"lsh:allocation error!\n");
        exit(EXIT_FAILURE);
      }
    }
  }
}

char **lsh_split_line(char *line){
  int buffsize = LSH_TOK_BUFFSIZE;
  int position = 0;
  char **tokens = malloc(buffsize * sizeof(char *));
  char *token;

  if (!tokens) {
    fprintf(stderr, "lsh: allocation error\n");
    exit(EXIT_FAILURE);
  }


  token = strtok(line, LSH_TOK_DELIM);
  while(token != NULL){
    tokens[position]=token;
    position++;

    if(position>=buffsize){
      buffsize += LSH_TOK_BUFFSIZE;
      tokens = realloc(tokens, buffsize*sizeof(char *));
      if(!tokens){
        fprintf(stderr,"lsh:allocation error!\n");
        exit(EXIT_FAILURE);
      }
    }

      token = strtok( NULL, LSH_TOK_DELIM );
    
  }
  tokens[position] = NULL;
  return tokens;
}

int lsh_launch(char **args)
{
  pid_t pid, wpid;
  int status;

  pid = fork();
  if(pid == 0){
    //Child Process
    if(execvp(args[0], args) == -1){
      perror("lsh");
    }
    exit(EXIT_FAILURE);
  }else if(pid<0)
  {
    perror("lsh");
  }else{
    do{
      wpid = waitpid(pid, &status, WUNTRACED);
    }while(!WIFEXITED(status) && !WIFSIGNALED(status));
  }

  return 1;
}

int lsh_cd(char **args){
  if(args[1]==NULL){
    fprintf(stderr,"lsh:expectedn argument to \"cd\".\n");
  }else{
    if(chdir(args[1]) != 0){
      perror("lsh");
    }
  }
  return 1;
}

int lsh_help(char **args){
  int i;
  printf("The only builtin commands for now are:");
  for(i=0;i<lsh_num_builtins();i++){
    printf("%s \n",Builtin_str[i]);
  }

  printf("Use the man command for information on other commands.\n");
  return 1;
}

int lsh_exit(char **args){
  return 0;
}

int lsh_execute(char **args){
  int i;

  if(args[0]==NULL){
    return 1;
  }
  for(i=0;i<lsh_num_builtins();i++){
    if(strcmp(args[0],Builtin_str[i])==0){
      return (builtin_func[i])(args);
    }
  }

  return lsh_launch(args);
}

