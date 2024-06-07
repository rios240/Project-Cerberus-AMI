#include "testing/pldm/fwup_testing.h"
#include "libpldm/firmware_update.h"
#include "pldm/cmd_channel/cmd_channel_tcp.h"
#include "pldm/pldm_fwup_handler.h"
#include "status/rot_status.h"

const struct pldm_fwup_protocol_component_parameter_entry PLDM_FWUP_PARAMETER_ENTRIES[] = {
    {
        .capabilities_during_update.value = 0,
        .comp_activation_methods.value = 0,
        .comp_activation_methods.bits.bit1 = 1,
        .comp_classification = PLDM_COMP_FIRMWARE,
        .comp_classification_index = 187,
        .comp_identifier = 29485,
        .active_comp_release_date = {(2024 >> 8) & 0xFF, 2024 & 0xFF, 6, 3, 12, 30, 45, 0},
        .active_comp_comparison_stamp = 3780935207,
        .active_comp_ver.version_str = PLDM_FWUP_ACTIVE_FIRMWARE_COMP_VER,
        .active_comp_ver.version_str_length = PLDM_FWUP_ACTIVE_FIRMWARE_COMP_VER_LEN,
        .active_comp_ver.version_str_type = PLDM_STR_TYPE_ASCII,
        .pending_comp_comparison_stamp = 3780935208,
        .pending_comp_release_date = {(2024 >> 8) & 0xFF, 2024 & 0xFF, 7, 3, 12, 30, 45, 0},
        .pending_comp_ver.version_str = PLDM_FWUP_PENDING_FIRMWARE_COMP_VER,
        .pending_comp_ver.version_str_length = PLDM_FWUP_PENDING_FIRMWARE_COMP_VER_LEN,
        .pending_comp_ver.version_str_type = PLDM_STR_TYPE_ASCII,
    },
    {
        .capabilities_during_update.value = 0,
        .comp_activation_methods.value = 0,
        .comp_activation_methods.bits.bit1 = 1,
        .comp_classification = PLDM_COMP_MIDDLEWARE,
        .comp_classification_index = 190,
        .comp_identifier = 29490,
        .active_comp_release_date = {(2024 >> 8) & 0xFF, 2024 & 0xFF, 8, 3, 12, 30, 45, 0},
        .active_comp_comparison_stamp = 3780935210,
        .active_comp_ver.version_str = PLDM_FWUP_ACTIVE_MIDDLEWARE_COMP_VER,
        .active_comp_ver.version_str_length = PLDM_FWUP_ACTIVE_MIDDLEWARE_COMP_VER_LEN,
        .active_comp_ver.version_str_type = PLDM_STR_TYPE_ASCII,
        .pending_comp_comparison_stamp = 3780935211,
        .pending_comp_release_date = {(2024 >> 8) & 0xFF, 2024 & 0xFF, 9, 3, 12, 30, 45, 0},
        .pending_comp_ver.version_str = PLDM_FWUP_PENDING_MIDDLEWARE_COMP_VER,
        .pending_comp_ver.version_str_length = PLDM_FWUP_PENDING_MIDDLEWARE_COMP_VER_LEN,
        .pending_comp_ver.version_str_type = PLDM_STR_TYPE_ASCII,
    }
};

const struct pldm_fwup_protocol_firmware_parameters PLDM_FWUP_FD_FIRMWARE_PARAMETERS = {
    .capabilities_during_update.value = 0,
    .active_comp_img_set_ver.version_str_type = PLDM_STR_TYPE_ASCII,
    .active_comp_img_set_ver.version_str = PLDM_FWUP_ACTIVE_COMP_IMG_SET_VER,
    .active_comp_img_set_ver.version_str_length = PLDM_FWUP_ACTIVE_COMP_IMG_SET_VER_LEN,
    .pending_comp_img_set_ver.version_str_type = PLDM_STR_TYPE_ASCII,
    .pending_comp_img_set_ver.version_str = PLDM_FWUP_PENDING_COMP_IMG_SET_VER,
    .pending_comp_img_set_ver.version_str_length = PLDM_FWUP_PENDING_COMP_IMG_SET_VER_LEN,
    .count = PLDM_FWUP_NUM_COMPONENTS,
    .entries = NULL
};

const struct pldm_fwup_fup_component_image_entry PLDM_FWUP_UA_FUP_COMP_INFO_LIST[] = {
    {
        .comp_classification = PLDM_COMP_FIRMWARE,
        .comp_identifier = 29485,
        .comp_comparison_stamp = 3780935208,
        .comp_ver.version_str = PLDM_FWUP_PENDING_FIRMWARE_COMP_VER,
        .comp_ver.version_str_length = PLDM_FWUP_PENDING_FIRMWARE_COMP_VER_LEN,
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
        .comp_ver.version_str = PLDM_FWUP_PENDING_MIDDLEWARE_COMP_VER,
        .comp_ver.version_str_length = PLDM_FWUP_PENDING_MIDDLEWARE_COMP_VER_LEN,
        .comp_ver.version_str_type = PLDM_STR_TYPE_ASCII,
        .comp_size = PLDM_FWUP_COMP_SIZE,
        .comp_options.value = 0,
        .comp_options.bits.bit0 = 1,
        .requested_comp_activation_method.value = 0,
        .requested_comp_activation_method.bits.bit1 = 1
    }
};

const struct pldm_fwup_protocol_version_string PLDM_FWUP_UA_FUP_COMP_IMG_SET_VER = {
    .version_str = PLDM_FWUP_PENDING_COMP_IMG_SET_VER,
    .version_str_length = PLDM_FWUP_PENDING_COMP_IMG_SET_VER_LEN,
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

const struct pldm_fwup_flash_manager PLDM_FWUP_FD_FLASH_MANAGER = {
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



void setup_flash_ctx(struct pldm_fwup_protocol_flash_ctx *flash_ctx, CuTest *test) 
{
    int status = flash_virtual_disk_init(&flash_ctx->fd_flash, PLDM_FWUP_FLASH_FD, &flash_ctx->fd_flash_state, PLDM_FWUP_FLASH_SIZE);
    CuAssertIntEquals(test, 0, status);

    status = flash_virtual_disk_init(&flash_ctx->ua_flash, PLDM_FWUP_FLASH_UA, &flash_ctx->ua_flash_state, PLDM_FWUP_FLASH_SIZE);
    CuAssertIntEquals(test, 0, status);
}

void release_flash_ctx(struct pldm_fwup_protocol_flash_ctx *flash_ctx)
{
    flash_virtual_disk_release(&flash_ctx->fd_flash);
    flash_virtual_disk_release(&flash_ctx->ua_flash);
}

void setup_testing_ctx(struct pldm_fwup_protocol_testing_ctx *testing_ctx, struct pldm_fwup_protocol_flash_ctx *flash_ctx) 
{
    testing_ctx->fd_fw_parameters = PLDM_FWUP_FD_FIRMWARE_PARAMETERS;
    testing_ctx->fd_fw_parameters.entries = platform_calloc(PLDM_FWUP_NUM_COMPONENTS, sizeof (struct pldm_fwup_protocol_component_parameter_entry));
    memcpy(testing_ctx->fd_fw_parameters.entries, PLDM_FWUP_PARAMETER_ENTRIES, PLDM_FWUP_NUM_COMPONENTS * sizeof (struct pldm_fwup_protocol_component_parameter_entry));

    testing_ctx->fup_comp_img_list = platform_calloc(PLDM_FWUP_NUM_COMPONENTS, sizeof (struct pldm_fwup_fup_component_image_entry));
    memcpy(testing_ctx->fup_comp_img_list, PLDM_FWUP_UA_FUP_COMP_INFO_LIST, PLDM_FWUP_NUM_COMPONENTS * sizeof (struct pldm_fwup_fup_component_image_entry));

    testing_ctx->fd_flash_mgr = PLDM_FWUP_FD_FLASH_MANAGER;
    testing_ctx->fd_flash_mgr.comp_regions = platform_calloc(PLDM_FWUP_NUM_COMPONENTS, sizeof (struct flash_region));
    memcpy(testing_ctx->fd_flash_mgr.comp_regions, PLDM_FWUP_COMP_REGIONS, PLDM_FWUP_NUM_COMPONENTS * sizeof (struct flash_region));
    testing_ctx->fd_flash_mgr.flash = &flash_ctx->fd_flash.base;

    testing_ctx->ua_flash_mgr = PLDM_FWUP_UA_FLASH_MANAGER;
    testing_ctx->ua_flash_mgr.comp_regions = platform_calloc(PLDM_FWUP_NUM_COMPONENTS, sizeof (struct flash_region));
    memcpy(testing_ctx->ua_flash_mgr.comp_regions, PLDM_FWUP_COMP_REGIONS, PLDM_FWUP_NUM_COMPONENTS * sizeof (struct flash_region));
    testing_ctx->ua_flash_mgr.flash = &flash_ctx->ua_flash.base;

    testing_ctx->fup_comp_img_set_ver = PLDM_FWUP_UA_FUP_COMP_IMG_SET_VER;

}

void release_testing_ctx(struct pldm_fwup_protocol_testing_ctx *testing_ctx)
{
    free(testing_ctx->fd_fw_parameters.entries);
    free(testing_ctx->fup_comp_img_list);
    free(testing_ctx->fd_flash_mgr.comp_regions);
    free(testing_ctx->ua_flash_mgr.comp_regions);
}


void setup_ua_device_manager(struct device_manager *device_mgr, CuTest *test)
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

void setup_fd_device_manager(struct device_manager *device_mgr, CuTest *test)
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

void release_device_manager(struct device_manager *device_mgr)
{
    device_manager_release(device_mgr);
}


void setup_testing(struct pldm_fwup_protocol_commands_testing *testing, struct pldm_fwup_protocol_testing_ctx *testing_ctx, CuTest *test)
{
    int status = pldm_fwup_manager_init(&testing->fwup_mgr, &testing_ctx->fd_fw_parameters, testing_ctx->fup_comp_img_list,
        &testing_ctx->fd_flash_mgr, &testing_ctx->ua_flash_mgr, &testing_ctx->fup_comp_img_set_ver, PLDM_FWUP_NUM_COMPONENTS);
    CuAssertIntEquals(test, 0, status);

    status = cmd_interface_pldm_init(&testing->pldm, &testing->fwup_mgr, &testing->device_mgr);
    CuAssertIntEquals(test, 0, status);

    status = cmd_channel_init(&testing->channel, testing->device_mgr.entries[DEVICE_MANAGER_SELF_DEVICE_NUM].smbus_addr);
    CuAssertIntEquals(test, 0, status);

    testing->channel.send_packet = send_packet;
    testing->channel.receive_packet = receive_packet;

    status = mctp_interface_init(&testing->mctp, &testing_ctx->cmd_cerberus, &testing_ctx->cmd_mctp, &testing_ctx->cmd_spdm,
        &testing->pldm.base, &testing->device_mgr);
    CuAssertIntEquals(test, 0, status);

    testing->timeout_ms = PLDM_TESTING_MS_TIMEOUT;
}

void release_testing(struct pldm_fwup_protocol_commands_testing *testing) 
{
    mctp_interface_deinit(&testing->mctp);
    cmd_channel_release(&testing->channel);
    cmd_interface_pldm_deinit(&testing->pldm);
    pldm_fwup_manager_deinit(&testing->fwup_mgr);

}

int receive_and_respond_full_mctp_message(struct cmd_channel *channel, struct mctp_interface *mctp, int timeout_ms)
{
    int status;
    int counter = 1;
    do {
        status = cmd_channel_receive_and_process(channel, mctp, timeout_ms);
        printf("Counter: %d.\n", counter);
        if (status != 0) {
            return status;
        }
        counter++;
    } while (mctp->req_buffer.length != 0);
    
    return status;
}


int send_and_receive_full_mctp_message(struct pldm_fwup_protocol_commands_testing *testing, int command)
{
    int status;
    int req_length = pldm_fwup_handler_generate_request(testing->mctp.cmd_pldm, command, testing->req_buffer, sizeof (testing->req_buffer));
    if (ROT_IS_ERROR(req_length) && req_length == CMD_HANDLER_PLDM_TRANSPORT_ERROR) {
        return CMD_HANDLER_PLDM_TRANSPORT_ERROR;
    } 
    else if (ROT_IS_ERROR(req_length) && req_length == PLDM_FWUP_HANDLER_UNKNOWN_REQUEST) {
        return PLDM_FWUP_HANDLER_UNKNOWN_REQUEST;
    }

    uint8_t addr = testing->device_mgr.entries[2].smbus_addr;
    uint8_t eid = testing->device_mgr.entries[2].eid;
    
    status = mctp_interface_issue_request(&testing->mctp, &testing->channel, addr, eid, testing->req_buffer, 
        req_length, testing->req_buffer, sizeof (testing->req_buffer), 0);
    if (status != 0) {
        return status;
    }

    status = receive_and_respond_full_mctp_message(&testing->channel, &testing->mctp, testing->timeout_ms);
    return status;
}