#include <stdio.h>
#include <stdlib.h>
#include <gdal.h>
#include <cpl_conv.h> // for CPLMalloc()
#include <stdint.h>
#include <string.h>

// Function to convert RGB values to grayscale
uint8_t rgb_to_gray(uint8_t r, uint8_t g, uint8_t b) {
    return (uint8_t)(0.299 * r + 0.587 * g + 0.114 * b);
}

// Function to calculate the Local Binary Pattern (LBP) of a pixel
uint8_t calculate_lbp(uint8_t *gray_image, int width, int height, int x, int y) {
    uint8_t center = gray_image[y * width + x]; // Center pixel value
    uint8_t lbp = 0; // Initialize LBP value

    // Array of offsets to neighboring pixels
    int offsets[8][2] = {
        {-1, -1}, { 0, -1}, { 1, -1}, // Top row
        {-1,  0},          { 1,  0},  // Middle row (excluding center)
        {-1,  1}, { 0,  1}, { 1,  1}  // Bottom row
    };

    // Loop through the 8 neighboring pixels
    for (int i = 0; i < 8; i++) {
        int dx = offsets[i][0];
        int dy = offsets[i][1];
        int neighbor_x = x + dx;
        int neighbor_y = y + dy;

        // Ensure the neighbor is within image bounds
        if (neighbor_x >= 0 && neighbor_x < width && neighbor_y >= 0 && neighbor_y < height) {
            uint8_t neighbor = gray_image[neighbor_y * width + neighbor_x];
            // Update LBP value based on whether the neighbor is greater or equal to the center
            lbp |= (neighbor >= center) << i;
        }
    }

    return lbp;
}

// Function to read an image using GDAL
int read_image_with_gdal(const char *filename, uint8_t **gray_image, int *width, int *height) {
    GDALDatasetH dataset = GDALOpen(filename, GA_ReadOnly); // Open the image file with GDAL
    if (dataset == NULL) {
        fprintf(stderr, "Error opening file with GDAL: %s\n", filename);
        return 1;
    }

    *width = GDALGetRasterXSize(dataset);   // Get image width
    *height = GDALGetRasterYSize(dataset);  // Get image height
    int bands = GDALGetRasterCount(dataset); // Get the number of bands in the image

    if (bands < 1) {
        fprintf(stderr, "Error: the image does not have enough bands.\n");
        GDALClose(dataset);
        return 1;
    }

    // Allocate memory for the grayscale image
    *gray_image = (uint8_t *)malloc((*width) * (*height));
    if (*gray_image == NULL) {
        fprintf(stderr, "Error allocating memory for the grayscale image.\n");
        GDALClose(dataset);
        return 1;
    }

    // Read the first band (assuming a grayscale image)
    GDALRasterBandH band = GDALGetRasterBand(dataset, 1);
    if (GDALRasterIO(band, GF_Read, 0, 0, *width, *height, *gray_image, *width, *height, GDT_Byte, 0, 0) != CE_None) {
        fprintf(stderr, "Error reading data band with GDAL.\n");
        free(*gray_image);
        GDALClose(dataset);
        return 1;
    }

    // If the image has RGB bands, convert it to grayscale
    if (bands >= 3) {
        uint8_t *red_band = (uint8_t *)malloc((*width) * (*height));
        uint8_t *green_band = (uint8_t *)malloc((*width) * (*height));
        uint8_t *blue_band = (uint8_t *)malloc((*width) * (*height));
        if (red_band == NULL || green_band == NULL || blue_band == NULL) {
            fprintf(stderr, "Error allocating memory for RGB bands.\n");
            free(*gray_image);
            if (red_band) free(red_band);
            if (green_band) free(green_band);
            if (blue_band) free(blue_band);
            GDALClose(dataset);
            return 1;
        }

        // Read the RGB bands
        GDALRasterIO(GDALGetRasterBand(dataset, 1), GF_Read, 0, 0, *width, *height, red_band, *width, *height, GDT_Byte, 0, 0);
        GDALRasterIO(GDALGetRasterBand(dataset, 2), GF_Read, 0, 0, *width, *height, green_band, *width, *height, GDT_Byte, 0, 0);
        GDALRasterIO(GDALGetRasterBand(dataset, 3), GF_Read, 0, 0, *width, *height, blue_band, *width, *height, GDT_Byte, 0, 0);

        // Convert RGB to grayscale
        for (int i = 0; i < (*width) * (*height); i++) {
            (*gray_image)[i] = rgb_to_gray(red_band[i], green_band[i], blue_band[i]);
        }

        // Free the RGB bands memory
        free(red_band);
        free(green_band);
        free(blue_band);
    }

    GDALClose(dataset);
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    const char *filename = argv[1];
    uint8_t *gray_image = NULL;
    uint8_t *lbp_image = NULL;
    int width, height;

    // Initialize GDAL
    GDALAllRegister();

    // Read the image file using GDAL
    if (read_image_with_gdal(filename, &gray_image, &width, &height) != 0) {
        return 1;
    }

    // Allocate memory for the LBP image
    lbp_image = malloc(width * height);
    if (lbp_image == NULL) {
        fprintf(stderr, "Error allocating memory for the LBP image.\n");
        free(gray_image);
        return 1;
    }

    // Calculate the LBP for each pixel in the image
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            lbp_image[y * width + x] = calculate_lbp(gray_image, width, height, x, y);
        }
    }

    // Write the LBP image to a file in PGM format
    FILE *output_file = fopen("lbp_image.pgm", "wb");
    if (output_file == NULL) {
        fprintf(stderr, "Error creating file lbp_image.pgm\n");
        free(gray_image);
        free(lbp_image);
        return 1;
    }

    // Write PGM header
    fprintf(output_file, "P5\n%d %d\n255\n", width, height);
    // Write LBP image data
    fwrite(lbp_image, 1, width * height, output_file);
    fclose(output_file);

    // Free allocated memory
    free(gray_image);
    free(lbp_image);

    printf("LBP filter applied and image saved as lbp_image.pgm\n");
    return 0;
}
