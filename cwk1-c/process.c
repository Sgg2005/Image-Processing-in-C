/* This coursework specification, and the example code provided during the
 * course, is Copyright 2025 Heriot-Watt University.
 * Distributing this coursework specification or your solution to it outside
 * the university is academic misconduct and a violation of copyright law. */

 #include <stdbool.h>
 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <time.h>
 #include <stdint.h>
 
 /* The RGB values of a pixel. */
 struct Pixel 
 {
     uint8_t red;
     uint8_t green;
     uint8_t blue;  
 };
 
 /* An image loaded from a file. */
 struct Image
 {
     int width;            
     int height;         
     struct Pixel *pixels; 
 };
 
//frees the memory allocated for the image
 void free_image(struct Image *img)
 {
    //checks if the image is NULL
     if (img == NULL){
         return; 
        }
    //the code will free the memory allocated for the pixels array and the image
     free(img->pixels);
     free(img);
 }
 
 /* Opens and reads an image file, returning a pointer to a new struct Image.
  * On error, prints an error message and returns NULL. */
 struct Image *load_image(const char *filename)
 {
     /* Open the file for reading */
     FILE *f = fopen(filename, "rb");
     //checks if the file could not be opened
     if (f == NULL)
     {
        //print an error message
         fprintf(stderr, "File %s could not be opened.\n", filename);
         //returns NULL
         return NULL;
     }

     struct Image *img = malloc(sizeof(struct Image));
     //checks if the memory allocation is succesful
     if (img == NULL)
     {
         fprintf(stderr, "Memory allocation failed.\n");
         fclose(f);
         return NULL;
     }
 
     char format[4]; //to store the image format
     //reads the image format and the image dimensions from the file 
     if (fscanf(f, "%3s %d %d\n", format, &img->height, &img->width) != 3 || strcmp(format, "HP8") != 0)
     {
         fprintf(stderr, "Invalid or unsupported image format.\n");
         free(img); 
         fclose(f); 
         return NULL;                                             
     }
 
     img->pixels = malloc(img->height * img->width * sizeof(struct Pixel)); 
     //checks if the memory allocation is succesful
     if (img->pixels == NULL)
     {   
         fprintf(stderr, "Memory allocation failed for pixel data.\n");
         free(img); 
         fclose(f);                                               
         return NULL; //it returns null
     }

     //reads the pixel data from the file
     if (fread(img->pixels, sizeof(struct Pixel), img->height * img->width, f) != img->height * img->width)
     {
        fprintf(stderr, "Pixel data could not be read.\n");
        free(img->pixels);                        
        free(img);
        fclose(f);
        return NULL;
     }
     fclose(f);
     return img;
 }
 
 /* Write img to file filename. Return true on success, false on error. */
 bool save_image(const struct Image *img, const char *filename)
 {
     FILE *f = fopen(filename, "wb");
     //checks if the file could not be opened
     if (f == NULL)
     {   
        fprintf(stderr, "File %s could not be opened.\n", filename); 
        return false;
     }
     
     fprintf(f, "HP8 %d %d\n", img->height, img->width);

    /* Writes pixel data to file. Returns false if write fails, returns true if successful. */
    size_t num_pixels = img->height * img->width;
    //checks if the pixel data could not be written
    if (fwrite(img->pixels, sizeof(struct Pixel), num_pixels, f) != num_pixels){                                                          
        fprintf(stderr, "Pixel data could not be written.\n");
        fclose(f);
        return false;
    }
    fclose(f);
    return true;
 }
 
 /* Allocate a new struct Image and copy an existing struct Image's contents
  * into it. On error, returns NULL. */
struct Image *copy_image(const struct Image *source)
{
    struct Image *img_copy = malloc(sizeof(struct Image)); 
    //checks if the memory allocation is succesful if it is not, it prints an error message and returns NULL
    if (img_copy == NULL)
    {                                                   
        fprintf(stderr, "Memory allocation failed.\n"); 
        return NULL;
    }

    //it copies the image dimensions from the source image to the new image
    img_copy->height = source->height; 
    img_copy->width = source->width;  
 
    size_t num_pixels = source->height * source->width;//calculate the number of pixels in the image
    img_copy->pixels = malloc(num_pixels * sizeof(struct Pixel));

    //checks if the memory allocation is succesful
    if (img_copy->pixels == NULL)
    {                                                                 
        fprintf(stderr, "Memory allocation failed for pixel data.\n");
        free(img_copy);
        return NULL; 
    }
 
    //copies the pixel data from the source image to the new image
    memcpy(img_copy->pixels, source->pixels, num_pixels * sizeof(struct Pixel));
 
    return img_copy; //return the new image
 }

//linked list to store images and their filenames
struct ImageNode {
    struct Image *img;
    char *output_filename;
    struct ImageNode *next;
};

//creates a new node for the linked list
struct ImageNode *createlist(struct Image *img, char *output_filename) {
    struct ImageNode *node = malloc(sizeof(struct ImageNode));
    //checks if the memory allocation is succesful
    if (node == NULL) {
        return NULL;
    }
    //assigns the image and the output filename to the node
    node->img = img;
    node->output_filename = output_filename;
    node->next = NULL;
    return node;
}   

//frees all the images in the linked list
void free_image_list(struct ImageNode *head) {
    struct ImageNode *current = head;
    //loops through the linked list and frees the memory
    while (current != NULL) {
        struct ImageNode *next = current->next;
        free_image(current->img);
        free(current);
        current = next;
    }
}

//helper function for the median function
static int median(int values[], int size);

//helper function for the compare function
static int compare(const void *a, const void *b)
{
    return (*(int *)a - *(int *)b); //subtracts 2 integer values and helps out the qsort function
}

//helper function for the median filter
static int median(int values[], int size)
{
    qsort(values, size, sizeof(int), compare); //sorts the values array
    return values[size / 2];//returns the median value
}

//applies the median filter to the image and returns the result 
struct Image *apply_MEDIAN(const struct Image *source)
{
    //checks if the image is valid
    if (!source || !source->pixels)
    {
        fprintf(stderr, "Invalid source image\n");
        return NULL;
    }

    printf("Processing image: %dx%d pixels\n", source->width, source->height);//prints the image dimensions

    struct Image *result = malloc(sizeof(struct Image));
    //checks if the memory allocation is succesful
    if (!result)
    {
        fprintf(stderr, "Failed to allocate result image\n");
        return NULL;
    }

    //using the malloc function it allocates memory for the pixels array
    result->width = source->width;
    result->height = source->height;
    result->pixels = malloc(source->width * source->height * sizeof(struct Pixel));
    //checks if the memory allocation is succesful
    if (!result->pixels)
    {
        free(result);
        fprintf(stderr, "Failed to allocate pixel buffer\n");
        return NULL;
    }

    //define directions of the cross pattern (up, right, down, left) using the dx and dy arrays
    const int dx[] = {0, 0, 1, 0, -1};
    const int dy[] = {0, -1, 0, 1, 0};
    int values[5]; // Fixed-size array for 5 elements

    //using nested loops it goes through the image pixels to apply the median filter
    for (int y = 0; y < source->height; y++)
    {
        for (int x = 0; x < source->width; x++)
        {
            int count = 0;
            
            //collects values for each color channel from cross pattern
            for (int i = 0; i < 5; i++)
            {
                int nx = x + dx[i];
                int ny = y + dy[i];
                //checks if the pixel is within the image boundaries for the red channel
                if (nx >= 0 && nx < source->width && ny >= 0 && ny < source->height)
                {
                    int idx = ny * source->width + nx;
                    values[count++] = source->pixels[idx].red;
                }
            }
            result->pixels[y * source->width + x].red = median(values, count);//

            count = 0;
            //checks if the pixel is within the image boundaries for the green channel
            for (int i = 0; i < 5; i++)
            {
                int nx = x + dx[i];
                int ny = y + dy[i];
                if (nx >= 0 && nx < source->width && ny >= 0 && ny < source->height)
                {
                    int idx = ny * source->width + nx;
                    values[count++] = source->pixels[idx].green;
                }
            }
            result->pixels[y * source->width + x].green = median(values, count);
            count = 0;

            //checks if the pixel is within the image boundaries
            for (int i = 0; i < 5; i++)
            {
                int nx = x + dx[i];
                int ny = y + dy[i];
                //checks if the pixel is within the image boundaries for the blue channel
                if (nx >= 0 && nx < source->width && ny >= 0 && ny < source->height)
                {
                    int idx = ny * source->width + nx;
                    values[count++] = source->pixels[idx].blue;
                }
            }
            result->pixels[y * source->width + x].blue = median(values, count);
        }
    }

    printf("Median filter processing complete\n");
    return result;
}

/*compute and display histogram of RGB values in the image */
bool apply_HIST(const struct Image *source) {
    //checks if the image is valid
    if (!source || !source->pixels) {
        fprintf(stderr, "Invalid source image\n");
        return false;
    }

    //goes through the histogram array from 0-255 
    unsigned long histogram[256] = {0};
    
    //calculate the histogram values for each color channel
    for (int i = 0; i < source->height * source->width; i++) {
        histogram[source->pixels[i].red]++; 
        histogram[source->pixels[i].green]++; 
        histogram[source->pixels[i].blue]++;
    }
    
    //display histogram values in a loop
    for (int i = 0; i < 256; i++) {
        //prints the histogram values
        printf("Value %d: %lu pixels\n", i, histogram[i]);
    }
    return true;
}

int main(int argc, char *argv[]) {
    //checks if the number of arguments is correct
    if (argc < 3 || argc % 2 != 1) {
        fprintf(stderr, "Usage: process INPUT1 OUTPUT1 [INPUT2 OUTPUT2 ...]\n");
        return 1;
    }

    struct ImageNode *head = NULL;
    struct ImageNode *current = NULL;

    /* Load all input images into linked list */
    for (int i = 1; i < argc; i += 2) {
        struct Image *img = load_image(argv[i]);
        if (img == NULL) {
            fprintf(stderr, "Failed to load image: %s\n", argv[i]);
            free_image_list(head);
            return 1;
        }

        //creates a new node for the image and its output filename
        struct ImageNode *node = createlist(img, argv[i + 1]);
        if (node == NULL) {
            fprintf(stderr, "Failed to create node for image: %s\n", argv[i]);
            free_image(img);
            free_image_list(head);
            return 1;
        }

        //adds the node to the linked list
        if (head == NULL) {
            head = node;
            current = node;
        } else {
            current->next = node;
            current = node;
        }
    }

    /* Process all images */
    current = head;
    while (current != NULL) {
        /* Apply median filter */
        struct Image *processed = apply_MEDIAN(current->img);
        if (processed == NULL) {
            fprintf(stderr, "Median filter failed for an image\n");
            free_image_list(head);
            return 1;
        }

        /* Apply histogram */
        if (!apply_HIST(processed)) {
            fprintf(stderr, "Histogram process failed for an image\n");
            free_image(processed);
            free_image_list(head);
            return 1;
        }

        /* Save the processed image */
        if (!save_image(processed, current->output_filename)) {
            fprintf(stderr, "Failed to save image to %s\n", current->output_filename);
            free_image(processed);
            free_image_list(head);
            return 1;
        }

        free_image(processed);
        current = current->next;
    }
    free_image_list(head);
    return 0;
}
