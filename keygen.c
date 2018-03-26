/*********************************************************************
Creates a key file of specified length. The ASCII charaters generated 
will be any of the 26 capital letters plus the space character

USAGE: ./keygen <length of key>
*********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>	
#include <sys/stat.h>

/******************************************************************
** char* generateKey(int)
**
** Generates random string composed of upper case alphaber letters
** plus space character of specified length 
** Args: (int) length of string
** Return: Randomly created string  
*******************************************************************/
char* generateKey(int keyLength) {
	// Add 2 to account for newline and null-terminator
	char* key = malloc((keyLength + 2) * sizeof(char));

	int randomChar; // Holds the ASCII decimal value 
	int i;
	for (i = 0; i < keyLength; i++) {
		// Random number from 65-91
		randomChar = rand() % 27 + 65; 
		// Change '[' to space character
		if (randomChar == 91) randomChar = 32;	
		key[i] = randomChar;
	}
	// End key with a new line
	key[i] = '\n';
	
	return key;
}

int main(int argc, char* argv[]) {
	// Ensure user enters correct number of arguments 	
	if (argc < 2) {
		fprintf(stderr, "Too few arguments");
		return 1;
	}
	
	// Start random time seed
	srand(time(NULL));
	// Convert string input to an integer value
	int keyLength = atoi(argv[1]);
	char* key = generateKey(keyLength);
	printf("%s", key);	
	
	// Deallocate memory 	
	free(key);

	return 0;
}
