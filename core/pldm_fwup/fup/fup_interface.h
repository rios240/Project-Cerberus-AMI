#ifndef FUP_INTERFACE_H_
#define FUP_INTERFACE_H_



#include <stdio.h>
#include "flash/flash_virtual_disk.h"



int fup_interface_setup_test_fup(const struct flash *fup, size_t fup_size, uint32_t fup_base_addr);


#endif /* FUP_INTERFACE_H_ */