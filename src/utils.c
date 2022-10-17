#include "myftp.h"

int max_len = 0;

void init_ftph(struct ftph *f)
{
    f->code = f->length = f->type = 0;
    // if (f->data != NULL)
    //     free(f->data);
}

void ssend(int sock, int type, int code, char *data)
{
    int l = strlen(data);
    _ssend(sock, type, code, data, l);
}

void _ssend(int sock, int type, int code, char *data, int l)
{
    struct ftph *buf;
    size_t size = sizeof(struct ftph) + sizeof(char) * l;
    if ((buf = (struct myftph *)malloc(size)) == NULL)
        exit_perror("malloc");

    init_ftph(buf);
    buf->type = type, buf->code = code;
    buf->length = htons(l);
    // printf("send\n"), print_header(buf), fflush(stdout);

    if (l > 0 && memcpy(buf->data, data, sizeof(char) * l) == NULL)
        exit_perror("memcpy");

    //  printf("send2\n"),print_header(buf), fflush(stdout);
    if (write(sock, buf, size) < 0)
        exit_perror("write");

    free(buf);
}

struct ftph *sread(int sock)
{
    int l, n, code, type;
    struct ftph *buf;
    mem_alloc(buf, struct ftph, 1, EXIT_FAILURE);

    init_ftph(buf);
    size_t size = sizeof(struct ftph);
    if ((n = read(sock, buf, size)) < 0)
        exit_perror("read");

    if (n == 0)
    {
        free(buf);
        return NULL;
    }

    // printf("read\n"), print_header(buf), fflush(stdout);

    // memo: buf->dataはケツに'\0'が入ってないのでstrの場合を考えてincrement
    l = ntohs(buf->length), max_len = MAX(l + 1, max_len);
    code = buf->code, type = buf->type;
    if ((buf = (struct ftph *)realloc(buf, (size += sizeof(char) * max_len))) == NULL)
        exit_perror("realloc");

    memset(buf->data, 0, sizeof(char) * max_len);
    if (l > 0)
    {
        buf->code = code, buf->type = type, buf->length = htons(l);
        if (read(sock, buf->data, sizeof(char) * l) < 0)
            exit_perror("read");
    }
    return buf;
}

char *print_type(int type)
{
    switch (type)
    {
    case QUIT:
        return "QUIT";
    case PWD:
        return "PWD";
    case CWD:
        return "CWD";
    case LIST:
        return "LIST";
    case RETR:
        return "RETR";
    case STOR:
        return "STOR";
    case OK:
        return "OK";
    case CMD_ERR:
        return "CMD-ERR";
    case FILE_ERR:
        return "FILE-ERR";
    case UNKWN_ERR:
        return "UNKWN-ERR";
    case DATA:
        return "DATA";
    default:
        return "UNKNOWN";
    }
}

char *print_code(int code)
{
    switch (code)
    {
    case CODE0:
        return "0";
    case CODE1:
        return "1";
    case CODE2:
        return "2";
    case CODE3:
        return "3";
    case CODE4:
        return "4";
    case CODE5:
        return "5";
    default:
        return "UNKNOWN";
    }
}

void print_header(struct ftph *f)
{
    char *type = print_type(f->type), *code = print_code(f->code);
    print_bar(0);
    printf("| type: %s\n| code: %s\n| data-length: %d bytes\n", type, code, ntohs(f->length));
    print_bar(1);
}