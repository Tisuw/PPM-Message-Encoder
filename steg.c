#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* The RGB values of a pixel. */
struct Pixel {
	int red;
	int green;
	int blue;
};

/* An image	loaded from	a PPM file.	*/
struct PPM {
	/*	TODO: Question 1 */
	char *fileType;
	int height;
	int width;
	int max;
	char *comment;
	int numPix;

	struct Pixel *data;
};


/* Reads an	image from an open PPM file.
 * Returns a new struct	PPM, or	NULL if	the	image cannot be	read. */
struct PPM *getPPM(FILE	* f)
{
	/*	TODO: Question 2a */
	
	struct PPM *ppm1 = malloc(sizeof(*ppm1));
	ppm1->fileType = malloc(4*sizeof(char));
	ppm1->comment = calloc(100,sizeof(char));
	char *buff = malloc(100*sizeof(char));
	
	int buff1;
	
	fgets(ppm1->fileType, 4*sizeof(char), f);
	
	

	while(1){
		buff1 = fgetc(f);
		if(buff1 == 35 || buff1 == 10){		// Check if the charater is a # or \n
			ungetc(buff1, f);							// if it is, then add that character back to the file, and then add the whole line to ppm1->comment
			fgets(buff, 100*sizeof(char), f);
			strncat(ppm1->comment, buff, strlen(buff));
		}
		else{
			ungetc(buff1, f);			// else there are no comments, add back the number and find the width/height
			break;
		}
		}
	free(buff);								// free the buff, now useless
	
	fscanf(f, "%i", &ppm1->width);
	fscanf(f, "%i", &ppm1->height);
	fscanf(f, "%i", &(ppm1->max));				// find max and numPix
	ppm1->numPix = (ppm1->width)*(ppm1->height);
	
	ppm1->data = calloc(ppm1->numPix, sizeof(struct Pixel));		// Create then cycle through data array, assign values to each pixel.
	
	for (int i = 0; i < (ppm1->numPix); i++){
		fscanf(f, "%i %i %i", &((ppm1->data)[i].red), &((ppm1->data)[i].green), &((ppm1->data)[i].blue));	
	}
	
	return ppm1;
}

/* Write img to	stdout in PPM format. */
void showPPM(const struct PPM *img)
{
	/*	TODO: Question 2b */
	printf("%s\n%s\n%i %i\n%i", img->fileType, img->comment, img->width, img->height, img->max); // Print values in img
	
	for(int i = 0; i<(img->numPix); i++){		// print the data array, adding \n at the end of each row
		if(i%img->width == 0){
			printf("\n");
		}
		printf("%i %i %i	", img->data[i].red, img->data[i].green, img->data[i].blue);
	}
	printf("\n");
}

/* Opens and reads a PPM file, returning a pointer to a	new	struct PPM.
 * On error, prints	an error message and returns NULL. */
struct PPM *readPPM(const char *filename)
{
	/*	Open the file for reading */
	FILE *f = fopen(filename, "r");
	if	(f == NULL)	{
		fprintf(stderr, "File	%s could not be	opened.\n",	filename);
		return NULL;
	}

	/*	Load the image using getPPM	*/
	struct	PPM	*img = getPPM(f);

	/*	Close the file */
	fclose(f);

	if	(img ==	NULL) {
		fprintf(stderr, "File	%s could not be	read.\n", filename);
		return NULL;
	}

	return	img;
}

// Create a deep copy of img to edit into encoded img
struct PPM *dcopy(const struct PPM *img){
	
	struct PPM *newimg = malloc(sizeof(*newimg));
	
	newimg->fileType = malloc(4*sizeof(char));
	strcpy(newimg->fileType, img->fileType);
	
	newimg->comment = malloc(100*sizeof(char));
	strcpy(newimg->comment, img->comment);
	
	newimg->height = img->height;
	newimg->width = img->width;
	newimg->max = img->max;
	newimg->numPix = img->height*newimg->width;
	newimg->data = calloc((newimg->numPix), sizeof(struct Pixel));
	for (int i = 0; i < (newimg->numPix); i++){
		newimg->data[i].red = img->data[i].red;
		newimg->data[i].green = img->data[i].green;
		newimg->data[i].blue = img->data[i].blue;
	}
	return newimg;
}

/* Encode the string text into the red channel of image	img.
 * Returns a new struct	PPM, or	NULL on	error. */
struct PPM *encode(const char *text, const struct PPM *img)
{
	/*	TODO: Question 2c */

	//create a new PPM to return
	struct PPM *newimg = malloc(sizeof(*newimg));
	newimg = dcopy(img);
	
	int len = strlen(text); // readability
	
	//check the image is big enough for the message
	if ((5*len) > (newimg->numPix)){
		printf("Error: message too big for file");
		exit(1);
	}
	int start;
	do {
		//Pick a start point for the message
		start = (rand() % ((newimg->numPix) - (5*len)));

		//Check the value of the starting red pixel is not equal to the first character of the message, else find a new start red pixel
	}while (newimg->data[start].red == text[0]);
	
	//Assign the first char to the start location
	newimg->data[start].red = text[0];
	
	//cycle through string, changing value of (randomly selected) red pixels in newimg->data array, finishing with /0 character.
	int randomiser;
	int count;
	for (int i = 1; i<len; i++){
		count = 0;
		do{
			//select a value between 1 and 5, and check that the red value in the data array is not the same as the character, else randomise again
			randomiser = ((rand() % 4)+1);
			if (count>20) {
				fprintf(stderr, "The encoder is having trouble finding a pixel to hide your message, please try again/n");
				exit(1);
			}
			count++;
		}while(newimg->data[start+randomiser].red == text[i]);
		
		//change the value of the red pixel at the randomly selected location
		newimg->data[start+randomiser].red = text[i];
		//Make sure the next char is placed after the last
		start += randomiser;
	}
	
	return	newimg;
}

/* Extract the string encoded in the red channel of	newimg,	by comparing it
 * with	oldimg.	The	two	images must	have the same size.
 * Returns a new C string, or NULL on error. */
char *decode(const struct PPM *oldimg, const struct	PPM	*newimg)
{
	/*	TODO: Question 2d */
	//Create a string for the message, the size cannot be bigger than the image
	char *message = calloc((newimg->numPix), sizeof(char));
	int j = 0;
	//Iterate through the arrays until you find a difference
	for (int i = 0; i < (newimg->numPix); i++){
		if (oldimg->data[i].red != newimg->data[i].red){
			//Write the hidden string into message
			message[j] = newimg->data[i].red;
			j++;
		}
	}
	message[j] = 0;
	
	if (message[0] != 0)
		return message;
	else {
		printf("Error: No difference in files found\n");
		exit(1);
	}
}


// Free all the elements of img
void freePPM(struct PPM *img){
	free(img->data);
	free(img->comment);
	free(img->fileType);
	free(img);
}


/* TODO: Question 3	*/
int	main(int argc, char	*argv[])
{
	/*	Initialise the random number generator,	using the time as the seed */
	srand(time(NULL));

	/*	Parse command-line arguments */
	if	(argc == 3 && strcmp(argv[1], "t") == 0) {
		/* Mode "t" -	test PPM reading and writing */
		struct PPM *img =	readPPM(argv[2]);
		showPPM(img);
		struct PPM *newimg = encode("ah", img);
		showPPM(newimg);
		
		char *message = malloc(img->numPix);
		message = decode(img, newimg);
		
		printf("\n%s\n", message);
	
		freePPM(img);
		freePPM(newimg);
		free(message);
	
	} else	if (argc ==	3 && strcmp(argv[1], "e") == 0)	{
		/* Mode "e" -	encode PPM */

		struct PPM *oldimg = readPPM(argv[2]);

		/* prompt for a	message	from the user, and read	it into	a string */
		char *message = malloc(oldimg->numPix);
		fprintf(stderr, "Please enter your message:\n");
		scanf("%[^\n]", message);
		struct PPM *newimg;
		/* encode the text into	the	image with encode, and assign to newimg	*/
		newimg = encode(message, oldimg);

		/* write the image to stdout with showPPM */
		
		showPPM(newimg);
		freePPM(oldimg);
		freePPM(newimg);
		free(message);

	} else	if (argc ==	4 && strcmp(argv[1], "d") == 0)	{
		/* Mode "d" -	decode PPM */

		struct PPM *oldimg;
		/* get original	file filename from argv, load it with
			 readPPM, then assign to	oldimg	*/
		oldimg = readPPM(argv[2]);
		
		
		struct PPM *newimg;
		/*  get encoded file	filename from argv,	load it	with
			 readPPM, then assign to	newimg	*/
		newimg = readPPM(argv[3]);
		
		char *message;
		/* decode the encodedPPM with the comparisonPPM	and	assign to message */
		message = decode(oldimg, newimg);

		/* print the decoded message to	stdout */
		printf("%s\n", message);
		
		freePPM(oldimg);
		freePPM(newimg);
		free(message);

	} else	{
		fprintf(stderr, "Unrecognised	or incomplete command line.\n");
		return 1;
	}

	return	0;
}
