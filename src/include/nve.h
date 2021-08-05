#ifndef _NVE__H_
#define _NVE__H_

/*
 * This struct, contans all the information 
 * stored in the NVME's partition header.
 */
typedef struct 
{
    char nve_partition_name[32];    /*used for checking current partition is valid or not*/
    unsigned int nve_version;       /*should be built in image with const value*/
    unsigned int nve_block_id;      /*should be built in image with const value*/
    unsigned int nve_block_count;   /*should be built in image with const value*/
    unsigned int valid_items;       /*should be built in image with const value*/
    unsigned int nv_checksum;
    unsigned char reserved[72];
    unsigned int nve_age;
} NVE_partition_header;

/*
 * All the nvme-related functions are listed here.
 */
size_t nve_get_offset(char *nve_block, char *value);
int nve_check_header(char *nve_block);
int nve_calc_space(char *value);
int nve_write_value(char *nve_block, char *name, char *value);
int nve_read_value(char *nve_block, char *name);
int nve_set_fblock(char *nve_block, int mode);
NVE_partition_header *nve_read_header_info(char *nve_block);

/*
 * The huawei NVME image resides in a block under the /dev folder.
 * Since most phones (nowadays) follow the standard partition scheme, 
 * instead of trying to find the block, try these two routes and ask
 * to the user for manual input if none worked (this shouldn't happen).
 */
#define NVME1 "/dev/block/platform/hi_mci.0/by-name/nvme"
#define NVME2 "/dev/block/bootdevice/by-name/nvme"

/*
 * Huawei NVME images contain a header at the beginning and 
 * at the end that is mostly used to identify the type of NVME 
 * image. Unfortunately, these headers are not always the same,
 * there are several types of headers (based on the platform).
 */
#define HEADER1 "Hisi-NV-Partition" // Newer phones (hi6250 <)
#define HEADER2 "Hisi-K3v200-NV-Partition" // Older phones (hi6210sft >)

/*
 * This offset, points out to the end header of the NVME image.
 * As far as I know, it should be always located at the same
 * position.
 */
#define NVE_HDR_OFFSET 0x0FFF80

/*
 * FBLOCK defines used by nve_set_fblock
 */
#define FBLOCK_ENTRY "FBLOCK"
#define FBLOCK_GAP_SPACE 6

#endif
