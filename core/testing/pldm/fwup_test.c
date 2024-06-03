#include "testing/pldm/fwup_testing.h"
#include "libpldm/firmware_update.h"

const struct pldm_fwup_protocol_component_parameter_entry PLDM_FWUP_PARAMETER_ENTRIES[] = {
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

const struct flash_region PLDM_FWUP_COMP_REGIONS[] = {
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



static void setup_flash_ctx(struct pldm_fwup_protocol_flash_ctx *flash_ctx, CuTest *test) 
{
    int status = flash_virtual_disk_init(&flash_ctx->fd_flash, PLDM_FWUP_FLASH_FD, &flash_ctx->fd_flash_state, PLDM_FWUP_FLASH_SIZE);
    CuAssertIntEquals(test, 0, status);

    status = flash_virtual_disk_init(&flash_ctx->ua_flash, PLDM_FWUP_FLASH_UA, &flash_ctx->ua_flash_state, PLDM_FWUP_FLASH_SIZE);
    CuAssertIntEquals(test, 0, status);
}

static void release_flash_ctx(struct pldm_fwup_protocol_flash_ctx *flash_ctx)
{
    flash_virtual_disk_release(&flash_ctx->fd_flash);
    flash_virtual_disk_release(&flash_ctx->ua_flash);
}

static void setup_testing_ctx(struct pldm_fwup_protocol_testing_ctx *testing_ctx, struct pldm_fwup_protocol_flash_ctx *flash_ctx) 
{
    testing_ctx->fd_fw_parameters = PLDM_FWUP_FD_FIRMWARE_PARAMETERS;
    testing_ctx->fd_fw_parameters.entries = platform_calloc(PLDM_FWUP_NUM_COMPONENTS, sizeof (struct pldm_fwup_protocol_component_parameter_entry));
    memcpy(testing_ctx->fd_fw_parameters.entries, PLDM_FWUP_PARAMETER_ENTRIES, PLDM_FWUP_NUM_COMPONENTS * sizeof (struct pldm_fwup_protocol_component_parameter_entry));

    testing_ctx->fup_comp_img_list = platform_calloc(PLDM_FWUP_NUM_COMPONENTS, sizeof (struct pldm_fwup_fup_component_image_entry));
    memcpy(testing_ctx->fup_comp_img_list, PLDM_FWUP_UA_FUP_COMP_INFO_LIST, PLDM_FWUP_NUM_COMPONENTS * sizeof (struct pldm_fwup_fup_component_image_entry));

    testing_ctx->fd_flash_mgr = PLDM_FWUP_FD_FLASH_MANAGER;
    testing_ctx->fd_flash_mgr.comp_regions = platform_calloc(PLDM_FWUP_NUM_COMPONENTS, sizeof (struct flash_region));
    memcpy(testing_ctx->fd_flash_mgr.comp_regions, PLDM_FWUP_COMP_REGIONS, PLDM_FWUP_NUM_COMPONENTS * sizeof (struct flash_region));
    testing_ctx->fd_flash_mgr.flash = &flash_ctx->fd_flash;

    testing_ctx->ua_flash_mgr = PLDM_FWUP_UA_FLASH_MANAGER;
    testing_ctx->ua_flash_mgr.comp_regions = platform_calloc(PLDM_FWUP_NUM_COMPONENTS, sizeof (struct flash_region));
    memcpy(testing_ctx->ua_flash_mgr.comp_regions, PLDM_FWUP_COMP_REGIONS, PLDM_FWUP_NUM_COMPONENTS * sizeof (struct flash_region));
    testing_ctx->ua_flash_mgr.flash = &flash_ctx->ua_flash;

    testing_ctx->fup_comp_img_set_ver = PLDM_FWUP_UA_FUP_COMP_IMG_SET_VER;

}

static void release_testing_ctx(struct pldm_fwup_protocol_testing_ctx *testing_ctx)
{
    free(testing_ctx->fd_fw_parameters.entries);
    free(testing_ctx->fup_comp_img_list);
    free(testing_ctx->fd_flash_mgr.comp_regions);
    free(testing_ctx->ua_flash_mgr.comp_regions);
}


static void setup_ua_device_manager(struct device_manager *device_mgr, CuTest *test)
{
    int status = device_manager_init(device_mgr, 1, 2, DEVICE_MANAGER_AC_ROT_MODE, DEVICE_MANAGER_MASTER_BUS_ROLE,
        0, 0, 0, 0, 0, 0, 0);
    CuAssertIntEquals(test, 0, status);

    device_mgr->entries[DEVICE_MANAGER_SELF_DEVICE_NUM].eid = PLDM_FWUP_UA_EID;
    device_mgr->entries[DEVICE_MANAGER_SELF_DEVICE_NUM].smbus_addr = PLDM_FWUP_UA_SMBUS_ADDR;
    device_mgr->entries[DEVICE_MANAGER_SELF_DEVICE_NUM].pci_device_id = 5678;
    device_mgr->entries[DEVICE_MANAGER_SELF_DEVICE_NUM].pci_vid = 1234;
    device_mgr->entries[DEVICE_MANAGER_SELF_DEVICE_NUM].pci_subsystem_id = 5432;
    device_mgr->entries[DEVICE_MANAGER_SELF_DEVICE_NUM].pci_subsystem_vid = 9876;

    device_mgr->entries[2].eid = PLDM_FWUP_FD_EID;
    device_mgr->entries[2].smbus_addr = PLDM_FWUP_FD_SMBUS_ADDR;

    device_mgr->entries[2].capabilities.request.max_message_size = MCTP_BASE_PROTOCOL_MAX_MESSAGE_BODY;
    device_mgr->entries[2].capabilities.request.max_packet_size = MCTP_BASE_PROTOCOL_MAX_TRANSMISSION_UNIT;
}

static void setup_fd_device_manager(struct device_manager *device_mgr, CuTest *test)
{
    int status = device_manager_init(device_mgr, 1, 2, DEVICE_MANAGER_AC_ROT_MODE, DEVICE_MANAGER_MASTER_BUS_ROLE,
        0, 0, 0, 0, 0, 0, 0);
    CuAssertIntEquals(test, 0, status);

    device_mgr->entries[DEVICE_MANAGER_SELF_DEVICE_NUM].eid = PLDM_FWUP_FD_EID;
    device_mgr->entries[DEVICE_MANAGER_SELF_DEVICE_NUM].smbus_addr = PLDM_FWUP_FD_SMBUS_ADDR;
    device_mgr->entries[DEVICE_MANAGER_SELF_DEVICE_NUM].pci_device_id = 8765;
    device_mgr->entries[DEVICE_MANAGER_SELF_DEVICE_NUM].pci_vid = 4321;
    device_mgr->entries[DEVICE_MANAGER_SELF_DEVICE_NUM].pci_subsystem_id = 2109;
    device_mgr->entries[DEVICE_MANAGER_SELF_DEVICE_NUM].pci_subsystem_vid = 6789;

    device_mgr->entries[2].eid = PLDM_FWUP_UA_EID;
    device_mgr->entries[2].smbus_addr = PLDM_FWUP_UA_SMBUS_ADDR;

    device_mgr->entries[2].capabilities.request.max_message_size = MCTP_BASE_PROTOCOL_MAX_MESSAGE_BODY;
    device_mgr->entries[2].capabilities.request.max_packet_size = MCTP_BASE_PROTOCOL_MAX_TRANSMISSION_UNIT;
}

static void release_device_manager(struct device_manager *device_mgr)
{
    device_manager_release(device_mgr);
}



