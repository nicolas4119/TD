#include "shell_fct.h"

int *val_fork=NULL, nb_processus=0;

void watchdog()
{
	int i;
	for(i=0;i<nb_processus;i++){
		printf("Le chien de garde tue le processus %d \n", val_fork[i]);
    		kill(val_fork[i],SIGKILL);
	}
}
int exec_commande(cmd* ma_cmd)
{

    int fd,j,i,**tube;

	if(!strcmp(ma_cmd->cmd_initiale,"exit"))
		return MYSHELL_FCT_EXIT;

	nb_processus=(ma_cmd->nb_cmd_membres);//nb_processus correspond au nombre de commandes à executer (c'est à dire au nombre de fils à créer)

	val_fork=(int *)malloc(sizeof(int)*(nb_processus));
    tube=(int **)malloc(sizeof(int*)*(nb_processus-1));
    for(i=0; i<nb_processus-1; i++)
    {
        tube[i]=(int*)malloc(sizeof(int)*2);
    }


    for(i=0 ; i<nb_processus-1 ; i++)
    {
        pipe(tube[i]);
    }

    for(i=0 ; i< nb_processus ; i++)
    {
        val_fork[i]=fork();
        if(val_fork[i]==0)//si je suis dans le fils
        {
            for(j=0 ; j< nb_processus-1 ; j++)
                if(j!=i && j!=i-1)
                {
                    close(tube[j][0]);
                    close(tube[j][1]);
                }
            if(i!=0)//on redirige l'entrée sauf pour le premier
            {
                close(tube[i-1][1]);
                dup2(tube[i-1][0],0); //on redirige l'entrée standard (on lit le précédent)
                close(tube[i-1][0]);
            }
            else if((ma_cmd->redirection)[i][STDIN]!=NULL)//si on est sur le premier on vérifie s'il y  aune redirection d'entrée
            {
                fd=open((ma_cmd->redirection)[i][STDIN],O_RDONLY,0666);
                dup2(fd,0);
                close(fd);
            }

            if(i!=nb_processus-1)//on redirige la sortie pour tous les processus sauf pour le dernier
            {
                close(tube[i][0]);
                dup2(tube[i][1],1);//on redirige la sortie standard (on écrit pour le suivant)
                close(tube[i][1]);
            }
            else
            {
            	if((ma_cmd->redirection)[i][STDOUT]!=NULL)
            	{
            	    if(ma_cmd->type_redirection[i][STDOUT]==OVERWRITE)
                        fd=open((ma_cmd->redirection)[i][STDOUT],O_WRONLY|O_CREAT| O_TRUNC,0666);
                    else
                        fd=open((ma_cmd->redirection)[i][STDOUT],O_WRONLY|O_APPEND|O_CREAT,0666);
                    dup2(fd,1);
                    close(fd);
            	}
            	if((ma_cmd->redirection)[i][STDERR]!=NULL)
            	{
            		if(ma_cmd->type_redirection[i][STDERR]==OVERWRITE)
                        fd=open((ma_cmd->redirection)[i][STDERR],O_WRONLY|O_CREAT| O_TRUNC,0666);
                    else
                        fd=open((ma_cmd->redirection)[i][STDERR],O_WRONLY|O_APPEND|O_CREAT,0666);
                    dup2(fd,2);
                    close(fd);
            	}
            }

            if(execvp(ma_cmd->cmd_args[i][0],ma_cmd->cmd_args[i])<0)//on execute la ieme commande
		{
			printf("Erreur lors de l'exécution de la commande %s\n",ma_cmd->cmd_args[i][0]);
			exit(1);
		}
        }

    }

    for(j=0 ; j< nb_processus-1 ; j++)
    {
        close(tube[j][0]);
        close(tube[j][1]);
    }

signal(SIGALRM,watchdog);
alarm(TIMEOUT);





    waitpid(val_fork[nb_processus-1], NULL, 0);
	alarm(0);
    add_history(ma_cmd->cmd_initiale);

	free(val_fork);
    for(i=0; i<nb_processus-1; i++)
    {
        free(tube[i]);
    }
    free(tube);


    return MYSHELL_CMD_OK;
}


