#ifndef _NVME__H_
#define _NVME__H_

size_t nve_get_offset(char *nve_block, char *value);
int nve_check_header(char *nve_block);
int nve_calc_space(char *value);
char *nve_read_by_offset(char *nve_block, size_t offset);
int nve_write_by_offset(char *nve_block, size_t offset, char *data);

#endif
