#include "dynalloc.asmith16.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


// Sets capacity and sets Allocs and Holes to NULL
void initializeAllocation(int capacity, AllocInfo *allocInfo){
    // set capacity for allocinfo
    allocInfo->allocations = capacity;
    // make a block node for allocs and holes
    BlockListNode *allocs = malloc(sizeof(BlockListNode));
    BlockListNode *holes = malloc(sizeof(BlockListNode));
    // set values for each
    allocs = NULL;
    holes->start = 0;
    holes->size = capacity;
    holes->next = NULL;
    // set the alloc info to the inits
    allocInfo->allocations = allocs;
    allocInfo->holes = holes;

}

// adds a allocation to the end of the list
int addAllocation(int start, int size, AllocInfo *allocInfo){
    if(allocInfo != NULL){
        BlockListNode *curr = malloc(sizeof(BlockListNode));
        BlockListNode *prev = malloc(sizeof(BlockListNode));

        // new block to be added
        BlockListNode *newBlock = malloc(sizeof(BlockListNode));
        newBlock->next = NULL;
        newBlock->size = size;
        newBlock->start = start;

        curr = allocInfo->allocations;
        prev = NULL;
        
       	if(curr == NULL){
            // if first node
            allocInfo->allocations = newBlock;
        } else {
            // if not first node
            while(curr != NULL){
            prev = curr;
            curr = curr->next;
            } // end of this loop means the end of the list
            prev->next = newBlock;
        }
    } else {
	// if no where to put info then return 1
        return 1;
    }
    return 0;
}
int deleteAllocation(int start, AllocInfo *allocInfo){
    BlockListNode *curr = malloc(sizeof(BlockListNode));
    BlockListNode *prev = malloc(sizeof(BlockListNode));
    prev = NULL;
    curr = allocInfo->allocations;

    // if the fist node is being released
    if(curr != NULL && curr->start == start){
        allocInfo->allocations = curr->next;
        free(curr);
        return 0;
    }

    while(curr != NULL && curr->start != start){
        prev = curr;
        curr = curr->next;
    }

    if(curr != NULL){
        // if node in the middle or end is being deleted
        prev->next = curr->next;
    } else {
        // if the start being looked for does not exist
        return 1;
    }
    return 0;
}

int addHole(int start, int size, AllocInfo *allocInfo){
    if(allocInfo != NULL) {
        printf("got into add hold\n");
        BlockListNode *curr = malloc(sizeof(BlockListNode));
        BlockListNode *prev = malloc(sizeof(BlockListNode));

        // new block to be added
        BlockListNode *newBlock = malloc(sizeof(BlockListNode));
        newBlock->next = NULL;
        newBlock->size = size;
        newBlock->start = start;

        curr = allocInfo->holes;
        prev = NULL;
        
        if(curr == NULL){
            // if first node
            allocInfo->holes = newBlock;
        } else {
            // if not first node
            while(curr != NULL){
            prev = curr;
            curr = curr->next;
            } // end of this loop means the end of the list
            prev->next = newBlock;
        }
        } else {
        // if info is null return 1
        return 1;
    }
    return 0;
 }

int deleteHole(int start, AllocInfo *allocInfo){
    BlockListNode *curr = malloc(sizeof(BlockListNode));
    BlockListNode *prev = malloc(sizeof(BlockListNode));

    prev = NULL;
    curr = allocInfo->holes;

    // if the fist node is being released
    if(curr != NULL && curr->start == start){
        allocInfo->holes = curr->next;
        free(curr);
        return 0;
    }

    while(curr != NULL && curr->start != start){
        prev = curr;
        curr = curr->next;
    }

    if(curr != NULL){
        // if node in the middle or end is being deleted
        prev->next = curr->next;
    } else {
        // if the start being looked for does not exist
        return 1;
    }
    return 0;
}

int shrinkHole(int requestSize, BlockListNode *holeNode, AllocInfo *allocInfo){
    if(holeNode != NULL && holeNode->size < requestSize){
        holeNode->start = holeNode->start + requestSize;
        holeNode->size = holeNode->size - requestSize;
        return 0;
    } else {
        return 1;
    }
}

int expandHole(int newSize, BlockListNode *holeNode, AllocInfo *allocInfo){
    if(holeNode != NULL && holeNode->size > newSize){
        holeNode->size = newSize;
        return 0;
    } else {
        return 1;
    }
}