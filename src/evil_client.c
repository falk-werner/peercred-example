#define _GNU_SOURCE
#include <sched.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <fcntl.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STACK_SIZE (1024 * 1024)
char child_stack[STACK_SIZE];

int child_main(void * context)
{
    int * pfd = context;
    char c;
    read(*pfd, &c, 1);

    int rc = setuid(0); 
    if (rc != 0) {
        perror("error: setuid failed");
        return EXIT_FAILURE;
    }

    int fd = socket(AF_LOCAL, SOCK_STREAM, 0);
    if (0 > fd)
    {
        fprintf(stderr, "error: failed to create socket\n");
        return EXIT_FAILURE;
    }

    struct sockaddr_un address;
    memset(&address, 0, sizeof(address));
    address.sun_family = AF_LOCAL;
    strcpy(address.sun_path, "/tmp/test.sock");

    rc = connect(fd, (struct sockaddr*) &address, sizeof(address));
    if (0 != rc)
    {
        fprintf(stderr, "error: failed to connect\n");
        close(fd);
        return EXIT_FAILURE;
    }

    close(fd);
    return EXIT_SUCCESS;
}

int main(int argc, char* argv[])
{
    (void) argc;
    (void) argv;

    int fds[2];
    char const msg = 42;
    int rc = socketpair(AF_LOCAL, SOCK_STREAM, 0, fds);
    if (0 != rc)
    {
        fprintf(stderr, "error: failed to socket pair\n");
        return EXIT_FAILURE;
    }
    int const id = clone(
        &child_main,
        &child_stack[STACK_SIZE],
        CLONE_NEWUSER,
        &fds[1]);

    if (id == -1)
    {
        fprintf(stderr, "error: clone failed\n");
        close(fds[0]);
        close(fds[1]);
        return EXIT_FAILURE;
    }

    char command[80];
    sprintf(command, "evil_setuidmap %d 0 0 1", id);
    FILE * file = popen(command, "r");
    if (NULL == file)
    {
        fprintf(stderr, "error: failed to set uid map: popen failed\n");
        write(fds[0], &msg, 1);
        waitpid(id, NULL, 0);
        close(fds[0]);
        close(fds[1]);
        return EXIT_FAILURE;
    }

    rc = WEXITSTATUS(pclose(file));
    if (0 != rc)
    {
        fprintf(stderr, "error: failed to set uid map\n");
        write(fds[0], &msg, 1);
        waitpid(id, NULL, 0);
        close(fds[0]);
        close(fds[1]);
        return EXIT_FAILURE;
    }

    write(fds[0], &msg, 1);
    waitpid(id, NULL, 0);
    close(fds[0]);
    close(fds[1]);
    return EXIT_SUCCESS;
}
