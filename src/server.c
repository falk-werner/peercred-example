#define _GNU_SOURCE
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <poll.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <signal.h>

#define BACKLOG 5

static bool g_shutdown_requested = false;
static void on_shutdown_requested(int signal_number)
{
    (void) signal_number;
    g_shutdown_requested = true;
}


int main(int argc, char* argv[])
{
    (void) argc;
    (void) argv;

    signal(SIGINT, &on_shutdown_requested);
    signal(SIGTERM, &on_shutdown_requested);
    signal(SIGPIPE, SIG_IGN);

    int fd = socket(AF_LOCAL, SOCK_STREAM, 0);
    if (0 > fd)
    {
        fprintf(stderr, "error: failed to create socket\n");
        return EXIT_FAILURE;
    }

    unlink("/tmp/test.sock");

    struct sockaddr_un address;
    memset(&address, 0, sizeof(address));
    address.sun_family = AF_LOCAL;
    strcpy(address.sun_path, "/tmp/test.sock");

    int rc = bind(fd, (struct sockaddr *) &address, sizeof(address));
    if (0 != rc)
    {
        fprintf(stderr, "error: failed to bind socket\n");
        close(fd);
        return EXIT_FAILURE;
    }

    chmod("/tmp/test.sock", 0777);

    rc = listen(fd, BACKLOG);
    if (0 != rc)
    {
        fprintf(stderr, "error: failed to enter listen mode\n");
        close(fd);
        return EXIT_FAILURE;
    }

    while (!g_shutdown_requested)
    {
        struct pollfd pfd = { fd, POLLIN , 0};
        rc = poll(&pfd, 1, 1000);

        if (0 < rc)
        {
            int client_fd = accept(fd, NULL, NULL);
            if (0 <= client_fd)
            {
                struct ucred creds;
                socklen_t len = sizeof(creds);

                rc = getsockopt(client_fd, SOL_SOCKET, SO_PEERCRED, &creds, &len);
                if (0 != rc)
                {
                    fprintf(stderr, "error: failed to get credentials\n");
                    close(client_fd);
                    continue;                
                }

                printf("process id: %d\n", creds.pid);
                printf("user id   : %d\n", creds.uid);
                printf("group id  : %d\n", creds.gid);
                printf("\n");

                close(client_fd);
            }
        }
   }

    close(fd);
    return EXIT_SUCCESS;
}