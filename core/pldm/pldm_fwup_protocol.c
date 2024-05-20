#include "pldm_fwup_protocol.h"
#include "libpldm/firmware_update.h"



void reset_multipart_transfer(struct pldm_fwup_protocol_multipart_transfer *multipart_transfer)
{
    multipart_transfer->transfer_op_flag = PLDM_GET_FIRSTPART;
    multipart_transfer->transfer_flag = PLDM_START;
    multipart_transfer->data_transfer_handle = 0;
    multipart_transfer->next_data_transfer_handle = 0;
}
void reset_firmware_request(struct pldm_fwup_protocol_request_firmware *request_fw)
{
    request_fw->length = 0;
    request_fw->offset = 0;
}