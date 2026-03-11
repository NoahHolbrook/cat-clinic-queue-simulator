#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define INPUT_FILE "logs.txt"



typedef struct Cat_s {
    int arrival;            // Cat's arrival time at the clinic
                            // Minutes since clinic opened (unique)
    char *name;             // Cat's unique name (dynamic string)
    int duration;           // Required treatment time in minutes
} Cat;

typedef struct SLLNode_s {
    Cat *cat;               // Pointer to a dynamically allocated Cat
    struct SLLNode_s *next;
} SLLNode;


// Function Prototypes
void myMain(FILE *ifile);
SLLNode *createNode(Cat *cat);
void insertSorted(SLLNode **head, Cat *cat);
void freeQueue(SLLNode *head);
void freeListOnly(SLLNode *head);



#ifndef MAIN_FUNCTION
int main(void) {
   
    FILE *ifile = fopen(INPUT_FILE, "r");

    if( ifile == NULL ) {
        printf("File Does Not Exist!\n");
        return 1;
    }


    myMain(ifile);

    // Close the file
    fclose(ifile);

    return 0;
}
#endif




SLLNode *createNode(Cat *cat) {
    SLLNode *node = malloc(sizeof(SLLNode));
    node->cat = cat;
    node->next = NULL;
    return node;
}

// Inserts a cat into the sorted linked list (ordered by arrival time)

void insertSorted(SLLNode **head, Cat *cat) {
    SLLNode *newNode = createNode(cat);

    // Insert at head if list is empty or cat arrives before current head
    if (*head == NULL || cat->arrival < (*head)->cat->arrival) {
        newNode->next = *head;
        *head = newNode;
        return;
    }

    // Walk to the correct insertion point
    SLLNode *curr = *head;
    while (curr->next != NULL && curr->next->cat->arrival < cat->arrival) {
        curr = curr->next;
    }
    newNode->next = curr->next;
    curr->next = newNode;
}

// Frees all nodes AND their cats (used for the main queue)
void freeQueue(SLLNode *head) {
    while (head != NULL) {
        SLLNode *next = head->next;
        free(head->cat->name);
        free(head->cat);
        free(head);
        head = next;
    }
}

// Frees list nodes only — cats are owned by the main queue
void freeListOnly(SLLNode *head) {
    while (head != NULL) {
        SLLNode *next = head->next;
        free(head);
        head = next;
    }
}


// Function Definitions
void myMain(FILE *ifile) {
    SLLNode *queue = NULL;   // Sorted queue of cats (by arrival time)
    char tempName[26];       // Temporary buffer for reading names
    int arrival, duration;

    // Read input and build sorted queue using ordered insertion
    while (fscanf(ifile, "%d", &arrival) == 1 && arrival != -1) {
        fscanf(ifile, "%s %d", tempName, &duration);

        // Zero-duration cats leave immediately; skip entirely
        if (duration == 0) continue;

        // Allocate cat with exact memory for name
        Cat *cat = malloc(sizeof(Cat));
        cat->arrival = arrival;
        cat->duration = duration;
        cat->name = malloc(strlen(tempName) + 1);
        strcpy(cat->name, tempName);

        // Insert immediately into sorted position (no sorting after build)
        insertSorted(&queue, cat);
    }

    // Doctor availability times (both start free at minute 0)
    int uno_free = 0;
    int dos_free = 0;

    // Exposure list: push to front so traversal gives reverse-chron order
    SLLNode *exposureHead = NULL;

    // Rejection queue: append to tail to preserve rejection order
    SLLNode *rejectHead = NULL;
    SLLNode *rejectTail = NULL;

    // Process each cat in FIFO queue order
    SLLNode *curr = queue;
    while (curr != NULL) {
        Cat *cat = curr->cat;

        // Earliest possible start time for each doctor
        int eff_uno = (cat->arrival > uno_free) ? cat->arrival : uno_free;
        int eff_dos = (cat->arrival > dos_free) ? cat->arrival : dos_free;

        // Choose doctor: whoever is free first; ties go to Uno
        int start, use_uno;
        if (eff_uno <= eff_dos) {
            start   = eff_uno;
            use_uno = 1;
        } else {
            start   = eff_dos;
            use_uno = 0;
        }

        if (start + cat->duration > 480) {
            // Treatment cannot finish before clinic closes — reject
            SLLNode *rejNode = createNode(cat);
            if (rejectHead == NULL) {
                rejectHead = rejectTail = rejNode;
            } else {
                rejectTail->next = rejNode;
                rejectTail = rejNode;
            }
        } else {
            // Treat the cat
            printf("Doctor %s treated %s at %d\n",
                   use_uno ? "Uno" : "Dos", cat->name, start);

            if (use_uno) {
                uno_free = start + cat->duration;
            } else {
                dos_free = start + cat->duration;

                // Push to front of exposure list (builds reverse-chron order)
                SLLNode *expNode = malloc(sizeof(SLLNode));
                expNode->cat  = cat;
                expNode->next = exposureHead;
                exposureHead  = expNode;
            }
        }

        curr = curr->next;
    }

    // Print cats that could not be accommodated (in rejection order)
    SLLNode *rejCurr = rejectHead;
    while (rejCurr != NULL) {
        printf("Cannot accommodate %s\n", rejCurr->cat->name);
        rejCurr = rejCurr->next;
    }

    // Print exposure report
    if (exposureHead == NULL) {
        printf("No Exposed Cats\n");
    } else {
        printf("Exposed Cats\n");
        SLLNode *expCurr = exposureHead;
        while (expCurr != NULL) {
            printf("%s\n", expCurr->cat->name);
            expCurr = expCurr->next;
        }
    }

    // Free all dynamically allocated memory
    freeQueue(queue);          // frees all Cat objects and main queue nodes
    freeListOnly(rejectHead);  // frees reject wrapper nodes (cats already freed)
    freeListOnly(exposureHead);// frees exposure wrapper nodes (cats already freed)
}
