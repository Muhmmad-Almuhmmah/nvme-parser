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

#include "config.h"
#include "helper.h"
#include "nvme.h"

int main(int argc, char *argv[])
{
    int read_all = 0, default_backup = 0, default_restore = 0;
    char *backup_cmd = malloc(BUFFER_SIZE);
    char *backup_src = malloc(BUFFER_SIZE);
    size_t offset;

    if ((argc < 2) || strstr(argv[1], "-h"))
    {
        __show_usage();
    }
    
    if (strstr(argv[1], "-r"))
    {
        if (argc < 3)
        {
           return 0;
        }
        
        if (__find_block() != NULL)
        {
            if (nve_check_header(__find_block() != 0))
            {
                printf("[-] Your nvme looks corrupted and/or invalid :(!\n");
                return -1;
            }
        
            offset = nve_get_offset(argv[2]) + strlen(argv[2]) + nve_calc_space(argv[2]);
            
            if (offset == 19) // -1
            {
                printf("[-] Bad value name: %s.\n", argv[2]);
                return -1;
            }
            
            printf("[+] Found value name offset at 0x%02zx!\n", offset);
            printf("[+] %s => %s\n", argv[2], nve_read_by_offset(offset));
            return 0;
        }
        else
        {
            printf("[-] Couldn't find a suitable nvme block :(!\n");
            return -1;
        }
    }
    
    else if (strstr(argv[1], "-w"))
    {
        if (argc < 4)
        {
           printf("[-] You need to specify the name and the value.\n");
           return -1;
        }
        
        if (__find_block() != NULL)
        {
            if (nve_check_header(__find_block() != 0))
            {
                printf("[-] Your device's nvme looks corrupted and/or invalid :(!\n");
                return -1;
            }
        
            offset = nve_get_offset(argv[2]) + strlen(argv[2]) + nve_calc_space(argv[2]);
            
            if (offset == 19) // -1
            {
                printf("[-] Bad value name: %s.\n", argv[2]);
                return -1;
            }
            
            printf("[+] Found value name offset at 0x%02zx!\n", offset);
            
            if (strlen(argv[3]) != strlen(nve_read_by_offset(offset)))
            {
                printf("[-] Invalid value length (should be %lu)!\n", strlen(nve_read_by_offset(offset)));
                return -1;
            }
            
            if (nve_write_by_offset(offset, argv[3]) != 0)
            {
                printf("[-] Something went wrong while trying to set %s!\n", argv[2]);
                return -1;
            }
            else
            {
                printf("[+] Successfully set %s to %s :)!\n", argv[2], argv[3]);
                return 0;
            }
            return 0;
        }
        else
        {
            printf("[-] Couldn't find a suitable nvme block :(!\n");
            return -1;
        }
        
    }

    else if(strstr(argv[1], "-b"))
    {
        if (argc < 3)
        {
            printf("[!] Backing up as %s since no backup path was specified.\n", DEFAULT_BACKUP_PATH);
            default_backup = 1;
        } 
        
        if (__find_block() != NULL)
        {
            if (default_backup)
            {
                snprintf(backup_cmd, BUFFER_SIZE, "dd if=%s of=%s &> /dev/null", __find_block(), DEFAULT_BACKUP_PATH);
                __exec_sh(backup_cmd);
                if (__file_exists(DEFAULT_BACKUP_PATH))
                {
                    printf("[+] Successfully backed up nvme block to %s!\n", DEFAULT_BACKUP_PATH);
                    return 0;
                }
                else
                {
                    printf("[-] Something went wrong while trying to backup the nvme block :(!\n");
                    return -1;
                }
            }
            else
            {
                snprintf(backup_cmd, BUFFER_SIZE, "dd if=%s of=%s &> /dev/null", __find_block(), argv[2]);
                __exec_sh(backup_cmd);
                if (__file_exists(argv[2]))
                {
                    printf("[+] Successfully backed up nvme block to %s!\n", argv[2]);
                    return 0;
                }
                else
                {
                    printf("[-] Something went wrong while trying to backup the nvme block :(!\n");
                    return -1;
                }
            }
        }
        else
        {
            printf("[-] Couldn't find a suitable nvme block :(!\n");
            return -1;
        }
    }

    else if(strstr(argv[1], "-e"))
    {
        if (argc < 3)
        {
            printf("[!] Trying to restore backup from %s since no backup path was specified.\n", DEFAULT_BACKUP_PATH);
            default_restore = 1;
        }

        if (__find_block() != NULL)
        {
            if (default_restore)
            {
                snprintf(backup_cmd, BUFFER_SIZE, "dd if=%s of=%s &> /dev/null", DEFAULT_BACKUP_PATH, __find_block());
                __exec_sh(backup_cmd);
                if (__file_exists(__find_block()))
                {
                    printf("[+] Successfully restored nvme block from %s!\n", DEFAULT_BACKUP_PATH);
                    return 0;
                }
                else
                {
                    printf("[-] Something went wrong while trying to retore the nvme block :(!\n");
                    return -1;
                }
            }
            else
            {
                snprintf(backup_cmd, BUFFER_SIZE, "dd if=%s of=%s &> /dev/null", argv[2], __find_block());
                __exec_sh(backup_cmd);
                if (__file_exists(__find_block()))
                {
                    printf("[+] Successfully restored nvme block from %s!\n", argv[2]);
                    return 0;
                }
                else
                {
                    printf("[-] Something went wrong while trying to retore the nvme block :(!\n");
                    return -1;
                }
            }
        }
        else
        {
            printf("[-] Couldn't find a suitable nvme block :(!\n");
            return -1;
        }

    }
    return 0;
}
    
