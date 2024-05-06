#ifndef FUP_INTERFACE_H_
#define FUP_INTERFACE_H_



#include <stdio.h>
#include "flash/flash.h"
#include "cmd_interface/device_manager.h"
#include "libpldm/firmware_update.h"

//#ifdef PLDM_FWUP_UA_ENABLE

int fup_interface_setup_test_fup(const struct flash *fup_flash, size_t fup_size, uint32_t fup_base_addr);

int fup_interface_get_device_id_record(const struct flash *fup_flash, 
    uint32_t fup_base_addr, struct device_manager_entry *entry,
    struct pldm_firmware_device_id_record *device_id_record, struct variable_field *applicable_components,
    struct variable_field *comp_image_set_version_str, struct variable_field *record_descriptors,
    struct variable_field *fw_device_pkg_data);

uint8_t fup_interface_get_num_components(const struct flash *fup_flash, uint32_t fup_base_addr,
    struct device_manager_entry *entry);

//#endif

#endif /* FUP_INTERFACE_H_ */