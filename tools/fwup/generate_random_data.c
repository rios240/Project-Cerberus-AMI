#include <stdio.h>
#include <stdlib.h>
#include <stdint.h> // Include this header for uint8_t

int main() {
    // Size of the data to generate (2MB)
    const size_t dataSize = 2 * 1024 * 1024; // 2MB
    FILE *file;
    uint8_t *data; // Use uint8_t instead of unsigned char

    // Allocate memory to hold the data
    data = (uint8_t*) malloc(dataSize);
    if (data == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return 1;
    }

    // Generate random data
    for (size_t i = 0; i < dataSize; i++) {
        data[i] = rand() % 256; // rand() returns a pseudo-random integer between 0 and RAND_MAX.
    }

    // Open the file for writing
    file = fopen("/s/bach/j/under/tylerios/Research/Project-Cerberus-AMI/Flash/flash_disk_region_test.bin", "wb");
    if (file == NULL) {Cerberus
        fprintf(stderr, "Failed to open file for writing\n");
        free(data);
        return 1;
    }

    // Write the data to the file
    if (fwrite(data, 1, dataSize, file) != dataSize) {
        fprintf(stderr, "Failed to write data to file\n");
        fclose(file);
        free(data);
        return 1;
    }

    // Clean up
    fclose(file);
    free(data);

    printf("Successfully wrote 2MB of random data to 'disk_flash_region_test.bin'\n");

    return 0;
}
