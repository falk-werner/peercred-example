cmake_minimum_required (VERSION 3.22)
project(peercred
    VERSION 1.0.0
    DESCRIPTION "Peercreed")

set(CMAKE_C_STANDARD 11)

add_compile_options(-Wall -Wextra -Wpedantic -Werror)

add_executable(server src/server.c)
add_executable(client src/client.c)
add_executable(evil_client src/evil_client.c)
add_executable(evil_setuidmap src/evil_setuidmap.c)

install(TARGETS evil_setuidmap
    PERMISSIONS OWNER_WRITE OWNER_READ OWNER_EXECUTE
    GROUP_READ GROUP_EXECUTE
    WORLD_READ WORLD_EXECUTE SETUID)
