# peercred-example

This repository contains an example of how to use `SO_PEERCRED` socket option in order to identify the user connected to a unix domain socket.
After testing the application using [Docker](https://www.docker.com/) it drifts away toward [user namespaces](https://man7.org/linux/man-pages/man7/user_namespaces.7.html).

## Lightweight authentication for unix domain sockets

Unix domain sockets (aka local sockets) provides a lightweight facility to obtain credentials from the connected peer: the `SO_PEERCRED` socket option.

Once a peer is connected, it's credentials can be obtained as shown below:

```C
#define _GNU_SOURCE
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/un.h>

struct ucred creds;
socklen_t len = sizeof(creds);

rc = getsockopt(client_fd, SOL_SOCKET, SO_PEERCRED, &creds, &len);
if (0 == rc)
{
    printf("process id: %d\n", creds.pid);
    printf("user id   : %d\n", creds.uid);
    printf("group id  : %d\n", creds.gid);
}
```

The returned credentials are the credentials that were in effect at the time the connection was established. _(If you are interested in credentials at
the time a particular message is sent, take a look at `SO_PASSCRED`.)_

### Examples

The provided examples [server.c](src/server.c) and [client.c](src/client.c) demonstated the usage. The server creates a socket `/tmp/test.sock` and prints process id, user id and group id of each connection. The client just connects to the sockets.

To build and run the example, use:

```bash
cmake -B build
cmake --build build
./build/server &
./build/client
```

This should print something like:

```
process id: 53475
user id   : 1000
group id  : 1000
```

## Run in Docker

For testing purposes, the client was put into a Docker container.

```bash
docker build -t client .
docker run -it -v /tmp/test.sock:/tmp/test.sock client
```

Running the example (with the server still active) print the following output.

```
process id: 54657
user id   : 0
group id  : 0
```

I first got confused about this output, since I did not specified the `--privileged` flag and assumed to get my own user id (1000) instead of root (0).

## Run in Podman

// ToDo


## References

- [SO_PEERCRED (unix(7))](https://man7.org/linux/man-pages/man7/unix.7.html)
- [user namespace(7)](https://man7.org/linux/man-pages/man7/user_namespaces.7.html)
- [clone(2)](https://man7.org/linux/man-pages/man2/clone.2.html)
- [Docker](https://www.docker.com/)
- [Podman](https://podman.io/)