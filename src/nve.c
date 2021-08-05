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

#include "helper.h"
#include "common.h"
#include "nve.h"

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

int nve_set_fblock(char *nve_block, int mode)
{
    int ch, len;
    size_t offsets[6];
    FILE *fp;
    int i = 0, count = 0;
    unsigned char on = 0x1;
    unsigned char off = 0x0;

    fp = fopen(nve_block, "r+b");
    len = strlen(FBLOCK_ENTRY);
    
    if (fp == NULL)
        return -1;

    // find 7 entries
    printf("[?] Searching for FBLOCK entries...\n");
    for(;;)
    {
        if (EOF == (ch = fgetc(fp))) break;
        if ((char)ch != *FBLOCK_ENTRY) continue;
        
        for(i = 1; i < len ; ++i)
        {
            if (EOF == (ch = fgetc(fp))) goto write;
            
            if ((char)ch != FBLOCK_ENTRY[i])
            {
                fseek(fp, 1-i, SEEK_CUR);
                goto next;
            }
        }

        printf("set 0x%02zx to %d\n", ftell(fp) + FBLOCK_GAP_SPACE, mode);
        fseek(fp, ftell(fp) + FBLOCK_GAP_SPACE, SEEK_SET);
        if (mode == 0)
            fwrite(&off, sizeof(off), 1, fp);
        else if (mode == 1)
            fwrite(&on, sizeof(on), 1, fp);

        next: ;
    }
write:
    fclose(fp);
    return 0;
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

NVE_partition_header *nve_read_header_info(char *nve_block)
{
    FILE *fd;
    NVE_partition_header *nve_header = malloc(sizeof(NVE_partition_header));
    int i;
    
    fd = fopen(nve_block, "r");
    if (fd == NULL)
        return NULL;
    
    fseek(fd, NVE_HDR_OFFSET, SEEK_SET); // 0x0FFF80 ?
    fread(&nve_header->nve_partition_name, sizeof(nve_header->nve_partition_name), 1, fd);
    fread(&nve_header->nve_version, sizeof(nve_header->nve_version), 1, fd);
    fread(&nve_header->nve_block_id, sizeof(nve_header->nve_block_id), 1, fd);
    fread(&nve_header->nve_block_count, sizeof(nve_header->nve_block_count), 1, fd);
    fread(&nve_header->valid_items, sizeof(nve_header->valid_items), 1, fd);
    fread(&nve_header->nv_checksum, sizeof(nve_header->nv_checksum), 1, fd);
    fread(&nve_header->nve_block_id, sizeof(nve_header->nve_block_id), 1, fd);
    fread(&nve_header->reserved, sizeof(nve_header->reserved), 1, fd);
    fread(&nve_header->nve_age, sizeof(nve_header->nve_age), 1, fd);
    
    fclose(fd);
    return nve_header;
}

int nve_calc_space(char *value) {
    return (MAX_GAP_SPACE - strlen(value));
}

int nve_check_header(char* nve_block) {
    return (nve_get_offset(nve_block, HEADER1) != 19 || nve_get_offset(nve_block, HEADER2) != 19 ? 0 : -1);
}
