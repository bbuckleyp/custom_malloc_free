/* INCLUDES */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>

#include<time.h>

/* DEFINES */
#define BEST_FIT 0
#define WORST_FIT 1
#define MAGIC 1234567

/* Function Protoypes */
int psumeminit( int, int );
void psumemdump( void );
void* psumalloc( int );
int psufree( void* );

/* Data Structures */
// Free List Node
typedef struct __nodeFree {
	struct __nodeFree *next;
	struct __nodeFree *prev;
	int memFree; 
} __nodeFree;

// Malloc Header
typedef struct _malloc_h {
	int size;
	int magic;
} _malloc_h;
