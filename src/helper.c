/*
 * Copyright (C) 2021 Roger Ortiz (R0rt1z2)
 *
 * Licensed under the GNU GPL, Version 3.0;
 * you may not use this file except in compliance 
 * with the License. You may obtain a copy of the 
 * License at: <https://www.gnu.org/licenses/>
 *
 * Unless required by applicable law or agreed to in 
 * writing, software distributed under the License is 
 * distributed on an "AS IS" BASIS, WITHOUT WARRANTIES 
 * OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing 
 * permissions and limitations under the License.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

#include <sys/stat.h>

#include "config.h"

bool __file_exists(char *filename) {
    struct stat buffer;
    return (stat (filename, &buffer) == 0);
}

char *__find_block(void) {
    return (__file_exists(NVME1) ? NVME1 : __file_exists(NVME2) ? NVME2 : NULL);
}

char *__exec_sh(char *cmd)
{
    FILE *fp = popen(cmd, "r");
    char buffer[256];
    size_t chread, alloc = 256, len = 0;
    char *out = malloc(alloc);

    if (fp == NULL)
       return NULL;
 
    while ((chread = fread(buffer, 1, sizeof(buffer), fp)) != 0) 
    {
        if (len + chread >= alloc) 
        {
            alloc *= 2;
            out = realloc(out, alloc);
        }
        memmove(out + len, buffer, chread);
        len += chread;
    }

    pclose(fp);
    return out;
}

void __show_usage(void) {
    printf("[!] USAGE: nvme [options] \n");
    printf("  -r <value>        | dump desired <value> from nvme.\n");
    printf("  -w <name> <value> | write desired <value> to nvme.\n");
    printf("  -b <path>         | backup the raw nvme block to <path>.\n");
    printf("  -e <path>         | write <path> backup to the nvme block.\n");
    exit(1);
}
