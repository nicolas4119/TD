#include <sys/types.h>
#include <sys/socket.h>
#include <linux/in.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

int main(int argc, char **argv)
{
	struc sockaddr_in SockAdr;
	int idSocket,fdsocket,taille;
	idSocket=socket(AF_INET,SOCK_STREAM,0);
	SockAdr.sin_family=AF_UNIX;
	SockAdr.sin_port=htons(PORT);
	SockAdr.sin_add_.s_addr=htonl(INADDR_ANY);
	if(bind(idSocket,&SockAdr,sizeof(SockAdr)) == -1)
	{

	}
	listen(idSocket,10)
	while(1)
	{
		taille=sizeof(struck sockaddr_in);
		fdsocket=accept(idSocket,&sockAdr,&taille);
		//traiter la connexion
	
	}
}
