#include <unistd.h>
#include <pthread.h>

#include <stdio.h>

#include "memory.h"

typedef long Align;    /* for alignment to long boundary */

union header {         /* block header */
  struct {
    union header *ptr; /* next block if on free list */
    unsigned int size;     /* size of this block */
  } s;
  Align x;             /* force alignment of blocks */
};

typedef union header Header;

static Header base;
static Header *freep = NULL;

static char heap[SIZE_ALLOC];

static pthread_mutex_t myLock;

/* free: put block ap in free list */
void gFree(void *ap)
{
    Header *bp, *p;
    bp = (Header *)ap - 1;

    pthread_mutex_lock(&myLock);
    /* point to block header */
    for (p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
    {
        if (p >= p->s.ptr && (bp > p || bp < p->s.ptr))
        {
             break; /* freed block at start or end of arena */
        }
    }
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
    if (p + p->s.size == bp) 
    {
        /* join to lower nbr */
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
void *gMalloc(unsigned nbytes)
{
    Header *p, *prevp;
    unsigned nunits;

    /* No Free List Yet, Need to Initialise */
    if ((prevp = freep) == NULL) 
    {  
        Header *up;
        base.s.ptr = freep = prevp = &base;
        base.s.size = 0;
        pthread_mutex_init (&myLock, NULL);

        up = (Header *) heap; 
        /*up->s.size = (SIZE_ALLOC+sizeof(Header)-1)/sizeof(Header) + 1;*/
        up->s.size = SIZE_ALLOC/sizeof(Header);
        /*up->s.size = SIZE_ALLOC - sizeof(Header);*/
        gFree((void *)(up+1));
        prevp = freep;
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
            }
            freep = prevp;
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

int getGMemTotal()
{
    return SIZE_ALLOC;
}

int getGMemFree()
{
    Header *p, *prevp;
    int sum = 0;
    pthread_mutex_lock(&myLock);
    prevp = freep;
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
