
#include <stdio.h>
#include "k_api.h"

extern uint8_t _iram_heap_start, _iram_heap_size;

/* Heap should align to IRAM_HEAP_ALIGNMENT */
#define IRAM_HEAP_BASE              ((uintptr_t)&_iram_heap_start)
/* Size should be smaller than ~IRAM_HEAP_MAGIC */
#define IRAM_HEAP_SIZE              ((uintptr_t)&_iram_heap_size)

#define ALIGN(x,a)                  (((x) + (a) - 1) & ~((a) - 1))
#define IRAM_HEAP_ALIGNMENT         4

/* At the beginning of allocated memory */
#define IRAM_HEAP_BLK_HEAD_SIZE     ALIGN(sizeof(ih_blk_head_t), IRAM_HEAP_ALIGNMENT)
/* Block sizes must not get too small. */
#define IRAM_HEAP_BLK_MIN	        ((IRAM_HEAP_BLK_HEAD_SIZE) << 1)

/* blk belong to APP, magic set
   blk belong to free list, magic clear */
#define IRAM_HEAP_MAGIC             (0xF0000000)

/* block head, before each blocks */
typedef struct ih_blklist
{
    /* free memory block list, order by address*/
	struct ih_blklist *next;
	/* when blk in APP: IRAM_HEAP_MAGIC & size
	   when blk in freelist: only size
	   size include ih_blk_head_t self */
	size_t magic_size;
} ih_blk_head_t;

/* free block list: order by address, from low to high */
/* first free block. */
static ih_blk_head_t s_freelist_head;
/* point to last free block. */
static ih_blk_head_t *s_freelist_tail;

/* For statistic. */
static size_t s_heap_free_size      = IRAM_HEAP_SIZE;
static size_t s_heap_free_size_min  = IRAM_HEAP_SIZE;

/* Init heap, with one free block */
static void iram_heap_init(void)
{
    ih_blk_head_t *free_blk;

    if (  IRAM_HEAP_BASE%IRAM_HEAP_ALIGNMENT != 0
       || IRAM_HEAP_SIZE > (~IRAM_HEAP_MAGIC)
       || IRAM_HEAP_SIZE < IRAM_HEAP_BLK_MIN*2 )
    {
        printf("[iram_heap] fatal error: heap parameter invalid!\n");
        return;
    }

	s_freelist_head.next = (void *) IRAM_HEAP_BASE;
	s_freelist_head.magic_size = (size_t) 0;

	s_freelist_tail = (void *)(IRAM_HEAP_BASE + IRAM_HEAP_SIZE - IRAM_HEAP_BLK_HEAD_SIZE);
	s_freelist_tail->next = NULL;
	s_freelist_tail->magic_size = 0;

    /* Only one block when init. */
	free_blk = (void *) IRAM_HEAP_BASE;
	free_blk->next = s_freelist_tail;
	free_blk->magic_size = (size_t)s_freelist_tail - (size_t) free_blk;

	s_heap_free_size     = free_blk->magic_size;
	s_heap_free_size_min = free_blk->magic_size;
}

static void iram_heap_freeblk_insert(ih_blk_head_t *blk_insert)
{
    ih_blk_head_t *blk_before; /* before the blk_insert */
    ih_blk_head_t *blk_after;  /* after  the blk_insert */

    /* freelist is ordered by address, find blk_before */
    for (blk_before = &s_freelist_head; blk_before->next < blk_insert; blk_before = blk_before->next)
    {
        if (blk_before == s_freelist_tail)
        {
            return;
        }
    }
    blk_after = blk_before->next;

    /* now: blk_before < blk_insert < blk_after */

    /* try to merge blk_before and blk_insert */
	if ((char *)blk_before + blk_before->magic_size == (char *)blk_insert)
    {
    	blk_before->magic_size += blk_insert->magic_size;
    	blk_insert = blk_before;
    }
    else
    {
        /* do not merge, just insert new node to freelist */
    	blk_before->next = blk_insert;
    }

    /* try to merge blk_insert and blk_after */
	if (blk_after != s_freelist_tail &&
	    (char *)blk_insert + blk_insert->magic_size == (char *)blk_after)
    {
    	blk_insert->magic_size += blk_after->magic_size;
    	blk_insert->next = blk_after->next;
    }
	else
    {
        /* do not merge, just insert new node to freelist */
    	blk_insert->next = blk_after;
    }
}

void *iram_heap_malloc(size_t alloc_size)
{
    ih_blk_head_t *blk_alloc;
    ih_blk_head_t *blk_prev;
    ih_blk_head_t *blk_left;

	krhino_sched_disable();

    /* first call to malloc, init is needed */
	if (s_freelist_tail == NULL)
    {
    	iram_heap_init();
    }

	if  ((alloc_size == 0)
      || (alloc_size > s_heap_free_size))
    {
        /* not enough memory */
        krhino_sched_enable();
        return NULL;
    }

	alloc_size += IRAM_HEAP_BLK_HEAD_SIZE;
    alloc_size  = ALIGN(alloc_size, IRAM_HEAP_ALIGNMENT);

    /* find a free block bigger than alloc_size */
	blk_prev = &s_freelist_head;
	blk_alloc = s_freelist_head.next;
	while ((blk_alloc->magic_size < alloc_size) && (blk_alloc->next != NULL))
    {
    	blk_prev = blk_alloc;
    	blk_alloc = blk_alloc->next;
    }
	if (blk_alloc->next == NULL)
    {
        /* this means "blk_alloc == s_freelist_tail" */
        krhino_sched_enable();
        return NULL;
    }

    /* delete blk_alloc from freelist */
	blk_prev->next = blk_alloc->next;

    /* check whether blk_alloc can split */
	if (blk_alloc->magic_size - alloc_size > IRAM_HEAP_BLK_MIN)
    {
        /* split */
        blk_left = (void *)((char *)blk_alloc + alloc_size);
    	blk_left->magic_size = blk_alloc->magic_size - alloc_size;

    	blk_alloc->magic_size = alloc_size;

        /* Insert the new block into the list of free blocks. */
    	iram_heap_freeblk_insert(blk_left);
    }

    /* update statistic */
	s_heap_free_size -= blk_alloc->magic_size;
	if (s_heap_free_size < s_heap_free_size_min)
    {
    	s_heap_free_size_min = s_heap_free_size;
    }

    krhino_sched_enable();

    /* blk belong to APP, magic set */
	blk_alloc->magic_size |= IRAM_HEAP_MAGIC;
	blk_alloc->next = NULL;

    /* app used addr is after ih_blk_head_t */
	return (void *)((char *)blk_alloc + IRAM_HEAP_BLK_HEAD_SIZE);
}

void iram_heap_free(void *pfree)
{
    ih_blk_head_t *free_blk;

	if (pfree == NULL)
    {
        return;
    }

    /* app used addr is after ih_blk_head_t */
	free_blk = (ih_blk_head_t *)((char *)pfree - IRAM_HEAP_BLK_HEAD_SIZE);

    /* blk damaged check */
    if (free_blk->next != NULL
      ||(free_blk->magic_size & IRAM_HEAP_MAGIC) != IRAM_HEAP_MAGIC)
    {
        printf("[iram_heap] fatal error: block has been destroyed!\n");
        return;
    }

    /* blk belong to free list, magic clear */
	free_blk->magic_size &= ~IRAM_HEAP_MAGIC;
    /* update statistic */
	s_heap_free_size += free_blk->magic_size;

	krhino_sched_disable();

	iram_heap_freeblk_insert(free_blk);

    krhino_sched_enable();
}

size_t iram_heap_free_size(void)
{
	return s_heap_free_size;
}

size_t iram_heap_free_size_min(void)
{
	return s_heap_free_size_min;
}

/* if addr is in iram_heap, return 1
   else return 0*/
int iram_heap_check_addr(void* addr)
{
    if ((size_t)addr - IRAM_HEAP_BASE < IRAM_HEAP_SIZE)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

