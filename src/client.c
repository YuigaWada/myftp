#include "client.h"

char cmds[MAX_STR_LEN][MAX_STR_LEN];
char cwd[BUF_LEN] = "";
char *currentdir()
{
    if ((getcwd(cwd, BUF_LEN)) == NULL)
        exit_perror("getcwd");
    return cwd;
}

struct func_table ftab[11] = {
    {"quit", NO_ARGS, quit, "quit:\n\tquit the program\n\n"},
    {"pwd", NO_ARGS, pwd, "pwd:\n\tprint current directory\n\n"},
    {"cd", ARGS1, cd, "cd path:\n\tchange current directory\n\n"},
    {"dir", ARGS0or1, dir, "dir [path]:\n\tlist current directory\n\n"},
    {"lpwd", NO_ARGS, lpwd, "lpwd:\n\tprint client current directory\n\n"},
    {"lcd", ARGS1, lcd, "lcd path:\n\tchange client current directory\n\n"},
    {"ldir", ARGS0or1, ldir, "ldir [path]:\n\tlist client current directory\n\n"},
    {"get", ARGS1or2, get, "get path1 [path2]:\n\tget the file of 'path1' from server and save to 'path2'\n\n"},
    {"put", ARGS1or2, put, "put path1 [path2]:\n\tput file of 'path1' to server and save to 'path2'\n\n"},
    {"help", NO_ARGS, help, "help:\n\tprint help message\n\n"},
    {"", 0, NULL}};

void quit(int argc, int sock)
{
    struct ftph *buf;
    ssend(sock, QUIT, CODE0, "");
    buf = sread(sock); // wait server

    if (!is_ok(buf))
        return;

    // ok
    printf("exit ...");

    close(sock);
    free(buf);
    exit(0);
}

void get(int argc, int sock)
{
    struct ftph *buf;
    char *src, *dst; // todo: パスなので修正

    src = cmds[1];
    dst = argc == 3 ? cmds[2] : src;

    ssend(sock, RETR, CODE0, src);

    // ack
    buf = sread(sock);
    if (!is_ok(buf))
    {
        return;
    }
    else if (!ok1(buf))
    {
        fprintf(stderr, "unknown or invalid response\n");
        print_header(buf);
        return;
    }

    int fd, n;
    printf("> ack => ok\n"), fflush(stdout);
    if ((fd = open(dst, O_CREAT | O_WRONLY | O_TRUNC, 0755)) < 0) // todo: 権限なかったらエラー→サーバ処理
        exit_perror("open");

    // get data
    printf("> downloading %s ---> %s ... ", src, dst), fflush(stdout);
    init_ftph(buf);
    buf->type = DATA, buf->code = CODE1;
    while (cont_data(buf))
    {
        free(buf);
        buf = sread(sock);
        if (cont_data(buf) || nocont_data(buf))
            write(fd, buf->data, sizeof(char) * ntohs(buf->length));
    }

    // end-check
    if (!nocont_data(buf))
    {
        fprintf(stderr, "unknown or invalid response\n");
        print_header(buf);
        return;
    }

    printf("fin\n");
    close(fd);
    free(buf);
}

void pwd(int argc, int sock)
{
    ssend(sock, PWD, CODE0, "");
    struct ftph *buf = sread(sock); // wait server

    if (!is_ok(buf))
        return;

    // ok
    printf("%s\n", buf->data);
    free(buf);
}
void cd(int argc, int sock)
{
    ssend(sock, CWD, CODE0, cmds[1]);

    struct ftph *buf = sread(sock); // wait server

    if (!is_ok(buf))
        return;

    // ok
    printf("change current dir: %s\n", cmds[1]);
    free(buf);
}
void dir(int argc, int sock)
{
    ssend(sock, LIST, CODE0, cmds[1]);

    // ack
    struct ftph *buf = sread(sock);
    if (!is_ok(buf))
    {
        return;
    }
    else if (!ok1(buf))
    {
        fprintf(stderr, "unknown or invalid response\n");
        print_header(buf);
        return;
    }

    // get data
    printf("[server]\n");
    init_ftph(buf);
    buf->type = DATA, buf->code = CODE1;
    while (cont_data(buf))
    {
        free(buf);
        buf = sread(sock); // ack
        if (cont_data(buf) || nocont_data(buf))
            printf("%s\n", buf->data);
    }

    // end-check
    if (!nocont_data(buf))
    {
        fprintf(stderr, "unknown or invalid response\n");
        print_header(buf);
        return;
    }
    free(buf);
}
void lpwd(int argc, int sock)
{
    printf("[client] %s\n", currentdir());
}

void lcd(int argc, int sock)
{
    if (chdir(cmds[1]) < 0)
        perror("chdir");
    printf("[client] change current dir: %s\n", currentdir());
}

void ldir(int argc, int sock)
{
    struct stat st;
    if (strlen(cmds[1]) > 0 && stat(cmds[1], &st) != 0)
    {
        perror("error");
        return;
    }

    FILE *fp;
    char cmd[BUF_LEN] = "ls -l ";
    strcat(cmd, cmds[1]);
    // ack or error
    if (strlen(cmds[1]) > 0 && !S_ISDIR(st.st_mode) && !S_ISREG(st.st_mode))
    {
        fprintf(stderr, "Not found\n");
        return;
    }

    else if ((fp = popen(cmd, "r")) == NULL)
    {
        perror("popen");
        return;
    }

    char str[BUF_LEN];
    printf("[client]\n");
    while (!feof(fp))
    {
        memset(str, 0, sizeof(char) * BUF_LEN);
        fgets(str, sizeof(char) * BUF_LEN, fp);
        printf("%s", str);
    }

    close(fp);
}

void put(int argc, int sock)
{
    int n, fd;
    struct ftph *buf;
    char *src, *dst;

    src = cmds[1];
    dst = argc == 3 ? cmds[2] : src;

    struct stat st;
    if (stat(src, &st) != 0)
    {
        perror("stat");
        return;
    }

    if (S_ISDIR(st.st_mode) && !S_ISREG(st.st_mode))
    {
        if (S_ISDIR(st.st_mode))
            fprintf(stderr, "error: is directory\n");
        else
            fprintf(stderr, "error: is invalid file\n");
        return;
    }
    else if ((fd = open(src, O_RDONLY)) < 0)
    {
        if (errno == EPERM || errno == EACCES)
        {
            fprintf(stderr, "error: no permittion\n");
            return;
        }
        fprintf(stderr, "not found: %s\n", src);
        return;
    }

    printf("> uploading %s ---> %s ...\n", src, dst);

    ssend(sock, STOR, CODE0, dst); // retr
    buf = sread(sock);             // ack

    if (!is_ok(buf))
    {
        return;
    }
    else if (!(buf->type == OK && buf->code == CODE2))
    {
        fprintf(stderr, "error: unknown or invalid response\n");
        print_header(buf);
        return;
    }

    // send file
    printf("> send file\n"), fflush(stdout);
    int code = CODE0;
    char str[BUF_LEN];
    while ((n = read(fd, str, sizeof(char) * BUF_LEN)) >= 0)
    {
        printf("> reading %d bytes\n", n), fflush(stdout);
        code = n < sizeof(char) * BUF_LEN ? CODE0 : CODE1;
        _ssend(sock, DATA, code, str, n);
        memset(str, 0, sizeof(char) * BUF_LEN);
        if (code == CODE0)
            break;
    }

    printf("> ok\n"), fflush(stdout);
    close(fd);
    free(buf);
}

void help(int argc, int sock)
{
    struct func_table *ft;
    for (ft = ftab; ft->func != NULL; ft++)
    {
        printf(ft->help_msg);
    }
}

int is_ok(struct ftph *buf) // エラー起きたらfree(buf)するので注意
{
    int type = buf->type, code = buf->code;
    if (type == OK || type == DATA)
    {
        return 1;
    }
    else if (QUIT <= type && type <= STOR)
    {
        return 1;
    }
    else if (type == CMD_ERR && code == CODE1)
    {
        fprintf(stderr, "error: syntax error\n");
        free(buf);
        return 0;
    }
    else if (type == CMD_ERR && code == CODE2)
    {
        fprintf(stderr, "error: undefined error\n");
        free(buf);
        return 0;
    }
    else if (type == CMD_ERR && code == CODE3)
    {
        fprintf(stderr, "error: protocol error\n");
        free(buf);
        return 0;
    }
    else if (type == FILE_ERR && code == CODE0)
    {
        fprintf(stderr, "error: file is not found \n");
        free(buf);
        return 0;
    }
    else if (type == FILE_ERR && code == CODE1)
    {
        fprintf(stderr, "error: no permission error\n");
        free(buf);
        return 0;
    }
    else
    {
        free(buf);
        exit_fprintf("unknown error\n");
    }
}
