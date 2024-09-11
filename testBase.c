#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include "dynalloc.asmith16.h"

#define CAPACITY 30

// if you are using sorted lists, then set the following to 1
const int sortedLists = 0;

int testBaseOne() {
  AllocInfo allocInfo;
  int sts, fail;

  printf("------------------------------------------------\n");
  printf("this is testBaseOne(), with capacity = %d\n", CAPACITY);
  printf("------------------------------------------------\n");

  fail = 0;

  initializeAllocation(CAPACITY, &allocInfo);

// (0, 3), (6, 6), (14, 5) <= allocations
// (3, 3), (12, 2), (19, 11) <= holes

  printAllocation(&allocInfo);
  sts = validateAllocation(&allocInfo, sortedLists);
  if (sts != 0) {
    printf("ERROR: testBaseOne() failed first validateAllocation()\n");
    fail = 1;
  }
  printf("\n");

  deleteHole(0, &allocInfo);
  addAllocation(6, 6, &allocInfo);
  addAllocation(0, 3, &allocInfo);
  addAllocation(14, 5, &allocInfo);
  addAllocation(19, 4, &allocInfo);
  addAllocation(23, 1, &allocInfo);
  addHole(3, 3, &allocInfo);
  addHole(12, 2, &allocInfo);
  addHole(24, 6, &allocInfo);
  printAllocation(&allocInfo);
  sts = validateAllocation(&allocInfo, sortedLists);
  if (sts != 0) {
    printf("ERROR: testBaseOne() failed second validateAllocation()\n");
    fail = 1;
  }

  if ( ! fail )
    printf("testBaseOne() passed\n");
  printf("\n");

  return fail;
}
//--------------------------------------------------------------

int testBaseTwo() {
  AllocInfo allocInfo;
  BlockListNode *currNode;
  int fail, sts;

  printf("------------------------------------------------\n");
  printf("this is testBaseTwo(), with capacity = %d\n", CAPACITY);
  printf("------------------------------------------------\n");

  initializeAllocation(CAPACITY, &allocInfo);

  // (5, 3), (12, 2), (17, 1), (20, 4), (25, 2) <= allocations
  // (0, 5), (8, 4), (14, 3), (18, 2), (24, 1), (27, 3) <= holes

  fail = 0;

  addAllocation(5, 3, &allocInfo);
  addAllocation(12, 2, &allocInfo);
  addAllocation(17, 1, &allocInfo);
  addAllocation(20, 4, &allocInfo);
  addAllocation(25, 2, &allocInfo);
  deleteHole(0, &allocInfo);
  addHole(0, 5, &allocInfo);
  addHole(8, 4, &allocInfo);
  addHole(14, 3, &allocInfo);
  addHole(18, 2, &allocInfo);
  addHole(24, 1, &allocInfo);
  addHole(27, 3, &allocInfo);

  printAllocation(&allocInfo);
  sts = validateAllocation(&allocInfo, sortedLists);
  if (sts != 0) {
    printf("ERROR: testBaseTwo() failed first validateAllocation()\n");
    fail = 1;
  }

  printf("\n");
  // (5, 3), (12, 2), (17, 1), (20, 4), (25, 2) <= allocations
  // (0, 5), (8, 4), (14, 3), (18, 2), (24, 1), (27, 3) <= holes

  // now delete the (25, 2) allocation and the (27, 3) hole
  // and expand the (24, 1) hole to be of size 6, giving this:
  // (5, 3), (12, 2), (17, 1), (20, 4) <= allocations
  // (0, 5), (8, 4), (14, 3), (18, 2), (24, 6) <= holes

  deleteAllocation(25, &allocInfo);
  deleteHole(27, &allocInfo);
  currNode = allocInfo.holes;
  while (currNode != NULL && currNode->start != 24)
    currNode = currNode->next;
  if (currNode == NULL) {
    printf("ERROR: testBaseThree failed: could not find record in holes list with start = 24\n");
    return 1;
  }
expandHole(6, currNode, &allocInfo);

  printAllocation(&allocInfo);
  sts = validateAllocation(&allocInfo, sortedLists);
  if (sts != 0) {
    printf("ERROR: testBaseTwo() failed second validateAllocation()\n");
    fail = 1;
  }

  if ( ! fail )
    printf("testBaseTwo() passed\n");

  printf("\n");
  return fail;
}

//--------------------------------------------------------------

int testBaseThree() {
  AllocInfo allocInfo;
  BlockListNode *currNode;
  int sts;

  printf("------------------------------------------------\n");
  printf("this is testBaseThree(), with capacity = %d\n", CAPACITY);
  printf("------------------------------------------------\n");

  initializeAllocation(CAPACITY, &allocInfo);

  // (0, 10) <= allocation
  // (10, 20) <= holes

  currNode = allocInfo.holes;
  while (currNode != NULL && currNode->start != 0)
    currNode = currNode->next;
  if (currNode == NULL) {
    printf("ERROR: testBaseThree failed: could not find record in holes list with start = 0\n");
    return 1;
  }

  shrinkHole(10, currNode, &allocInfo);

  addAllocation(0, 10, &allocInfo);

  printAllocation(&allocInfo);
  sts = validateAllocation(&allocInfo, sortedLists);
  if (sts != 0)
    printf("ERROR: testBaseThree() failed validateAllocation()\n");
  else
    printf("testBaseThree() passed\n");

  printf("\n");
  return sts;
}