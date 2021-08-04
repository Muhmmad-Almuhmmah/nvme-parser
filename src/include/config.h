#ifndef _CONFIG__H_
#define _CONFIG__H_

/*
 * Local defines used by the program
 */
#define BUFFER_SIZE 1024
#define MAX_GAP_SPACE 20

/*
 * FBLOCK defines used by nve_set_fblock
 */
#define FBLOCK_ENTRY "FBLOCK"
#define FBLOCK_GAP_SPACE 6

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

#endif
