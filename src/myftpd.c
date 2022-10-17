#include "server.h"

int main(int argc, char **argv)
{
    if (argc > 2)
        exit_fprintf_("Usage: %s [current-dir]\n", argv[0]);

    int alen; // size of caddress
    int sock0, sock;
    struct sockaddr_in saddress, caddress;

    struct func_table *ft;
    struct ftph *buf;
    int tmp = 1;
    mem_alloc(buf, struct ftph, 1, EXIT_FAILURE);

    if (argc == 2)
    {
        printf("Change current dir: %s\n", argv[1]);
        if (chdir(argv[1]) < 0)
            exit_perror("chdir");
    }

    printf("Prepare socket ...\n");
    // open socket
    if ((sock0 = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        exit_perror("socket");

    if (setsockopt(sock0, SOL_SOCKET, SO_REUSEADDR, (const char *)&tmp, sizeof(tmp)) < 0)
        exit_perror("setsockopt");

    // setup ip address and port
    memset(&saddress, 0, sizeof(saddress));
    saddress.sin_family = AF_INET;
    saddress.sin_port = htons(TCP_PORT);
    saddress.sin_addr.s_addr = htonl(INADDR_ANY);

    // bind
    if (bind(sock0, (struct sockaddr *)&saddress, sizeof(saddress)) < 0)
        exit_perror("bind");

    if (listen(sock0, MAX_LISTEN) != 0)
        exit_perror("listen");

    int isparent = 1;
    while (1)
    {
        if (isparent > 0)
        {
            printf("[parent] Waiting connection...\n");
            alen = sizeof(caddress);
            if ((sock = accept(sock0, (struct sockaddr *)&caddress, (socklen_t *)&alen)) < 0)
                exit_perror("accept");

            if ((isparent = fork()) < 0) // fork
            {
                exit_perror("fork");
            }
            if (isparent)
                printf("> connected!\n> forked child process\n");
        }

        if (!isparent) // child
        {
            printf(">> current dir: %s <<\n", currentdir());

            if ((buf = sread(sock)) == NULL)
            {
                printf("[child] disconnected\n");
                close(sock);
                free(buf);
                exit(0);
            }

            printf(">> Receive Message << \n"), print_header(buf);
            for (ft = ftab; ft->func != NULL; ft++)
            {
                if (ft->type == buf->type)
                {
                    (ft->func)(sock, buf);
                    break;
                }
            }
        }
        else // parent
        {
        }
    }

    close(sock0);
    free(buf);

    return 0;
}