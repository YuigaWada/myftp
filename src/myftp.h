#ifndef MYFTP_H
#define MYFTP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>

#define TCP_PORT (unsigned short)50021
#define BUF_LEN 1000
#define MAX_STR_LEN 256
#define MAX_LISTEN 10000
#define FTPH_LEN ((sizeof(struct ftph)))

// type
#define QUIT 0x01
#define PWD 0x02
#define CWD 0x03
#define LIST 0x04
#define RETR 0x05
#define STOR 0x06

#define OK 0x10
#define CMD_ERR 0x11
#define FILE_ERR 0x12
#define UNKWN_ERR 0x13
#define DATA 0x20

// code
#define CODE0 0x00
#define CODE1 0x01
#define CODE2 0x02
#define CODE3 0x03
#define CODE4 0x04
#define CODE5 0x05

// util
#define NO_ARGS 0
#define ARGS1 1
#define ARGS1or2 2
#define ARGS0or1 3

// struct
struct ftph
{
    uint8_t type;
    uint8_t code;
    uint16_t length;
    char data[0];
};

extern int max_len;

// func

void init_ftph(struct ftph *f),
    init_ftph_msg(struct ftph *f),
    print_header(struct ftph *f),
    ssend(int sock, int type, int code, char *data),
    _ssend(int sock, int type, int code, char *data, int l);

struct ftph *sread(int sock);
int is_ok(struct ftph *buf);

#define print_bar(x) printf("%c+++++++++++++++++++++++++++++++++++++++++%c\n", "\n+"[x], "\n+"[!x])
#define is_endtoken(x) ((x)[0] == '\0')
#define ok1(buf) ((buf->type == OK && buf->code == CODE1))
#define cont_data(buf) ((buf->type == DATA && buf->code == CODE1))
#define nocont_data(buf) ((buf->type == DATA && buf->code == CODE0))
#define MAX(x, y) (((x) > (y) ? (x) : (y)))

#define exit_fprintf_(message, id)    \
    do                                \
    {                                 \
        fprintf(stderr, message, id); \
        exit(EXIT_FAILURE);           \
    } while (0)

#define exit_fprintf(message)     \
    do                            \
    {                             \
        fprintf(stderr, message); \
        exit(EXIT_FAILURE);       \
    } while (0)

#define exit_perror(message) \
    do                       \
    {                        \
        perror(message);     \
        exit(EXIT_FAILURE);  \
    } while (0)

#define mem_realloc(ptr, type, size, errno)                                               \
    do                                                                                    \
    {                                                                                     \
        if ((ptr = (type *)realloc(ptr, sizeof(type) * (size))) == NULL)                  \
        {                                                                                 \
            fprintf(stderr, "myftp: cannot allocate %ldbyte. \n", sizeof(type) * (size)); \
            exit(errno);                                                                  \
        }                                                                                 \
    } while (0)

#define mem_alloc(ptr, type, size, errno)                                                 \
    do                                                                                    \
    {                                                                                     \
        if ((ptr = (type *)malloc(sizeof(type) * (size))) == NULL)                        \
        {                                                                                 \
            fprintf(stderr, "myftp: cannot allocate %ldbyte. \n", sizeof(type) * (size)); \
            exit(errno);                                                                  \
        }                                                                                 \
    } while (0)

#define read_socket(buf, sock)             \
    do                                     \
    {                                      \
        init_ftph(buf);                    \
        if (read(sock, buf, FTPH_LEN) < 0) \
        {                                  \
            exit_perror("read");           \
        }                                  \
    } while (0)

#endif
