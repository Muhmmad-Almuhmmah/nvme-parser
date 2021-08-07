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
    size_t offset = 0, crc_offset;
    FILE *fp;
    int i = 0, count = 0;
    unsigned char on = 0x1;
    unsigned char off = 0x0;
    unsigned char crc[4];

    fp = fopen(nve_block, "r+b");
    len = strlen(FBLOCK_ENTRY);
    
    if (fp == NULL)
        return -1;

    for(;;)
    {
        if (EOF == (ch = fgetc(fp))) break;
        if ((char)ch != *FBLOCK_ENTRY) continue;
        
        for(i = 1; i < len ; ++i)
        {
            if (EOF == (ch = fgetc(fp))) goto exit;
            
            if ((char)ch != FBLOCK_ENTRY[i])
            {
                fseek(fp, 1-i, SEEK_CUR);
                goto next;
            }
        }

        /* 
         * Check the CRC of the NV entry and avoid editing NVME
         * on newer SoCs (where doing so hard bricks the device)
         */
        offset = ftell(fp) + FBLOCK_GAP_SPACE;
        crc_offset = ftell(fp) - strlen(FBLOCK_ENTRY) + 16;

        fseek(fp, crc_offset, SEEK_SET);
        fread(&crc, sizeof(crc), 1, fp);
        //printf("%s\n", crc);
        
        if (crc && !crc[0])
        {
            /* No valid CRC was found, we can write NVME without problems */
        }
        else
        {
            printf("[?] Can't write to the NVME image (too new?)\n");
            return -1;
        }

        fseek(fp, offset, SEEK_SET);

        if (mode == 0)
            fwrite(&off, sizeof(off), 1, fp);
        else if (mode == 1)
            fwrite(&on, sizeof(on), 1, fp);

        next: ;
    }
exit:
    fclose(fp);
    return (offset != 0 ? 0 : -1);
}

int nve_write_value(char *nve_block, char *name, char *value)
{
    int ch, len, i = 0, j = 0, count = 0;
    FILE *fp;
    size_t offset = 0, crc_offset;
    unsigned char crc[4];

    fp = fopen(nve_block, "r+b");
    len = strlen(name);
    
    if (fp == NULL)
        return -1;

    for(;;)
    {
        if (EOF == (ch = fgetc(fp))) break;
        if ((char)ch != *name) continue;
        
        for(i = 1; i < len ; ++i)
        {
            if (EOF == (ch = fgetc(fp))) goto exit;
            
            if ((char)ch != name[i])
            {
                fseek(fp, 1-i, SEEK_CUR);
                goto next;
            }
        }

        /* 
         * Check against the CRC of the NV entry and avoid editing NVME
         * on newer SoCs (where doing so hard bricks the device)
         */
        offset = ftell(fp) + nve_calc_space(name); // we also care about nv_number
        crc_offset = ftell(fp) - strlen(name) + 16;

        fseek(fp, crc_offset, SEEK_SET);
        fread(&crc, sizeof(crc), 1, fp);
        //printf("%s\n", crc);
        
        if (crc && !crc[0])
        {
            /* No valid CRC was found, we can write NVME without problems */
        }
        else
        {
            printf("[?] Can't write to the NVME image (too new?)\n");
            return -1;
        }

        printf("[?] Found %s at 0x%02zx!\n", name, offset);
        fseek(fp, offset, SEEK_SET);
        for (j = 0; j < strlen(value); j++)
        {
            fwrite(&value[j], 1, 1, fp);
        }
        next: ;
    }
exit:
    fclose(fp);
    return (offset != 0 ? 0 : -1);
}

int nve_read_item(char *nve_block, char *name)
{
    int ch, len, i = 0, j = 0, count = 0;
    FILE *fp;
    size_t offset;
    char c[1];
    NV_items_struct *nve_item = malloc(sizeof(NV_items_struct));

    fp = fopen(nve_block, "r+b");
    len = strlen(name);
    
    if (fp == NULL)
        return -1;

    printf("~~~~~~~~~~~~~~~~~ NVME ITEM INFO ~~~~~~~~~~~~~~~~~\n");
    for(;;)
    {
        if (EOF == (ch = fgetc(fp))) break;
        if ((char)ch != *name) continue;
        
        for(i = 1; i < len ; ++i)
        {
            if (EOF == (ch = fgetc(fp))) goto exit;
            
            if ((char)ch != name[i])
            {
                fseek(fp, 1-i, SEEK_CUR);
                goto next;
            }
        }

        offset = ftell(fp) - strlen(name) - 4; // we also care about nv_number
        fseek(fp, offset, SEEK_SET);
        fread(&nve_item->nv_number, sizeof(nve_item->nv_number), 1, fp);
        fread(&nve_item->nv_name, sizeof(nve_item->nv_name), 1, fp);
        fread(&nve_item->nv_property, sizeof(nve_item->nv_property), 1, fp);
        fread(&nve_item->valid_size, sizeof(nve_item->valid_size), 1, fp);
        fread(&nve_item->reserved, sizeof(nve_item->reserved), 1, fp);
        fread(&nve_item->nv_data, sizeof(nve_item->nv_data), 1, fp);
        printf("[?] -> NV Number: %d (0x%02zx)\n", nve_item->nv_number, offset);
        printf("[?] -> NV Name: %s (0x%02zx)\n", nve_item->nv_name, offset);
        printf("[?] -> NV Property: %d (0x%02zx)\n", nve_item->nv_property, offset);
        printf("[?] -> NV Valid Size: %d (0x%02zx)\n", nve_item->valid_size, offset);
        printf("[?] -> NV Reserved: %d (0x%02zx)\n", nve_item->reserved, offset);
        printf("[?] -> NV Data: %s (0x%02zx)\n", nve_item->nv_data, offset);
        next: ;
    }
exit:
    printf("~~~~~~~~~~~~~~~~~ NVME ITEM INFO ~~~~~~~~~~~~~~~~~\n");
    fclose(fp);
    return (offset != 0 ? 0 : -1);
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
