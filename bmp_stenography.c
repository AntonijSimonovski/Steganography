#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#pragma pack(1)
typedef struct{
    short bmtype;
    int bm_size;
    short reservedOne;
    short reservedTwo;
    int offset;
} bmp_header;

typedef struct{
    int dib_size;
    int img_width;
    int img_height;
    short color_panels;
    short bits_pixel;
    int compression;
    int img_size;
    int horiz_res;
    int vert_rez;
    int colors;
    int important_colors;
} dib_header;

typedef struct{
    unsigned char b;
    unsigned char g;
    unsigned char r;
} pixel;

void print(bmp_header,  dib_header);
void swap(pixel *x);
void hide(pixel *x, pixel *y);

int main(int argc, char* argv[]){

    if(argc < 3){
        printf("Not enough arguments\n");
        return 0;
    }

    FILE *file = fopen(argv[2], "r+"); 
    if(!file){
        printf("File doesn't exist. \n");
        return 0;
    }

    bmp_header bmp;
    fread(&bmp, sizeof(bmp_header), 1, file);

    //Check if the file is .bmp
    if(bmp.bmtype!=19778){
        printf("File format not supported\n");
        return 0;
    }

    dib_header dib;
    fread(&dib, sizeof(dib_header), 1, file);

    //Check if the dib size is correct
    if(dib.dib_size != 40){
        printf("File format not supported\n");
        return 0;
    }

    //Check if bits per pixel is 24
    if(dib.bits_pixel != 24){
        printf("File format not supported\n");
        return 0;
    }

    fseek(file, bmp.offset, SEEK_SET);

    pixel *pixels = malloc(dib.img_size * sizeof(pixel));
    fread(pixels, sizeof(pixel), dib.img_size, file);

    if(strcmp(argv[1],"--info") == 0){
        print(bmp, dib);
        return 0;
    }

    else if(strcmp(argv[1],"--reveal") == 0){
        pixel *temp = pixels;
        for(int i = 0; i < dib.img_size; i++, pixels++){
            swap(pixels);
        }
        fseek(file, bmp.offset, SEEK_SET);
        fwrite(temp, sizeof(pixel), dib.img_size, file);
    }

    else if(strcmp(argv[1],"--hide") == 0){
        FILE *file2 = fopen(argv[3], "r");

        bmp_header bmp2;
        fread(&bmp2, sizeof(bmp_header), 1, file2);

        if(bmp2.bmtype!=19778){
        printf("File format not supported\n");
        return 0;
        }

        dib_header dib2;
        fread(&dib2, sizeof(dib_header), 1, file2);

        //Check if the dib size is correct
        if(dib2.dib_size != 40){
            printf("File format not supported\n");
            return 0;
        }

        //Check if bits per pixel is 24
        if(dib2.bits_pixel != 24){
            printf("File format not supported\n");
            return 0;
        }

        //check dimensions
        if(dib.img_height != dib2.img_height || dib2.img_width != dib.img_width){
            printf("File format not supported\n");
            return 0;
        }

        fseek(file2, bmp2.offset, SEEK_SET);

        pixel *pixels2 = malloc(dib2.img_size * sizeof(pixel));
        fread(pixels2, sizeof(pixel), dib2.img_size, file2);

        pixel *temp = pixels;

        for(int i = 0; i < dib.img_size; i++, pixels++, pixels2++){
            hide(pixels, pixels2);
            swap(pixels);

        }

        fseek(file, bmp.offset, SEEK_SET);
        fwrite(temp, sizeof(pixel), dib.img_size, file);
    }

    fclose(file);
} //end of main

void print(bmp_header bmp, dib_header dib){
    printf("=== BMP Header ===\n");
    printf("Type: bm");
    printf("Size: %d\n", bmp.bm_size);
    printf("Reserved 1: %d\n", bmp.reservedOne);
    printf("Reserved 2: %d\n", bmp.reservedTwo);
    printf("Image Offset: %d\n\n\n", bmp.offset);

    printf("=== DIB Header ===\n");
    printf("Size: %d\n", dib.dib_size);
    printf("Width: %d\n", dib.img_width);
    printf("Height: %d\n", dib.img_height);
    printf("#Color Planes: %d\n", dib.color_panels);
    printf("#Bits per Pixel: %d\n", dib.bits_pixel);
    printf("Compression Scheme: %d\n", dib.compression);
    printf("Image Size: %d\n", dib.img_size);
    printf("Horizontal Resolution: %d\n", dib.horiz_res);
    printf("Vertical Resolution: %d\n", dib.vert_rez);
    printf("#Colors in Palette: %d\n", dib.colors);
    printf("#Important Colors: %d\n", dib.important_colors);
}

void swap(pixel *x){

    unsigned char MSB = x->b & 0xF0;
    unsigned char LSB = x->b & 0x0f;
    x->b = (LSB << 4) | (MSB >> 4);

    MSB = x->g & 0xF0;
    LSB = x->g & 0x0f;
    x->g = (LSB << 4) | (MSB >> 4);

    MSB = x->r & 0xF0;
    LSB = x->r & 0x0f;
    x->r = (LSB << 4) | (MSB >> 4);

}

void hide(pixel *x, pixel *y){ 
    x -> b = ((x->b & 0xf0)>>4) | (y->b&0xf0);
    x -> g = ((x->g & 0xf0)>>4) | (y->g&0xf0);
    x -> r = ((x->r & 0xf0)>>4) | (y->r&0xf0);
}
