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
	int height;
	int width;
	int max;
	struct Pixel *data;
};

/* Reads an	image from an open PPM file.
 * Returns a new struct	PPM, or	NULL if	the	image cannot be	read. */
struct PPM *getPPM(FILE	* f)
{
	/*	TODO: Question 2a */
	
	struct PPM *ppm1 = malloc(sizeof(*ppm1));
	
	fscanf(f, "%*s %i", &(ppm1->width));
	fscanf(f, "%i", &(ppm1->height));
	fscanf(f, "%i", &(ppm1->max));
	
	ppm1->data = calloc(((ppm1->width)*(ppm1->height)), sizeof(struct Pixel));
	
	for (int i = 0; i < ((ppm1->width)*(ppm1->height)); i++){
		fscanf(f, "%i %i %i", &((ppm1->data)[i].red), &((ppm1->data)[i].green), &((ppm1->data)[i].blue));
	}
	
	//printf("This is the contents of PPM1: width = %i\n height = %i\n max = %i\n", ppm1->width, ppm1->height, ppm1->max);
		
	return ppm1;
}

/* Write img to	stdout in PPM format. */
void showPPM(const struct PPM *img)
{
	/*	TODO: Question 2b */
	for(int i = 0; i<(img->height*img->width); i++){
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

struct PPM *dcopy(const struct PPM *img){
	
	struct PPM *newimg = malloc(sizeof(*newimg));
	newimg->height = img->height;
	newimg->width = img->width;
	newimg->max = img->max;
	newimg->data = calloc(((newimg->width)*(newimg->height)), sizeof(struct Pixel));
	for (int i = 0; i < ((newimg->width)*(newimg->height)); i++){
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
	if (len > ((newimg->width)*(newimg->height))){
		printf("Error: message too big for file");
		exit(1);
	}
	int start = (rand() % len);
	
	//check theres enough space for the message from start, if not, set the start as close to the end as possible
	if (((img->width*img->height)-start)<len)
		start = (img->width*img->height) - len;
	
	//cycle through string, changing value of red values in array, starting at start and finishing with /0 character
	for (int i = 0; i<len+1; i++){
		newimg->data[start+i].red = text[i];
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
	char *message = calloc((newimg->width)*(newimg->height), sizeof(char));
	
	//Iterate through the arrays until you find a difference
	for (int i = 0; i < ((newimg->width)*(newimg->height)); i++){
		if (oldimg->data[i].red != newimg->data[i].red){
			//Write the hidden string into message, wait for "/0"
			int j = 0;
			while(newimg->data[i].red != 0){
				message[j] = newimg->data[i].red;
				j++;
				i++;
			}
			return message;
		}
	}
	printf("Error: No difference in files found");
	exit(1);
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
		
		struct PPM *newimg = encode("asdfgh", img);
		printf("%i", (int)strlen("asdfgh"));
		showPPM(newimg);
		printf("\n%s\n", decode(img, newimg));

	} else	if (argc ==	3 && strcmp(argv[1], "e") == 0)	{
		/* Mode "e" -	encode PPM */

		struct PPM *oldimg = readPPM(argv[2]);

		/* prompt for a	message	from the user, and read	it into	a string */
		char *message = malloc(oldimg->width*oldimg->height);
		printf("Please enter your message");
		scanf("%s", message);

		struct PPM *newimg;
		/* encode the text into	the	image with encode, and assign to newimg	*/
		newimg = encode(message, oldimg);

		/* write the image to stdout with showPPM */
		
		showPPM(newimg);

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
		printf("%s", message);

	} else	{
		fprintf(stderr, "Unrecognised	or incomplete command line.\n");
		return 1;
	}

	return	0;
}
