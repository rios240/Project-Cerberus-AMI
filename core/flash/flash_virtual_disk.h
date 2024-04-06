#ifndef FLASH_VIRTUAL_DISK_H_
#define FLASH_VIRTUAL_DISK_H_

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "status/rot_status.h"
#include "platform_api.h"
#include "flash.h"


/**
 * Block size of the virtual flash instance.
 */
#define	VIRTUAL_FLASH_DISK_BLOCK_SIZE		256
   

enum {
    VIRTUAL_FLASH_DISK_OPEN_FAILURE,
    VIRTUAL_FLASH_DISK_CLOSE_FAILURE,
    VIRTUAL_FLASH_DISK_SEEK_FAILURE,
    VIRTUAL_FLASH_DISK_READ_FAILURE,
    VIRTUAL_FLASH_DISK_WRITE_FAILURE
};

/**
 * Variable context for a virtual flash instance.
 */
struct flash_virtual_disk_state {
	platform_mutex lock;			            /**< Lock to synchronize access to the hardware. */
};


/**
 * Defines a flash implementation that uses disk as a virtual flash device. This can be
 * used in the same way as any other flash device.
 */
struct flash_virtual_disk {
	struct flash base;							/**< Base flash API. */
	struct flash_virtual_disk_state *state;		/**< Variable context for the virtual  instance. */
    const char *disk_region;                    /**< Pointer to the disk region (file path must be in relation to the location of flash_virtual_disk.c). */
    size_t size;								/**< Size in bytes. */
};


int flash_virtual_disk_init (struct flash_virtual_disk *virtual_flash,
	struct flash_virtual_disk_state *state_ptr, size_t size);
int flash_virtual_disk_init_state (struct flash_virtual_disk *virtual_disk);
void flash_virtual_disk_release (struct flash_virtual_disk *virtual_disk);


#endif /* FLASH_VIRTUAL_DISK_H_ */