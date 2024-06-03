#include "testing/pldm/fwup_testing.h"
#include "libpldm/firmware_update.h"

struct pldm_fwup_protocol_component_parameter_entry parameter_entries[] = {
    {
        .capabilities_during_update.value = 0,
        .comp_activation_methods.value = 0,
        .comp_activation_methods.bits.bit1 = 1,
        .comp_classification = PLDM_COMP_FIRMWARE,
        .comp_classification_index = 187,
        .comp_identifier = 29485,
        .active_comp_release_date = {2024, 6, 3, 12, 30, 45, 0, 0},
        .active_comp_comparison_stamp = 3780935207,
        .active_comp_ver.version_str = "firmware_v1.0",
        .active_comp_ver.version_str_length = strlen("firmware_v1.0"),
        .active_comp_ver.version_str_type = PLDM_STR_TYPE_ASCII,
        .pending_comp_comparison_stamp = 3780935208,
        .pending_comp_release_date = {2024, 7, 3, 12, 30, 45, 0, 0},
        .pending_comp_ver.version_str = "firmware_v2.0",
        .pending_comp_ver.version_str_length = strlen("firmware_v2.0"),
        .pending_comp_ver.version_str_type = PLDM_STR_TYPE_ASCII,
    },
    {
        .capabilities_during_update.value = 0,
        .comp_activation_methods.value = 0,
        .comp_activation_methods.bits.bit1 = 1,
        .comp_classification = PLDM_COMP_MIDDLEWARE,
        .comp_classification_index = 190,
        .comp_identifier = 29490,
        .active_comp_release_date = {2024, 8, 3, 12, 30, 45, 0, 0},
        .active_comp_comparison_stamp = 3780935210,
        .active_comp_ver.version_str = "middleware_v1.0",
        .active_comp_ver.version_str_length = strlen("middleware_v1.0"),
        .active_comp_ver.version_str_type = PLDM_STR_TYPE_ASCII,
        .pending_comp_comparison_stamp = 3780935211,
        .pending_comp_release_date = {2024, 9, 3, 12, 30, 45, 0, 0},
        .pending_comp_ver.version_str = "middleware_v2.0",
        .pending_comp_ver.version_str_length = strlen("middleware_v2.0"),
        .pending_comp_ver.version_str_type = PLDM_STR_TYPE_ASCII,
    }
};

const struct pldm_fwup_protocol_firmware_parameters PLDM_FWUP_FD_FIRMWARE_PARAMETERS = {
    .capabilities_during_update.value = 0,
    .active_comp_img_set_ver.version_str_type = PLDM_STR_TYPE_ASCII,
    .active_comp_img_set_ver.version_str = "cerberus_v1.0",
    .active_comp_img_set_ver.version_str_length = strlen("cerberus_v1.0"),
    .pending_comp_img_set_ver.version_str_type = PLDM_STR_TYPE_ASCII,
    .pending_comp_img_set_ver.version_str = "cerberus_v2.0",
    .pending_comp_img_set_ver.version_str_length = strlen("cerberus_v2.0"),
    .count = PLDM_FWUP_NUM_COMPONENTS,
    .entries = &parameter_entries
};

const struct pldm_fwup_fup_component_image_entry PLDM_FWUP_UA_FUP_COMP_INFO_LIST[] = {
    {
        .comp_classification = PLDM_COMP_FIRMWARE,
        .comp_identifier = 29485,
        .comp_comparison_stamp = 3780935208,
        .comp_ver.version_str = "firmware_v2.0",
        .comp_ver.version_str_length = strlen("firmware_v2.0"),
        .comp_ver.version_str_type = PLDM_STR_TYPE_ASCII,
        .comp_size = PLDM_FWUP_COMP_SIZE,
        .comp_options.value = 0,
        .comp_options.bits.bit0 = 1,
        .requested_comp_activation_method.value = 0,
        .requested_comp_activation_method.bits.bit1 = 1
    },
    {
        .comp_classification = PLDM_COMP_MIDDLEWARE,
        .comp_identifier = 29490,
        .comp_comparison_stamp = 3780935211,
        .comp_ver.version_str = "middleware_v2.0",
        .comp_ver.version_str_length = strlen("middleware_v2.0"),
        .comp_ver.version_str_type = PLDM_STR_TYPE_ASCII,
        .comp_size = PLDM_FWUP_COMP_SIZE,
        .comp_options.value = 0,
        .comp_options.bits.bit0 = 1,
        .requested_comp_activation_method.value = 0,
        .requested_comp_activation_method.bits.bit1 = 1
    }
};

const struct pldm_fwup_protocol_version_string PLDM_FWUP_UA_FUP_COMP_IMG_SET_VER = {
    .version_str = "cerberus_v2.0",
    .version_str_length = strlen("cerberus_v2.0"),
    .version_str_type = PLDM_STR_TYPE_ASCII
};

struct flash_region comp_regions[] = {
    {
        .length = PLDM_FWUP_FLASH_MANAGER_REGION_SIZE,
        .start_addr = PLDM_FWUP_FLASH_MANAGER_COMP_ONE_ADDR
    },
    {
        .length = PLDM_FWUP_FLASH_MANAGER_REGION_SIZE,
        .start_addr = PLDM_FWUP_FLASH_MANAGER_COMP_TWO_ADDR
    }
};

const struct pldm_fwup_flash_manager PLDM_FWUP_UA_FLASH_MANAGER = {
    .comp_regions = &comp_regions,
    .device_meta_data_region = {
        .length = PLDM_FWUP_FLASH_MANAGER_REGION_SIZE,
        .start_addr = PLDM_FWUP_FLASH_MANAGER_META_DATA_ADDR
    },
    .device_meta_data_size = 0,
    .package_data_region = {
        .length = PLDM_FWUP_FLASH_MANAGER_REGION_SIZE,
        .start_addr = PLDM_FWUP_FLASH_MANAGER_PKG_DATA_ADDR
    },
    .package_data_size = PLDM_FWUP_PKG_DATA_SIZE
};

const extern struct pldm_fwup_flash_manager PLDM_FWUP_FD_FLASH_MANAGER = {
    .comp_regions = &comp_regions,
    .device_meta_data_region = {
        .length = PLDM_FWUP_FLASH_MANAGER_REGION_SIZE,
        .start_addr = PLDM_FWUP_FLASH_MANAGER_META_DATA_ADDR
    },
    .device_meta_data_size = PLDM_FWUP_META_DATA_SIZE,
    .package_data_region = {
        .length = PLDM_FWUP_FLASH_MANAGER_REGION_SIZE,
        .start_addr = PLDM_FWUP_FLASH_MANAGER_PKG_DATA_ADDR
    },
    .package_data_size = 0
};