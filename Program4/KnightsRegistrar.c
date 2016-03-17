/**
 * Clayton Walker
 * COP 3502C-0001
 * Assignment #4 - KnightsRegistrar
 */

#include <stdio.h>
#include <string.h>

#define EMPTY -1
#define TIMELEN 25
#define MININHR 60
#define MINTOREG 5

//Structure definitions
typedef struct ucfClass {
    char ID[9]; // class ID, ex: "COP-3502"
    char days[6]; // a combination of "MTWRF" (days)
    char time[21]; // class time, ex: "10:30 AM - 12:00 PM"
} UCFclass;

typedef struct ucfStudent {
    char lastName[21]; // student last name
    char firstName[21]; // student first name
    int ID; // student ID
    int laptopID; // serial number of the laptop the student picks up
    int errorCode; // flag to determine if they will make mistakes
    int numClasses; // number of classes the student will register
    UCFclass *classes; // array of said classes (to be dynamically allocated)
    int enterTime; // time student arrived, in minutes, after 12:00 PM
    int timeRegistered; // Time the student registered successfully, in minutes
    
    int minutesLeft; // How many more minutes it is going to take the student to complete registration
    
    struct ucfStudent* next; // student* pointer to use in linked lists.
} student;

typedef struct queue {
    student* front;
    student* back;
}queue;

typedef struct stack {
    int * laptops;
    int top;   
}stack;

// Queue functions.
void init(queue* qPtr);
int enqueue(queue* qPtr, student *newStudent);
student* dequeue(queue* qPtr);
int emptyQ(queue* qPtr);
int front(queue* qPtr);

// Stack functions.
void initialize(stack* stackPtr, int numlaptops);
int full(stack* stackPtr, int stackSize);
int push(stack* stackPtr, int value, int stackSize);
int emptyS(stack* stackPtr);
int pop(stack* stackPtr);
int top(stack* stackPtr);

// Other functions.
void printTime(FILE* fout, int time);
int compareNames(char* firstname1, char* firstname2, char* lastname1, char* lastname2);
student* insert_inorder(student *front, student* newStudent);
void dellist(student *p);
void printDay(student * list, int day, int num, FILE* fout);


int main()
{
    FILE* fin;
    FILE* fout;
    
    fin = fopen("KnightsRegistrar.in", "r");
    fout = fopen("KnightsRegistrar.out", "w");
    
    if(fin == NULL) {
        printf("The input file could not be opened.\n");
        system("PAUSE");
        return 0;    
    }
    
    // Multiple pointers and other variables that are used through simulation.
    student* newStudent;
    student* LDM;
    student* LRM;
    student* temp;
    student* temp2;
    
    // Head pointer for linked list of students.
    student* list;
    
    newStudent = LDM = LRM = temp = temp2 = list = NULL;
    
    int i, j, k, l, numLaptops, numDays, serial, numStudents, studentsRemaining, minutes;
    
    // Make and initialize the three queues.
    queue* outsideLine = (queue*)malloc(sizeof(queue));
    queue* laptopGet = (queue*)malloc(sizeof(queue));
    queue* laptopReturn = (queue*)malloc(sizeof(queue));
    
    init(outsideLine);
    init(laptopGet);
    init(laptopReturn);
    
    fscanf(fin, "%d", &numLaptops);
    
    // Make and initialize the stack.
    stack Lap;
    initialize(&Lap, numLaptops);
    
    // Array of pointers used to hold students that are registering for classes.
    student* working[100];
    
    // Pushes laptops onto the stack.
    for(i = 0; i < numLaptops; i++)
    {
        fscanf(fin, "%d", &serial);
        push(&Lap, serial, numLaptops);
        working[i] = NULL;
    }
    
    fscanf(fin, "%d", &numDays);
    
    // For loop that runs for each day of the simulation.
    for(i = 0; i < numDays; i++)
    {
        fprintf(fout, "**********\nDay %d:\n**********\n\n", i+1);
        
        fscanf(fin, "%d", &numStudents);
        
        // Total keeps track of how many students have yet to finish registration.
        // The main while loop will end when total == 0
        studentsRemaining = numStudents;
        
        // This for loop runs as many times as there are students and stores them in the outsideLine queue.
        for(j = 0; j < numStudents; j++)
        {
            // Malloc space.
            newStudent = (student*)malloc(sizeof(student));
            
            // Scan in all the information from the file.
            fscanf(fin, "%d %s %s %d %d %d", &newStudent->enterTime, newStudent->lastName, newStudent->firstName, 
                &newStudent->ID, &newStudent->numClasses, &newStudent->errorCode);
            
            // Allocates space for array of classes.
            newStudent->classes = (UCFclass*)malloc(sizeof(UCFclass)*newStudent->numClasses);
            
            // Scan in class information
            for(k = 0; k < newStudent->numClasses; k++)
            {
                fscanf(fin, "%s %s ", newStudent->classes[k].ID, newStudent->classes[k].days);
                
                // Get the time of the class using fgets and strtok and the newline character "\n" as the delimeter.
                fgets(newStudent->classes[k].time, TIMELEN, fin);
                strtok(newStudent->classes[k].time, "\n");
            }
            
            // For registration purposes initialize the minutes left equal to five,
            // and its next to NULL for being put into a linked list.
            newStudent->minutesLeft = MINTOREG;
            newStudent->next = NULL;
            
            // Enqueue the new student into the outside line.
            enqueue(outsideLine, newStudent);
                            
        }
        
        minutes = 0;
        
        // Main while loop for simulation. Maintains simulation until number of students left to process = 0.
        while(studentsRemaining > 0)
        {
            // Reset the two temps to NULL.
            temp = temp2 = NULL;
            
            //=====PROCESS THE LRM=====
            
            // If the LRM is not NULL, i.e. if there is a student with them, take their laptop.
            if(LRM != NULL)
            {
                // If the student did not make an error registering,
                // take their laptop and insert them into the linked list of finished students.
                if(LRM->errorCode == 0)
                {
                    printTime(fout, minutes);
                    fprintf(fout, ":  %s %s successfully registered and returned laptop # %d.\n",
                        LRM->firstName, LRM->lastName, LRM->laptopID);
                    
                    // Assign the time they finished registration.
                    LRM->timeRegistered = minutes;
                    
                    // Push their laptop back onto the stack, and place them in the list.
                    push(&Lap, LRM->laptopID, numLaptops);
                    list = insert_inorder(list, LRM);
                    
                    // Decrement the number of students that have not finished registration.
                    studentsRemaining--;
                }
                
                // If the student made an error during registration,
                // place them back into the working student array.
                else
                {
                    printTime(fout, minutes);
                    fprintf(fout, ":  %s %s made an error and must redo the registration.\n",
                        LRM->firstName, LRM->lastName);
                    
                    // Reset their minutesLeft and errorCode.
                    LRM->minutesLeft = MINTOREG;
                    LRM->errorCode = 0;
                    
                    // Find a spot in the working array to insert them.
                    for(l = 0; l < numLaptops; l++)
                        if(working[l] == NULL)
                        {
                            working[l] = LRM;
                            break;
                        }
                }
                
                // Reset the LRM to NULL to show that there is not student with them.
                LRM = NULL;
            }
            
            //=====PROCESS THE LDM=====
            
            // If the LDM is not NULL, i.e. if there is a student with them, give them a laptop.
            if(LDM != NULL)
            {
                // Pop a laptop and give it to the student.
                LDM->laptopID = pop(&Lap);
                
                // Find a spot in the working array to place the student while they register.
                for(l = 0; l < numLaptops; l++)
                    if(working[l] == NULL)
                    {
                        working[l] = LDM;
                        break;
                    }
                    
                printTime(fout, minutes);
                fprintf(fout, ":  %s %s has checked-out laptop # %d.\n", LDM->firstName, LDM->lastName, LDM->laptopID);
                
                // Reset the LDM to NULL to show that there is now no student with them.
                LDM = NULL;
            }
            
            //=====PROCESS THE STUDENTS REGISTERING=====
            
            //Student registering
            for(l = 0; l < numLaptops; l++)
            {
                //If a student is not done yet, decrement minutesLeft.
                if(working[l] != NULL && working[l]->minutesLeft > 0)
                    working[l]->minutesLeft--;
                    
                //Otherwise assign students that are done to the laptopReturn queue.
                else if(working[l] != NULL && working[l]->minutesLeft == 0)
                {
                    //Two temp pointers are used to store students since aa maximum of two students can finish at the same time.
                    if(temp == NULL)
                    {
                        temp = working[l];
                        working[l] = NULL;
                    }
                    //If temp != NULL, i.e. one student is already done at this time, assign the second student to temp2.
                    else
                    {
                        temp2 = working[l];
                        working[l] = NULL;
                    }
                }          
            }
            
            // If only one student finished registering, place them in the latopReturn queue.
            if(temp != NULL && temp2 == NULL)
            {
                enqueue(laptopReturn, temp);
                printTime(fout, minutes);
                fprintf(fout, ":  %s %s finished registering and entered the laptop return line.\n", temp->firstName, temp->lastName);
            }
            // Otherwise place the two students that finished at the same time into the queue in the proper order.
            else if(temp != NULL && temp2 != NULL)
            {
                // If the first student found in the array arrived at the same time or earlier then the second student,
                // enqeue them first and then the second student.
                if(temp->enterTime <= temp2->enterTime)
                {
                    enqueue(laptopReturn, temp);
                    printTime(fout, minutes);
                    fprintf(fout, ":  %s %s finished registering and entered the laptop return line.\n", temp->firstName, temp->lastName);
                    
                    enqueue(laptopReturn, temp2);
                    printTime(fout, minutes);
                    fprintf(fout, ":  %s %s finished registering and entered the laptop return line.\n", temp2->firstName, temp2->lastName);
                }
                // Otherwise enqueue the second student found into the list and then the first student.
                else
                {
                    enqueue(laptopReturn, temp2);
                    printTime(fout, minutes);
                    fprintf(fout, ":  %s %s finished registering and entered the laptop return line.\n", temp2->firstName, temp2->lastName);
                    
                    enqueue(laptopReturn, temp);
                    printTime(fout, minutes);
                    fprintf(fout, ":  %s %s finished registering and entered the laptop return line.\n", temp->firstName, temp->lastName);
                }
                
            }
            
            //=====PROCESS THE LAPTOP RETURN LINE=====
            
            // If there is someone in the laptop return queue, place them with the LRM.
            if(!emptyQ(laptopReturn))
            {
                temp = dequeue(laptopReturn);
                LRM = temp;
            }
            
            //=====PROCESS THE OUTSIDE LINE=====
            
            // While there are people waiting to arrive at a given minute,
            // enqueue them into the laptopGet queue.
            while(front(outsideLine) == minutes)
            {
                temp = dequeue(outsideLine);
                
                enqueue(laptopGet, temp);
                
                printTime(fout, minutes);
                fprintf(fout, ":  %s %s has arrived at the Registrar and entered the laptop line.\n", temp->firstName, temp->lastName);
            }
            
            //=====PROCESS THE LAPTOP CHECKOUT LINE=====
            
            // If there is someone in the laptop get queue, place them with the LDM.
            if(!emptyQ(laptopGet) && !emptyS(&Lap))
            {
                temp = dequeue(laptopGet);
                LDM = temp;        
            }
            
            // Increment the minute counter.
            minutes++;
        }
        
        //Print the information for the day.
        printDay(list, i+1, numStudents, fout);
        
        //Free the linked list of students.
        dellist(list);
        list = NULL;
    }
    
    //Free the queues and close the files.
    free(outsideLine);
    free(laptopGet);
    free(laptopReturn);
    
    fclose(fin);
    fclose(fout);
    
    return 0;
}

// Pre-condition: qPtr points to a valid struct queue.
// Post-condition: The struct that qPtr points to will be set up to represent an
//                 empty queue.
void init(queue* qPtr) {
     
     // Just set both pointers to NULL!
     qPtr->front = NULL;
     qPtr->back = NULL;
}

// Pre-condition: qPtr points to a valid struct queue and val is the value to
//                enqueue into the queue pointed to by qPtr.
// Post-condition: If the operation is successful, 1 will be returned, otherwise
//                 no change will be made to the queue and 0 will be returned.
int enqueue(queue* qPtr, student* newStudent) {

    student* temp;

        // Set up our node to enqueue into the back of the queue.
        temp = newStudent;
        temp->next = NULL;
        
        // If the queue is NOT empty, we must set the old "last" node to point
        // to this newly created node.
        if (qPtr->back != NULL)
            qPtr->back->next = temp;
        
        // Now, we must reset the back of the queue to our newly created node.
        qPtr->back = temp;
        
        // If the queue was previously empty we must ALSO set the front of the
        // queue.
        if (qPtr->front == NULL)
            qPtr->front = temp;
        
        // Signifies a successful operation.
        return 1;
      
}

// Pre-condition: qPtr points to a valid struct queue.
// Post-condition: If the queue pointed to by qPtr is non-empty, then the value
//                 at the front of the queue is deleted from the queue and 
//                 returned. Otherwise, -1 is returned to signify that the queue
//                 was already empty when the dequeue was attempted.
student* dequeue(queue* qPtr) {
    
    student* retval;
    
    // Check the empty case.
    if(qPtr->front == NULL)
        return NULL;
    
    // Store the front value to return.
    retval = qPtr->front;
    
    // Make front point to the next node in the queue.
    qPtr->front = qPtr->front->next;
    
    // If deleting this node makes the queue empty, we have to change the back
    // pointer also!
    if(qPtr->front == NULL)
        qPtr->back = NULL;
    
    // Return the value that just got dequeued.
    return retval;
}

// Pre-condition: qPtr points to a valid struct queue.
// Post-condition: returns true iff the queue pointed to by pPtr is empty.
int emptyQ(queue* qPtr) {
    return qPtr->front == NULL;
}

// Pre-condition: qPtr points to a valid struct queue.
// Post-condition: if the queue pointed to by qPtr is non-empty, the value 
//                 stored at the front of the queue is returned. Otherwise,
//                 -1 is returned to signify that this queue is empty.
int front(queue* qPtr) {
    if (qPtr->front != NULL)
        return qPtr->front->enterTime;
    else
        return EMPTY;
}

void initialize(stack* stackPtr, int numlaptops) {
     stackPtr->top = -1;
     
     stackPtr->laptops = (int*)malloc(sizeof(int)*numlaptops);
}

// If the push occurs, 1 is returned. If the
// stack is full and the push can't be done, 0 is
// returned.
int push(stack* stackPtr, int value, int stackSize) {
    
    // Check if the stack is full.
    if (full(stackPtr, stackSize))
        return 0;
    
    // Add value to the top of the stack and adjust the value of the top.
    stackPtr->laptops[stackPtr->top+1] = value;
    (stackPtr->top)++;
    return 1;
}

// Returns true iff the stack pointed to by stackPtr is full.
int full(stack* stackPtr, int stackSize) {
    return (stackPtr->top == stackSize - 1);
}

// Returns true iff the stack pointed to by stackPtr is empty.
int emptyS(stack* stackPtr) {
    return (stackPtr->top == -1);
}

// Pre-condition: The stack pointed to by stackPtr is NOT empty.
// Post-condition: The value on the top of the stack is popped and returned.
// Note: If the stack pointed to by stackPtr is empty, -1 is returned.
int pop(stack* stackPtr) {
    
    int retval;
    
    // Check the case that the stack is empty.
    if (emptyS(stackPtr))
        return -1;
    
    // Retrieve the item from the top of the stack, adjust the top and return
    // the item.
    retval = stackPtr->laptops[stackPtr->top];
    (stackPtr->top)--;
    return retval;
}

// Pre-condition: The stack pointed to by stackPtr is NOT empty.
// Post-condition: The value on the top of the stack is returned.
// Note: If the stack pointed to by stackPtr is empty, -1 is returned.
int top(stack* stackPtr) {
    
    // Take care of the empty case.
    if (emptyS(stackPtr))
        return EMPTY;
    
    // Return the desired item.
    return stackPtr->laptops[stackPtr->top];
}

// Take in the number of minutes past 12:00 PM and print the corresponding time.
void printTime(FILE* fout, int time) {
    
    int hours = (time / MININHR) + 12;
    
    if(hours > 12)
        hours %= 12;
    
    int minutes = (time % MININHR);
    
    fprintf(fout, "%d:%.2d PM", hours, minutes);
}

// Insert a student into a linked list that is sorted alphabetically.
student* insert_inorder(student *front, student* newStudent) {
    
    // Helper pointer to traverse the list.
    student *help = front;
    
    if(front == NULL || compareNames(newStudent->firstName, help->firstName, newStudent->lastName, help->lastName) < 0)
    {
        newStudent->next = front;
        return newStudent;
    }

    // Traverse to the proper place to insert newStudent.
    while (help->next != NULL && 
            compareNames(newStudent->firstName, help->next->firstName, newStudent->lastName, help->next->lastName) > 0)
        help = help->next;

    // Save the node to patch to.
    newStudent->next = help->next;

    // Insert newStudent into the list.
    help->next = newStudent;
    
    // Return a pointer to the front of the list.
    return front;
}

// Compares two first and last names to sort them.
int compareNames(char* firstname1, char* firstname2, char* lastname1, char* lastname2) {
    
    if(strcmp(lastname1, lastname2) != 0)
        return strcmp(lastname1, lastname2);
    
    return strcmp(firstname1, firstname2);
}

// Delete the linked list of students.
void dellist(student* p) {
    
    student* temp;

    // Recursively free each element in the list.
    if (p !=NULL)
    {
        temp = p -> next;
        dellist(temp);
        
        free(p->classes);
        free(p);
    }
}
// Print all of the information pertaining to a day's registrations.
void printDay(student * list, int day, int num, FILE* fout)
{
    fprintf(fout, "\n*** Day %d:  UCF Daily Registration Report ***:\n\n", day);
    fprintf(fout, "The Registrar received %d registrations as follows:\n\n", num);
    
    if(list == NULL)
        return;
    
    int i;
    
    student* help = list;
    
    while(help != NULL)
    {
        fprintf(fout, "%s, %s, ID # %d\n", help->lastName, help->firstName, help->ID);
        fprintf(fout, "\tTime Registered:  ");
        printTime(fout, help->timeRegistered);
        fprintf(fout, "\n");
        fprintf(fout, "\tClasses:\n");
        
        for(i = 0; i < help->numClasses; i++)
            fprintf(fout, "\t| %-8s | %-5s | %-19s |\n", help->classes[i].ID, help->classes[i].days, help->classes[i].time);
        
        help = help->next;
    }
    fprintf(fout, "\n\n");
}
