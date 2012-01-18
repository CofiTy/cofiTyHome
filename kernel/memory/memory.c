#include <unistd.h>
#include <pthread.h>

#include "memory.h"

typedef long Align;    /* for alignment to long boundary */

union header {         /* block header */
  struct {
    union header *ptr; /* next block if on free list */
    unsigned size;     /* size of this block */
  } s;
  Align x;             /* force alignment of blocks */
};

typedef union header Header;

static Header base;
static Header *freep = NULL;

static pthread_mutex_t myLock;

void initMemory()
{
    pthread_mutex_init (&myLock, NULL);
}

/* free: put block ap in free list */
void gFree(void *ap)
{
  Header *bp, *p;
  bp = (Header *)ap - 1;

  pthread_mutex_lock(&myLock);
  /* point to block header */
  for (p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
    if (p >= p->s.ptr && (bp > p || bp < p->s.ptr))
      break; /* freed block at start or end of arena */
  if (bp + bp->s.size == p->s.ptr) {
    /* join to upper nbr */
    bp->s.size += p->s.ptr->s.size;
    bp->s.ptr = p->s.ptr->s.ptr;
  } else
    bp->s.ptr = p->s.ptr;
  if (p + p->s.size == bp) {
    /* join to lower nbr */
    p->s.size += bp->s.size;
    p->s.ptr = bp->s.ptr;
  } else
    p->s.ptr = bp;
  freep = p;
  pthread_mutex_unlock(&myLock);
}

#define NALLOC 1024
/* minimum #units to request */
/* morecore: ask system for more memory */
static Header *morecore(unsigned nu)
{
  char *cp;
  Header *up;
  if (nu < NALLOC)
    nu = NALLOC;
  cp = sbrk(nu * sizeof(Header));
  if (cp == (char *) -1)
    /* no space at all */
    return NULL;
  up = (Header *) cp;
  up->s.size = nu;
  gFree((void *)(up+1));
  return freep;
}

/* malloc: general-purpose storage allocator */
void *gMalloc(unsigned nbytes)
{
  Header *p, *prevp;
  unsigned nunits;
  pthread_mutex_lock(&myLock);
  nunits = (nbytes+sizeof(Header)-1)/sizeof(Header) + 1;
  if ((prevp = freep) == NULL) {
    /* no free list yet */
    base.s.ptr = freep = prevp = &base;
    base.s.size = 0;
  }
  for (p = prevp->s.ptr; ; prevp = p, p = p->s.ptr) {
    if (p->s.size >= nunits) { /* big enough */
      if (p->s.size == nunits) /* exactly */
	prevp->s.ptr = p->s.ptr;
      else {
	/* allocate tail end */
	p->s.size -= nunits;
	p += p->s.size;
	p->s.size = nunits;
      }
      freep = prevp;
      return (void *)(p+1);
    }
    if (p == freep) /* wrapped around free list */
      if ((p = morecore(nunits)) == NULL)
    pthread_mutex_unlock(&myLock);
	return NULL;
    /* none left */
  }
}
