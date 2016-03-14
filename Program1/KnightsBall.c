/**
 * Clayton Walker
 * COP 3502C-0001
 * Assignment #1 - KnightsBall (Memory Allocation)
 */

#include <stdio.h>
#include <stdlib.h>

#define nullCheck( ptr ); if( ptr == NULL ) { printf("\n\nFailure allocating memory! Check line %d! Terminating Program!\n", __LINE__); exit(EXIT_FAILURE); }
#define fileCheck( ptr ); if( ptr == NULL ) { printf("\n\nFile not found! Check line %d! Terminating Program!\n", __LINE__); exit(EXIT_FAILURE); }

typedef struct KnightsBallLottoPlayer {
	char firstName[20];
	char lastName[20];
	int numbers[6];
} KBLottoPlayer;

enum match_ {
	FOURTH = 3,
	THIRD,
	SECOND,
	FIRST
};

int main() {
	FILE *fin = fopen("KnightsBall.in", "r");
	fileCheck(fin);
	FILE *fout = fopen("KnightsBall.out", "w");
	fileCheck(fout);

	int numPlayers;
	int i, j, k;
	KBLottoPlayer *players;
	int winningNumbers[6];
	int playerPtr, winningPtr;
	int match;

	// Scan in the number of lottery players to be read.
	fscanf(fin, "%d", &numPlayers);

	// Allocate space for the lottery player structures.
	players = malloc(numPlayers * sizeof(KBLottoPlayer));
	nullCheck(players);

	// Scan in the data to the lottery player structs.
	for(i = 0; i < numPlayers; i++) {
		fscanf(fin, "%s%s%", players[i].lastName, players[i].firstName);
		for(j = 0; j < 6; j++) {
			fscanf(fin, "%d", &players[i].numbers[j]);
		}
	}

	// Prompt the user for the winning numbers.
	printf("Enter the winning Lottery numbers:\n");
	for(i = 0; i < 6; i++) {
		scanf("%d", &winningNumbers[i]);
	}

	// For each player, check to see how many winning numbers they have.
	for(i = 0; i < numPlayers; i++) {
		//--- Extra credit sorted list match searching solution, O(n):
		playerPtr = 0;
		winningPtr = 0;
		match = 0;
		while(playerPtr < 6 && winningPtr < 6) {
			if(players[i].numbers[playerPtr] < winningNumbers[winningPtr]) {
				playerPtr++;
			}
			else if(players[i].numbers[playerPtr] > winningNumbers[winningPtr]) {
				winningPtr++;
			}
			else {
				playerPtr++;
				winningPtr++;
				match++;
			}
		}

		//--- No extra credit match searching solution, O(n^2):
		/*
		match = 0;
		for(j = 0; j < 6; j++) {
			for(k = 0; k < 6; k++) {
				if(players[i].numbers[j] == winningNumbers[k]) {
					match++;
				}
			}
		}
		//*/

		// If the player has enough winning numbers, print out his/her winnings.
		if(match >= FOURTH) {
			fprintf(fout, "%s %s matched %d numbers and won $", players[i].firstName, players[i].lastName, match);
			switch (match) {
				case FOURTH:
				fprintf(fout, "10.\n");
				break;
				case THIRD:
				fprintf(fout, "100.\n");
				break;
				case SECOND:
				fprintf(fout, "10000.\n");
				break;
				case FIRST:
				fprintf(fout, "1000000.\n");
				break;
			}
		}
	}

	// Free allocated memory, then exit.
	free(players);
	exit(EXIT_SUCCESS);
}
