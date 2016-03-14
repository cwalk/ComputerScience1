/**
 * Clayton Walker
 * COP 3502C-0001
 * Assignment #2 - KnightsMart (Linked Lists)
 */
#undef DEBUG
#define DEBUG
#include <strings.h>

/******************************************************************************************/
/* File IO and generic boilerplate for homework assignments.
 * Skip ahead to the next star line.
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>


#ifdef DEBUG
#define INPUTFILE "intest"
#define OUTPUTFILE "outtest"
#else
#define INPUTFILE "KnightsMart.in"
#define OUTPUTFILE "KnightsMart.out"
#endif

/* Open file, with helpful error messages if it doesn't work. */
FILE *openFile(char *filename, char *mode) {
    FILE *fptr;
    if ((fptr = fopen(filename, mode)) == NULL) { // fopen() returns null pointer on failure
        fprintf(stderr, "Failed to open input file [%s] with mode [%s]\n", filename, mode);
        exit(EXIT_FAILURE);
    }
    return fptr;
}

/* A malloc which prints error if it doesn't work. */
void *smalloc(size_t size) {
    void *tmp;
    if ((tmp = malloc(size)) == NULL) { // malloc() returns NULL when allocation fails. 
        fprintf(stderr, "Memory allocation failed.\n");
        exit(EXIT_FAILURE); //Quit.
    }
    return tmp;
}
/* dbg() is a printf() like function that is easily turned off to avoid risk of messing
 * up code by deleting print statements after it is working. 
 * They could also considered to be comments.
 * Hopefully a decent compiler will remove the statements completely if DEBUG is not defined.
 */
#ifdef DEBUG

void dbg(const char *fmt, ...) {
    va_list args;
    int i;
    char buf[1024];

    va_start(args, fmt);
    vsprintf(buf, fmt, args);
    va_end(args);
    fprintf(stderr, "DEBUG: %s\n", buf);
}
#else

void dbg(const char *fmt, ...) {
}
#endif
/******************************************************************************************/

/* Structs given in assignment. */
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
    int numItemsOnList; // # of items on shopping list
    int *itemsPurchased; // array of item numbers
    struct KnightsMartSale *next;
} KMSale;

/* Start of "program" */

// This is a struct that contains all of the pointers we need to shorten argument lists. 

typedef struct p01nt3rs3t {
    KMProduct *headKMProduct;
    KMRestockProduct *headKMRestockProduct;
    KMSale *headKMSale;

    FILE *inputFile;
    FILE *outputFile;
} pSet;


//add a new item to the KMProduct linked list. 

void doAddItem(pSet *p) {
    KMProduct *n, *ntmp; // n is the new node, ntmp is the helper node pointer.
    int fail=0; // "boolean" for failed insert of duplicate item
    n = (KMProduct *) smalloc(sizeof (KMProduct)); // allocate space for our new node.

    dbg("Called doAddItem()");
    fscanf(p->inputFile, "%d%s%lf%d%d",
            &n->itemNum, n->itemName, &n->unitPrice, &n->stockQty, &n->restockQty); //read all the info from the file.
    dbg("%d %s %.2lf %d %d",
            n->itemNum, n->itemName, n->unitPrice, n->stockQty, n->restockQty);

    /* Linked List Stuff */
    ntmp = p->headKMProduct; //start at beginning of list
    if (p->headKMProduct == NULL) { // Start a new list.
        dbg("Starting a new KMProduct");
        n->next = NULL; // only node means next is null.
        p->headKMProduct = n; // only node in list, our new node is head.
    } else if (ntmp->itemNum > n->itemNum) { // insert at beginning of list
        n->next = ntmp; // ntmp is currently the head, and we're before it.
        p->headKMProduct = n; // make our node the head. 
    } else { // we need to insert in order.
        // move ahead until ntmp->next->itemNum is higher than ours
        while ((ntmp->next != NULL) && (ntmp->next->itemNum) < (n->itemNum)) // traverse list while next value is lower than our value.
            ntmp = ntmp->next; // keep moving forward
        // now ntmp->next has our successor node and ntmp is our predecessor node.
        if(ntmp->itemNum != n->itemNum){ // no duplicate item numbers
        n->next = ntmp->next; // our next node is the successor. 
        ntmp->next = n; // our predecessor's next node is us. 
        }else(fail=1);
    }
    //Item 4011, USA_FLAG, with a cost of $10.99 and initial stock of 1, has been added to the product database.
    if(!fail)
    fprintf(p->outputFile, "Item %d, %s, with a cost of $%.2lf and initial stock of %d, has been added to the product database.\n", n->itemNum, n->itemName, n->unitPrice, n->stockQty);

}

// Restock an individual item by item number. (change stockQty in KMProduct)
// called from doRestock.

void restockItem(pSet *p, int item) {
    KMProduct *n; // n is the current node as we traverse the list. 
    dbg("called restockItem()");
    n = p->headKMProduct; // start at the beginning of the list. 
    while (n != NULL) { //traverse the list searching for our item. 
        if (n->itemNum == item && n->stockQty < n->restockQty) { // we found our item. Don't restock if we already did. 
            dbg("Restocking item %d. Have %d, adding %d.", item, n->stockQty, n->restockQty);

            n->stockQty += n->restockQty; // add restock to inventory. 
            fprintf(p->outputFile, "\tItem %d, %s, restocked to a quantity of %d.\n", n->itemNum, n->itemName, n->stockQty);
        }
        n = n->next; // move forward in the list. 
    }

}

// This is the RESTOCK command. 
// for each item in this list, we need to go through the inventory, restock if we're out,
// and after we do that clear the item out of the restock list. 

void doRestock(pSet *p) {
    KMRestockProduct *n, *old; // n is a helper, old is the one we're deleting after finishing up with a node.
    dbg("Called doRestock()");
    n = p->headKMRestockProduct; // start the helper at the beginning of th list.

    if (n == NULL) { // empty list. 
        fprintf(p->outputFile, "RESTOCK:  the Restock List contains no items.\n");
        return;
    }
    fprintf(p->outputFile, "RESTOCK:  the following items have been reordered and restocked:\n");

    while (n != NULL) {
        dbg("Restocking item %d", n->itemNum);
        restockItem(p, n->itemNum); // do the actual list modification somewhere else. 
        old = n; //save the current item pointer so we can erase it
        n = n->next; // move forward in the list
        free(old); //free the item we're done with
    }
    p->headKMRestockProduct = NULL; // we've freed the whole list by now since n==NULL to exit the while
}

// We've run out of stuff, this is called from the doCustomer().
// add to the end of the restock list. 

void addRestock(pSet *p, int item) {
    KMRestockProduct *n, *tmp; // n is new restock node, tmp is helper
    int found = 0; // "boolean" if we found the item.
    dbg("Called addRestock()");
    n = (KMRestockProduct *) smalloc(sizeof (KMRestockProduct)); // allocate a new node

    n->next = NULL; // always adding to end of list. 
    n->itemNum = item; //fiil in our node's data
    dbg("Restocking item %d", n->itemNum);
    if (p->headKMRestockProduct == NULL) { // new list
        dbg("new restock list.");
        p->headKMRestockProduct = n; //beginning of list
        return;
    }
    tmp = p->headKMRestockProduct; // iterate through list with tmp
    dbg("Adding to restock list");
    while (tmp->next != NULL && found == 0) { // traverse the list to check if the item is already in the restock list.
        dbg("addRestock loop");
        if (tmp->itemNum == item) {
            found = 1; //don't add twice
            dbg("Found item in restock list already."); 
            free(n); // we don't need it anymore
            return;
        }
        tmp = tmp->next; // move to next
    }
    if(tmp->itemNum != item) // we never checked the last item's value. 
        tmp->next = n;
    else
        free(n); // we don't need it. 

    dbg("item=%d", item);

}

/* This is called from doCustomer() to remove what's possible from the inventory.
 * It returns how many items the customer got.
 * Returns 0 for either item doesn't exist or we're out.
 */
int takeAvail(pSet *p, int item, int quantity) {
    KMProduct *n;
    int qtmp;

    n = p->headKMProduct; //start iterating at the beginning. 

    while (n != NULL) { //iterate through entire list
        if (n->itemNum == item) { // we have the item, or at least it's known.
            if (n->stockQty > quantity) { // we have enough stuff.
                dbg("Checking item %d, we have %d.", item, n->stockQty);
                n->stockQty -= quantity; // remove the items from stock.
                dbg("removing %d from item %d leaving %d", quantity, item, n->stockQty);
                return quantity;
            } else { // we don't have enough (or we have exactly enough)
                qtmp = n->stockQty; //customer is going to take what's left if any
                dbg("We had %d", qtmp);
                n->stockQty = 0; //it's all gone.
                addRestock(p, item); //put it in the list to restock.
                return qtmp;
            }
        }
        n = n->next; // move ahead
    }
    return 0; //no such item.
}

//this is the CUSTOMER command.
// create a node for the sale. 

void doCustomer(pSet *p) {
    int i;
    KMSale *n, *ntmp;
    int bought; // This is how much the customer actually bought vs how much wanted to buy.
    int purchased = 0; // this will end up being a "boolean" to say whether customer actually bought something
    char *ptext[] = {"no", "some"}; // if customer bought something or not ptext[purchased] will be "no" if no and "some" if yes.

    n = (KMSale *) malloc(sizeof (KMSale)); //allocate a new node. 
    dbg("Called doCustomer()");
    fscanf(p->inputFile, "%s%s%d", n->firstName, n->lastName, &n->numItemsOnList); // read information
    n->itemsPurchased = (int *) malloc(n->numItemsOnList * sizeof (int)); // allocate our integer array.
    for (i = 0; i < n->numItemsOnList; i++) { //read in the array from the file.
        fscanf(p->inputFile, "%d", &n->itemsPurchased[i]);
        dbg("%s %s n.ItemsPurchased[%d]=%d", n->firstName, n->lastName, i, n->itemsPurchased[i]);
    }

    for (i = 0; i < n->numItemsOnList; i += 2) { // skip 2 because i is the item and i+1 is the quantity of the item.
        dbg("Wants %d of item %d", n->itemsPurchased[i + 1], n->itemsPurchased[i]);
        bought = takeAvail(p, n->itemsPurchased[i], n->itemsPurchased[i + 1]); // see how many we can get.
        dbg("doCustomer Found %d of item %d", bought, n->itemsPurchased[i]);
        n->itemsPurchased[i + 1] = bought; //change to how many customer got.
        if (bought > 0)
            purchased++; //bought something.
    }

    // add the sale to the end of the list
    n->next = NULL; //n will be at end of list so next is null
    ntmp = p->headKMSale; // helper to iterate through list
    if (ntmp == NULL) // create a new list
        p->headKMSale = n; //our node is the only node.
    else {
        while (ntmp->next != NULL) //traverse list
            ntmp = ntmp->next;
        ntmp->next = n; // change last element next from NULL to n. 
    }

    purchased = (purchased == 0) ? 0 : 1; //change purchased to 1 or 0.
    fprintf(p->outputFile, "Customer %s %s came and made %s purchases.\n", n->firstName, n->lastName, ptext[purchased]);


}

// this pretty much just dumps out the KMProduct list.

void doInventory(pSet *p) {
    KMProduct *n;
    dbg("Called doInventory()");

    n = p->headKMProduct; // start at beginning of list. 
    if (p->headKMProduct == NULL) { // list is empty. 
        fprintf(p->outputFile, "INVENTORY:  contains no items.\n");
        return;
    }
    fprintf(p->outputFile, "INVENTORY:  contains the following items:\n");
    while (n != NULL) { // traverse list and print the info
        fprintf(p->outputFile, "\t| Item %6d | %-20s | $%7.2lf | %4d unit(s) |\n",
                n->itemNum, n->itemName, n->unitPrice, n->stockQty);
        n = n->next; //move ahead. 
    }
}

// Find the pointer to the node in the list for item and return it, used by doPrintDaySummary()

KMProduct *findProduct(pSet *p, int item) {
    KMProduct *n;
    dbg("Finding product %d", item);
    n = p->headKMProduct; //start at beginning of list
    while (n != NULL) { //traverse list until we find item
        if (n->itemNum == item) // we found item, return its id
            return n;
        n = n->next; //move ahead otherwise
    }
    dbg("Returning null");
    return NULL; //no item found
}


// The PRINTDAYSUMMARY command.
// traverse the KMSale list, printing the info and deleting nodes as we go through.

void doPrintDaySummary(pSet *p) {
    KMSale *n, *old;
    KMProduct *info;
    int i;
    int scount; // sale number
    double ctot, dtot; // customer total and day total of $
    int items; // how many items a customer bought

    ctot = dtot = 0;
    scount = 1;
    n = p->headKMSale; // we'll be printing from this list. 

    fprintf(p->outputFile, "KnightsMart Daily Sales report:\n");
    dbg("Called doPrintDaySummary()");
    if (n == NULL) { // must be a recession
        fprintf(p->outputFile, "\tThere were no sales today.\n\n");
        return;
    }
    while (n != NULL) { // traverse the KMSale list.
        ctot = items = 0; // Set to zero because this is for one customer. 
        old = n; //save pointer to free it when done
        for (i = 0; i < n->numItemsOnList; i += 2) { // get the total number of items purchased by customer
            items += n->itemsPurchased[i + 1]; // we're skipping because of the layout of the array. 
        }
        if (items > 0) { // don't bother if didn't buy anything.
            fprintf(p->outputFile, "\tSale #%d, %s %s purchased %d item(s):\n", scount++, n->firstName, n->lastName, items); // not ++scount!
            for (i = 0; i < n->numItemsOnList; i += 2) { //[i] is the item number, [i+1] is the quantity
                info = findProduct(p, n->itemsPurchased[i]); // get the information about item to show about it.
                if (info != NULL && n->itemsPurchased[i + 1] != 0) { // if something actually was bought. 
                    dbg("Trying to print line");
                    fprintf(p->outputFile, "\t\t| Item %6d | %-20s | $%7.2lf (x%4d) |\n", info->itemNum, info->itemName, info->unitPrice, n->itemsPurchased[i + 1]);
                    ctot += n->itemsPurchased[i + 1] * info->unitPrice; // we're adding up all the stuff customer bought. 
                }
            }

            fprintf(p->outputFile, "\t\tTotal: $%.2lf\n", ctot); // total of what customer spent. 
        }
        n = n->next; // move to next sale
        free(old->itemsPurchased);
        free(old); //delete the node we're done with. 
        dtot += ctot; // add to the day's total. 
    }
    fprintf(p->outputFile, "\tGrand total: $%.2lf\n\n", dtot);
    p->headKMSale = NULL; // the list is gone, so change the head. 
}

// This runs a single day of the simulation.

void runDay(pSet *p) {
    int i, j; // loop counters
    int commands; // the number of commands to run today 
    char cname[20]; // the input name of command
    enum cmdenum {ADDITEM = 0, RESTOCK = 1, CUSTOMER = 2, INVENTORY = 3, PRINTDAYSUMMARY = 4} whatrun; // enumerated list which is matched to strings above of command names
    char *Cmdnames[] = {"ADDITEM", "RESTOCK", "CUSTOMER", "INVENTORY", "PRINTDAYSUMMARY"};
    int NUMCOMMANDS=5;
    int gotCmd; // "boolean", we got a command or not. 

    fscanf(p->inputFile, "%d", &commands);
    dbg("Running %d commands", commands);
    for (i = 0; i < commands; i++) { // run for each command in today's activities
        fscanf(p->inputFile, "%s", cname); // read a command. 
        dbg("Received command %s", cname);
        gotCmd = j = 0;

        do { // Iterate through commands until we recognize one. 
            /* I spent a lot of time trying to figure out why my program broke when I added another item! 
              I didn't add to the number of commands to run. */
            if (j >= NUMCOMMANDS) { // It doesn't match any of the commands we know about. 
                fprintf(stderr, "Invalid command received. Check command count.\n");
                exit(EXIT_FAILURE);
            }
            if (strcmp(cname, Cmdnames[j]) == 0) { // match command
                dbg("whatrun is %d", j);
                whatrun = j; // number of command
                gotCmd = 1; // we got a command, so get out of this loop.
            }
            j++; // go to next command in our list.
        } while (gotCmd == 0);
        switch (whatrun) { // call a function for each command.
            case ADDITEM: doAddItem(p);
                break;
            case RESTOCK: doRestock(p);
                break;
            case CUSTOMER: doCustomer(p);
                break;
            case INVENTORY: doInventory(p);
                break;
            case PRINTDAYSUMMARY: doPrintDaySummary(p);
                break;
        }

    }
}

// the next three functions just erase a list completely.
// They traverse a list, recursively erasing the previous node. 

void freeKMProduct(pSet *p, KMProduct *n) { // we need to give both p and n because n changes when we call again
    KMProduct *tmp; // a helper
    if (n != NULL) { // traverse the list
        tmp = n->next; // save the node to delete
        freeKMProduct(p, tmp); // delete the next node recursively
        free(n); // delete the old node
    }
    p->headKMProduct = NULL; // set list head to null. 
}

void freeKMRestockProduct(pSet *p, KMRestockProduct *n) { // works exactly same as freeKMProduct
    KMRestockProduct *tmp;
    if (n != NULL) {
        tmp = n->next;
        freeKMRestockProduct(p, tmp);
        free(n);
    }
    p->headKMRestockProduct == NULL;
}

void freeKMSale(pSet *p, KMSale *n) { // same as freeKMProduct, with additional free of int array
    KMSale *tmp;
    if (n != NULL) {
        tmp = n->next;
        freeKMSale(p, tmp);
        free(n->itemsPurchased); //each list member has another array pointer in it
        free(n);
    }
    p->headKMSale = NULL;
}

int main(int argc, char** argv) {
    int i, j, k; // counters
    int daysToRun; // how many days we run the simulation
    pSet *p; // our collection of pointers to lists and files. 

    p = (pSet *) smalloc(sizeof (pSet)); // allocate our pointer list. 

    p->inputFile = openFile(INPUTFILE, "r"); // open input file
    p->outputFile = openFile(OUTPUTFILE, "w"); // open output file


    fscanf(p->inputFile, "%d", &daysToRun); // first number in file is days to run.
    dbg("Run for %d days", daysToRun);
    for (i = 1; i <= daysToRun; i++) {
        fprintf(p->outputFile, "*************************\nUCF KnightsMart Day %d\n*************************\n", i);
        runDay(p); // our next "main", run all the commands for this day. 
    }

    // get rid of any leftovers in the lists
    freeKMProduct(p, p->headKMProduct);
    freeKMRestockProduct(p, p->headKMRestockProduct);
    freeKMSale(p, p->headKMSale);

    fclose(p->inputFile); // close files
    fclose(p->outputFile);
    free(p);
    return (EXIT_SUCCESS);
}

