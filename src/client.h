#include "myftp.h"

struct func_table
{
    char *name;
    int type;
    void (*func)(int argc, int sock, struct ftph *buf);
    char *help_msg;
};

extern struct func_table ftab[11];
extern char cmds[MAX_STR_LEN][MAX_STR_LEN];

void quit(int argc, int sock),
    pwd(int argc, int sock),
    get(int argc, int sock),
    cd(int argc, int sock),
    dir(int argc, int sock),
    lpwd(int argc, int sock),
    lcd(int argc, int sock),
    ldir(int argc, int sock),
    put(int argc, int sock),
    help(int argc, int sock);
