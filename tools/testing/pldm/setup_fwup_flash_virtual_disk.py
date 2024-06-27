import os
import random

'''Virtual flash setup for PLDM FWUP testing.'''

def setup_fwup_flash_virtual_disk(filename, regions):
    with open(filename, 'wb') as f:
        for region in regions:
            if region == 'empty':
                f.write(bytes(1024 * 1024 * 1024))
            elif region == 'random':
                f.write(os.urandom(1024 * 1024 * 1024)) 


fd_flash_regions = ['empty', 'random', 'empty', 'empty']
ua_flash_regions = ['random', 'empty', 'random', 'random']

setup_fwup_flash_virtual_disk('../../../core/testing/pldm/flash_virtual_disk/fd_flash.bin', fd_flash_regions)
setup_fwup_flash_virtual_disk('../../../core/testing/pldm/flash_virtual_disk/ua_flash.bin', ua_flash_regions)

print("Flash virtual disk setup successfully.")