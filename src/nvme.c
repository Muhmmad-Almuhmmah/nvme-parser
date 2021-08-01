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
#include <errno.h>
#include <fcntl.h>

#include <sys/sendfile.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/mman.h>

#include "config.h"
#include "helper.h"

size_t nve_get_offset(char *nve_block, char *value)
{
    int fd;
    off_t length;
    void *data, *ptr;
    size_t offset;
    
    fd = open(nve_block, O_RDONLY);
    if (fd < 0)
       return -1;
    
    length = lseek(fd, 0, SEEK_END);
    data = mmap(NULL, length, PROT_READ, MAP_PRIVATE, fd, 0);
    ptr = memmem(data, length, value, strlen(value));
    
    if (ptr == NULL)
       return -1;
       
    offset = ptr-data;
    munmap(data, length);
    
    close(fd);
    return offset;
}

char *nve_read_by_offset(char *nve_block, size_t offset)
{
    FILE *fd;
    char c[1];
    size_t pos;
    char *value = malloc(BUFFER_SIZE);
    
    fd = fopen(nve_block, "r");
    if (fd < 0)
        return NULL;
     
    fseek(fd, offset, SEEK_SET);
    fread(&c, sizeof(char), 1, fd);
    snprintf(value, BUFFER_SIZE, "%s", c);
    
    while (c[0] != '\0')
    {
        fread(&c, sizeof(char), 1, fd);
        snprintf(value, BUFFER_SIZE, "%s%s", value, c);
    }
    
    fclose(fd);
    
    return value;
}

int nve_write_by_offset(char* nve_block, size_t offset, char *data)
{
    int fd;
    char data_initial;
    
    fd = open(nve_block, O_CREAT|O_RDWR, S_IRUSR|S_IWUSR);
    if (fd < 0)
        return -1;
    
    lseek(fd, offset, SEEK_SET);
    
    for (int k = 0; k < strlen(data); k++)
    {
        data_initial = data[k];
        write(fd, &data[k], 1);
    }
   
    close(fd);
    return 0;
}

int nve_calc_space(char *value) {
    return (MAX_GAP_SPACE - strlen(value));
}

int nve_check_header(char* nve_block) {
    return (nve_get_offset(nve_block, HEADER1) != 19 || nve_get_offset(nve_block, HEADER2) != 19 ? 0 : -1);
}
