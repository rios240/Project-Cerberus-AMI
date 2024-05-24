#ifndef PLDM_FWUP_PROTOCOL_COMMANDS_H_
#define PLDM_FWUP_PROTOCOL_COMMANDS_H_


#include <stdint.h>
#include "cmd_interface_pldm.h"
#include "cmd_interface/cmd_interface.h"
#include "cmd_interface/device_manager.h"
#include "mctp/mctp_base_protocol.h"
#include "pldm_fwup_protocol.h"
#include "platform_config.h"
#include "libpldm/base.h"
#include "libpldm/firmware_update.h"

/*******************
 * FD Inventory commands
 *******************/
int pldm_fwup_process_query_device_identifiers_request(struct pldm_fwup_fd_state *state,
    struct device_manager *device_mgr, struct cmd_interface_msg *request);
int pldm_fwup_prcocess_get_firmware_parameters_request(struct pldm_fwup_fd_state *state,
    struct pldm_fwup_protocol_firmware_parameters *fw_parameters, struct cmd_interface_msg *request);

/*******************
 * FD Update commands
 *******************/
int pldm_fwup_process_request_update_request(struct pldm_fwup_fd_state *state, 
    struct pldm_fwup_flash_manager *flash_mgr, struct pldm_fwup_fd_update_info *update_info,
    struct cmd_interface_msg *request);
    
int pldm_fwup_generate_get_package_data_request(struct pldm_fwup_fd_state *state, 
    struct pldm_fwup_protocol_multipart_transfer *get_cmd_state,
    uint8_t *buffer, size_t buf_len);
int pldm_fwup_process_get_package_data_response(struct pldm_fwup_fd_state *state, 
    struct pldm_fwup_flash_manager *flash_mgr, struct pldm_fwup_protocol_multipart_transfer *get_cmd_state,
    struct cmd_interface_msg *response);

int pldm_fwup_process_get_device_meta_data_request(struct pldm_fwup_fd_state *state, 
    struct pldm_fwup_flash_manager *flash_mgr, struct pldm_fwup_protocol_multipart_transfer *get_cmd_state,
    struct pldm_fwup_fd_update_info *update_info, struct cmd_interface_msg *request);

int pldm_fwup_process_pass_component_table_request(struct pldm_fwup_fd_state *state, 
    struct pldm_fwup_fd_update_info *update_info, struct pldm_fwup_protocol_firmware_parameters *fw_parameters,
    struct cmd_interface_msg *request);

int pldm_fwup_process_update_component_request(struct pldm_fwup_fd_state *state,
    struct pldm_fwup_fd_update_info *update_info, struct pldm_fwup_protocol_component_entry *comp_entries, 
    struct cmd_interface_msg *request);

int pldm_fwup_generate_request_firmware_data_request(struct pldm_fwup_fd_state *state,
    struct pldm_fwup_fd_update_info *update_info, uint8_t *buffer, size_t buf_len);
int pldm_fwup_process_request_firmware_data_response(struct pldm_fwup_fd_state *state,
    struct pldm_fwup_fd_update_info *update_info, struct pldm_fwup_flash_manager *flash_mgr, 
    struct cmd_interface_msg *response);

int pldm_fwup_generate_transfer_complete_request(struct pldm_fwup_fd_state *state, uint8_t *buffer, size_t buf_len);
int pldm_fwup_process_transfer_complete_response(struct pldm_fwup_fd_state *state, struct cmd_interface_msg *response);

int pldm_fwup_generate_verify_complete_request(struct pldm_fwup_fd_state *state, uint8_t *buffer, size_t buf_len);
int pldm_fwup_process_verify_complete_response(struct pldm_fwup_fd_state *state, struct cmd_interface_msg *response);

int pldm_fwup_generate_apply_complete_request(struct pldm_fwup_fd_state *state, uint8_t *buffer, size_t buf_len);
int pldm_fwup_process_apply_complete_response(struct pldm_fwup_fd_state *state, struct cmd_interface_msg *response);

int pldm_fwup_generate_get_meta_data_request(struct pldm_fwup_fd_state *state, 
    struct pldm_fwup_protocol_multipart_transfer *get_cmd_state,
    uint8_t *buffer, size_t buf_len);
int pldm_fwup_process_get_meta_data_response(struct pldm_fwup_fd_state *state, 
    struct pldm_fwup_flash_manager *flash_mgr, struct pldm_fwup_protocol_multipart_transfer *get_cmd_state,
    struct cmd_interface_msg *response);

int pldm_fwup_process_activate_firmware_request(struct pldm_fwup_fd_state *state, 
    struct pldm_fwup_fd_update_info *update_info, struct cmd_interface_msg *request);

int pldm_fwup_process_get_status_request(struct pldm_fwup_fd_state *state, 
    struct pldm_fwup_fd_update_info *update_info, struct cmd_interface_msg *request);

int pldm_fwup_process_cancel_update_component_request(struct pldm_fwup_fd_state *state, 
    struct pldm_fwup_fd_update_info *update_info, struct cmd_interface_msg *request);

int pldm_fwup_process_cancel_update_request(struct pldm_fwup_fd_state *state, 
    struct pldm_fwup_fd_update_info *update_info, struct pldm_fwup_flash_manager *flash_mgr, struct cmd_interface_msg *request);

/*******************
 * UA Inventory commands
 *******************/

int pldm_fwup_generate_query_device_identifiers_request(struct pldm_fwup_ua_state *state, uint8_t *buffer, size_t buf_len);
int pldm_fwup_process_query_device_identifiers_response(struct pldm_fwup_ua_state *state, 
    struct device_manager *device_mgr, struct cmd_interface_msg *response);

int pldm_fwup_generate_get_firmware_parameters_request(struct pldm_fwup_ua_state *state, uint8_t *buffer, size_t buf_len);
int pldm_fwup_process_get_firmware_parameters_response(struct pldm_fwup_ua_state *state,
    struct pldm_fwup_protocol_firmware_parameters *rec_fw_parameters, struct cmd_interface_msg *response);

/*******************
 * UA Update commands
 *******************/

int pldm_fwup_generate_request_update_request(struct pldm_fwup_ua_manager *ua_mgr, 
    uint8_t *buffer, size_t buf_len);
int pldm_fwup_process_request_update_response(struct pldm_fwup_ua_state *state, 
    struct pldm_fwup_ua_update_info *update_info, struct cmd_interface_msg *response);

int pldm_fwup_process_get_package_data_request(struct pldm_fwup_ua_state *state, 
    struct pldm_fwup_flash_manager *flash_mgr, struct pldm_fwup_protocol_multipart_transfer *get_cmd_state,
    struct cmd_interface_msg *request);


int pldm_fwup_generate_get_device_meta_data_request(struct pldm_fwup_ua_state *state, 
    struct pldm_fwup_protocol_multipart_transfer *get_cmd_state,
    uint8_t *buffer, size_t buf_len);
int pldm_fwup_process_get_device_meta_data_response(struct pldm_fwup_ua_state *state, 
    struct pldm_fwup_flash_manager *flash_mgr, struct pldm_fwup_protocol_multipart_transfer *get_cmd_state,
    struct cmd_interface_msg *response);

int pldm_fwup_generate_pass_component_table_request(struct pldm_fwup_ua_manager *ua_mgr,
    uint8_t *buffer, size_t buf_len);
int pldm_fwup_process_pass_component_table_response(struct pldm_fwup_ua_state *state, 
    struct pldm_fwup_ua_update_info *update_info, struct cmd_interface_msg *response);

int pldm_fwup_generate_update_component_request(struct pldm_fwup_ua_state *state,
    uint16_t current_comp_num, struct pldm_fwup_fup_component_image_entry *comp_img_entries,
    struct pldm_fwup_protocol_firmware_parameters *rec_fw_parameters, uint8_t *buffer, size_t buf_len);

int pldm_fwup_process_update_component_response(struct pldm_fwup_ua_state *state,
    struct pldm_fwup_ua_update_info *update_info, struct cmd_interface_msg *response);

int pldm_fwup_process_request_firmware_data_request(struct pldm_fwup_ua_state *state,
    uint16_t current_comp_num, struct pldm_fwup_fup_component_image_entry *comp_img_entries,
    struct pldm_fwup_flash_manager *flash_mgr, struct cmd_interface_msg *request);

int pldm_fwup_process_transfer_complete_request(struct pldm_fwup_ua_state *state, 
    struct pldm_fwup_ua_update_info *update_info, struct cmd_interface_msg *request);

int pldm_fwup_process_verify_complete_request(struct pldm_fwup_ua_state *state,
    struct pldm_fwup_ua_update_info *update_info, struct cmd_interface_msg *request);

int pldm_fwup_process_apply_complete_request(struct pldm_fwup_ua_state *state,
    struct pldm_fwup_ua_update_info *update_info, struct cmd_interface_msg *request);

int pldm_fwup_process_get_meta_data_request(struct pldm_fwup_ua_state *state, 
    struct pldm_fwup_flash_manager *flash_mgr, struct pldm_fwup_protocol_multipart_transfer *get_cmd_state,
    struct cmd_interface_msg *request);

int pldm_fwup_generate_activate_firmware_request(struct pldm_fwup_ua_state *state, uint8_t *buffer, size_t buf_len);
int pldm_fwup_process_activate_firmware_response(struct pldm_fwup_ua_state *state, 
    struct pldm_fwup_ua_update_info *update_info, struct cmd_interface_msg *response);

int pldm_fwup_generate_get_status_request(struct pldm_fwup_ua_state *state, uint8_t *buffer, size_t buf_len);
int pldm_fwup_process_get_status_response(struct pldm_fwup_ua_state *state, struct cmd_interface_msg *response);

int pldm_fwup_generate_cancel_update_component_request(struct pldm_fwup_ua_state *state, uint8_t *buffer, size_t buf_len);
int pldm_fwup_process_cancel_update_component_response(struct pldm_fwup_ua_state *state, struct cmd_interface_msg *response);

int pldm_fwup_generate_cancel_update_request(struct pldm_fwup_ua_state *state, uint8_t *buffer, size_t buf_len);
int pldm_fwup_process_cancel_update_response(struct pldm_fwup_ua_state *state, struct cmd_interface_msg *response);

#endif /* PLDM_FWUP_PROTOCOL_COMMANDS_H_ */