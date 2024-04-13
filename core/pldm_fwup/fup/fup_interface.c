#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "firmware_update.h"
#include "fup_interface.h"
#include "common/unused.h"


void fill_random_bytes(uint8_t *buffer, size_t length) {
    for (size_t i = 0; i < length; i++) {
        buffer[i] = (uint8_t)rand();
    }
}

/**
 * Define a complete Firmware Update Package for testing and write to a virtual flash. This should only be used in a testing environment.
 * In producion the Firmware Update Package will be obtianed by the User Agent from another source.
 * 
 * @param flash The virtual disk flash device to write the Firmware Update Package to.
 * 
 * @return 0 if success otherwise 1
*/
int fup_interface_setup_test_fup(struct flash_virtual_disk *fup)
{

    //setup package header information
    struct package_header_information pkg_hdr_info;
    pkg_hdr_info.uuid[0] = 0xf0;
    pkg_hdr_info.uuid[1] = 0x18;
    pkg_hdr_info.uuid[2] = 0x87;
    pkg_hdr_info.uuid[3] = 0x8c;
    pkg_hdr_info.uuid[4] = 0xcb;
    pkg_hdr_info.uuid[5] = 0x7d;
    pkg_hdr_info.uuid[6] = 0x49;
    pkg_hdr_info.uuid[7] = 0x43;
    pkg_hdr_info.uuid[8] = 0x98;
    pkg_hdr_info.uuid[9] = 0x00;
    pkg_hdr_info.uuid[10] = 0xa0;
    pkg_hdr_info.uuid[11] = 0x2f;
    pkg_hdr_info.uuid[12] = 0x05;
    pkg_hdr_info.uuid[13] = 0x9a;
    pkg_hdr_info.uuid[14] = 0xca;
    pkg_hdr_info.uuid[15] = 0x02;
    pkg_hdr_info.package_header_format_version = 0x01;
    //pkg_hdr_info.package_header_size

    int16_t utc_offset = -120;
    pkg_hdr_info.package_release_date_time[0] = (uint8_t)(utc_offset & 0xFF);
    pkg_hdr_info.package_release_date_time[1] = (uint8_t)((utc_offset >> 8) & 0xFF);

    uint32_t microseconds = 123456;
    pkg_hdr_info.package_release_date_time[2] = (uint8_t)(microseconds & 0xFF);
    pkg_hdr_info.package_release_date_time[3] = (uint8_t)((microseconds >> 8) & 0xFF);
    pkg_hdr_info.package_release_date_time[4] = (uint8_t)((microseconds >> 16) & 0xFF);

    pkg_hdr_info.package_release_date_time[5] = 45;
    pkg_hdr_info.package_release_date_time[6] = 30;
    pkg_hdr_info.package_release_date_time[7] = 15;
    pkg_hdr_info.package_release_date_time[8] = 12;
    pkg_hdr_info.package_release_date_time[9] = 4;

    uint16_t year = 2024;
    pkg_hdr_info.package_release_date_time[10] = (uint8_t)(year & 0xFF);
    pkg_hdr_info.package_release_date_time[11] = (uint8_t)((year >> 8) & 0xFF);
    
    pkg_hdr_info.package_release_date_time[12] = 0x23;

    const char *pkg_version_str = "test_firmware_update_package_v1.0";
    pkg_hdr_info.component_bitmap_bit_length = 8;
    pkg_hdr_info.package_version_string_type = 1;
    pkg_hdr_info.package_version_string_length = strlen(pkg_version_str);

    UNUSED(pkg_hdr_info);

    //setup device id records
    uint8_t device_id_record_count = 1;
    UNUSED(device_id_record_count);

    struct firmware_device_id_record fw_device_id_record;
    fw_device_id_record.descriptor_count = 2;
    fw_device_id_record.device_update_option_flags.value = 0;
    fw_device_id_record.device_update_option_flags.bits.bit0 = 0x01;

    const char *comp_image_set_version_str = "test_cerberus_v1.0";
    fw_device_id_record.comp_image_set_version_string_type = 1;
    fw_device_id_record.comp_image_set_version_string_length = strlen(comp_image_set_version_str);

    
    uint8_t pkg_data[256];
    fill_random_bytes(pkg_data, 256);
    fw_device_id_record.fw_device_pkg_data_length = 256;

    bitfield8_t applicable_comp;
    applicable_comp.byte = 0;
    applicable_comp.bits.bit0 = 0x01;
    applicable_comp.bits.bit1 = 0x01;
    applicable_comp.bits.bit2 = 0x01;

    uint16_t pci_vid = 0x10DE;
    struct descriptor_tlv *pci_vendor_descriptor = (struct descriptor_tlv *)malloc(sizeof (struct descriptor_tlv) 
                                                    - sizeof (uint8_t) + sizeof (pci_vid));
    pci_vendor_descriptor->descriptor_type = 0x0000;
    pci_vendor_descriptor->descriptor_length = sizeof (pci_vid);
    memcpy(pci_vendor_descriptor->descriptor_data, &pci_vid, sizeof (pci_vid));


    uint16_t pci_did = 0x0408;
    struct descriptor_tlv *pci_device_descriptor = (struct descriptor_tlv *)malloc(sizeof (struct descriptor_tlv) 
                                                    - sizeof (uint8_t) + sizeof (pci_did));
    pci_device_descriptor->descriptor_type = 0x0100;
    pci_device_descriptor->descriptor_length = sizeof (pci_did);
    memcpy(pci_device_descriptor->descriptor_data, &pci_did, sizeof (pci_did));


    fw_device_id_record.record_length = sizeof (struct firmware_device_id_record) + fw_device_id_record.comp_image_set_version_string_length
                                        + fw_device_id_record.fw_device_pkg_data_length + sizeof (applicable_comp)
                                        + pci_vendor_descriptor->descriptor_length + pci_vendor_descriptor->descriptor_length;


    return 0;

}