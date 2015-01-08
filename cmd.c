#include "cmd.h"

void supprime_espaces_fin(char * s)
{
    int i=0;
    while(s[++i]!=0);
    while(s[--i]==' ')
        s[i]='\0';
}

void init(cmd *c)
{
    c->cmd_initiale=NULL;
    c->cmd_membres=NULL;
    c->nb_cmd_membres=0;
    c->cmd_args=NULL;
    c->nb_args=NULL;
    c->redirection=NULL;
    c->type_redirection=NULL;
}


void aff_args(cmd *c)
{
    int i,j;
    for(i=0 ; i< c->nb_cmd_membres ; i++)
    {
        printf("Commande %d : %s\n",i,(c->cmd_args)[i][0]);
        for(j=0 ; j<c->nb_args[i] ; j++)
            printf("Argument %d: %s\n",j,(c->cmd_args)[i][j]);
    }
}

void free_args(cmd *c)
{
    int i,j;
    for(i=0 ; i< c->nb_cmd_membres ; i++)
    {
        for(j=0 ; j<(c->nb_args)[i]-1 ; j++) // on ne free pas le dernier car il est forcément NULL
            free((c->cmd_args)[i][j]);
        free((c->cmd_args)[i]);
    }
    free(c->cmd_args);
	free(c->nb_args);
}

//Remplit les champs cmd_args et nb_args
void parse_args(cmd *c)
{
    int i,j,cpt=2;//on a au moins 2 arguments : le nom de la commande et NULL
    char *tmp,*ptr_tmp;

    c->cmd_args=(char ***)malloc(sizeof(char **)*c->nb_cmd_membres);//on alloue cmd_args
    c->nb_args=(unsigned int *)malloc(sizeof(unsigned int)*c->nb_cmd_membres);

    for(i=0; i<c->nb_cmd_membres ; i++)
    {
        tmp=strdup(c->cmd_membres[i]);
        ptr_tmp=tmp;
        //on remplace les espaces par des caracteres de fin de chaine
        for(j=0 , cpt=2; j<strlen(c->cmd_membres[i]) ; j++)
        {
            if(c->cmd_membres[i][j]==' ')
            {
                tmp[j]='\0';
                if(c->cmd_membres[i][j+1]!=' ' && c->cmd_membres[i][j+1]!='\0' &&c->cmd_membres[i][j+1]!='<' && c->cmd_membres[i][j+1]!='>')//si c'est un espace en trop on ne le compte pas
                    cpt++;
            }
            if(c->cmd_membres[i][j]=='<' || c->cmd_membres[i][j]=='>')
                break;
        }

        (c->nb_args)[i]=cpt;

        (c->cmd_args)[i]=(char **)malloc(sizeof(char *)*(cpt));//on alloue cmd_args[i]

        for(j=0; j<cpt-1 ; j++)
        {
            if(*tmp!='\0')
                c->cmd_args[i][j]=strdup(tmp);
            tmp+=strlen(tmp)+1;
        }
        c->cmd_args[i][j]=NULL;

        free(ptr_tmp);
    }
}

//Remplit les champs cmd_initial,cmd_membres et nb_cmd_membres
void parse_membres(char *chaine,cmd *ma_cmd)
{
    char *tmp=strdup(chaine), *ptr_tmp=tmp;
    unsigned int i=0,cpt=1;//cpt=1 car on aura au moins 1 membre
    ma_cmd->cmd_initiale=strdup(chaine);//duplique la chaine initale

    //on remplace les | par des caracteres de fin de chaine
    for(i=0; i<strlen(chaine) ; i++)
        if(chaine[i]=='|')
        {
            tmp[i]='\0';
            cpt++;
        }

    ma_cmd->nb_cmd_membres=cpt;
    ma_cmd->cmd_membres=(char **)malloc(sizeof(char *)*cpt);

    for(i=0; i<cpt ; i++)
    {
        while(*tmp==' ') tmp++;//on supprime les espaces en debut de chaine
        ma_cmd->cmd_membres[i]=strdup(tmp);
        tmp+=strlen(tmp)+1;
    }

    free(ptr_tmp);
}

void aff_membres(cmd *ma_cmd)
{
    int i;
    for(i=0 ; i< ma_cmd->nb_cmd_membres ; i++)
    {
        printf("Membre %d : %s\n",i,(ma_cmd->cmd_membres)[i]);
    }
}

void free_membres(cmd *ma_cmd)
{
    int i;
    for(i=0 ; i< ma_cmd->nb_cmd_membres ; i++)
    {
        free((ma_cmd->cmd_membres)[i]);
    }
    free(ma_cmd->cmd_membres);
	free(ma_cmd->cmd_initiale);
}

int parse_redirection(unsigned int i,cmd *c)
{
    char* str;
    int cpt=0;

    //si le membre d'existe pas on retourne -1
    if(i>=c->nb_cmd_membres)
        return -1;

    //recherche de redirection d'entrée
    str=strstr(c->cmd_membres[i],"<");
    if(str!=NULL)
    {
        //on avance jusqu'à la redirection en question dans la chaine str
        while(str[cpt]==' ' || str[cpt]=='\0' || str[cpt]=='<')
            cpt++;
        supprime_espaces_fin(str);
        c->redirection[i][STDIN]=strdup(str+cpt);
    }
    else c->redirection[i][STDIN]=NULL;

    //recherche de redirection d'erreur
    str=strstr(c->cmd_membres[i],"2>");
    cpt=2;//on commence après le 2>
    if(str!=NULL)
    {
        //on avance jusqu'à la redirection en question dans la chaine str
        while(str[cpt]==' ' || str[cpt]=='\0' || str[cpt]=='>')
            cpt++;
        supprime_espaces_fin(str);
        c->redirection[i][STDERR]=strdup(str+cpt);
        c->redirection[i][STDOUT]=NULL;//car on ne peut pas avoir de redirection de sortie et d'erreur en meme temps dans un meme membre

        if(str[2]=='>') c->type_redirection[i][STDERR]=APPEND;
        else c->type_redirection[i][STDERR]=OVERWRITE;

        return 0;//pour ne pas rechercher de redirection de sortie sachant qu'il n'y en a pas
    }
    else c->redirection[i][STDERR]=NULL;

    //recherche de redirection de sortie
    str=strstr(c->cmd_membres[i],">");
    cpt=0;
    if(str!=NULL)
    {
        //on avance jusqu'à la redirection en question dans la chaine str
        while(str[cpt]==' ' || str[cpt]=='\0' || str[cpt]=='>')
            cpt++;

        supprime_espaces_fin(str);
        c->redirection[i][STDOUT]=strdup(str+cpt);

        if(str[1]=='>') c->type_redirection[i][STDOUT]=APPEND;
        else c->type_redirection[i][STDOUT]=OVERWRITE;
    }
    else c->redirection[i][STDOUT]=NULL;

    return 0;

}

void free_redirection(cmd *c)
{
    int i;
    for(i=0; i<c->nb_cmd_membres; i++)
    {
        free((c->redirection)[i][STDIN]);
        free((c->redirection)[i][STDOUT]);
        free((c->redirection)[i][STDERR]);
        free(c->redirection[i]);
        free(c->type_redirection[i]);
    }
    free(c->redirection);
    free(c->type_redirection);
}

void aff_redirection(cmd c, int i)
{
    printf("redirection[%d][STDIN]=%s\n",i,(c.redirection)[i][STDIN]);
    printf("redirection[%d][STDOUT]=%s\n",i,(c.redirection)[i][STDOUT]);
    printf("redirection[%d][STDERR]=%s\n",i,(c.redirection)[i][STDERR]);
    if((c.type_redirection)[i][STDIN]==APPEND)
        printf("type_redirection[%d][STDIN]=APPEND\n",i);
    if((c.type_redirection)[i][STDIN]==OVERWRITE)
        printf("type_redirection[%d][STDIN]=OVERWRITE\n",i);
    if((c.type_redirection)[i][STDOUT]==OVERWRITE)
        printf("type_redirection[%d][STDOUT]=OVERWRITE\n",i);
    if((c.type_redirection)[i][STDOUT]==APPEND)
        printf("type_redirection[%d][STDOUT]=APPEND\n",i);
    if((c.type_redirection)[i][STDERR]==APPEND)
        printf("type_redirection[%d][STDERR]=APPEND\n",i);
    if((c.type_redirection)[i][STDERR]==OVERWRITE)
        printf("type_redirection[%d][STDERR]=OVERWRITE\n",i);
}

void lire_cmd(cmd *c,char *ligne)
{
    int i;


    init(c);
    parse_membres(ligne,c);
    parse_args(c);

    //on alloue redirection et type_redirection
    c->redirection=(char***)malloc(sizeof(char**)*c->nb_cmd_membres);
    for(i=0 ; i<c->nb_cmd_membres ; i++)
        c->redirection[i]=(char**)malloc(sizeof(char*)*3);

    c->type_redirection=(int**)malloc(sizeof(int*)*c->nb_cmd_membres);
    for(i=0 ; i<c->nb_cmd_membres ; i++)
	{
        c->type_redirection[i]=(int*)malloc(sizeof(int)*3);
	memset(c->type_redirection[i],0,3*sizeof(int));
	}


    for(i=0 ; i<c->nb_cmd_membres ; i++)
        parse_redirection(i,c);
}
void free_elements_cmd(cmd *c)
{
    free_membres(c);
    free_args(c);
    free_redirection(c);
}
void aff_cmd(cmd *c)
{
    int i;
    aff_membres(c);
    aff_args(c);
    for(i=0 ; i<c->nb_cmd_membres ; i++)
        aff_redirection(*c,i);
}

