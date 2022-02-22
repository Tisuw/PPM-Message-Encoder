# F28HS Coursework 1 gwc2000  

## Program Design  

### Methods  

##### getPPM(FILE * f)  
This method allocates memory for the PPM struct it will return (ppm1) and all the structs inside of PPM. It then uses variables named buff and buff1  
to determine if there are comments in the file and, if so, read them into ppm1->comment. The buff variable is freeed in this method as it is not needed  
elsewhere in the program. The pixel data is read in by iterating over each set of pixels, using a for loop.  

##### void freePPM(struct PPM * img)  
This method frees all the structs in the PPM and then the PPM itself. This increases the modularity and readability of the code.  

##### struct PPM * dcopy(const struct PPM * img)  
This method deep copies a PPM image to allow for encoding a message. This method is here to increase modularity and readability.  

##### showPPM(const struct PPM * img)  
This method writes the contents of a PPM image to the stdout. It is assumed valid PPM image is inputed, else it will display unformatted data.  

##### main
The main method has 3 different "modes" which are specified in the command line. If not specified, it outputs an error.  
The test mode (t) reads a sample file from the command line, shows it, encodes a message into it, shows the encoded file, then uses decode() to output  
the encoded message. It then frees all structs used.   
The encode mode (e) takes a PPM image calls the encode method on it, then outputs the encoded image. It then frees all the structs used.  
The decode mode (d) takes 2 PPM images, the original and the encoded, it then calls the decode function on these files and outputs the encoded message.  


## Data Structures and Algorithms  

### Structures  
My struct PPM is made of 4 ints, 2 strings, and 1 pixel array. I chose to use a string for fileType and comments as they would be able to store   
any type of character and make it easier to print back out to stdout. The array holds pixels to make the organisation of each rgb value easier, and  
allows for easy traversal.  
The Pixel structure allows for easy storage and indexing of individual pixel's rgb values.  

### Algorithms  

##### Randomising pixel placement/Encoding  
The pixel placement is randomised by the rand() function in the encode() function. rand() is used to pick a number from 1 to 5, and then the method   
uses that number to select a pixel's red value to replace. If the red pixel's value is the same as the value of the character being placed there,   
then a new random number is selected. The method will try this 20 times, after which it will output an error, as it is most likely that there are   
no pixels in the 1 to 5 range that can store the character.  

##### Decoding  
To decode an image, the program iterates through all of the pixels in the original and encoded image, comparing the red values of each. If a value   
is different, this value is translated into a character and added to a string (message). If no difference can be found, then an error is outputed.   
