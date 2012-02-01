#include <unistd.h>
#include <pthread.h>

#include <stdio.h>

#include "memory.h"

#define SECRET_CODE 42

typedef long Align;    /* for alignment to long boundary */

union header {         /* block header */
  struct {
    union header *ptr; /* next block if on free list */
    unsigned long size;     /* size of this block */
    unsigned char sCode;
  } s;
  Align x;             /* force alignment of blocks */
};

typedef union header Header;

static Header base;
static Header *freep = NULL;

static char heap[SIZE_ALLOC];

static pthread_mutex_t myLock;

void initMemory()
{
    if(freep == NULL)
    { 
        Header *up;
        base.s.ptr = freep = &base;
        base.s.size = 0;
        pthread_mutex_init (&myLock, NULL);

        up = (Header *) heap; 
        /*up->s.size = (SIZE_ALLOC+sizeof(Header)-1)/sizeof(Header) + 1;*/
        up->s.size = SIZE_ALLOC/sizeof(Header);
        up->s.sCode = SECRET_CODE;
        /*up->s.size = SIZE_ALLOC - sizeof(Header);*/
        gFree((void *)(up+1));   
        printf("Début : %p, Fin: %p\n", (void *)heap, (void *)(heap+SIZE_ALLOC));
    }
}

/* free: put block ap in free list */
void gFree(void *ap)
{
    Header *bp, *p;

    /* Test if memory is initialised */
    if(freep == NULL)
        return;

    pthread_mutex_lock(&myLock);
 
    /* Test if pointer is within our memory */
    if(ap == NULL
            || (void *)ap < (void *)heap
            || (void *)ap >= (void *)(heap + SIZE_ALLOC))
    {
        pthread_mutex_unlock(&myLock);
        return;
    }
    bp = (Header *)ap - 1;

    /* Test if it's a valid Header */
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
        /* join to upper nbr */
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

/* malloc: general-purpose storage allocator */
void *gMalloc(unsigned long nbytes)
{
    Header *p, *prevp;
    unsigned long nunits;

    /* No Free List Yet, Need to Initialise */
    if ((prevp = freep) == NULL || nbytes > SIZE_ALLOC) 
    {
        return NULL;
    }

    pthread_mutex_lock(&myLock);

    /*printf("nBytes: %d\n", nbytes);*/

    nunits = (nbytes+sizeof(Header)-1)/sizeof(Header) + 1;
    /*nunits = nbytes+sizeof(Header);*/
    for (p = prevp->s.ptr; ; prevp = p, p = p->s.ptr) 
    {
        /* Chunk is big enough */
        if (p->s.size >= nunits) 
        { 
            /* Chunk fits exactly */
            if (p->s.size == nunits)
            {
	            prevp->s.ptr = p->s.ptr;
            }
            /* Chunk is too big, Cut it */
            else 
            {
                /*printf("Taille: %d et Retire: %d\n", p->s.size, nunits);*/
	            /* allocate tail end */
            	p->s.size -= nunits;
                /*printf("New Taille: %d\n", p->s.size);*/
            	p += p->s.size;
            	p->s.size = nunits;
                p->s.sCode = SECRET_CODE;
            }
            freep = prevp;
            pthread_mutex_unlock(&myLock);
            return (void *)(p+1);
        }
        if (p == freep) /* wrapped around free list */
        {

            /* System is not dynamic... */
            /*if ((p = morecore(nunits)) == NULL)*/
            {
                pthread_mutex_unlock(&myLock);
                return NULL;
            }
        }
    }
}

void destroyMemory()
{
	freep = NULL;
	pthread_mutex_destroy(&myLock);
}

unsigned long getGMemTotal()
{
    return SIZE_ALLOC;
}

unsigned long getGMemFree()
{
    Header *p, *prevp;
    unsigned long sum;
    if(freep == NULL)
        return 0;

    pthread_mutex_lock(&myLock);
    prevp = freep;
    sum = 0;
    for (p = prevp->s.ptr; ; prevp = p, p = p->s.ptr) 
    {
        sum += p->s.size*sizeof(Header);
        if (p == freep) /* wrapped around free list */
        {
            break;
        }
    }
    pthread_mutex_unlock(&myLock);
    return sum;
}
