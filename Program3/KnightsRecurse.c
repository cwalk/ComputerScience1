/**
 * Clayton Walker
 * COP 3502C-0001
 * Assignment #3 - KnightsRecurse (Recursion)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define nullCheck( ptr ); if( ptr == NULL ) { printf("\n\nFailure allocating memory! Check line %d! Terminating Program!\n", __LINE__); exit(EXIT_FAILURE); }
#define fileCheck( ptr ); if( ptr == NULL ) { printf("\n\nFile not found! Check line %d! Terminating Program!\n", __LINE__); exit(EXIT_FAILURE); }

//#define DEBUG

// KnightsMultiply
void KnightsMultiply(FILE *fin, FILE *fout);
double KnightsMultiplyRecurse(int k, int n);

// KnightsFlip
void KnightsFlip(FILE *fin, FILE *fout);
void KnightsFlipRecurse(char *string, int pos, int num, FILE *fout);

// KnightsShape
void KnightsShape(FILE *fin, FILE *fout);
void KnightsShapeRecurse(int num, int max, FILE *fout);
void KnightsShapePrint(int num, int max, FILE *fout);

// KnightsScotch
void KnightsScotch(FILE *fin, FILE *fout);
int KnightsScotchRecurse(int *line, int size, int start);

// KnightsDepot
void KnightsDepot(FILE *fin, FILE *fout);
void KnightsDepotRecurse(int *planks, int num2x4s, int maxLength, int numPlaced, int spaceLeftOn2x4, int num2x4sUsed);
void exchange2x4s(int *planks, int i, int j);

// GLOBAL # of 2x4s Needed
int numUsed;
int recursiveCall;

int main() {
	int ii = 1;
	FILE *fin = fopen("KnightsRecurse.in", "r");
	fileCheck(fin);
	FILE *fout = fopen("KnightsRecurse.out", "w");
	fileCheck(fout);

	while(!feof(fin)) {
		char token[50];
		fscanf(fin, "%s", token);

		if(!strcmp(token, "KnightsMultiply")) {
			KnightsMultiply(fin, fout);
		}

		else if(!strcmp(token, "KnightsScotch")) {
			//printf("KnightsScotch %d\n", ii);
			KnightsScotch(fin, fout);
		}

		else if(!strcmp(token, "KnightsDepot")) {
			//printf("Enter KnightsDepot\n");
			KnightsDepot(fin, fout);
		}

		else if(!strcmp(token, "KnightsShape")) {
			KnightsShape(fin, fout);
		}

		else if(!strcmp(token, "KnightsFlip")) {
			KnightsFlip(fin, fout);
		}
		ii++;
	}
	fclose(fin);
	fclose(fout);
	//system("PAUSE");
	exit(EXIT_SUCCESS);
}

// KnightsMultiply WRAPPER Funcion
void KnightsMultiply(FILE *fin, FILE *fout) {
	int k, n;
	double answer;
	fscanf(fin, "%d %d", &k, &n);
	
	answer = KnightsMultiplyRecurse(k, n);
	fprintf(fout, "KnightsMultiply:  %.0f\n\n", answer); 
}

// KnightsMultiply RECURSIVE Funcion
// This was meant to be a "gimmee".  Why?
// Because it is basically the SAME as the sumNumbers function shown on
// Day 2 of the Recursion slides (page 24).
// Instead of summing numbers, we're multiplying them.
double KnightsMultiplyRecurse(int k, int n) {
	if ( n == k ) 
		return k;
	else
		return (n * KnightsMultiplyRecurse(k, n-1));
}

// KnightsFlip WRAPPER Funcion
void KnightsFlip(FILE *fin, FILE *fout) {
	// Initialize the state.
	int num;
	fscanf(fin, "%d", &num);
	fprintf(fout, "KnightsFlip:\n");
	// malloc string, of size "num+1", for the string of F's and K's.
	// the "+ 1" in "num + 1" is for the terminating character (\0) of a string...gotta add that in
	char *string = malloc((num + 1) * sizeof(char));
	nullCheck(string);
	// adding the terminating character...just sytax stuff
	string[num] = '\0';
	// As of now, our string is empty.
	// We call our RECURSIVE Flip function for the first time:
	KnightsFlipRecurse(string, 0, num, fout);
	fprintf(fout, "\n");
	free(string);
}

// KnightsFlip RECURSIVE Funcion  (minus comments...only 8 lines of code)
void KnightsFlipRecurse(char *string, int pos, int num, FILE *fout) {
	// Permutation through possible 'F' and 'K' combinations (binary counting).
	// Once the string is COMPLETELY FILLED, we've reached the "deepest" level
	// of recursion for this particular case.  Simply PRINT the string and return.
	if (pos == num) {
		fprintf(fout, "%s\n", string);
		return;
	}

	// We need to place BOTH the K and the F at index "pos" ... we need to try BOTH posibilities
	// For EACH, place the letter at index "pos" and THEN recursively call the
	// KnightsFlipRecurse function with this NEWLY UPDATED string

	// So place 'K' and then recurse
	string[pos] = 'K';
	KnightsFlipRecurse(string, pos + 1, num, fout);
	// Now place 'F' and then recurse
	string[pos] = 'F';
	KnightsFlipRecurse(string, pos + 1, num, fout);
}

// KnightsShape WRAPPER Funcion
void KnightsShape(FILE *fin, FILE *fout) {
	int num;
	fscanf(fin, "%d", &num);
	fprintf(fout, "KnightsShape:\n");
	KnightsShapeRecurse(num, num, fout);
	fprintf(fout, "\n");
}

// KnightsShape RECURSIVE Funcion
void KnightsShapeRecurse(int num, int max, FILE *fout) {
	// If you reach halfway, print the middle line and return.
	if(num <= (max / 2 + 1)) {
		KnightsShapePrint(num, max, fout);
		return;
	}
	// Print, recurse, print to make an X. Essentially this is two hollow mirrored triangles.
	KnightsShapePrint(num, max, fout);
	KnightsShapeRecurse(num - 1, max, fout);
	KnightsShapePrint(num, max, fout);
}

// KnightsShape Auxilary Function to Print shape
void KnightsShapePrint(int num, int max, FILE *fout) {
	// Auxiliary printing function for KnightsShapeRecurse().
	int i;
	for(i = 1; i <= max; i++) {
		if(i == num || i == (max - num + 1)) {
			fprintf(fout, "X");
		}
		else {
			fprintf(fout, " ");
		}
	}
	fprintf(fout, "\n");
}

// KnightsScotch WRAPPER Funcion
void KnightsScotch(FILE *fin, FILE *fout) {
	int start, size, *line, i;
	fscanf(fin, "%d %d", &start, &size);
	line = malloc(size * sizeof(int));
	nullCheck(line);
	for(i = 0; i < size; i++) {
		fscanf(fin, "%d", &line[i]);
	}
	int retval = KnightsScotchRecurse(line, size, start);
	free(line);
	if(retval) {
		fprintf(fout, "KnightsScotch:  Solvable\n\n");
	}
	else {
		fprintf(fout, "KnightsScotch:  Not Solvable\n\n");
	}
}

// KnightsScotch RECURSIVE Funcion
int KnightsScotchRecurse(int *line, int size, int start) {
	// If you landed on a winning spot, you can return that you won.
	if(line[start] == 0) {
		return 1;
	}
	// State holders. "line" not only keeps track of the puzzle, but also the current state
	// and ends up getting destroyed as you mark off positions you have visited.
	// The original "line" can be preserved with a wrapper function or the puzzle can be
	// solved without states (a bit more tricky).
	int left = 0;
	int right = 0;
	int store = line[start];
	// Mark in the array that this current position has already been visited.
	line[start] = -1;
	// Check the left, then jump left if possible.
	if(((start - store) >= 0) && (line[start - store] != -1)) {
		left = KnightsScotchRecurse(line, size, start - store);
	}
	// Check the right, then jump right if possible.
	if(((start + store) < size) && (line[start + store] != -1)) {
		right = KnightsScotchRecurse(line, size, start + store);
	}
	// Determine results.
	// Return left or right, if one of them is true, then that path must have won,
	// otherwise they are both 0 and, both paths were a loss.
	return left || right;
}

// KnightsDepot WRAPPER Funcion
void KnightsDepot(FILE *fin, FILE *fout) {
	int *planks, size, maxlen, i;
	fscanf(fin, "%d %d", &maxlen, &size);  // read in data
	planks = malloc(size * sizeof(int));  // allocate space for array of 2x4s that client wants
	nullCheck(planks);
	for(i = 0; i < size; i++) {           // read in 2x4s
		fscanf(fin, "%d", &planks[i]);
	}

	// Initialize variables used in the recursive calls
	int numPlaced = 0;  // This is the # of 2x4s that have been "placed" (cut) from the larger 2x4s
	int spaceLeftOn2x4 = maxlen;  // space remaining on the 2x4 from which we are cutting from
	                              // we initialize to maxlen (the max size given of the store bought 2x4s)
	int num2x4sUsed = 0;  // total # of 2x4s required, which could ultimately be equal to # of pieces
	                      // Example:  if input was 4 7 6 5 7 5
	                      // We would need 4 2x4s to complete the job

	// The following variable is a GLOBAL variable to save the final state of the recursive calls
	// It represents the smallest # of 2x4s required.
	// We initialize it to size, thereby assuming we will need "size" units of 2x4s
	// But in the base case of the recursion, we update this GLOBAL numUsed if indeed
	// the current num2x4sUsed is less than the current value of numUsed
	numUsed = size;

	// Make initial Recursive Call
	KnightsDepotRecurse(planks, size, maxlen, numPlaced, spaceLeftOn2x4, num2x4sUsed);

	// free the array of 2x4s
	free(planks);

	// Print out Header and the global numUsed that we updated during recursive calls
	fprintf(fout, "KnightsDepot:  %d\n\n", numUsed);
}

// KnightsDepot RECURSIVE Funcion (very similar to Recursive Permutation Concept shown in class)
// NOTICE the return type is void
// For the base case, we simply update the global variable, numUsed, as needed.
void KnightsDepotRecurse(int *planks, int num2x4s, int maxLength, int numPlaced, int spaceLeftOn2x4, int num2x4sUsed) {
	int i, j, k, flag = 1;
	
	//
	// BASE CASE, we've cut all 2x4s
	// so numPlaced (number of 2x4s cut) equals the number of 2x4s requested
	// ...again, therefore, they've all be "placed" (all been cut)
	//
	if (numPlaced == num2x4s) {
		if ((1+num2x4sUsed) < numUsed)
			// the 1 + num2x4sUsed will make sense once you see the algorithm
			// the 1 represents the last 2x4 that wasn't "complete"...so I'm adding it on here
			numUsed = 1 + num2x4sUsed;
	}

	///
	// ELSE, we have more 2x4s to cut
	//    Try ALL Permutations
	///
	else {
		for (j=numPlaced; j<num2x4s; j++){

			// ************** STEP 1 **************  //
			// Try j different planks at position (index) represented by numPlaced
			// Just like the exchangeCharacters on the Permutation function
			exchange2x4s(planks, numPlaced, j);

			// ************** STEP 2 **************  //
			// PREPARE data for recursive call, and then RECURSE!

			// IF plank "j" can be cut from the current 2x4, DO SO!
			if (planks[numPlaced] <= spaceLeftOn2x4) {
				// Subtract current 2x4 cut length from space left on large 2x4
				spaceLeftOn2x4 = spaceLeftOn2x4 - planks[numPlaced];
				// Increase the number of "Placed" 2x4s (the # of 2x4s that have been successfully cut)
				++numPlaced;
			}

			// ELSE, plank "j" is too big to be cut from the space left on the current 2x4
			else {
				// j'th piece can't be cut from spaceLeftOnPlank
				// So we've now "used" a full 2x4 (well, maybe it wasn't full...you get the idea though)
				++num2x4sUsed;
				// Reset space left on plank, cuz now we're cutting from a NEW, full-sized 2x4
				spaceLeftOn2x4 = maxLength;
				// Subtract current 2x4 cut length from space left on the NEW 2x4
				spaceLeftOn2x4 = spaceLeftOn2x4 - planks[numPlaced];
				// Increase the number of "Placed" 2x4s (the # of 2x4s that have been successfully cut)
				++numPlaced;
			}

			// ******************************
			// RECURSIVELY CALL OUR FUNCTION!
			KnightsDepotRecurse(planks, num2x4s, maxLength, numPlaced, spaceLeftOn2x4, num2x4sUsed);


			// ************** STEP 3 **************  //
			// SWAP 2x4s BACK
			--numPlaced;
			spaceLeftOn2x4 = planks[numPlaced] + spaceLeftOn2x4;

			if ((spaceLeftOn2x4 == maxLength) && (num2x4sUsed > 0)) {
				// Reduce num2x4s that have been used
				--num2x4sUsed;
				// Fix the spaceLeftOn2x4 length to previous space left on last 2x4
				// Note easy at first thought, because what was that previous space left?
				// It's not just a full 2x4 minus the last plank, because possibly
				// multiple cuts were made from that last 2x4.
				// The following FOR loop basically finds the correct space left
				// on the previous 2x4 by checking how many previous planks "fit"
				// and then subracting their lengths to come up with a final
				// space left on the previous 2x4.
				for (flag = 1, k = 1; flag; k++) {
					if ((numPlaced-k) >= 0) {
						if (!((spaceLeftOn2x4-planks[numPlaced-k]) < 0)) {
							spaceLeftOn2x4 = spaceLeftOn2x4 - planks[numPlaced-k];
						}
						else
							flag = 0;
					}
					else
						flag = 0;
				}
			}
			exchange2x4s(planks, j, numPlaced);
		}
	}

}

// EXCHANGE Function:  auxilary function to exchange 2x4s (just lap swap funciton in recursive permutation example)
// Pre-condition: str is a valid C String and i and j are valid indexes
//                to that string.
// Post-condition: The characters at index i and j will be swapped in
//                 str.
void exchange2x4s(int *planks, int i, int j) {
     
    int temp = planks[i];
    planks[i] = planks[j];
    planks[j] = temp;
}
