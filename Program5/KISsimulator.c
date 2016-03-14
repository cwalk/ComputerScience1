/**
 * Clayton Walker
 * COP 3502C-0001
 * Assignment #5-  KnightsHoc Internet Share Simulator
 * Practice with Binary Search Trees & Linked Lists
*/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>

/******************************************************************************/
/************************  Various structs we will use  ***********************/
/******************************************************************************/

typedef struct allowedMACs {
	int linkedMAC;
	struct allowedMACs *next;
} links;

typedef struct tree_node {
	int MAC;
	char firstName[21];
	char lastName[21];
	int broadcastRange;
	int x;
	int y;
	links *deviceLinks;
	int numLinks;
	struct tree_node *left;
	struct tree_node *right;
} BSTnode;


/******************************************************************************/
/**************************   FUNCTION PROTOTYPES   ***************************/
/******************************************************************************/
BSTnode* createNode(FILE* ifp);
BSTnode* insert(BSTnode *root, BSTnode *element);
BSTnode* getUser(BSTnode *current_ptr, int val);
int getMAC(BSTnode *current_ptr, char *fname, char *lname);
int alreadyLinked(BSTnode *root, int MAC1, int MAC2);
void makeLink(BSTnode *root, int MAC1, int MAC2);
void unLink(BSTnode *root, int MAC1, int MAC2);
links* insertLink(links *front, int num);
links* deleteLink(links *front, int num);
void printLinks(FILE *ofp, BSTnode *root, int MAC);
int withinRange(BSTnode *temp1, BSTnode *temp2);
void removeAllLinks(BSTnode *root, int MAC);
BSTnode* deleteUser(BSTnode* root, int value);
BSTnode* parent(BSTnode *root, BSTnode *node);
BSTnode* minVal(BSTnode *root);
BSTnode* maxVal(BSTnode *root);
int isLeaf(BSTnode *node);
int hasOnlyLeftChild(BSTnode *node);
int hasOnlyRightChild(BSTnode *node);
void inorder(FILE *ofp, BSTnode *current_ptr);
void moveDevices(BSTnode *current_ptr, int sizeX, int sizeY);
void destroyTree(BSTnode *root);
void destroyLinks(BSTnode *user);


/******************************************************************************/
/******************************   MAIN PROGRAM   ******************************/
/******************************************************************************/
int main(void) {
	FILE *ifp, *ofp;
	int seed;
	int sizeX, sizeY;
	int numOps;  // Number of operations to read from file
	char command[20];  // variable to save "command" into
	// variables to save info into when we read from file
	char fName1[21], lName1[21];
	int MAC1 = 0, MAC2 = 0;
	int currentlyLinked;  // conditional variable used to save result
	                       // from alreadyLinked function
	int i;

	BSTnode *myRoot = NULL;  // root of our BST
	BSTnode *tempNode, *tempNode2;  // temporary BST nodes

	// Open the file pointers
	ifp = fopen("KISsimulator.in","r");
	ofp = fopen("KISsimulator.out","w");

	if (ifp != NULL) {
		// Get the seed for psuedo-random number generator
		fscanf(ifp, "%d", &seed);
		// Use read in seed to initialize the generator
		srand(seed);
		// Get the X and Y sizes of coordinate plane
		fscanf(ifp, "%d %d", &sizeX, &sizeY);

		// Get number of operations that will be read from file
		fscanf(ifp, "%d", &numOps);

		// For loop over the number of operations
		for (i = 0; i < numOps; i++) {
			// Read in the command from file
			fscanf(ifp, "%s", command);

			// Here we perform the specified commands
			//fprintf(ofp, "%d.  ", i+1);
			// JOIN Command
			if (strcmp(command, "JOIN")==0) {
				//printf("JOIN\n");
				// call createNode function, which reads in the data
				// and returns a pointer of type BSTnode
				tempNode = createNode(ifp);
				// first make sure the user is NOT currently a member
				// So search the tree with this user MAC and see if a user is found
				tempNode2 = getUser(myRoot, tempNode->MAC);
				// If NOT found, add user to KIS system (call insert function)
				if (tempNode2 == NULL) {
					fprintf(ofp, "%s %s, MAC %d, joined KIS.\n", tempNode->firstName, tempNode->lastName, tempNode->MAC);
					myRoot = insert(myRoot, tempNode);
				}
				// ELSE the user is already a member...no need to ADD them
				else {
					fprintf(ofp, "Cannot Perform JOIN Command:\n");
					fprintf(ofp, "\tMAC %d, %s %s - already a participant in the KIS program.\n", tempNode->MAC, tempNode->firstName, tempNode->lastName);
				}
			}
			// FINDMAC Command
			else if (strcmp(command, "FINDMAC")==0) {
				//printf("FINDMAC\n");
				// Read in the user MAC
				fscanf(ifp, "%d", &MAC1);
				// Call the getUser function to get a pointer (access) to this user
				tempNode = getUser(myRoot, MAC1);
				// IF tempNode does not equal to NULL, the user was FOUND.
				if (tempNode != NULL) {					
					// Print out the appropriate information
					if (tempNode->numLinks == 1)
						fprintf(ofp, "Found:  MAC %d, %s %s, currently at position (%d, %d), %d Link\n", tempNode->MAC, tempNode->firstName,
						                                                    tempNode->lastName, tempNode->x, tempNode->y, tempNode->numLinks);
					else
						fprintf(ofp, "Found:  MAC %d, %s %s, currently at position (%d, %d), %d Links\n", tempNode->MAC, tempNode->firstName,
						                                                    tempNode->lastName, tempNode->x, tempNode->y, tempNode->numLinks);
				}
				// ELSE the user was not found.  Print out message.
				else
					fprintf(ofp, "MAC %d not found in the KIS system.\n", MAC1);

			}
			// LINK Command
			else if (strcmp(command, "LINK")==0) {
				//printf("LINK\n");
				// Read in both MACs
				fscanf(ifp, "%d %d", &MAC1, &MAC2);
				// Call the getUser function, on both MACs, to get access to user node
				tempNode = getUser(myRoot, MAC1);
				tempNode2 = getUser(myRoot, MAC2);
				// If either of the people are not currently participants:
				if ((tempNode == NULL) || (tempNode2 == NULL)) {
					fprintf(ofp, "Cannot Perform LINK Command:\n");
					// If both are not participants
					if ((tempNode == NULL) && (tempNode2 == NULL)) {
						fprintf(ofp, "\tMAC %d - This MAC Address is not in the KIS system.\n", MAC1);
						fprintf(ofp, "\tMAC %d - This MAC Address is not in the KIS system.\n", MAC2);
					}
					// Or if one of them is not a participant
					else if ((tempNode == NULL) && (tempNode2 != NULL))
						fprintf(ofp, "\tMAC %d - This MAC Address is not in the KIS system.\n", MAC1);
					else // ((tempNode != NULL) && (tempNode2 == NULL))
						fprintf(ofp, "\tMAC %d - This MAC Address is not in the KIS system.\n", MAC2);
				}
				// ELSE, they are both participants and can be LINKed
				else {
					// First check to make sure they are not already LINKed
					currentlyLinked = alreadyLinked(myRoot, MAC1, MAC2);
					// IF they are not currently LINKed
					if (currentlyLinked==0) {
						// LINK 'em
						makeLink(myRoot, MAC1, MAC2);
						fprintf(ofp, "MAC %d and MAC %d are now linked.\n", MAC1, MAC2);
					}
					// ELSE print out the appropriate error message.
					else {
						fprintf(ofp, "Cannot Perform LINK Command:\n");
						fprintf(ofp, "\tMAC %d and MAC %d are already linked.\n", MAC1, MAC2);
					}
				}
			}
			// UNLINK Command
			else if (strcmp(command, "UNLINK")==0) {
				//printf("UNLINK\n");
				// Read in both MACs
				fscanf(ifp, "%d %d", &MAC1, &MAC2);
				// Call the getUser function, on both MACs, to get access to user node
				tempNode = getUser(myRoot, MAC1);
				tempNode2 = getUser(myRoot, MAC2);
				// If either of the people are not currently participants:
				if ((tempNode == NULL) || (tempNode2 == NULL)) {
					fprintf(ofp, "Cannot Perform UNLINK Command:\n");
					// If both are not participants
					if ((tempNode == NULL) && (tempNode2 == NULL)) {
						fprintf(ofp, "\tMAC %d - This MAC Address is not in the KIS system.\n", MAC1);
						fprintf(ofp, "\tMAC %d - This MAC Address is not in the KIS system.\n", MAC2);
					}
					// Or if one of them is not a participant
					else if ((tempNode == NULL) && (tempNode2 != NULL))
						fprintf(ofp, "\tMAC %d - This MAC Address is not in the KIS system.\n", MAC1);
					else // ((tempNode != NULL) && (tempNode2 == NULL))
						fprintf(ofp, "\tMAC %d - This MAC Address is not in the KIS system.\n", MAC2);
				}
				// ELSE, they are both participants and can be unlinked
				else {
					// First make sure they are already linked (so we can unlink them)
					currentlyLinked = alreadyLinked(myRoot, MAC1, MAC2);
					// IF they are already linked
					if (currentlyLinked==1) {
						// Unlink 'em
						unLink(myRoot, MAC1, MAC2);
						fprintf(ofp, "MAC %d and MAC %d are no longer linked.\n", MAC1, MAC2);
					}
					// ELSE print out the appropriate error message.
					else {
						fprintf(ofp, "Cannot Perform UNLINK Command:\n");
						fprintf(ofp, "\tMAC %d and MAC %d are not currently linked.\n", MAC1, MAC2);
					}
				}
			}
			// QUIT Command
			else if (strcmp(command, "QUIT")==0) {
				//printf("QUIT\n");
				// Read in the MAC of user being deleted
				fscanf(ifp, "%d", &MAC1);
				// Get the ID of that user by calling the getUser function
				tempNode = getUser(myRoot, MAC1);
				// IF the tempNode is NULL, then the user does NOT exist
				// Print appropriate error messsage
				if (tempNode == NULL) {
					fprintf(ofp, "Cannot Perform QUIT Command:\n");
					fprintf(ofp, "\tMAC %d not found in the KIS system.\n", MAC1);
				}
				// ELSE, the user does exist and should be deleted
				else {
					// Before a user can be deleted, they must be removed from the
					// deviceLinks linked-list of anyone they were linked with.  So call this
					// removeAllLinks function for this purpose
					removeAllLinks(myRoot, MAC1);
					// Then we delete the user from the BST and print the message.
					myRoot = deleteUser(myRoot, MAC1);
					fprintf(ofp, "MAC %d has been removed from the KIS system.\n", MAC1);
				}
			}
			// SHOWCONNECTIONS Command
			else if (strcmp(command, "SHOWCONNECTIONS")==0) {
				//printf("SHOWCONNECTIONS\n");
				// Read in the MAC of user being deleted
				fscanf(ifp, "%d", &MAC1);
				// Get the ID of that user by calling the getUser function
				tempNode = getUser(myRoot, MAC1);
				// IF the tempNode is NULL, then the user does NOT exist
				// Print appropriate error messsage
				if (tempNode == NULL) {
					fprintf(ofp, "Cannot Perform SHOWCONNECTIONS Command:\n");
					fprintf(ofp, "\tMAC %d - This MAC Address is not in the KIS system.\n", MAC1);
				}
				// ELSE call the printLinks function that prints an
				// ascending list of this user's linked MACs
				else {
					printLinks(ofp, myRoot, MAC1);
				}
			}
			// PRINTKISMEMBERS Command
			else  if (strcmp(command, "PRINTKISMEMBERS")==0) {
				//printf("PRINTKISMEMBERS\n");
				// IF the root does not equal NULL (meaning, there IS a tree)
				if (myRoot != NULL) {
					// Print out the BST
					fprintf(ofp, "Members of KnightsHoc Internet Share:\n");
					// via an INORDER traversal
					inorder(ofp, myRoot);
				}
				// ELSE print out the appropriate error message
				else {
					fprintf(ofp, "Cannot Perform PRINTKISMEMBERS Command:\n");
					fprintf(ofp, "\tThere are currently no participants of the KIS system.\n");
				}
			}
			// MOVEDEVICES Command
			else  if (strcmp(command, "MOVEDEVICES")==0) {
				if (myRoot != NULL) {
					//printf("MOVEDEVICES\n");
					// call moveDevices function
					moveDevices(myRoot, sizeX, sizeY);
					// Print out success message
					fprintf(ofp, "All devices successfully moved.\n");
				}
				else {
					fprintf(ofp, "Cannot Perform MOVEDEVICES Command:\n");
					fprintf(ofp, "\tThere are currently no participants of the KIS system.\n");
				}
			}
			else;
		}

		// free the two temporary BST nodes
		free(tempNode);
		free(tempNode2);

		// free the BST
		destroyTree(myRoot);

	}
	else
		printf("Can't find file\n");

	system("PAUSE");
	return 0;
}


/*******************************************************************************/
/*                                                                             */
/*                                  BST FUNCTIONS                              */
/*                                                                             */
/*******************************************************************************/

//
// createNode Function
// Arguments:  a file pointer to the input file
// Returns:  a pointer of type BSTnode, pointing to the newly created node in memory
// Description:  this function mallocs space for a new BSTnode, reads in the
// appropriate data from the file, initializes all remaining struct members
// accordingly, and then returns a pointer to this node.
//
BSTnode* createNode(FILE* ifp) {
	BSTnode *tempNode;
	tempNode = (BSTnode*)malloc(sizeof(BSTnode));
	fscanf(ifp, "%d %s %s %d %d %d", &tempNode->MAC, tempNode->firstName, tempNode->lastName,
	                           &tempNode->broadcastRange, &tempNode->x, &tempNode->y);
	tempNode->numLinks = 0;
	tempNode->deviceLinks = NULL;
	tempNode->left = NULL;
	tempNode->right = NULL;

	return tempNode;
}


//
// insert Function
// Arguments:  (1) a pointer to the root of the BST
//             (2) a pointer to the node (user) we want to insert
// Returns:  a pointer of type BSTnode, pointing to the (possibly updated) root
// Description:  this function inserts a node into a BST, following the ordering
// property of a BST, based on the ID of the user.
//
BSTnode* insert(BSTnode *root, BSTnode *element) {

  // Inserting into an empty tree.
  if (root == NULL) 
    return element;
  else {

    // element should be inserted to the right.
    if (element->MAC > root->MAC) {

      // There is a right subtree to insert the node.
      if (root->right != NULL)
        root->right = insert(root->right, element);

      // Place the node directly to the right of root.
      else
        root->right = element;
    }

    // element should be inserted to the left.
    else {

      // There is a left subtree to insert the node.
      if (root->left != NULL)
        root->left = insert(root->left, element);

      // Place the node directly to the left of root.
      else
        root->left = element;
    } 

    // Return the root pointer of the updated tree.
    return root;
  }
}


//
// getUser Function
// Arguments:  (1) a pointer to the root of the BST
//             (2) an int representing the ID of the user we wish to find
// Returns:  a pointer of type BSTnode, pointing to the user in the tree (if found)
// or returns NULL if no such user is found.
// Description:  this function searcches the BST based on an ID value and returns
// a pointer to the node (user) if it is found or returns NULL otherwise.
//
BSTnode* getUser(BSTnode *current_ptr, int val) {

  // Check if there are nodes in the tree.
  if (current_ptr != NULL) {

    // Found the value at the root.
    if (current_ptr->MAC == val)
      return current_ptr;

    // Search to the left.
    if (val < current_ptr->MAC) 
      return getUser(current_ptr->left, val);
    
    // Or...search to the right.
    else 
      return getUser(current_ptr->right, val);
    
  }
  else
    return NULL;
}

int getMAC(BSTnode *current_ptr, char *fname, char *lname) {
  // Check if there are nodes in the tree.
  if (current_ptr != NULL) {

    // Found the value at the root.
    if ((strcmp(current_ptr->lastName, lname)==0) && (strcmp(current_ptr->firstName, fname)==0))
      return current_ptr->MAC;

    // Search to the left.
    if ((strcmp(current_ptr->lastName, lname)==1) || ((strcmp(current_ptr->lastName, lname)==0) && (strcmp(current_ptr->firstName, fname)==1)))
      return getMAC(current_ptr->left, fname, lname);
    
    // Or...search to the right.
    else 
      return getMAC(current_ptr->right, fname, lname);
    
  }
  else
    return 0;	
}

int alreadyLinked(BSTnode *root, int MAC1, int MAC2) {
	BSTnode *temp1, *temp2;
	links *helpPtr1, *helpPtr2;
	int MAC1isBuddy, MAC2isBuddy;

	temp1 = getUser(root, MAC1);
	temp2 = getUser(root, MAC2);
	helpPtr1 = temp1->deviceLinks;
	helpPtr2 = temp2->deviceLinks;

	while (helpPtr1 != NULL) {
		if (helpPtr1->linkedMAC == MAC2) {
			MAC2isBuddy = 1;
		}
		helpPtr1 = helpPtr1->next;
	}
	while (helpPtr2 != NULL) {
		if (helpPtr2->linkedMAC == MAC1) {
			MAC1isBuddy = 1;
		}
		helpPtr2 = helpPtr2->next;
	}
	if ((MAC1isBuddy==1) && (MAC2isBuddy==1))
		return 1;
	else
		return 0;
}


void makeLink(BSTnode *root, int MAC1, int MAC2) {
	BSTnode *temp1, *temp2;

	temp1 = getUser(root, MAC1);
	temp2 = getUser(root, MAC2);
	temp1->deviceLinks = insertLink(temp1->deviceLinks, MAC2);
	temp2->deviceLinks = insertLink(temp2->deviceLinks, MAC1);

	temp1->numLinks++;
	temp2->numLinks++;
}

// Pre-condition: front points to the head of a linked list structure that
//                has its nodes arranged in ascending numerical order.
// Post-condition: a new node containing num will be created and added
//                 into the appropriate position in the linked list
//                 front and a pointer to the front of this new linked
//                 list will be returned.
links* insertLink(links *front, int num) {
  links *temp;
  links *helpPtr;
  links *savelink;

  // Create the new node.
  temp = (links*)malloc(sizeof(links));
  temp->linkedMAC = num;
  temp->next = NULL;

  // Take care of case inserting into an empty list.
  if (front == NULL)
    return temp;

  if (num < front->linkedMAC) {
    temp->next = front;
    return temp;
  }

  // make helpPtr point to front, so helpPtr can traverse the list
  helpPtr = front;

  // Use front to iterate to the right spot to insert temp.
  while (helpPtr->next != NULL && helpPtr->next->linkedMAC < num)
    helpPtr = helpPtr->next;

  // Save the node to patch to.
  savelink = helpPtr->next;

  // Insert temp.
  helpPtr->next = temp;
  temp->next = savelink;
  
  // Return a pointer to the front of the list.
  return front;
}


void unLink(BSTnode *root, int MAC1, int MAC2) {
	BSTnode *temp1, *temp2;

	temp1 = getUser(root, MAC1);
	temp2 = getUser(root, MAC2);
	temp1->deviceLinks = deleteLink(temp1->deviceLinks, MAC2);
	temp2->deviceLinks = deleteLink(temp2->deviceLinks, MAC1);

	temp1->numLinks--;
	temp2->numLinks--;
}


// Pre-condition: front points to the head of a linked list structure.
// Post-condition: The first node that contains the value num will be
//                 removed from the list. A pointer to the front of the
//                 list will be returned. If no such value is stored,
//                 the list will remain unchanged.
links* deleteLink(links *front, int num) {
  links *helpPtr, *node2delete, *temp;
  helpPtr = front;

  // Only need to delete if the list is not null.
  if (helpPtr != NULL) {
    
    // Take care of the case where first node needs to be deleted.
    if (helpPtr->linkedMAC == num) {
      temp = helpPtr -> next;
      free(helpPtr);
      return temp;
    }
    // Otherwise, loop until you find the node to delete and do so.
    while (helpPtr->next != NULL) {
      if (helpPtr->next->linkedMAC == num) {
        node2delete = helpPtr -> next;
        helpPtr->next = helpPtr->next->next;
        free(node2delete);   
        return front;
      }
      helpPtr = helpPtr->next;
    }
    
  }
  return front;
}


void printLinks(FILE *ofp, BSTnode *root, int MAC) {
	BSTnode *temp, *temp2;
	links *helpPtr;
	int inRange;
	int count = 0;

	temp = getUser(root, MAC);
	helpPtr = temp->deviceLinks;

	if (temp->numLinks == 0)
		fprintf(ofp, "MAC %d has no links.\n", MAC);
	else {
		fprintf(ofp, "Connections for MAC %d, %s %s, currently at position (%d, %d):\n", MAC, temp->firstName, temp->lastName, temp->x, temp->y);
		fprintf(ofp, "\tThere are a total of %d link(s).\n", temp->numLinks);
		while (helpPtr != NULL) {
			temp2 = getUser(root, helpPtr->linkedMAC);
			inRange = withinRange(temp, temp2);
			if (inRange == 1)
				count++;
			helpPtr = helpPtr->next;
		}
		fprintf(ofp, "\tThere are %d active link(s) within the broadcast range of %d:\n", count, temp->broadcastRange);
		// Reset helpPtr and again iterate through list
		helpPtr = temp->deviceLinks;
		while (helpPtr != NULL) {
			temp2 = getUser(root, helpPtr->linkedMAC);
			inRange = withinRange(temp, temp2);
			if (inRange == 1)
				fprintf(ofp, "\t\tMAC %d, %s %s, currently at position (%d, %d)\n", temp2->MAC, temp2->firstName, temp2->lastName, temp2->x, temp2->y);
			helpPtr = helpPtr->next;
		}
	}
}

int withinRange(BSTnode *temp1, BSTnode *temp2) {
	int distance;
	int diffx, diffy;
	diffx = temp1->x - temp2->x;
	diffy = temp1->y - temp2->y;
	distance = (int)sqrt(((diffx*diffx)+(diffy)*(diffy)));
	if (distance <= temp1->broadcastRange)
		return 1;
	else
		return 0;
}

void inorder(FILE *ofp, BSTnode *current_ptr) {
	// Only traverse the node if it's not null.
	if (current_ptr != NULL) {
		inorder(ofp, current_ptr->left); // Go Left.
					if (current_ptr->numLinks == 1)
						fprintf(ofp, "\tMAC %d, %s %s, currently at position (%d, %d), %d Link\n", current_ptr->MAC, current_ptr->firstName,
						                                                    current_ptr->lastName, current_ptr->x, current_ptr->y, current_ptr->numLinks);
					else
						fprintf(ofp, "\tMAC %d, %s %s, currently at position (%d, %d), %d Links\n", current_ptr->MAC, current_ptr->firstName,
						                                                    current_ptr->lastName, current_ptr->x, current_ptr->y, current_ptr->numLinks);
		inorder(ofp, current_ptr->right); // Go Right.
  }
}


void removeAllLinks(BSTnode *root, int MAC) {
	BSTnode *user, *temp;
	user = getUser(root, MAC);
	links *helpPtr;
	helpPtr = user->deviceLinks;
	while (helpPtr != NULL) {
		temp = getUser(root, helpPtr->linkedMAC);
		temp->deviceLinks = deleteLink(temp->deviceLinks, MAC);
		temp->numLinks--;
		helpPtr = helpPtr->next;
	}

	// Now destroy the buddy list of this user.
	destroyLinks(user);
}


// Will delete the node storing value in the tree rooted at root. The
// value must be present in the tree in order for this function to work.
// The function returns a pointer to the root of the resulting tree.
BSTnode* deleteUser(BSTnode* root, int value) {
	BSTnode *delnode, *new_del_node, *save_node;
	BSTnode *par;
	int save_MAC;
	char save_firstName[21];
	char save_lastName[21];
	int save_x;
	int save_y;
	int save_broadcastRange;
	int save_numLinks;
	links *save_deviceLinks;

	delnode = getUser(root, value); // Get a pointer to the node to delete.
	
	par = parent(root, delnode); // Get the parent of this node.
	
	// Take care of the case where the node to delete is a leaf node.
	if (isLeaf(delnode)) {
		// Deleting the only node in the tree.
		if (par == NULL) { 
			free(root); // free the memory for the node.
			return NULL;
		}
		// Deletes the node if it's a left child.
		if (value < par->MAC) {
			free(par->left); // Free the memory for the node.
			par->left = NULL;
		}
		// Deletes the node if it's a right child.
		else {
			free(par->right); // Free the memory for the node.
			par->right = NULL;    
		}
		
		return root; // Return the root of the new tree.
	}
	
	// Take care of the case where the node to be deleted only has a left 
	// child.
	if (hasOnlyLeftChild(delnode)) {
		// Deleting the root node of the tree.
		if (par == NULL) {
			save_node = delnode->left;
			free(delnode); // Free the node to delete.
			return save_node; // Return the new root node of the resulting tree.
		}
		
		// Deletes the node if it's a left child.
		if (value < par->MAC) {
			save_node = par->left; // Save the node to delete.
			par->left = par->left->left; // Readjust the parent pointer.
			free(save_node); // Free the memory for the deleted node.
		}
		
		// Deletes the node if it's a right child.
		else {
			save_node = par->right; // Save the node to delete.
			par->right = par->right->left; // Readjust the parent pointer.
			free(save_node); // Free the memory for the deleted node.
		}	
		return root; // Return the root of the tree after the deletion.
	}
	
	// Takes care of the case where the deleted node only has a right child.
	if (hasOnlyRightChild(delnode)) {
		// Node to delete is the root node.
		if (par == NULL) {
			save_node = delnode->right;
			free(delnode);
			return save_node;
		}
		
		// Delete's the node if it is a left child.
		if (value < par->MAC) {
			save_node = par->left;
			par->left = par->left->right;
			free(save_node);
		}
		
		// Delete's the node if it is a right child.
		else {
			save_node = par->right;
			par->right = par->right->right;
			free(save_node);
		}
		return root;
	}
	
	// Find the new physical node to delete.
	new_del_node = minVal(delnode->right);

	// Save the data from that minVal node to temp variables
	// After we delete this minVal node, we will copy the temporarily saved
	// data into the correct spots on the delnode (the node we are really "deleting")
	save_MAC = new_del_node->MAC;
	strcpy(save_firstName, new_del_node->firstName);
	strcpy(save_lastName, new_del_node->lastName);
	save_broadcastRange = new_del_node->broadcastRange;
	save_x = new_del_node->x;
	save_y = new_del_node->y;
	save_numLinks = new_del_node->numLinks;
	save_deviceLinks = new_del_node->deviceLinks;
	
	deleteUser(root, save_MAC);  // Now, delete the minVal node
	
	// Restore the data (from minVal node) to the original node to be deleted.
	delnode->MAC = save_MAC;
	strcpy(delnode->firstName, save_firstName);
	strcpy(delnode->lastName, save_lastName);
	delnode->broadcastRange = save_broadcastRange;
	delnode->x = save_x;
	delnode->y = save_y;
	delnode->numLinks = save_numLinks;
	delnode->deviceLinks = save_deviceLinks;
	
	return root;
}


// Returns the parent of the node pointed to by node in the tree rooted at
// root. If the node is the root of the tree, or the node doesn't exist in
// the tree, null will be returned.
BSTnode* parent(BSTnode *root, BSTnode *node) {
	// Take care of NULL cases.
	if (root == NULL || root == node)
		return NULL;
	
	// The root is the direct parent of node.
	if (root->left == node || root->right == node)
		return root;
	
	// Look for node's parent in the left side of the tree.
	if (node->MAC < root->MAC)
		return parent(root->left, node);
	
	// Look for node's parent in the right side of the tree.
	else if (node->MAC > root->MAC)
		return parent(root->right, node);
	
	return NULL; // Catch any other extraneous cases.
}


// Returns a pointer to the node storing the minimum value in the tree
// with the root, root. Will not work if called with an empty tree.
BSTnode* minVal(BSTnode *root) {

  // Root stores the minimal value.
  if (root->left == NULL)
    return root;

  // The left subtree of the root stores the minimal value.
  else
    return minVal(root->left);
}


// Returns a pointer to the node storing the maximum value in the tree
// with the root, root. Will not work if called with an empty tree.
BSTnode* maxVal(BSTnode *root) {

  // Root stores the maximal value.
  if (root->right == NULL)
    return root;

  // The right subtree of the root stores the maximal value.
  else
    return maxVal(root->right);
}


// Returns 1 if node is a leaf node, 0 otherwise.
int isLeaf(BSTnode *node) {

  return (node->left == NULL && node->right == NULL);
}

// Returns 1 iff node has a left child and no right child.
int hasOnlyLeftChild(BSTnode *node) {
  return (node->left!= NULL && node->right == NULL);
}

// Returns 1 iff node has a right child and no left child.
int hasOnlyRightChild(BSTnode *node) {
  return (node->left== NULL && node->right != NULL);
}

void moveDevices(BSTnode *current_ptr, int sizeX, int sizeY) {
	// Only traverse the node if it's not null.
	if (current_ptr != NULL) {
		moveDevices(current_ptr->left, sizeX, sizeY); // Go Left.
		current_ptr->x = rand() % sizeX;
		current_ptr->y = rand() % sizeY;
		moveDevices(current_ptr->right, sizeX, sizeY); // Go Right.
	}
}


void destroyTree(BSTnode *root) {
	if(root != NULL) {
		destroyTree(root->left);
		destroyTree(root->right);
		destroyLinks(root);
		free(root);
	}
}

void destroyLinks(BSTnode *user) {
	links *helpPtr, *temp;
	helpPtr = user->deviceLinks;

	while (helpPtr != NULL) {
		temp = helpPtr->next;
		free(helpPtr);
		helpPtr = temp;
	}

	return;
}
