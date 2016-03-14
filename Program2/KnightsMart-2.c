/**
 * Clayton Walker
 * COP 3502C-0001
 * Assignment #2 - KnightsMart (Linked Lists)
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct KnightsMartProduct {
	int itemNum;
	char itemName[21];
	double unitPrice;
	int stockQty;
	int restockQty;
	struct KnightsMartProduct *next;
} KMProduct;

typedef struct KnightsMartRestockProduct {
	int itemNum;
	struct KnightsMartRestockProduct *next;
} KMRestockProduct;

typedef struct KnightsMartSale {
	char firstName[21];
	char lastName[21];
	int numItemsOnList;

	/* format: {item_number0, quantity0, item_number1, ...} */
	int *itemsPurchased;

	struct KnightsMartSale *next;
} KMSale;


/* Adds a new item to the store inventory list given by KMProducts */
void cmdAddItem(KMProduct **KMProducts, FILE *filIn, FILE *filOut);

/*
 * Reads from the restock list and restocks items into the product list.
 * prints messages as it goes along. restock list is empty at end of function
 */
void cmdRestock(KMRestockProduct **KMRestockList, KMProduct **KMProducts,
		FILE *filOut);

/*
 * Reads from filIn the info of a customer who comes into the store.
 * Tries to sell as many items to the customer as possible; if at least one
 * sale was made, the customer gets added to the customer list. A message is
 * printed to filOut regardless of whether the node was added or not
 */
void cmdCustomer(KMSale **KMSales, KMProduct **KMProducts,
		KMRestockProduct **KMRestockList, FILE *filIn, FILE *filOut);

/* Prints an inventory of the current items in the product list to filOut */
void cmdInventory(KMProduct **KMProducts, FILE *filOut);

/* Prints a summary of the the day's sales and empties the sales list */
void cmdPrintDaySummary(KMSale **KMSales, KMProduct **KMProducts,
		FILE *filOut);

/* Reads a command from filIn and calls the appropriate function */
void readInputCmd(KMSale **KMSales, KMRestockProduct **KMRestockList,
		KMProduct **KMProducts, FILE *filIn, FILE *filOut);

/*
 * Trys to sell itemQty of the item itemNum, deducting stockQty from the
 * KMProducts list and adding to the KMRestockList as necissary.
 * Returns the amount of item actually sold.
 */
int sellItem(KMRestockProduct **KMRestockList, KMProduct **KMProducts,
		int itemNum, int itemQty);

/* adds a new item to the restock list */
void addRestock(int itemNum, KMRestockProduct **KMRestockList);

/* Prints the header for a given day */
void msgDayHeader(int day, FILE *filOut);

/* Prints the message for the ADDITEM command */
void msgAddItem(int itemNum, char itemName[21], double unitPrice,
		int stockQty, FILE *filOut);

/* prints of the RESTOCK command for each item it restocks */
void msgRestock(int itemNum, char itemName[21], int stockQty, FILE *filOut);

/*
 * Prints the message for the CUSTOMER command the flag determines whether
 * the word "some" or "no" gets printed
 */
void msgCustomer(char firstName[21], char lastName[21], int flagPurchased,
		FILE *filOut);




int main()
{
	FILE *filIn = NULL;
	FILE *filOut = NULL;

	KMProduct *KMProducts = NULL;
	KMRestockProduct *KMRestockList = NULL;
	KMSale *KMSales = NULL;

	int numDays, numCmds;
	int cntDay, cntCmd;

	filIn = fopen("KnightsMart.in", "r");
	filOut = fopen("KnightsMart.out", "w");

	/* for every day */
	fscanf(filIn, "%d", &numDays);
	for (cntDay=1; cntDay<=numDays; cntDay++)
	{
		/* print the header for the current day */
		msgDayHeader(cntDay, filOut);

		/* for every command */
		fscanf(filIn, "%d", &numCmds);
		for (cntCmd=0; cntCmd<numCmds; cntCmd++)
		{
			/* read a command and execute its associated function */
			readInputCmd(&KMSales, &KMRestockList, &KMProducts, filIn, filOut);
		}

		/* end the day with a new line seperating it from the next day */
		fputs("\n", filOut);

	}/**END for (cntDay=1; cntDay<=numDays; cntDay++)**/

	/* clean up/close file before exiting (unnecissary execpt for fclose) */
	fclose(filIn);
	fclose(filOut);
	free(KMProducts);
	free(KMRestockList);
	free(KMSales);

	exit(EXIT_SUCCESS);
}/**END int main()**/




/*===========================================================================*/
/* Adds a new item to the store inventory list given by KMProducts */
void cmdAddItem(KMProduct **KMProducts, FILE *filIn, FILE *filOut)
{
	/* Allocate a node, and read in its data */
	KMProduct *nodeProduct = (KMProduct *) malloc(sizeof(KMProduct));
	fscanf(filIn, "%d %20s %lf %d %d", &(nodeProduct->itemNum),
			nodeProduct->itemName, &(nodeProduct->unitPrice),
			&(nodeProduct->stockQty), &(nodeProduct->restockQty));

	/* if list is empty, return a list with the one new node */
	if (*KMProducts == NULL)
	{
		nodeProduct->next = NULL;
		*KMProducts = nodeProduct;
	}

	/* if new node comes before first item put it at the front */
	else if (nodeProduct->itemNum < (*KMProducts)->itemNum)
	{
		nodeProduct->next = *KMProducts;
		*KMProducts =  nodeProduct;
	}

	else
	{
		/* find the node which the new node should come after */
		KMProduct *travProduct = *KMProducts;
		while (travProduct->next != NULL && nodeProduct->itemNum > travProduct->next->itemNum)
			travProduct = travProduct->next;

		/* insert the new node */
		nodeProduct->next = travProduct->next;
		travProduct->next = nodeProduct;
	}

	/* print the ADDITEM message */
	msgAddItem(nodeProduct->itemNum, nodeProduct->itemName,
			nodeProduct->unitPrice, nodeProduct->stockQty, filOut);
}/**END void cmdAddItem(..**/




/*===========================================================================*/
/*
 * Reads from the restock list and restocks items into the product list.
 * prints messages as it goes along. restock list is empty at end of function
 */
void cmdRestock(KMRestockProduct **KMRestockList, KMProduct **KMProducts,
		FILE *filOut)
{
	fputs("RESTOCK:  ", filOut);
	if ((*KMRestockList) == NULL)
		fputs("the Restock List contains no items.\n", filOut);
	else
	{
		fputs("the following items have been reordered and restocked:\n",
				filOut);
		KMRestockProduct *travRestock = *KMRestockList;
		while (travRestock != NULL)
		{
			KMProduct *travProduct = *KMProducts;
			while (travProduct != NULL
					&& travProduct->itemNum != travRestock->itemNum)
				travProduct = travProduct->next;

			if (travProduct->stockQty != travProduct->restockQty)
			{
				travProduct->stockQty = travProduct->restockQty;
				msgRestock(travProduct->itemNum, travProduct->itemName,
						travProduct->stockQty, filOut);
			}

			KMRestockProduct *delRestock = travRestock;
			travRestock = travRestock->next;

			free(delRestock);
		}/**END while**/
		*KMRestockList = NULL;
	}/**END else**/
}/**END void cmdRestock(..**/




/*===========================================================================*/
/*
 * Reads from filIn the info of a customer who comes into the store.
 * Tries to sell as many items to the customer as possible; if at least one
 * sale was made, the customer gets added to the customer list. A message is
 * printed to filOut regardless of whether the node was added or not
 */
void cmdCustomer(KMSale **KMSales, KMProduct **KMProducts,
		KMRestockProduct **KMRestockList, FILE *filIn, FILE *filOut)
{
	int cntItems;
	int flagPurchased = 0;

	/* allocate a new node and read in most of its data */
	KMSale *nodeSale = (KMSale *) malloc(sizeof(KMSale));
	fscanf(filIn, "%20s %20s %d", nodeSale->firstName, nodeSale->lastName,
			&(nodeSale->numItemsOnList));
	nodeSale->next = NULL;

	/* allocate space for the itemsPurchased array in the new node */
	nodeSale->itemsPurchased = (int *) malloc(
			nodeSale->numItemsOnList*sizeof(int));

	/* read the customer's desired purchases into the array */
	for (cntItems=0; cntItems < nodeSale->numItemsOnList; cntItems+=2)
	{
		fscanf(filIn, "%d %d", (nodeSale->itemsPurchased)+cntItems,
				(nodeSale->itemsPurchased)+cntItems+1);

		/* attempt to sell item and set amount sold to max customer can buy */
		nodeSale->itemsPurchased[cntItems+1] =
			sellItem(KMRestockList, KMProducts,
				nodeSale->itemsPurchased[cntItems],
				nodeSale->itemsPurchased[cntItems+1]);

		/* at end of loop, flag will be 1 if anything was sold, 0 otherwise */
		if (nodeSale->itemsPurchased[cntItems+1] > 0 )
			flagPurchased = 1;
	}

	msgCustomer(nodeSale->firstName, nodeSale->lastName, flagPurchased, filOut);

	/* if customer bought something, add them to the list */
	if (flagPurchased == 1)
	{
		/* if sales list is empty, create a new sales list with one node */
		if (*KMSales == NULL)
			*KMSales = nodeSale;

		/* else, traverse to end of sales list and append newly created node */
		else
		{
			KMSale *travSales = *KMSales;
			while (travSales->next != NULL)
				travSales = travSales->next;
			travSales->next = nodeSale;
		}
	}

	/* otherwise, delete the newly allocated node */
	else
	{
		free(nodeSale->itemsPurchased);
		free(nodeSale);
	}

}/**END void cmdCustomer(..**/




/*===========================================================================*/
/* Prints an inventory of the current items in the product list to filOut */
void cmdInventory(KMProduct **KMProducts, FILE *filOut)
{
	fputs("INVENTORY:  ", filOut);
	if ((*KMProducts) == NULL)
		fputs("contains no items.\n", filOut);
	else
	{
		fputs("contains the following items:\n", filOut);

		/* traverse the product list and print its contents */
		KMProduct *travProduct = *KMProducts;
		while(travProduct != NULL)
		{
			fprintf(filOut, "\t| Item %6d | %-20s | $%7.2lf | %4d unit(s) |\n",
					travProduct->itemNum,
					travProduct->itemName,
					travProduct->unitPrice,
					travProduct->stockQty);
			travProduct = travProduct->next;
		}
	}
}/**END void cmdInventory(..**/




/*===========================================================================*/
/* Prints a summary of the the day's sales and empties the sales list */
void cmdPrintDaySummary(KMSale **KMSales, KMProduct **KMProducts,
		FILE *filOut)
{

	fputs("KnightsMart Daily Sales report:\n", filOut);
	if (*KMSales == NULL)
		fputs("\tThere were no sales today.\n", filOut);
	else
	{
		int saleNum = 1;
		int totalItemsPurchased = 0;
		int i;
		double totalValue = 0.0;
		double customerValue;
		KMProduct *travProduct;
		KMSale *delSale;

		KMSale *travSales = *KMSales;
		while (travSales != NULL)
		{
			totalItemsPurchased = 0;
			customerValue = 0.0;

			/* count the total number of items this customer purchased */
			for(i=1; i<travSales->numItemsOnList; i+=2)
				totalItemsPurchased += travSales->itemsPurchased[i];

			/* print header about customer before detailed sale information */
			fprintf(filOut, "\tSale #%d, %s %s purchased %d item(s):\n",
					saleNum, travSales->firstName, travSales->lastName,
					totalItemsPurchased);

			/* For every item purchased */
			for(i=0; i<travSales->numItemsOnList; i+=2)
			{
				/* if the customer bought at least one of this item */
				if (travSales->itemsPurchased[i+1] > 0) {

					/* search the products list for that item */
					travProduct = *KMProducts;
					while(travProduct->itemNum != travSales->itemsPurchased[i])
						travProduct = travProduct->next;

					/* print out information associated with it. */
					fprintf(filOut, "\t\t| Item %6d | %-20s | $%7.2lf (x%4d) |\n",
							travProduct->itemNum, travProduct->itemName,
							travProduct->unitPrice, travSales->itemsPurchased[i+1]);

					/* Add the item(s) cost into a rolling sum */
					customerValue += travSales->itemsPurchased[i+1]*travProduct->unitPrice;

				}/**END if (travSales->itemsPurchased[i+1] > 0)**/

			}/**END for(i=0; i<travSales->numItemsOnList; i+=2)**/

			/* print rolling sum and add it into a grand total rolling sum */
			fprintf(filOut, "\t\tTotal: $%.2lf\n", customerValue);
			totalValue += customerValue;

			/* delete current node and continue traversing sales list */
			delSale = travSales;
			travSales = travSales->next;
			free(delSale->itemsPurchased);
			free(delSale);

			saleNum++;

		}/**END while (travSales != NULL)**/

		/* All customers have been checked, print grand total */
		fprintf(filOut, "\tGrand total: $%.2lf\n", totalValue);

		/* sales list should now be empty */
		*KMSales = NULL;
	}/**END else**/

}/**END void *cmdPrintDaySummary(..**/




/*===========================================================================*/
/* Reads a command from filIn and calls the appropriate function */
void readInputCmd(KMSale **KMSales, KMRestockProduct **KMRestockList,
		KMProduct **KMProducts, FILE *filIn, FILE *filOut)
{
	/* read buffer for user's command; longest command is 15 chars */
	char inCmd[20];

	/* Select and execute a function given input from file */
	fscanf(filIn,"%19s", inCmd);
	if (strcmp(inCmd, "INVENTORY") == 0)
		cmdInventory(KMProducts, filOut);

	else if (strcmp(inCmd, "RESTOCK") == 0)
		cmdRestock(KMRestockList, KMProducts, filOut);

	else if (strcmp(inCmd, "ADDITEM") == 0)
		cmdAddItem(KMProducts, filIn, filOut);

	else if (strcmp(inCmd, "CUSTOMER") == 0)
		cmdCustomer(KMSales, KMProducts, KMRestockList, filIn, filOut);

	else if (strcmp(inCmd, "PRINTDAYSUMMARY") == 0)
		cmdPrintDaySummary(KMSales, KMProducts, filOut);
}/**END void readInputCmd(..**/




/*===========================================================================*/
/*
 * Trys to sell itemQty of the item itemNum, deducting stockQty from the
 * KMProducts list and adding to the KMRestockList as necissary.
 * Returns the amount of item actually sold.
 */
int sellItem(KMRestockProduct **KMRestockList, KMProduct **KMProducts,
		int itemNum, int itemQty)
{
	/* traverse product list until itemNum if found, or end is reached */
	KMProduct *travProduct = *KMProducts;
	while (travProduct != NULL && travProduct->itemNum != itemNum)
		travProduct = travProduct->next;

	/* if itemNum not found return that nothing was sold */
	if (travProduct == NULL)
	{
		return 0;
	}

	/* subtract wanted quantity from stock */
	travProduct->stockQty -= itemQty;

	/*
	 * If more was sold than the store has, reset the itemQty to how much
	 * was sold and add the item to the restock list
	 */
	if (travProduct->stockQty <= 0)
	{
		itemQty += travProduct->stockQty;
		travProduct->stockQty = 0;
		addRestock(travProduct->itemNum, KMRestockList);
	}
	return itemQty;
}/**END int sellItem(..**/




/*===========================================================================*/
/* adds a new item to the restock list */
void addRestock(int itemNum, KMRestockProduct **KMRestockList)
{
	/* allocate a new restock node and give it the itemNum */
	KMRestockProduct *nodeRestock =
			(KMRestockProduct *) malloc(sizeof (KMRestockProduct));
	nodeRestock->itemNum = itemNum;
	nodeRestock->next = NULL;

	/* if list is empty, return list with one node, the newly allocated one */
	if (*KMRestockList == NULL)
		*KMRestockList = nodeRestock;

	/* else traverse until end of list, and append new node to end of list */
	else
	{
		KMRestockProduct *travRestock = *KMRestockList;
		while (travRestock->next != NULL)
			travRestock = travRestock->next;

		travRestock->next = nodeRestock;
	}
}/**END void addRestock(..**/




/*===========================================================================*/
/* Prints the header for a given day */
void msgDayHeader(int day, FILE *filOut)
{
	fputs("*************************\n", filOut);
	fprintf(filOut, "UCF KnightsMart Day %d\n", day);
	fputs("*************************\n", filOut);
}/**END void msgDayHeader(..**/




/*===========================================================================*/
/* Prints the message for the ADDITEM command */
void msgAddItem(int itemNum, char itemName[21], double unitPrice,
		int stockQty, FILE *filOut)
{
	fprintf(filOut, "Item %d, %s, with a cost of $%.2lf and initial stock of %d, has been added to the product database.\n",
						itemNum, itemName,
						unitPrice, stockQty);
}/**END void msgAddItem(..**/




/*===========================================================================*/
/* prints of the RESTOCK command for each item it restocks */
void msgRestock(int itemNum, char itemName[21], int stockQty, FILE *filOut)
{
	fprintf(filOut, "\tItem %d, %s, restocked to a quantity of %d.\n",
			itemNum, itemName, stockQty);
}/**END void msgRestock(..**/




/*===========================================================================*/
/*
 * Prints the message for the CUSTOMER command the flag determines whether
 * the word "some" or "no" gets printed
 */
void msgCustomer(char firstName[21], char lastName[21], int flagPurchased,
		FILE *filOut)
{
	fprintf(filOut, "Customer %s %s came and made %s purchases.\n",
			firstName, lastName,
			((flagPurchased==1) ? "some" : "no"));
}/**END void msgCustomer(**/
