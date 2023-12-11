#include "list.h"
#include <stdlib.h>
#include <stdio.h>

static Node nodePool[LIST_MAX_NUM_NODES];
static List listPool[LIST_MAX_NUM_HEADS];

static Node* freeN = NULL;
static List* freeL = NULL;

static int i = 0;



List* List_create() {

    // If it's the first time we're creating a list so we initialize the pools.
    if (!i) {
        // Setting up the list head pool, linking each head to the next one.
        for (int j = 0; j < LIST_MAX_NUM_HEADS - 1; j++) {
            listPool[j].next = &listPool[j + 1];
        }
        listPool[LIST_MAX_NUM_HEADS - 1].next = NULL;
        freeL = &listPool[0];

        // Setting up the node pool in a similar , linking each node to the next.
        for (int j = 0; j < LIST_MAX_NUM_NODES - 1; j++) {
            nodePool[j].next = &nodePool[j + 1];
        }
        nodePool[LIST_MAX_NUM_NODES - 1].next = NULL;
        freeN = &nodePool[0];

        i = 1;
    }
  
    // If there are no free list heads available, we return NULL to indicate failure.
    if (freeL == NULL) {
        return NULL;
    }

    // We take a list head from the pool and set up its fields for a new list.
    List* n_list = freeL;
    freeL = freeL->next;

    n_list->head = NULL;
    n_list->tail = NULL;
    n_list->current = NULL;
    n_list->counter = 0;

    return n_list;
}


int List_count(List* pList) {
    if (pList == NULL) {
        return LIST_FAIL;
    }
    return pList->counter;
}

void* List_first(List* pList) {
    // If the list doesn't exist or is empty, we return NULL and reset the current pointer.
    if (pList == NULL || pList->head == NULL) {
        pList->current = (Node*)LIST_OOB_START;
        return NULL;
    }
    // Otherwise, we set the current pointer to the head and return the first item.
    pList->current = pList->head;
    return pList->current->item;
}

void* List_last(List* pList) {
    // If the list doesn't exist or is empty, we return NULL and reset the current pointer.
   if (pList == NULL || pList->tail == NULL) {
        pList->current = (Node*)LIST_OOB_END;
        return NULL;
    }
    // Otherwise, we set the current pointer to the tail and return the last item.
    pList->current = pList->tail;
    if (pList->current != NULL) {
        return pList->current->item;
    }
    return NULL;
}

void* List_next(List* pList) {
    if (pList == NULL) {
        return NULL; // Explicitly return NULL
    }

    if (pList->current == NULL || pList->current->next == NULL) {
        pList->current = (Node*)LIST_OOB_END; 
        return NULL; 
    }

    pList->current = pList->current->next;
    return pList->current->item;
}

void* List_prev(List* pList) {
    if (pList == NULL) {
        return NULL; 
    }

    if (pList->current == NULL || pList->current->prev == NULL) {
        pList->current = (Node*)LIST_OOB_START; 
        return NULL; 
    }

    pList->current = pList->current->prev;
    return pList->current->item;
}


void* List_curr(List* pList) {
    // If the list doesn't exist or the current pointer is NULL, we return NULL.
    if (pList == NULL || pList->current == NULL) {
        return NULL;
    }
    return pList->current->item;
}

// Helper function to get a free node from the pool, returning NULL if no nodes are available.
Node* gfNode() {
    if (freeN == NULL) {
        return NULL;
    }
    Node* mynode = freeN;
    freeN = freeN->next;
    return mynode;
}

// Helper function to release a node back to the pool.
void relNode(Node* node) {
    if (node == NULL) {
        return;
    }
    node->prev = node->next;
    node->next = freeN;
    freeN = node;
}

// Helper function to release a list head back to the pool.
void relList(List* list) {
    if (list == NULL) {
        return;
    }
    list->prev = list->next;
    list->next = freeL;
    freeL = list;
}
// Adds the new item to pList directly after the current item, and makes item the current item. 
// If the current pointer is before the start of the pList, the item is added at the start. If 
// the current pointer is beyond the end of the pList, the item is added at the end. 
// Returns 0 on success, -1 on failure.
int List_insert_after(List* pList, void* pItem) {
    // Check if the list exists
    if (pList == NULL) {
        return LIST_FAIL;
    }

    // Get a free node from the pool and set its item to the given item
    Node* mynode = gfNode();
    if (mynode == NULL) {
        return LIST_FAIL;
    }
    mynode->item = pItem;

    // If the list is empty or the current pointer is at the tail, insert the new node at the end
    if (pList->head == NULL || pList->current == pList->tail) {
        mynode->prev = pList->tail;
        mynode->next = NULL;
        if (pList->tail != NULL) {
            pList->tail->next = mynode;
        }
        pList->tail = mynode;
        if (pList->head == NULL) {
            pList->head = mynode;
        }
    } else {
        // Otherwise, insert the new node after the current node
        mynode->next = pList->current->next;
        mynode->prev = pList->current;
        pList->current->next = mynode;
        mynode->next->prev = mynode;
    }

    // Set the current pointer to the new node and increase the item counter
    pList->current = mynode;
    pList->counter++;

    return LIST_SUCCESS;
}
// Adds item to pList directly before the current item, and makes the new item the current one. 
// If the current pointer is before the start of the pList, the item is added at the start. 
// If the current pointer is beyond the end of the pList, the item is added at the end. 
// Returns 0 on success, -1 on failure.
int List_insert_before(List* pList, void* pItem) {
    // Check if the list exists
    if (pList == NULL ) {
        return LIST_FAIL;
    }

    // Get a free node from the pool and set its item to the given item
    Node* mynode = gfNode();
    // Check to see if we are at max capacity 
    if (mynode == NULL) {
        return LIST_FAIL;
    }
    mynode->item = pItem;

    // If the list is empty or the current pointer is at the head, insert the new node at the start
    if (pList->head == NULL || pList->current == pList->head) {
        mynode->next = pList->head;
        mynode->prev = NULL;
        if (pList->head != NULL) {
            pList->head->prev = mynode;
        }
        pList->head = mynode;
        if (pList->tail == NULL) {
            pList->tail = mynode;
        }
    } else {
        // Otherwise, insert the new node before the current node
        mynode->prev = pList->current->prev;
        mynode->next = pList->current;
        pList->current->prev = mynode;
        mynode->prev->next = mynode;
    }

    // Set the current pointer to the new node and increase the item counter
    pList->current = mynode;
    pList->counter++;

    return LIST_SUCCESS;
}

// Adds item to the end of pList, and makes the new item the current one. 
// Returns 0 on success, -1 on failure.
int List_append(List* pList, void* pItem) {
    // Check if the list exists
    if (pList == NULL) {
        return LIST_FAIL;
    }
    // Get a free node from the pool and set its item to the given item
    Node* mynode = gfNode();
    if (mynode == NULL) {
        return LIST_FAIL;
    }
    mynode->item = pItem;
    mynode->next = NULL;
    // If the list is not empty, link the new node to the tail of the list
    if (pList->tail != NULL) {
        mynode->prev = pList->tail;
        pList->tail->next = mynode;
    } else {
        // If the list is empty, set the head pointer to the new node
        mynode->prev = NULL;
        pList->head = mynode;
    }

    // Set the tail and current pointers to the new node and increase the item counter
    pList->tail = mynode;
    pList->current = mynode;
    pList->counter++;

    return LIST_SUCCESS;
}

// Adds item to the front of pList, and makes the new item the current one. 
// Returns 0 on success, -1 on failure.
int List_prepend(List* pList, void* pItem) {
    // Check if the list exists
    if (pList == NULL) {
        return LIST_FAIL;
    }

    // Get a free node from the pool and set its item to the given item
    Node* mynode = gfNode();
    if (mynode == NULL) {
        return LIST_FAIL;
    }
    mynode->item = pItem;
    mynode->prev = NULL;

    // If the list is not empty, link the new node to the head of the list
    if (pList->head != NULL) {
        mynode->next = pList->head;

        //pList->head-> = mynode;
        // have to link the prev list to our current one properly 
        pList->head->prev = mynode;
    } else {
        // If the list is empty, set the tail pointer to the new node
        mynode->next = NULL;
        pList->tail = mynode;
    }

    // Set the head and current pointers to the new node and increase the item counter
    pList->head = mynode;
    pList->current = mynode;
    pList->counter++;

    return LIST_SUCCESS;
}

// Return current item and take it out of pList. Make the next item the current one.
// If the current pointer is before the start of the pList, or beyond the end of the pList,
// then do not change the pList and return NULL.
void* List_remove(List* pList) {
    // Check if the list exists and if there is a current item to remove
    if (pList == NULL || pList->current == NULL) {
        return NULL;
    }

    // Store the item to return it later, and update the pointers of the surrounding nodes to remove the current node
    void* temp = pList->current->item;
    Node* mynode = pList->current;

    if (mynode->prev != NULL) {
        mynode->prev->next = mynode->next;
    } else {
        pList->head = mynode->next;
    }

    if (mynode->next != NULL) {
        mynode->next->prev = mynode->prev;
    } else {
        pList->tail = mynode->prev;
    }

    // Update the current pointer to the next node, or to NULL if we removed the last node
    pList->current = mynode->next;
    //release the node
    relNode(mynode);
    pList->counter--;

    return temp;
}
// Return last item and take it out of pList. Make the new last item the current one.
// Return NULL if pList is initially empty.
void* List_trim(List* pList) {
    // Check if the list is empty
    if (pList->counter == 0) {
        return NULL;
    }

    // Store the item to return it later, and update the tail pointer to the previous node
    void* temp = pList->tail->item;
    // lets declare a node for the last node 
    Node* mynode = pList->tail;
    // Make the new last item the current one
    pList->tail = mynode->prev;

    if (pList->tail != NULL) {
        // release it, as in remove the item
        pList->tail->next = NULL;
    } else {
        // if the list is empty 
        pList->head = NULL;
    }

    // Update the current pointer to NULL, as we have removed the last item
    pList->current = pList->tail;

    // release the node
    relNode(mynode);
    //update the counter 
    pList->counter--;

    return temp;
}

// Adds pList2 to the end of pList1. The current pointer is set to the current pointer of pList1. 
// pList2 no longer exists after the operation; its head is available
// for future operations.
void List_concat(List* pList1, List* pList2) {
    // We first check if both lists exist.
    if (pList1 == NULL || pList2 == NULL) {
        return;
    }
    // We check if the first list is empty, in which case we set its head to the head of the second list.
    if (pList1->counter == 0){
        pList1->head = pList2->head;
        pList1->tail = pList2->tail;

    }
    // if the second plist is empty, we just return the first list
    else if (pList2->counter == 0){
        return;
    }
    else {
        // If both lists are non-empty, we link the tail of the first list to the head of the second list.
        pList1->tail->next = pList2->head;
        pList2->head->prev = pList1->tail;
        pList1->tail = pList2->tail;
    }
    //We update the item counter of the first list to the combined total of both lists.
    pList1->counter += pList2->counter;
    // We release the second list head back to the pool, as it is no longer needed.
    
    relList(pList2);
}
// Delete pList. pItemFreeFn is a pointer to a routine that frees an item. 
// It should be invoked (within List_free) as: (*pItemFreeFn)(itemToBeFreedFromNode);
// pList and all its nodes no longer exists after the operation; its head and nodes are 
// available for future operations.
void List_free(List* pList, FREE_FN pItemFreeFn) {
    // We first check if the list exists.
    if (pList == NULL) {
        return;
    }
    // Start at the top to iterate through
    Node* mynode = pList->head;
    while (mynode != NULL) {
        // we copy the current location of the node
        Node* temp = mynode->next;
        if (pItemFreeFn != NULL) {
            pItemFreeFn(mynode->item);
        }
        //release the node
        relNode(mynode);
        // mynode = mynode->next
        mynode = temp;
    }
    // We release the list head back to the pool and reset all its fields.
 
    relList(pList);
}
// Search pList, starting at the current item, until the end is reached or a match is found. 
// In this context, a match is determined by the comparator parameter. This parameter is a
// pointer to a routine that takes as its first argument an item pointer, and as its second 
// argument pComparisonArg. Comparator returns 0 if the item and comparisonArg don't match, 
// or 1 if they do. Exactly what constitutes a match is up to the implementor of comparator. 
// 
// If a match is found, the current pointer is left at the matched item and the pointer to 
// that item is returned. If no match is found, the current pointer is left beyond the end of 
// the list and a NULL pointer is returned.
// 
// If the current pointer is before the start of the pList, then start searching from
// the first node in the list (if any).
void* List_search(List* pList, COMPARATOR_FN pComparator, void* pComparisonArg) {
    // We first check if the list exists 
    if (pList == NULL ) {
        return NULL;
    }
    // Set our node to the current one 
     Node* mynode = (pList->current == NULL) ? pList->head : pList->current;

    // lets loop through the items to find pComparisonArg
    while (mynode != NULL) {
        if (pComparator(mynode->item, pComparisonArg) == 1) {
            pList->current = mynode;
            return mynode->item;
        }
        // update the iterator 
        mynode = mynode->next;
    }
    // If we didn't find the item, we reset the current pointer and return NULL.
    pList->current = (Node*)LIST_OOB_END;
    return NULL;
}

