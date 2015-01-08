#ifndef SHELL_FCT_H_INCLUDED
#define SHELL_FCT_H_INCLUDED

#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include "cmd.h"
#include <stdio.h>
#include <signal.h>
#define TIMEOUT 10

int exec_commande(cmd* ma_cmd);


#endif // SHELL_FCT_H_INCLUDED
