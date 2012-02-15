#include <unistd.h>
#include <pthread.h>

#include <stdio.h>

#include "memory.h"

/**
 * Secret Cookie to verify Chunks.
 */
#define SECRET_CODE 42

/**
 * Chunks are aligned on long.
 */
typedef long Align;

/**
 * Header for chunks.
 */
union header 
{
	struct 
	{
		/* Next Chunk if FreeList. */
		union header *ptr;
		/* Chunk Size (Data, without Header). */
		unsigned long size;
		/* Cookie to verify alignment. */
		unsigned char sCode;
	} s;
	/* Force Alignement... */
	Align x;
};

/**
 * Type definition for convenience.
 */
typedef union header Header;

/**
 * Base of all memory.
 */
static Header base;

/**
 * Free Chunk List.
 */
static Header *freep = NULL;

/**
 * All Memory!
 */
static char heap[SIZE_ALLOC];

/**
 * Mutex for concurrent Access.
 */
static pthread_mutex_t myLock;

/**
 * Initialise memory if first call.
 */
void initMemory()
{
	/* If First Call. */
    if(freep == NULL)
    { 
		/* Create a big fatty fat chunk with all memory. */
        Header *up;
        base.s.ptr = freep = &base;
        base.s.size = 0;
        pthread_mutex_init (&myLock, NULL);
        up = (Header *) heap;
        gFree((void *)(up+1));
    }
}

/**
 * Put a block in the free list (if valid).
 */
void gFree(void *ap)
{
    Header *bp, *p;

    /* Test if memory is initialised */
    if(freep == NULL)
        return;

	/* Protect the free list. */
    pthread_mutex_lock(&myLock);
 
    /* Test if pointer is within our memory */
    if(ap == NULL
            || (void *)ap < (void *)heap
            || (void *)ap >= (void *)(heap + SIZE_ALLOC))
    {
        pthread_mutex_unlock(&myLock);
        return;
    }
    
    /* Get the header for this chunk. */
    bp = (Header *)ap - 1;

    /* Test if it's a valid Header (Cookie Test Miam) */
    if(bp->s.sCode != SECRET_CODE)
    {
        pthread_mutex_unlock(&myLock);
        return;
    }

    /* We search the preceding free block in memory */
    for (p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
    {
		/* Freelist is circular, we break if were at the 
		 * beginning or end of freelist.
		 */
        if (p >= p->s.ptr && (bp > p || bp < p->s.ptr))
        {
             break;
        }
    }
    
    /* Try to merge with next block. */
    if (bp + bp->s.size == p->s.ptr) 
    {
        bp->s.size += p->s.ptr->s.size;
        bp->s.ptr = p->s.ptr->s.ptr;
    } 
    else
    {
        bp->s.ptr = p->s.ptr;
    }
    
    /* Try to merge with previous block */
    if (p + p->s.size == bp) 
    {
        p->s.size += bp->s.size;
        p->s.ptr = bp->s.ptr;
    } 
    else
    {
        p->s.ptr = bp;
    }
    freep = p;
    pthread_mutex_unlock(&myLock);
}

/**
 * Marvellous Memory allocator. 
 */
void *gMalloc(unsigned long nbytes)
{
    Header *p, *prevp;
    unsigned long nunits;

    /* No Free List Yet, Need to Initialise */
    if ((prevp = freep) == NULL || nbytes > SIZE_ALLOC) 
    {
        return NULL;
    }

	/* Protect free list. */
    pthread_mutex_lock(&myLock);

    /*printf("nBytes: %d\n", nbytes);*/

	/* Compute the real needed size (Aligment + header) */
    nunits = (nbytes+sizeof(Header)-1)/sizeof(Header) + 1;
    
    /* Iterate the free list (First Fit). */
    for (p = prevp->s.ptr; ; prevp = p, p = p->s.ptr) 
    {
        /* Big enough chunk. */
        if (p->s.size >= nunits) 
        { 
            /* Perfect Chunk. */
            if (p->s.size == nunits)
            {
	            prevp->s.ptr = p->s.ptr;
            }
            /* Too Big Chunk, Cut it! */
            else 
            {
	            /* Diminuce remaining chunk. */
            	p->s.size -= nunits;
            	p += p->s.size;
            	/* Initialise our chunk. */
            	p->s.size = nunits;
                p->s.sCode = SECRET_CODE;
            }
            freep = prevp;
            pthread_mutex_unlock(&myLock);
            return (void *)(p+1);
        }
        /* We iterate the whole list... */
        if (p == freep)
        {
			/* No memory big enough, Bye Bye! */
			pthread_mutex_unlock(&myLock);
            return NULL;
        }
    }
}

/**
 * Quite simple, just say "no free list -> No initialisation...".
 */
void destroyMemory()
{
	freep = NULL;
	pthread_mutex_destroy(&myLock);
}

/**
 * Simple. 
 */
unsigned long getGMemTotal()
{
    return SIZE_ALLOC;
}

/**
 * Iterate the free list and sum the free memory.
 */
unsigned long getGMemFree()
{
    Header *p, *prevp;
    unsigned long sum;
    
    /* Memory Not Initialised... */
    if(freep == NULL)
        return 0;

	/* Protect Free List. */
    pthread_mutex_lock(&myLock);
    prevp = freep;
    sum = 0;
    /* Iterate. */
    for (p = prevp->s.ptr; ; prevp = p, p = p->s.ptr) 
    {
        sum += p->s.size*sizeof(Header);
        
        /* No More Chunk. */
        if (p == freep)
            break;
    }
    pthread_mutex_unlock(&myLock);
    return sum;
}
