#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_MAPPING_SIZE (1024)

int main(int argc, char * argv[])
{
    if (argc <= 4)
    {
        printf(
            "evil_setuidmap, (c) 2024 Falk Werner <github.com/falk-werner>\n"
            "Set uid map of a process without validity checks.\n"
            "\n"
            "Usage:\n"
            "   evil_setuidmap <PID> <UID> <PARENT_UID> <COUNT> [<UID> <PARENT_UID> <COUNT>] ...\n"
            "\n"
            "Arguments:\n"
            "   PID        - id of the process to set uid map\n"
            "   UID        - start of the uid mapping in the process user namespace\n"
            "   PARENT_UID - start of the uid mapping in the current user namespace\n"
            "   COUNT      - count of uids to map\n"
            "\n"
            "Note:\n"
            "   This executable is for testing purposes onyl. Do not use in production.\n"
        );

        return EXIT_SUCCESS;
    }
    
    int const pid = atoi(argv[1]);

    char mapping[MAX_MAPPING_SIZE];
    size_t mapping_len = 0;
    for(int i = 2, n = 0; ((i + 2) < argc) && (n < 10); i += 3, n++)
    {
        int const ns_uid_start = atoi(argv[i]);  
        int const parent_uid_start = atoi(argv[i+1]);
        int const count = atoi(argv[i+2]);            
        mapping_len += sprintf(&mapping[mapping_len], "%d %d %d\n", ns_uid_start, parent_uid_start, count);
    }

    char path[80];
    sprintf(path, "/proc/%d/uid_map", pid);
    FILE * file = fopen(path, "wb");
    if (NULL == file)
    {
        fprintf(stderr, "error: failed to open file: %s\n", path);
        return EXIT_FAILURE;
    }

    size_t const written = fwrite(mapping, 1, mapping_len, file);
    fclose(file);

    if (written != mapping_len)
    {
        fprintf(stderr, "failed to write\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
