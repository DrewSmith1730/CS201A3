#include <stdio.h>
#include "dynalloc.asmith16.h"

//----------------------------------------------------------
// this checks to see whether the lists are sorted by increasing
// start value
// - returns zero if OK (lists are sorted by start value
// - otherwise returns nonzero

int checkSorted(AllocInfo *allocInfo) {
  int currVal;
  int fail;
  BlockListNode *currNode;

  fail = 0;
  currVal = 0;

  currNode = allocInfo->allocations;
  while (currNode != NULL) {
    if (currNode->start < currVal) {
      printf("ERROR: alloc start val for alloc (%d, %d) is less than prev start val (%d)\n", currNode->start, currNode->size, currVal);
      fail = 1;
    }
    currVal = currNode->start;
    currNode = currNode->next;
  }

  currVal = 0;

  currNode = allocInfo->holes;
  while (currNode != NULL) {
    if (currNode->start < currVal) {
      printf("ERROR: hole start val for alloc (%d, %d) is less than prev start val (%d)\n", currNode->start, currNode->size, currVal);
      fail = 1;
    }
    currVal = currNode->start;
    currNode = currNode->next;
  }

  return fail;
}
//----------------------------------------------------------
// call this to make sure that your hole list and allocation list
// are self consistent
// - pass nonzero for sortedFlag to check whether your list is sorted
// - returns zero when everything is OK
// - otherwise returns nonzero

int validateAllocation(AllocInfo *allocInfo, int sortedFlag) {
  int allocSum, holeSum, idx, found;
  BlockListNode *currNode, *holeNode, *allocNode;
  int lookingFor, fail;

  int HOLE = 0;
  int ALLOC = 1;
  int EITHER = 2;

  fail = 0;
  allocSum = 0;
  holeSum = 0;

  if (sortedFlag)
    fail = checkSorted(allocInfo);

  currNode = allocInfo->allocations;
  while (currNode != NULL) {
    allocSum = allocSum + currNode->size;
    currNode = currNode->next;
  }

  currNode = allocInfo->holes;
  while (currNode != NULL) {
    holeSum = holeSum + currNode->size;
    currNode = currNode->next;
  }

  if (allocSum + holeSum != allocInfo->capacity) {
    printf("ERROR: capacity = %d, allocSum = %d, holeSum = %d\n",
           allocInfo->capacity, allocSum, holeSum);
    return 1;
  }

  idx = 0;

  found = 0;
  holeNode = allocInfo->holes;
  allocNode = allocInfo->allocations;

  // try to find idx=0 in either the holes list or the allocs list
  currNode = allocInfo->holes;

  while (currNode != NULL && currNode->start != idx)
    currNode = currNode->next;

  if ( currNode != NULL ) {
    // found idx=0 in holes list; next thing better be an alloc
    lookingFor = ALLOC;
    idx = idx + currNode->size;
  } else {
    // idx=0 is not in holes list; better be in allocs list!
    currNode = allocInfo->allocations;
    while (currNode != NULL && currNode->start != idx)
      currNode = currNode->next;
    if ( currNode != NULL ) {
      // found idx=0 in alloc list; next thing can be a hole
      // or another allocation
      lookingFor = EITHER;
      idx = idx + currNode->size;
    } else {
      printf("ERROR: cannot find idx %d in either holes or allocations\n", idx);
      return 1;
    }
  }

  while (idx < allocInfo->capacity) {
    if (lookingFor == HOLE) {
      currNode = allocInfo->holes;
      while (currNode != NULL && currNode->start != idx)
        currNode = currNode->next;
      if (currNode == NULL) {
        printf("ERROR: can't find expected idx %d as start value in holes list \n", idx);
        return 1;
      } else {
        // only alloc can follow hole
        lookingFor = ALLOC;
        idx = idx + currNode->size;
      }
    } else if (lookingFor == ALLOC) {
      currNode = allocInfo->allocations;
      while (currNode != NULL && currNode->start != idx)
        currNode = currNode->next;
      if (currNode == NULL) {
        printf("ERROR: can't find expected idx %d as start value in allocations list \n", idx);
        return 1;
      } else {
        // hole or another alloc can follow alloc
        lookingFor = EITHER;
        idx = idx + currNode->size;
      }
    } else {// look for either one: must look in one list and then the other
      currNode = allocInfo->holes;
      while (currNode != NULL && currNode->start != idx)
        currNode = currNode->next;
      if (currNode != NULL) {
        // only alloc can follow hole
        lookingFor = ALLOC;
        idx = idx + currNode->size;
      } else {
        // didn't find in holes; look in allocs
        currNode = allocInfo->allocations;
        while (currNode != NULL && currNode->start != idx)
          currNode = currNode->next;
        if (currNode == NULL) {
          printf("ERROR: expected either a hole or another allocation starting at idx = %d\n", idx);
          return 1;
        } else {
          // hole or another alloc can follow alloc
          lookingFor = EITHER;
          idx = idx + currNode->size;
        }
      }
    }
  } // while we haven't gone all the way through the list

  // separate test: check for consecutive holes
  currNode = allocInfo->holes;
  while (currNode != NULL) {
    idx = currNode->start + currNode->size;
    if (idx < allocInfo->capacity) {
      holeNode = allocInfo->holes;
      found = 0;
      while ( ! found && holeNode != NULL) {
        if (holeNode->start == idx)
          found = 1;
        else
          holeNode = holeNode->next;
      }
      if (found) {
        printf("ERROR: found two adjacent holes: (%d, %d) and (%d, %d)\n",
               currNode->start, currNode->size, holeNode->start, holeNode->size);
        return 1;
      }
    }
    currNode = currNode->next;
  }
  // note: it's OK to have two adjacent allocations

  printf("validation check passed\n");
  return fail;
}

//--------------------------------------------------------------
// print a depiction of the current allocations and holes

void printAllocation(AllocInfo *info) {
  BlockListNode *node, *prev;
  int idx, i;
  int numUsed, numHoles;
  char symbol;
  int doStats;

  doStats = 1;

  if (doStats || info->capacity > 40) {
    numUsed = 0;
    numHoles = 0;
    if (info->allocations == NULL) {
      numHoles = 1;
    } else {
      prev = NULL;
      node = info->allocations;
      while (node != NULL) {
        numUsed = numUsed + node->size;
        if (prev == NULL) {
          if (node->start > 0)
            numHoles = 1;
        } else {
          if (node->start > prev->start + prev->size)
            numHoles = numHoles + 1;
        }
        prev = node;
        node = node->next;
      }

      if (prev->start + prev->size < info->capacity)
        numHoles = numHoles + 1;
    }
    printf("numUsed = %d; numHoles = %d\n", numUsed, numHoles);
    if (numHoles > 0) {
      printf("fraction in use = %.2f; mean hole size = %.2f\n",
             (float) numUsed / info->capacity,
             (float) (info->capacity - numUsed) / numHoles);
    } else {
      printf("fraction in use = %.2f\n", (float) numUsed / info->capacity);
    }
  }if (info->capacity <= 40) {
    printf("A: ");
    node = info->allocations;
    while (node != NULL) {
      printf("(%d, %d) ", node->start, node->size);
      node = node->next;
    }
    printf("\n");

    printf("H: ");
    node = info->holes;
    while (node != NULL) {
      printf("(%d, %d) ", node->start, node->size);
      node = node->next;
    }
    printf("\n");

    symbol = 'x';
    idx = 0;
    node = info->allocations;
    while (node != NULL) {
      for (i=idx; i<node->start; ++i)
        printf("- ");
      for (i=0; i<node->size; ++i)
        printf("%c ", symbol);
      idx = node->start + node->size;
      node = node->next;
      if (node != NULL) {
        if (idx == node->start)
          symbol = symbol == 'x' ? 'y' : 'x';
      }
    }

    for (i=idx; i<info->capacity; ++i)
      printf("- ");
    printf("\n");

    idx = 0;
    node = info->holes;
    while (node != NULL) {
      for (i=idx; i<node->start; ++i)
        printf("- ");
      for (i=0; i<node->size; ++i)
        printf("h ");
      idx = node->start + node->size;
      node = node->next;
    }
    for (i=idx; i<info->capacity; ++i)
      printf("- ");
    printf("\n");
  }
} // end of function printAlloc()