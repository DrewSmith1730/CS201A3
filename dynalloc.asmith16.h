#define CAPACITY 30

// this represents a single record -- either an allocation or
// a hole -- in the allocations list or the holes list
typedef struct BlockListNodeStruct {
    int size;
    int start;
    struct BlockListNodeStruct *next;
} BlockListNode;

// here are the two lists, along with the size of the "memory"
typedef struct {
    int capacity;
    BlockListNode *allocations;
    BlockListNode *holes;
} AllocInfo;

// and this determines how a hole for an allocation is selected,
// in Part Two
typedef enum AllocTypeEnum {
    ALLOC_BEST_FIT,
    ALLOC_WORST_FIT,
    ALLOC_FIRST_FIT
} AllocType;


void initializeAllocation(int capacity, AllocInfo *allocInfo);
int addAllocation(int start, int size, AllocInfo *allocInfo);
int deleteAllocation(int start, AllocInfo *allocInfo);
int addHole(int start, int size, AllocInfo *allocInfo);
int deleteHole(int start, AllocInfo *allocInfo);
int shrinkHole(int requestSize, BlockListNode *holeNode, AllocInfo *allocInfo);
int expandHole(int newSize, BlockListNode *holeNode, AllocInfo *allocInfo);

