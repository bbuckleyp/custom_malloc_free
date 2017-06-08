// INCLUDES
#include "psumemory.h"


// Global Variables
int algoType;        // memory allocation algorithm strategy
__nodeFree *head;    // head node pointer to free list
//int tot_mem_free;    // keeps track of the total memory in freeList
//int not_enough_mem;  // keeps track of times there was not enought memory in malloc
//int internal_frag;   // keeps track of number of times there was internal fragmentation

//int main( void ) {
//	void *p1, *p2, *p3, *p4, *p5;
//
// 	if( !psumeminit( 1, 4090 ) ) {
//
//		psumemdump(); 
//
//		// malloc
//		p1 = psumalloc( 100 );
//		p2 = psumalloc( 256 );
//		p3 = psumalloc(   8 );
//		p4 = psumalloc( 115 );
//		p5 = psumalloc(  36 );
//
//		psumemdump(); 
//
//		//free
//		psufree(p1);
//		psufree(p2);
//		psufree(p3);
//		psufree(p4);
//		psufree(p5);
//
//		// printing freelist
//		psumemdump(); 
//	} else {
//		return -1;
//	}
//	return 0;
//}


//*******************************************************
// Description: Prints information about FreeList
//*******************************************************
void psumemdump( void ) {
	// Declaring / Initializing Variables
	__nodeFree *currNode = head; // pTemp nodeFree
	int i = 1;
	
	// Stepping through the free list
	do {
		printf("%s%d%s%p\n", "Node: ", i, " at address: ", currNode); 
		printf("\t%s%p\n", "next    = ", currNode->next);
		printf("\t%s%p\n", "prev    = ", currNode->prev);
		printf("\t%s%d\n", "memFree = ", currNode->memFree);
		currNode = currNode->next;
		i++;
	} while( currNode != NULL );

	//printf("%s%d\n", "The number of internal fragmentations: ", internal_frag );
	//printf("%s%d\n", "The number of not enought memory: ", not_enough_mem );
} 

//*******************************************************
// Description: Implementation of malloc()
// Input: size - number of bytes to allocate
// Output: void* - pointer to the beginning of the object
// 		   NULL - not enought contiguous space within
// 		          sizeOfRegion in psumeminit
//*******************************************************
void* psumalloc( int size ) {
	// Declaring/Initializing Variables 
	int memSize;                            // used to find the best/worst fit
	void* pTemp;
	__nodeFree* currNode = NULL;            // pTemporary node
	__nodeFree* nodeSel = NULL;             // pointer to the node selected (best/worst fit) 
	_malloc_h* malloc_h = NULL;             // pointer to malloc header
	int newSize = size + sizeof(_malloc_h); // size + _malloc_h

	// Verifying the FreeList has an entry
	if( head != NULL && size > 0 ) {
		currNode = head; // assigning node to head of FreeList to start search

		// BEST_FIT Implementation
		if( algoType == BEST_FIT ) {
			memSize = 9999999; // setting sentinal value

			// Searching through free list nodes to find "Best Fit"
			do {
				// checking if previous memSize is greater than the current memFree - newSize
				if( ( memSize > (currNode->memFree - newSize) ) && ( (currNode->memFree - newSize) >= 0 ) ) {
					memSize = currNode->memFree - newSize; // updating memFree
					nodeSel = currNode; // saving node w/ addr returning
				}
				currNode = currNode->next; // moving to the next node
			} while( currNode != NULL );
		} else {
		// WORST_FIT Implementation
			memSize = -9999999; // setting sentinal value

			// Searching through free list nodes to find "Worst Fit"
			do {
				// checking if previous memSize is greater than the current node memFree - newSize
				if( ( memSize < (currNode->memFree - newSize) ) && ( (currNode->memFree - newSize) >= 0 ) ) {
					memSize = currNode->memFree - newSize; // updating memFree
					nodeSel = currNode; // saving node w/ addr returning
				}
				currNode = currNode->next; // moving to the next node
			} while( currNode != NULL );
		}

		// Positioning pointers
		if( nodeSel != NULL ) {
			// setting up pointers
			currNode = nodeSel; // pointing current node to the old space in memory to copy variables over
			pTemp = (char*)nodeSel;
			pTemp += newSize;
			nodeSel = (__nodeFree*)pTemp;

			
			// Moving FreeList node selected down
			nodeSel->memFree = memSize; 
			
			// positioning pointers
			// settting nodeSel->prev pointer
			if( currNode->prev != NULL ) {
				nodeSel->prev = currNode->prev;
				currNode->prev->next = nodeSel;
				currNode->prev = NULL;
			} else {
				nodeSel->prev = NULL;
				head = nodeSel;
			}
			// setting nodeSel->next pointer
			if( currNode->next != NULL ) {
				nodeSel->next = currNode->next;
				currNode->next->prev = nodeSel;
				currNode->next = NULL;
			} else {
				nodeSel->next = NULL;
			}

			// set malloc header and return address of newly allocated memory
			malloc_h = (_malloc_h*)currNode;
			malloc_h->size = size;
			malloc_h->magic = MAGIC;

			return (char*)currNode + sizeof(_malloc_h);
		} else {
		// no node found
			
			//// checking if internal fragmentation or insufficient memory
			//currNode = head;
			//tot_mem_free = 0;
			//do {
			//	tot_mem_free += currNode->memFree;
			//	currNode = currNode->next;
			//} while( currNode != NULL );

			//if( tot_mem_free >= newSize ) {
			//	internal_frag++;
			//} else {
			//	not_enough_mem++;
			//}

			return NULL;
		}
	} else {
		// FreeList is empty
		return NULL;
	}
}

//*******************************************************
// Description: Implementation of free()
// Input: size - number of bytes to allocate
// Output: void* - pointer to the beginning of the object
// 		   NULL - not enought contiguous space within
// 		          sizeOfRegion in psumeminit
//*******************************************************
int psufree( void* ptr ) {
	// Declaring / Initializing Variables
	__nodeFree* currNode = NULL; // current node traversing through list
	__nodeFree* newNode = NULL;  // new node to add to the freelist
	int memSize; 		         // variable to hold size of the malloced memory
	void* coalPtr;               // pointer used to check if neighboring nodes can be coalesced

	if( ptr == NULL ) {
		return -1;
	} else {
		// checking if pointer is valid
		ptr = (int*)ptr - 1;
		if( *(int*)ptr == MAGIC ) {
			ptr = (int*)ptr - 1; // moving ptr to the top of the header
			memSize = *(int*)ptr;
			 
			// Insert into FreeList in correct order
			currNode = head;
			do {
				if( (void*)currNode > ptr ) {
					// Establishing new free node
					newNode = (__nodeFree*)ptr;
					newNode->next = currNode;
					newNode->memFree = memSize + sizeof(_malloc_h) - sizeof(__nodeFree);
					if( currNode->prev != NULL ) {
						newNode->prev = currNode->prev;
						currNode->prev->next = newNode;
						currNode->prev = newNode;
					} else {
					// currNode is FreeList.head
						newNode->prev = NULL;
						currNode->prev = newNode;
						head = newNode;
					}

					break; // node inserted, break
				} else {
					currNode = currNode->next;
				}
			} while( currNode != NULL );

			// Checking if currNode < ptr (must append to end of freelist)
			if( (void*)currNode < ptr ) {
				// Establishing new free node
				newNode = (__nodeFree*)ptr;
				newNode->next = NULL;
				newNode->prev = currNode;
				currNode->next = newNode;
				newNode->memFree = memSize + sizeof(_malloc_h) - sizeof(__nodeFree);
			}

			// Coalescing above
			if( newNode->prev != NULL ) {
				coalPtr = (void*)newNode->prev;
				coalPtr = (char*)coalPtr + newNode->prev->memFree + sizeof(__nodeFree);
				
				if( coalPtr == (void*)newNode ) {
					newNode->prev->memFree = newNode->memFree + newNode->prev->memFree + sizeof(__nodeFree);
					// moving node pointers
					if( newNode->next != NULL ) {
						newNode->prev->next = newNode->next;
						newNode->next->prev = newNode->prev;
						currNode = newNode; // setting to new node to clear pointers
						newNode = newNode->prev;
						currNode->next = NULL;
						currNode->prev = NULL;
					} else {
						// End of FreeList
						currNode = newNode; // setting to new node to clear pointers
						newNode->prev->next = NULL;
						newNode = newNode->prev;
						currNode->next = NULL;
						currNode->prev = NULL;
					}
				}
			}

			// Coalescing below
			if( newNode->next != NULL ) {
				coalPtr = (void*)newNode;
				coalPtr = (char*)coalPtr + sizeof(__nodeFree) + newNode->memFree;

				if( coalPtr == (void*)newNode->next ) {
					newNode->memFree = newNode->memFree + newNode->next->memFree + sizeof(__nodeFree);
					if( newNode->next->next != NULL ) {
						newNode->next->next->prev = newNode;
						currNode = newNode->next; // coalescing with this node, clearing pointers
						newNode->next = newNode->next->next;
						currNode->next = NULL;
						currNode->prev = NULL;
					} else {
						currNode = newNode->next; // clearing pointers at node
						newNode->next = NULL;
						currNode->prev = NULL;
					}
				}
			}
			return 0; // success
		} else {
		// not a valid pointer address to allocated memory
			return -1; 
		}
	}
}

//*******************************************************
// Description: This function allocates memory.
// Input: algo - memory allocation strategy
// 	      sizeOfRegion - number of bytes
// Output: 0 - success
// 		  -1 - failure
//*******************************************************
int psumeminit( int algo, int sizeOfRegion ) {
	// Declaring/Initializing Variables
	int pageSize = getpagesize(); // extracting system's page size
	algoType = algo;              // saving data globally

	// Determining sizeOfRegion - must be proportional to pageSize
	if( sizeOfRegion % pageSize != 0 ) { 
		sizeOfRegion = ( ( sizeOfRegion / pageSize ) + 1 ) * pageSize;
	}
	
	// Allocating memory and adding FreeList node
	head = (__nodeFree*)mmap( NULL, sizeOfRegion, PROT_READ|PROT_WRITE, MAP_ANON|MAP_PRIVATE, -1, 0 );
	head->next = NULL;
	head->prev = NULL;
	head->memFree = sizeOfRegion - sizeof(__nodeFree);

	// Returning success/failure
	if( head == (void*)-1) {
		return -1; //failure
	} else {
		return 0;  //success
	}
}
