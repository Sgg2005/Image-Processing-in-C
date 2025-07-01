HP8 MEDIAN HIST - H00456217

## Overview
This application is designed to process images in the custom HP8 format.  
It applies a median filter to an image and generates a histogram report based on the RGB values present in the image.

## Features
- Load and save images in HP8 format.
- Apply a median filter to smooth out pixel noise.
- Generate a histogram of all pixel values from the red, green, and blue channels.
- Efficient memory management to support large image processing.

## Requirements
- C Compiler (e.g., GCC)
- Standard C Libraries: `stdio.h`, `stdlib.h`, `string.h`, `stdbool.h`
- At least one pair of filenames should be given
- Input file must exist
- Output file will be created or overwritten
- Only HP8 file format is processed

## Compilation
To compile the program, use:

```bash
gcc -o process process.c
```

Or using Makefile:

```bash
make
```

## Usage
The application takes input files in pairs with an operation type (MEDIAN or HIST):

```bash
./process INPUTFILE OUTPUTFILE OPERATION
```

### Example – Apply Median Filter

```bash
./process flower.hp8 filtered.hp8 MEDIAN
```

### Example – Generate Histogram

```bash
./process flower.hp8 dummy.hp8 HIST
```

> Note: In HIST mode, the output file is still required by the argument list but not used.

## HP8 Image Format

**Header:**
```
HP8
height width
```

**Pixel Data:**
- Each pixel contains 3 unsigned 8-bit RGB values (red, green, blue)
- Pixels are stored row by row, from top-left to bottom-right
- All data after the header is in binary

## Functionality

### Loading an Image
- Parses the HP8 file header for dimensions
- Allocates memory dynamically based on height and width
- Reads binary RGB pixel data

### Saving an Image
- Writes a header and the pixel data into HP8 format
- Overwrites or creates a new file

### Applying a Median Filter
- Replaces each pixel with the median of itself and its four immediate neighbors (up, down, left, right)
- Each RGB channel is processed independently
- Uses `qsort` for calculating the median values
- Outputs the result into a new image file

### Generating a Histogram
- Counts frequency of every RGB value from 0 to 255 across the entire image
- Combines red, green, and blue channels
- Displays the results via `stdout`

## Sample Output

### Histogram Example

```bash
./process flower.hp8 unused.hp8 HIST
```

```
Value 0: 12 pixels
Value 1: 43 pixels
Value 2: 89 pixels
...
Value 255: 5 pixels
```

### Median Example

```bash
./process flower.hp8 output.hp8 MEDIAN
```

```
Median filter applied successfully.
Processed image saved as: output.hp8
```

## Error Handling
- Detects invalid or corrupted file headers
- Verifies format and file structure
- Handles memory allocation and file I/O errors gracefully

## Memory Management
- Uses `malloc` for pixel buffer allocation
- `free_image()` handles proper deallocation to avoid memory leaks
