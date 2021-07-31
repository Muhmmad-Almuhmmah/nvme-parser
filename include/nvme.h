#ifndef _NVME__H_
#define _NVME__H_

size_t nve_get_offset(char *value);
int nve_check_header();
char *nve_find_match(const char *buf, const char * buf_end, const char *pat, size_t len);
int nve_calc_space(char *value);
char *nve_read_by_offset(size_t offset);
int nve_write_by_offset(size_t offset, char *data);

#endif
