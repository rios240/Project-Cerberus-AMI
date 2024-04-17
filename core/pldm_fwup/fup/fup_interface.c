#include <stdio.h>
#include <stdlib.h>
#include <string.h>



#include "utils.h"
#include "fup_interface.h"
#include "common/unused.h"


void fill_random_bytes(uint8_t *buffer, size_t length) {
    for (size_t i = 0; i < length; i++) {
        buffer[i] = (uint8_t)rand();
    }
}

#ifdef PLDM_FWUP_UA_ENABLE

#include "firmware_update.h"

/**
 * Define a complete Firmware Update Package for testing and write to a virtual flash. This should only be used in a testing environment.
 * In producion the Firmware Update Package will be obtianed by the User Agent from another source.
 * 
 * @param flash The virtual disk flash device to write the Firmware Update Package to.
 * 
 * @return 0 if success otherwise 1
*/
int fup_interface_setup_test_fup(const struct flash *fup, size_t fup_size, uint32_t fup_base_addr)
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

    pkg_hdr_info.component_bitmap_bit_length = 8;

    const char *pkg_version_str = "test_firmware_update_package_v1.0";
    pkg_hdr_info.package_version_string_type = 1;
    pkg_hdr_info.package_version_string_length = strlen(pkg_version_str);



    //setup device id records
    uint8_t device_id_record_count = 1;


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
    applicable_comp.bits.bit0 = 1;
    applicable_comp.bits.bit1 = 1;
    applicable_comp.bits.bit2 = 1;

    uint16_t pci_vid = 0x10DE;
    size_t pci_vendor_descriptor_size = sizeof (struct descriptor_tlv) - sizeof (uint8_t) + sizeof (pci_vid);
    struct descriptor_tlv *pci_vendor_descriptor = (struct descriptor_tlv *)malloc(pci_vendor_descriptor_size);
    pci_vendor_descriptor->descriptor_type = 0x0000;
    pci_vendor_descriptor->descriptor_length = sizeof (pci_vid);
    memcpy(pci_vendor_descriptor->descriptor_data, &pci_vid, sizeof (pci_vid));


    uint16_t pci_did = 0x0408;
    size_t pci_device_descriptor_size = sizeof (struct descriptor_tlv) - sizeof (uint8_t) + sizeof (pci_did);
    struct descriptor_tlv *pci_device_descriptor = (struct descriptor_tlv *)malloc(pci_device_descriptor_size);
    pci_device_descriptor->descriptor_type = 0x0100;
    pci_device_descriptor->descriptor_length = sizeof (pci_did);
    memcpy(pci_device_descriptor->descriptor_data, &pci_did, sizeof (pci_did));


    fw_device_id_record.record_length = sizeof (struct firmware_device_id_record) + fw_device_id_record.comp_image_set_version_string_length
                                        + fw_device_id_record.fw_device_pkg_data_length + sizeof (applicable_comp)
                                        + (2 * (sizeof (struct descriptor_tlv) - sizeof (uint8_t)))
                                        + pci_vendor_descriptor->descriptor_length + pci_device_descriptor->descriptor_length;


    uint16_t comp_image_count = 3;

    struct component_image_information comp_one_info;
    comp_one_info.comp_classification = 0x000a;
    comp_one_info.comp_identifier = 0x3e7a;
    comp_one_info.comp_comparison_stamp = 0xa2f4c18b;
    comp_one_info.comp_options.value = 0;
    comp_one_info.comp_options.bits.bit1 = 1;
    comp_one_info.requested_comp_activation_method.value = 0;
    comp_one_info.requested_comp_activation_method.bits.bit0 = 1;
    comp_one_info.comp_size = 1048576;
    uint8_t comp_one[1048576];
    fill_random_bytes(comp_one, 1048576);

    const char *comp_one_version_str = "test_firmware_v1.0";
    comp_one_info.comp_version_string_type = 1;
    comp_one_info.comp_version_string_length = strlen(comp_one_version_str);

    struct component_image_information comp_two_info;
    comp_two_info.comp_classification = 0x0006;
    comp_two_info.comp_identifier = 0xb4f3;
    comp_two_info.comp_comparison_stamp = 0x7e4c9d23;
    comp_two_info.comp_options.value = 0;
    comp_two_info.comp_options.bits.bit1 = 1;
    comp_two_info.requested_comp_activation_method.value = 0;
    comp_two_info.requested_comp_activation_method.bits.bit0 = 1;
    comp_two_info.comp_size = 1048576;
    uint8_t comp_two[1048576];
    fill_random_bytes(comp_two, 1048576);

    const char *comp_two_version_str = "test_firmware/bios_v1.0";
    comp_two_info.comp_version_string_type = 1;
    comp_two_info.comp_version_string_length = strlen(comp_two_version_str);

    struct component_image_information comp_three_info;
    comp_three_info.comp_classification = 0x0003;
    comp_three_info.comp_identifier = 0x1a3f;
    comp_three_info.comp_comparison_stamp = 0x5f3a2c8b;
    comp_three_info.comp_options.value = 0;
    comp_three_info.comp_options.bits.bit1 = 1;
    comp_three_info.requested_comp_activation_method.value = 0;
    comp_three_info.requested_comp_activation_method.bits.bit0 = 1;
    comp_three_info.comp_size = 1048576;
    uint8_t comp_three[1048576];
    fill_random_bytes(comp_three, 1048576);

    const char *comp_three_version_str = "test_configuration_v1.0";
    comp_three_info.comp_version_string_type = 1;
    comp_three_info.comp_version_string_length = strlen(comp_three_version_str);

    comp_one_info.comp_location_offset = sizeof (pkg_hdr_info) + pkg_hdr_info.package_version_string_length
                                        + sizeof (device_id_record_count) + fw_device_id_record.record_length
                                        + sizeof (comp_image_count) + (3 * sizeof (struct component_image_information))
                                        + comp_one_info.comp_version_string_length + comp_two_info.comp_version_string_length
                                        + comp_three_info.comp_version_string_length;

    comp_two_info.comp_location_offset = sizeof (pkg_hdr_info) + pkg_hdr_info.package_version_string_length
                                        + sizeof (device_id_record_count) + fw_device_id_record.record_length
                                        + sizeof (comp_image_count) + (3 * sizeof (struct component_image_information))
                                        + comp_one_info.comp_version_string_length + comp_two_info.comp_version_string_length
                                        + comp_three_info.comp_version_string_length + comp_one_info.comp_size;

    comp_three_info.comp_location_offset = sizeof (pkg_hdr_info) + pkg_hdr_info.package_version_string_length
                                        + sizeof (device_id_record_count) + fw_device_id_record.record_length
                                        + sizeof (comp_image_count) + (3 * sizeof (struct component_image_information))
                                        + comp_one_info.comp_version_string_length + comp_two_info.comp_version_string_length
                                        + comp_three_info.comp_version_string_length + comp_one_info.comp_size + comp_two_info.comp_size;

    pkg_hdr_info.package_header_size = sizeof (pkg_hdr_info) + pkg_hdr_info.package_version_string_length
                                        + sizeof (device_id_record_count) + fw_device_id_record.record_length
                                        + sizeof (comp_image_count) + (3 * sizeof (struct component_image_information))
                                        + comp_one_info.comp_version_string_length + comp_two_info.comp_version_string_length
                                        + comp_three_info.comp_version_string_length;

    uint8_t fup_buf[pkg_hdr_info.package_header_size + comp_one_info.comp_size + comp_two_info.comp_size + comp_three_info.comp_size + sizeof (uint32_t)];
    memcpy(fup_buf, &pkg_hdr_info, sizeof (pkg_hdr_info));
    memcpy(fup_buf + sizeof (pkg_hdr_info), pkg_version_str, pkg_hdr_info.package_version_string_length);
    memcpy(fup_buf + sizeof (pkg_hdr_info) + pkg_hdr_info.package_version_string_length,
            &device_id_record_count, sizeof (device_id_record_count));
    memcpy(fup_buf + sizeof (pkg_hdr_info) + pkg_hdr_info.package_version_string_length + sizeof (device_id_record_count),
            &fw_device_id_record, sizeof (fw_device_id_record));
    memcpy(fup_buf + sizeof (pkg_hdr_info) + pkg_hdr_info.package_version_string_length + sizeof (device_id_record_count)
            + sizeof (fw_device_id_record), &applicable_comp, sizeof (applicable_comp));
    memcpy(fup_buf + sizeof (pkg_hdr_info) + pkg_hdr_info.package_version_string_length + sizeof (device_id_record_count)
            + sizeof (fw_device_id_record) + sizeof (applicable_comp), &comp_image_set_version_str, fw_device_id_record.comp_image_set_version_string_length);
    memcpy(fup_buf + sizeof (pkg_hdr_info) + pkg_hdr_info.package_version_string_length + sizeof (device_id_record_count)
            + sizeof (fw_device_id_record) + sizeof (applicable_comp) + fw_device_id_record.comp_image_set_version_string_length,
            pci_vendor_descriptor, pci_vendor_descriptor_size);
    memcpy(fup_buf + sizeof (pkg_hdr_info) + pkg_hdr_info.package_version_string_length + sizeof (device_id_record_count)
            + sizeof (fw_device_id_record) + sizeof (applicable_comp) + fw_device_id_record.comp_image_set_version_string_length
            + pci_vendor_descriptor_size, pci_device_descriptor, pci_device_descriptor_size);
    memcpy(fup_buf + sizeof (pkg_hdr_info) + pkg_hdr_info.package_version_string_length + sizeof (device_id_record_count)
            + sizeof (fw_device_id_record) + sizeof (applicable_comp) + fw_device_id_record.comp_image_set_version_string_length
            + pci_vendor_descriptor_size + pci_device_descriptor_size, pkg_data, 256);
    memcpy(fup_buf + sizeof (pkg_hdr_info) + pkg_hdr_info.package_version_string_length + sizeof (device_id_record_count)
            + sizeof (fw_device_id_record) + sizeof (applicable_comp) + fw_device_id_record.comp_image_set_version_string_length
            + pci_vendor_descriptor_size + pci_device_descriptor_size + 256, &comp_one_info, sizeof (comp_one_info));
    memcpy(fup_buf + sizeof (pkg_hdr_info) + pkg_hdr_info.package_version_string_length + sizeof (device_id_record_count)
            + sizeof (fw_device_id_record) + sizeof (applicable_comp) + fw_device_id_record.comp_image_set_version_string_length
            + pci_vendor_descriptor_size + pci_device_descriptor_size + 256 + sizeof (comp_one_info), comp_one_version_str, comp_one_info.comp_version_string_length);
    memcpy(fup_buf + sizeof (pkg_hdr_info) + pkg_hdr_info.package_version_string_length + sizeof (device_id_record_count)
            + sizeof (fw_device_id_record) + sizeof (applicable_comp) + fw_device_id_record.comp_image_set_version_string_length
            + pci_vendor_descriptor_size + pci_device_descriptor_size + 256 + sizeof (comp_one_info) + comp_one_info.comp_version_string_length,
            &comp_two_info, sizeof (comp_two_info));
    memcpy(fup_buf + sizeof (pkg_hdr_info) + pkg_hdr_info.package_version_string_length + sizeof (device_id_record_count)
            + sizeof (fw_device_id_record) + sizeof (applicable_comp) + fw_device_id_record.comp_image_set_version_string_length
            + pci_vendor_descriptor_size + pci_device_descriptor_size + 256 + sizeof (comp_one_info) + comp_one_info.comp_version_string_length
            + sizeof (comp_two_info), comp_two_version_str, comp_two_info.comp_version_string_length);
    memcpy(fup_buf + sizeof (pkg_hdr_info) + pkg_hdr_info.package_version_string_length + sizeof (device_id_record_count)
            + sizeof (fw_device_id_record) + sizeof (applicable_comp) + fw_device_id_record.comp_image_set_version_string_length
            + pci_vendor_descriptor_size + pci_device_descriptor_size + 256 + sizeof (comp_one_info) + comp_one_info.comp_version_string_length
            + sizeof (comp_two_info) + comp_two_info.comp_version_string_length, &comp_three_info, sizeof (comp_three_info));
    memcpy(fup_buf + sizeof (pkg_hdr_info) + pkg_hdr_info.package_version_string_length + sizeof (device_id_record_count)
            + sizeof (fw_device_id_record) + sizeof (applicable_comp) + fw_device_id_record.comp_image_set_version_string_length
            + pci_vendor_descriptor_size + pci_device_descriptor_size + 256 + sizeof (comp_one_info) + comp_one_info.comp_version_string_length
            + sizeof (comp_two_info) + comp_two_info.comp_version_string_length + sizeof (comp_three_info), comp_three_version_str, comp_three_info.comp_version_string_length);

    uint32_t crc = crc32(fup_buf, pkg_hdr_info.package_header_size + comp_one_info.comp_size + comp_two_info.comp_size + comp_three_info.comp_size);
    memcpy(fup_buf + sizeof (pkg_hdr_info) + pkg_hdr_info.package_version_string_length + sizeof (device_id_record_count)
            + sizeof (fw_device_id_record) + sizeof (applicable_comp) + fw_device_id_record.comp_image_set_version_string_length
            + pci_vendor_descriptor_size + pci_device_descriptor_size + 256 + sizeof (comp_one_info) + comp_one_info.comp_version_string_length
            + sizeof (comp_two_info) + comp_two_info.comp_version_string_length + sizeof (comp_three_info) + comp_three_info.comp_version_string_length,
            &crc, sizeof (crc));

    if (fup == NULL || fup_size < sizeof (fup_buf)) {
        free(pci_vendor_descriptor);
        free(pci_device_descriptor);
        return 1;
    }
    
    fup->write(fup, fup_base_addr, fup_buf, sizeof (fup_buf));

    return 0;

}

#endif