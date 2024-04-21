#ifndef FUP_INTERFACE_H_
#define FUP_INTERFACE_H_



#include <stdio.h>
#include "flash/flash_virtual_disk.h"


#ifdef PLDM_FWUP_UA_ENABLE
#include "firmware_update.h"

int fup_interface_setup_test_fup(const struct flash *fup, size_t fup_size, uint32_t fup_base_addr);

void fup_interface_get_fup_header_info(const struct flash *fup, uint32_t fup_base_addr, struct package_header_information *pkg_hdr_info);
void fup_interface_get_fup_package_version(const struct flash *fup, uint32_t fup_base_addr, const char *pkg_version_str, size_t pkg_version_str_len);

#endif

#endif /* FUP_INTERFACE_H_ */