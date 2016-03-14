/**
 * Clayton Walker
 * COP 3502C-0001
 * Assignment #2 - KnightsMart (Linked Lists)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define nullCheck( ptr ); if( ptr == NULL ) { printf("\n\nFailure allocating memory! Check line %d! Terminating Program!\n", __LINE__); exit(EXIT_FAILURE); }
#define fileCheck( ptr ); if( ptr == NULL ) { printf("\n\nFile not found! Check line %d! Terminating Program!\n", __LINE__); exit(EXIT_FAILURE); }

typedef struct KnightsMartProduct {
	int			itemNum;
	char		itemName[21];
	double		unitPrice;
	int			stockQty;
	int			restockQty;
	struct KnightsMartProduct *next;
} KMProduct;

typedef struct KnightsMartRestockProduct {
	int			itemNum;
	struct KnightsMartRestockProduct *next;
} KMRestockProduct;

typedef struct KnightsMartSale {
	char		firstName[21];
	char		lastName[21];
	int			*itemsPurchased;		// array of item numbers
	int			numItems;				// number of items in the itemsPurchased array
	struct KnightsMartSale *next;
} KMSale;

// addItem - Takes a pointer to the head of "productList" and adds a new item to the list based on
//		the arguement data.
// Returns a pointer to the updated head of "productList".
// Prints information to "fout".
KMProduct *addItem(
	KMProduct			*productList,
	int					itemNum,
	char				*itemName,
	double				unitPrice,
	int					stockQty,
	int					restockQty,
	FILE				*fout);

// restock - Updates "productList" using data from "restockList" while sequentially removing
//		nodes from "restockList".
// Returns a pointer to the updated head of "restockList".
// Prints information to "fout".
KMRestockProduct *restock(
	KMProduct			*productList,
	KMRestockProduct	*restockList,
	FILE				*fout);

// customer - Takes pointers to the heads of "restockList", "productList", and "saleList"
//		while parsing information about a customer and modifying the lists.
// Prints information to "fout" (maybe).
void customer(
	KMProduct			*productList,
	KMRestockProduct	**restockList,
	KMSale				**saleList,
	char				*firstName,
	char				*lastName,
	int					numItems,
	int					*items,
	FILE				*fout);

// inventory - Takes a pointer to the head of "productList", and prints out various
//		information to "fout".
void inventory(
	KMProduct			*productList,
	FILE				*fout);

// printDaySummary - Takes a pointer to the head of "saleList", and prints out various
//		information to "fout" while destroying "saleList".
KMSale *printDaySummary(
	KMProduct			*productList,
	KMSale				*saleList,
	FILE				*fout);

// Misc list destroyers.
KMProduct *destroyKMP(KMProduct *productList);
KMRestockProduct *destroyKMRP(KMRestockProduct *restockList);
KMSale *destroyKMS(KMSale *saleList);

int main() {
	// Open input and output files.
	FILE *fin = fopen("sampleinput/KnightsMart.in", "r");
	fileCheck(fin);
	FILE *fout = fopen("sampleinput/KnightsMart.out", "w");
	fileCheck(fout);

	int i, j, k;

	// Linked list headers.
	KMProduct			*productList = NULL;
	KMRestockProduct	*restockList = NULL;
	KMSale				*saleList = NULL;

	unsigned long numDays;	// Total number of days in file.
	unsigned long numCommands;	// Total number of commands in each day.
	char token[80];

	fscanf(fin, "%lu", &numDays);
	// For each day...
	for(i = 0; i < numDays; i++) {
		fprintf(fout, "*************************\nUCF KnightsMart Day %d\n*************************\n", i + 1);
		fscanf(fin, "%lu", &numCommands);
		// For each command...
		for(j = 0; j < numCommands; j++) {

			//Scan in command
			fscanf(fin, "%s", token);

            //Series of if statements using strcmp to determine which command was scanned in
			if(!strcmp(token, "ADDITEM")) {
				int itemNum;
				char itemName[21];
				double unitPrice;
				int stockQty;
				int restockQty;

				// Input item info from file.
				fscanf(fin, "%d%s%lf%d%d", &itemNum, itemName, &unitPrice, &stockQty, &restockQty);
				//Add item to the linked list and store in header
				productList = addItem(productList, itemNum, itemName, unitPrice, stockQty, restockQty, fout);
			}

			// Restock
			else if(!strcmp(token, "RESTOCK")) {
				restockList = restock(productList, restockList, fout);
			}

			else if(!strcmp(token, "CUSTOMER")) {
				char firstName[21];
				char lastName[21];
				int numItems;
				int *items;

				// Input customer info from file.
				fscanf(fin, "%s%s%d", firstName, lastName, &numItems);
				// Allocate memory for array to store item numbers and quantity the customer wishes to purchase for each item.
				items = malloc(numItems * sizeof(int));
				// Input item numbers and quantities for each item.
				for(k = 0; k < (numItems); k++) {
					fscanf(fin, "%d", &items[k]);
				}
				customer(productList, &restockList, &saleList, firstName, lastName, numItems, items, fout);
			}

			// Print out all items in product linked list.
			else if(!strcmp(token, "INVENTORY")) {
				inventory(productList, fout);
			}

			// Print all sales for the day
			else if(!strcmp(token, "PRINTDAYSUMMARY")) {
				saleList = printDaySummary(productList, saleList, fout);
			}
		}
		fprintf(fout, "\n");
	}

	// Call functions to free memory for all linked lists.
	productList = destroyKMP(productList);
	restockList = destroyKMRP(restockList);
	saleList = destroyKMS(saleList);

	exit(EXIT_SUCCESS);
}

// Adds product to linked list. Returns the front of the list.
KMProduct *addItem(
	KMProduct			*productList, // Parameters
	int					itemNum,
	char				*itemName,
	double				unitPrice,
	int					stockQty,
	int					restockQty,
	FILE				*fout) {

	// Helper pointer to traverse through list.
	KMProduct *ptr = productList;

	// Allocate memory for new node.
	KMProduct *add = malloc(sizeof(KMProduct));
	nullCheck(add); // Double check.

	// Fill in node with information passed in by parameters.
	add->itemNum = itemNum;
	strcpy(add->itemName, itemName); // Strcpy to copy over a string.
	add->unitPrice = unitPrice;
	add->stockQty = stockQty;
	add->restockQty = restockQty;

	// Print data to fout.
	fprintf(fout, "Item %d, %s, with a cost of $%3.2lf and initial stock of %d, has been added to the product database.\n",
			itemNum,
			itemName,
			unitPrice,
			stockQty);

	// Empty list insertion.
	if(ptr == NULL) {
		add->next = NULL;
		return add; // Add is the new front of the list.
	}

	// Front list insertion. Insert add to the front of the list when the item number is less than the current
    // front's item number.
	if(itemNum < ptr->itemNum) {
		add->next = ptr;
		return add; // Add is the new front of the list.
	}

	KMProduct *ptr2 = ptr->next;

	// Mid list insertion, based on itemNum order.
	// Ptr2 is the current node we are at. Ptr is the previous node we were at.
	while(ptr2 != NULL) {
		// If the item number that we wish to insert is less than the current node we are at
		if(itemNum < ptr2->itemNum) {
			add->next = ptr2; // Set next node for our new node to be the current node we are at in traversal.
			ptr->next = add; // Set the new node to be next after the previous node we were at in traversal.
			break;
		}
		ptr = ptr->next; // Set ptr to be the current node.
		ptr2 = ptr2->next; // Set ptr2 to be the next node for traversal.
	}

	// End list insertion.
	if(ptr2 == NULL) {
		add->next = NULL; // Set the next node for our new node to be NULL since it is at the end of the list.
		ptr->next = add; // Set the new node to be next after the previous node in traversal.
	}

	return productList;
}

// Takes all nodes in the restock linked list and replinshes quantities for those items in
// The product linked list. Deletes everything in the restock list afterwards.
KMRestockProduct *restock(
	KMProduct			*productList,
	KMRestockProduct	*restockList,
	FILE				*fout) {

    // Helper pointer to traverse through restock linked list.
	KMRestockProduct *restockPtr = restockList;

	// Print that no restock can occur since the list is empty.
	if(restockPtr == NULL) {
		fprintf(fout, "RESTOCK:  the Restock List contains no items.\n");
		return restockList;
	}

	// Print that a restock has occurred.
	fprintf(fout, "RESTOCK:  the following items have been reordered and restocked:\n");

	KMProduct *productPtr; // Used to traverse through the product linked list.
	KMRestockProduct *tempKMR; // Used to delete a node from the restock linked list.

	// For each restock item...
	while(restockPtr != NULL) {
		productPtr = productList;
		// Find the item in the inventory and update the inventory values.
		while(productPtr != NULL) {
			if(productPtr->itemNum == restockPtr->itemNum) {
				// Print that the item was restocked.
				fprintf(fout, "\tItem %d, %s, restocked to a quantity of %d.\n", productPtr->itemNum, productPtr->itemName, productPtr->restockQty);
				productPtr->stockQty = productPtr->restockQty; // Set quantity for product to amount in restockQty.
				break;
			}
			productPtr = productPtr->next; // Set productPtr to the next node in the product linked list for traversal.
		}

		tempKMR = restockPtr; // Store the current node in a temporary place to prepare for deletion.
		restockPtr = restockPtr->next; // Set current node to next node for traversal through restock linked list.
		free(tempKMR); // Delete node we were just at.
	}

    // Return NULL because after restock, the restock linked list should be empty.
	return NULL;
}

// Customer function. A customer either doesn't make a purchase and leaves, or they do make a purchase and must
// be inserted into the sales linked list.
void customer(
	KMProduct			*productList,
	KMRestockProduct	**restockList, // Double pointer is used to change header for restock linked list.
	KMSale				**saleList, // Double pointer is used to change header for sales linked list.
	char				*firstName,
	char				*lastName,
	int					numItems,
	int					*items, // Item Array of ints. Uses pointer.
	FILE				*fout) {

	KMProduct *productPtr; // Helper pointer to traverse through product linked list.
	KMRestockProduct *restockPtr = (*restockList); // Helper pointer to traverse through restock linked list.
	KMSale *salePtr; // Helper pointer to traverse through sales linked list.

	char saleMade = 0; // Flag to indicate that at least one item was purchased.

	// Send the restock pointer to the end of the restock list to prepare it for possible additions
	// if the list is not already empty.
	if(restockPtr != NULL) {
		while(restockPtr->next != NULL) {
			restockPtr = restockPtr->next; // Set restockPtr to next node for traversal through restock linked list.
		}
	}

	int i;
	// For each item...
	for(i = 0; i < (numItems); i += 2) {
		productPtr = productList; // Set productPtr to head of product linked list.
		// Find that item in the product list.
		while(productPtr != NULL) {
			// If the item was found...
			if(productPtr->itemNum == items[i]) {
				// If the item has stock, attempt to decrease the stock by the amount the customer wants.
				if(productPtr->stockQty != 0) {
					// If the customer wants more items than available.
					if(productPtr->stockQty < items[i+1]) {
						items[i+1] = productPtr->stockQty; // Set number of item purchased to number of item available.
						productPtr->stockQty = 0; // Set quantity available to 0 since all items were bought.
					}
					// If there is more stock than the customer wants.
					else {
						productPtr->stockQty -= items[i+1]; // Update quantity to reflect purchase of items.
					}
					saleMade = 1; // Set flag to true since a purchase was made.
					// If the item stock just became zero, add it to the restock list.
					if(productPtr->stockQty == 0) {
						// Empty list insertion. Remember that restock list was already set the end of the restock linked list.
						if(restockPtr == NULL) {
							(*restockList) = malloc(sizeof(KMRestockProduct)); // Allocated memory for a new node.
							nullCheck(*restockList);
							(*restockList)->itemNum = items[i]; // Set the item number for the restock product to the item number in the array.
							(*restockList)->next = NULL; // Set the next node in the restock list to NULL since it the only node in the list.
							// Update the traversal pointer again.
							restockPtr = (*restockList);
						}
						// End list insertion. Remember that restock list was already set the end of the restock linked list.
						else {
							restockPtr->next = malloc(sizeof(KMRestockProduct)); // Allocate memory for new restock node.
							nullCheck(restockPtr->next);
							restockPtr->next->itemNum = items[i]; // Set the item number for the restock product to the item number in the array.
							restockPtr->next->next = NULL; // Set the next to node to be NULL since it is at the end of the list.
							restockPtr = restockPtr->next; // Update restockPtr so that is it pointing the the new end of the list.
						}
					}
				}
				// If the item was found but had no stock, set the item number in the array to 0 for indexing.
				else {
					items[i] = 0; // Set number of items to 0 since no items could be purchased.
				}
				break;  // Break from while loop for products since the product was found in the linked list.
			}
			productPtr = productPtr->next;
		}
		// If the item was not found at all, set the item number in the array to 0 for indexing.
		if(productPtr == NULL) {
			items[i] = 0;
		}
	}

	// If a sale was made, the customer is added to the sales list.
	if(saleMade == 1) {
		KMSale *saleTemp = malloc(sizeof(KMSale)); // Allocate memory for a new sales node.
		strcpy(saleTemp->firstName, firstName); // Copy string over.
		strcpy(saleTemp->lastName, lastName); // Copy string over.
		saleTemp->itemsPurchased = items; // Copy over array.
		saleTemp->numItems = numItems;
		saleTemp->next = NULL;

		// Set salePtr to the head of the sales linked list for traversal.
		salePtr = *saleList;
		// Empty list insertion.
		if(salePtr == NULL) {
			(*saleList) = saleTemp;
		}
		// End list insertion.
		else {
			while(salePtr->next != NULL) {
				salePtr = salePtr->next; // Traverse through the sales linked list until the last node was reached.
			}
			salePtr->next = saleTemp; // Set the next node for the current last node to be the new node that was created.
		}
		fprintf(fout, "Customer %s %s came and made some purchases.\n", firstName, lastName);
	}
	// If a sale was not made, destroy the items array.
	else {
		fprintf(fout, "Customer %s %s came and made no purchases.\n", firstName, lastName); // Print that no purchase were made.
		free(items); // Free the items array since it was allocated memory that is no longer needed.
	}

	return; // Return from function.
}

// Prints out everything in the product linked link.
void inventory(
	KMProduct			*productList,
	FILE				*fout) {

	// Helper pointer to traverse through product linked list.
	KMProduct *ptr = productList;

	// Empty list, print that there is no inverntory.
	if(ptr == NULL) {
		fprintf(fout, "INVENTORY:  contains no items.\n");
		return;
	}

	// Print that there are items in the inverntory.
	fprintf(fout, "INVENTORY:  contains the following items:\n");

	// Traverse through all nodes in the product linked list and print the info for each one.
	while(ptr != NULL) {
		fprintf(fout, "\t| Item %6d | %-20s | $%7.2lf | %4d unit(s) |\n", ptr->itemNum, ptr->itemName, ptr->unitPrice, ptr->stockQty);
		ptr = ptr->next;
	}

	return; // Return from function.
}

// Prints out all sales that were made and deletes all nodes from the sales linked list.
KMSale *printDaySummary(
	KMProduct			*productList,
	KMSale				*saleList,
	FILE				*fout) {

	// Helper pointer to traverse through sales linked list.
	KMSale *salePtr = saleList;

	// Sales linked list is empty, no sales to print out.
	if(salePtr == NULL) {
		fprintf(fout, "KnightsMart Daily Sales report:\n\tThere were no sales today.\n");
		return saleList;
	}

	KMProduct *productPtr; // Helper pointer to traverse through product linked list.
	unsigned long counter = 1; // Counter stores current number for the sale.
	double total = 0; // Stores the total amount of money the store made from all sales.
	KMSale *tempKMS;

	fprintf(fout, "KnightsMart Daily Sales report:\n");

	int i;
	while(salePtr != NULL) {
		double subtotal = 0; // Stores the total amount of money made for the current sale.

		int temp = 0; // Stores the total number of items the customer was able to purchase.
		// Find number of items the customer was actually able to purchase.
		for(i = 0; i < (salePtr->numItems); i += 2) {
			if(salePtr->itemsPurchased[i] != 0) {
				temp += salePtr->itemsPurchased[i+1]; // Adds the number of items purchased.
			}
		}

		// Print customer purchase info.
		fprintf(fout, "\tSale #%d, %s %s purchased %d item(s):\n", counter, salePtr->firstName, salePtr->lastName, temp);

		// Print out data for each item.
		for(i = 0; i < (salePtr->numItems); i += 2) {
			// If an item was purchased.
			if(salePtr->itemsPurchased[i]) {
				productPtr = productList; // Set the pointer to the head of the product linked list.

				// Loop through all the nodes in the linked list.
				while(productPtr != NULL) {

					// Loop through all the nodes in the linked list.
					if(productPtr->itemNum == salePtr->itemsPurchased[i]) {
						fprintf(fout, "\t\t| Item %6d | %-20s | $%7.2lf (x%4d) |\n", salePtr->itemsPurchased[i], productPtr->itemName, productPtr->unitPrice, salePtr->itemsPurchased[i+1]);
						subtotal += productPtr->unitPrice * salePtr->itemsPurchased[i+1]; // Update the subtotal for the current sale.
						break; // Break from the while loop.
					}
					productPtr = productPtr->next; // Set the current node to the next node in the product linked list for traversal.
				}
			}
		}
		// Print out the total amount for the current sale.
		fprintf(fout, "\t\tTotal: $%3.2lf\n", subtotal);
		total += subtotal; // Update the total for all the sales.
		counter++; // Increment counter for number of sales made.

		tempKMS = salePtr; // Store the current sales node in a temporary variable to prepare for deletion.
		salePtr = salePtr->next; // Set the current node to the next node in the sales linked list for traversal.
		free(tempKMS->itemsPurchased); // Free the allocated memory for the items array in the node.
		free(tempKMS); // Free the node itself.
	}
	// Print out the grand total for the day.
	fprintf(fout, "\tGrand total: $%3.2lf\n", total);

	return NULL; // Returns NULL since the sales list should be empty since all nodes were freed from memory.
}

// Frees all nodes in the product linked list from memory
KMProduct *destroyKMP(KMProduct *productList) {
	KMProduct *ptr = productList; // Helper pointer for traversal through the linked list
	KMProduct *tempKMP; // Helper pointer that points to the node that will be freed

    // Loop through all nodes in the list
	while(ptr != NULL) {
		tempKMP = ptr; // Prepare current node for deletion
		ptr = ptr->next; // Set current node to next node for traversal
		free(tempKMP); // Free node from memory
	}

	return NULL; // Return NULL since the product linked list should be empty
}

// Frees all nodes in the restock linked list from memory
KMRestockProduct *destroyKMRP(KMRestockProduct *restockList) {
	KMRestockProduct *ptr = restockList; // Helper pointer to traverse through the restock linked list
	KMRestockProduct *tempKMR; // Helper pointer that points to the node that will be freed

    // Loop through all the nodes in the list
	while(ptr != NULL) {
		tempKMR = ptr; // Prepare current node for deletion
		ptr = ptr->next; // Set current node to the next node for traversal
		free(tempKMR); // Free node from memory
	}

	return NULL; // Return NULL since the restock list should be empty
}

// Frees all nodes in the sales list from memory
KMSale *destroyKMS(KMSale *saleList) {
	KMSale *ptr = saleList; // Helper pointer to traverse through the sales linked list
	KMSale *tempKMS; // Helper pointer that points to the node that will be freed

    // Loop through all the nodes in the sales linked list
	while(ptr != NULL) {
		tempKMS = ptr; // Prepare current node for deletion
		ptr = ptr->next; // Set the current node to the next node
		free(tempKMS->itemsPurchased); // Free the allocated array in the node first
		free(tempKMS); // Free the node itself
	}

	return NULL; // Returns NULL since the sales list should be empty
}
