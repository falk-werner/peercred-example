#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char* argv[])
{
    (void) argc;
    (void) argv;

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

    int rc = connect(fd, (struct sockaddr*) &address, sizeof(address));
    if (0 != rc)
    {
        fprintf(stderr, "error: failed to connect\n");
        close(fd);
        return EXIT_FAILURE;
    }

    char c = 42;
    write(fd, &c, 1);

    close(fd);
    return EXIT_SUCCESS;
}