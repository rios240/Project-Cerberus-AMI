#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "flash_virtual_disk.h"




int flash_virtual_disk_get_device_size (const struct flash *virtual_flash, uint32_t *bytes)
{
	const struct flash_virtual_disk *disk = (const struct flash_virtual_disk*) virtual_flash;

	if ((disk == NULL) || (bytes == NULL)) {
		return FLASH_INVALID_ARGUMENT;
	}

	*bytes = disk->size;

	return 0;
}

int flash_virtual_disk_read (const struct flash *virtual_flash, uint32_t address, uint8_t *data,
	size_t length)
{
	struct flash_virtual_disk *disk = (struct flash_virtual_disk*) virtual_flash;

	if ((disk == NULL) || (data == NULL)) {
		return FLASH_INVALID_ARGUMENT;
	}

	if ((address >= disk->size) || (length > (disk->size - address))) {
		return FLASH_ADDRESS_OUT_OF_RANGE;
	}

	platform_mutex_lock (&disk->state->lock);

	FILE *file = fopen(disk->disk_region, "rb");
    if (file == NULL) {
        platform_mutex_unlock (&disk->state->lock);
        return VIRTUAL_FLASH_DISK_OPEN_FAILURE;
    }

    if (fseek(file, address, SEEK_SET) != 0) {
        fclose(file);
        platform_mutex_unlock (&disk->state->lock);
        return VIRTUAL_FLASH_DISK_SEEK_FAILURE;
    }

    size_t bytes = fread(data, sizeof(uint8_t), length, file);

    if (bytes < length) {
        fclose(file);
        platform_mutex_unlock (&disk->state->lock);
        return VIRTUAL_FLASH_DISK_READ_FAILURE;
    }

    fclose(file);

	platform_mutex_unlock (&disk->state->lock);

	return 0;
}


int flash_virtual_disk_get_block_size (const struct flash *virtual_flash, uint32_t *bytes)
{
	if ((virtual_flash == NULL) || (bytes == NULL)) {
		return FLASH_INVALID_ARGUMENT;
	}

	*bytes = VIRTUAL_FLASH_DISK_BLOCK_SIZE;

	return 0;
}

int flash_virtual_disk_write (const struct flash *virtual_flash, uint32_t address,
	const uint8_t *data, size_t length)
{
	struct flash_virtual_disk *disk = (struct flash_virtual_disk*) virtual_flash;

	if ((disk == NULL) || (data == NULL)) {
		return FLASH_INVALID_ARGUMENT;
	}

	if ((address >= disk->size) || (length > (disk->size - address))) {
		return FLASH_ADDRESS_OUT_OF_RANGE;
	}

	platform_mutex_lock (&disk->state->lock);

	FILE *file = fopen(disk->disk_region, "r+b");
    if (file == NULL) {
        platform_mutex_unlock (&disk->state->lock);
        return VIRTUAL_FLASH_DISK_OPEN_FAILURE;
    }

    if (fseek(file, address, SEEK_SET) != 0) {
        fclose(file);
        platform_mutex_unlock (&disk->state->lock);
        return VIRTUAL_FLASH_DISK_SEEK_FAILURE;
    }

    size_t bytes = fwrite(data, sizeof(uint8_t), length, file);

    if (bytes < length) {
        fclose(file);
        platform_mutex_unlock (&disk->state->lock);
        return VIRTUAL_FLASH_DISK_WRITE_FAILURE;
    }

    fclose(file);

	platform_mutex_unlock (&disk->state->lock);

	return length;
}


int flash_virtual_disk_block_erase (const struct flash *virtual_flash, uint32_t address)
{
	const struct flash_virtual_disk *disk = (const struct flash_virtual_disk*) virtual_flash;

	if (disk == NULL) {
		return FLASH_INVALID_ARGUMENT;
	}

	if (address >= disk->size) {
		return FLASH_ADDRESS_OUT_OF_RANGE;
	}

	address = FLASH_REGION_BASE (address, VIRTUAL_FLASH_DISK_BLOCK_SIZE);

	uint8_t *buffer = malloc(VIRTUAL_FLASH_DISK_BLOCK_SIZE * sizeof (uint8_t));
	memset(buffer, 0xFF, VIRTUAL_FLASH_DISK_BLOCK_SIZE);

	platform_mutex_lock (&disk->state->lock);

	FILE *file = fopen(disk->disk_region, "r+b");
    if (file == NULL) {
        platform_mutex_unlock (&disk->state->lock);
        return VIRTUAL_FLASH_DISK_OPEN_FAILURE;
    }

    if (fseek(file, address, SEEK_SET) != 0) {
        fclose(file);
        platform_mutex_unlock (&disk->state->lock);
        return VIRTUAL_FLASH_DISK_SEEK_FAILURE;
    }

    size_t bytes = fwrite(buffer, sizeof(uint8_t), VIRTUAL_FLASH_DISK_BLOCK_SIZE, file);

    if (bytes < VIRTUAL_FLASH_DISK_BLOCK_SIZE) {
        fclose(file);
        platform_mutex_unlock (&disk->state->lock);
        return VIRTUAL_FLASH_DISK_WRITE_FAILURE;
    }

    fclose(file);

	platform_mutex_unlock (&disk->state->lock);

	free(buffer);
	return 0;
}


int flash_virtual_disk_region_erase (const struct flash *virtual_flash)
{
	const struct flash_virtual_disk *disk = (const struct flash_virtual_disk*) virtual_flash;

	if (disk == NULL) {
		return FLASH_INVALID_ARGUMENT;
	}

	uint8_t *buffer = malloc(disk->size * sizeof (uint8_t));
	memset(buffer, 0xFF, disk->size);

	platform_mutex_lock (&disk->state->lock);

	FILE *file = fopen(disk->disk_region, "r+b");
    if (file == NULL) {
        platform_mutex_unlock (&disk->state->lock);
        return VIRTUAL_FLASH_DISK_OPEN_FAILURE;
    }

    size_t bytes = fwrite(buffer, sizeof(uint8_t), disk->size, file);

    if (bytes < VIRTUAL_FLASH_DISK_BLOCK_SIZE) {
        fclose(file);
        platform_mutex_unlock (&disk->state->lock);
        return VIRTUAL_FLASH_DISK_WRITE_FAILURE;
    }

    fclose(file);

	platform_mutex_unlock (&disk->state->lock);

	free(buffer);
	return 0;
}





/**
 * Initialize the virtual flash device using disk for the data storage.
 *
 * @param virtual_flash The device instance to initialize.
 * @param state_ptr Variable context for the virtual flash interface.
 * @param size Maximum size of the disk region.
 *
 * @return 0 if the device was successfully initialized or an error code.
 */
int flash_virtual_disk_init (struct flash_virtual_disk *virtual_flash,
	struct flash_virtual_disk_state *state_ptr, size_t size)
{
	if (virtual_flash == NULL) {
		return FLASH_INVALID_ARGUMENT;
	}

	memset (virtual_flash, 0, sizeof (struct flash_virtual_disk));

	virtual_flash->base.get_device_size = flash_virtual_disk_get_device_size;
	virtual_flash->base.read = flash_virtual_disk_read;
	virtual_flash->base.get_page_size = flash_virtual_disk_get_block_size;
	virtual_flash->base.minimum_write_per_page = flash_virtual_disk_get_block_size;
	virtual_flash->base.write = flash_virtual_disk_write;
	virtual_flash->base.get_sector_size = flash_virtual_disk_get_block_size;
	virtual_flash->base.get_block_size = flash_virtual_disk_get_block_size;
	virtual_flash->base.chip_erase = flash_virtual_disk_region_erase;
	virtual_flash->base.block_erase = flash_virtual_disk_block_erase;

	virtual_flash->size = size;
	virtual_flash->state = state_ptr;

	return flash_virtual_disk_init_state (virtual_flash);
}



/**
 * Initialize only the variable state for an virtual flash interface.  The rest of the interface is
 * assumed to have already been initialized.
 *
 * This would generally be used with a statically initialized instance.
 *
 * @param virtual_flash The virtual flash instance that contains the state to initialize.
 *
 * @return 0 if the state was successfully initialized or an error code.
 */
int flash_virtual_disk_init_state (struct flash_virtual_disk *virtual_flash)
{
	if ((virtual_flash == NULL) || (virtual_flash->state == NULL) ||
	    (virtual_flash->size == 0)) {
		return FLASH_INVALID_ARGUMENT;
	}

	memset (virtual_flash->state, 0, sizeof (struct flash_virtual_disk_state));

	return platform_mutex_init (&virtual_flash->state->lock);
}


/**
 * Release the resources used by the virtual disk instance.
 *
 * @param virtual_flash The virtual flash instance to release.
 */
void flash_virtual_disk_release (struct flash_virtual_disk *virtual_flash)
{
	if (virtual_flash) {
		platform_mutex_free (&virtual_flash->state->lock);
	}
}