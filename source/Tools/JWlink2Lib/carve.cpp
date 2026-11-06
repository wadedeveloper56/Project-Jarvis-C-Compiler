#include "pch.h"
#include "fileio.h"
#include "carve.h"

#define MK_INDEX( b, o )        (((b)<<16)|(o))
#define GET_BLOCK( i )          (((i)>>16)&0x0ffff)
#define GET_OFFSET( i )         ((i)&0x0ffff)

struct blk {
    blk_t *     next;
    unsigned    index;
    unsigned    modified : 1;
    unsigned    : 15;
    char        data[1];
};

struct free_t {
    free_t *    next_free;
};

#define _REMOVE_FROM_FREE( pcv, p ) \
    { \
        free_t *head = pcv->free_list; \
        pcv->free_list = head->next_free; \
        p = head; \
    }

#define _ADD_TO_FREE( fl, p ) \
    { \
        free_t *node = (free_t *) (p); \
        node->next_free = (fl); \
        (fl) = node; \
    }


static blk_t * newBlk( cv_t *cv )
{
    blk_t *     newblk;
    blk_t **    blklist;

    newblk = (blk_t*)malloc(sizeof(blk_t) - 1 + cv->blk_size);
    //_ChkAlloc( blk_t *, newblk, sizeof( blk_t ) - 1 + cv->blk_size );
    blklist = &cv->blk_list;
    while( *blklist > newblk ) {              
        blklist = &(*blklist)->next;      
    }
    newblk->next = *blklist;
    *blklist = newblk;
    cv->blk_count++;
    cv->size_chg = true;
    return newblk;
}

static void MakeFreeList( cv_t *cv, blk_t *newblk, unsigned offset )
{
    size_t      elm_size;
    char *      top_elm;
    char *      bottom_elm;
    char *      free_elm;
    free_t *    free_list;

    elm_size = cv->elm_size;
    bottom_elm = newblk->data + offset;
    top_elm = newblk->data + cv->blk_top;
    free_list = cv->free_list;
    free_elm = top_elm;
    do {                                
        free_elm -= elm_size;
        //FIXME DbgZapFreed( free_elm, elm_size );
        _ADD_TO_FREE( free_list, free_elm );
    } while( free_elm != bottom_elm );
    cv->free_list = free_list;
}

carve_t CarveCreate( size_t elm_size, size_t blk_size )
{
    cv_t *      cv;

    elm_size = ( elm_size + (sizeof(int)-1) ) & ~(sizeof(int)-1);
    if( elm_size < sizeof( free_t ) ) {
        elm_size = sizeof( free_t );
    }
	cv = (cv_t*)malloc(sizeof(*cv));
    //_ChkAlloc( cv_t *, cv, sizeof( *cv ) );
    cv->elm_size = elm_size;
    cv->blk_size = blk_size;
    cv->elm_count = cv->blk_size / cv->elm_size;
    cv->blk_top = cv->elm_count * elm_size;
    cv->blk_count = 0;
    cv->blk_list = NULL;
    cv->free_list = NULL;
    cv->blk_map = NULL;
    cv->size_chg = false;
    //FIXME DbgAssert( cv->elm_size >= 2 * sizeof(void *) );
    //FIXME DbgAssert( cv->elm_count != 0 );
    //FIXME DbgVerify( cv->blk_top < 0x10000, "carve: size * #/block > 64k" );
    return( cv );
}

#ifndef NDEBUG
void CarveVerifyAllGone( carve_t cv, char *node_name )
{
    free_t *    check;
    blk_t *     block;
    char *      compare;
    char        buff[80];
    bool        some_unfreed;

    some_unfreed = false;
    for( block = cv->blk_list; block != NULL; block = block->next ) {
        compare = block->data + cv->blk_top;
        do {
            compare -= cv->elm_size;
            for( check = cv->free_list; check != NULL; check = check->next_free ) {
                if( compare == (void *) check ) break;
            }
            if( check == NULL ) {
                if( ! some_unfreed ) {
                    //FIXME FmtStr( buff, 80, "carve %s unfreed:", node_name );
                    //FIXME WriteStdOut( buff );
                    some_unfreed = true;
                }
                //FIXME FmtStr( buff, 80, " %h", compare );
                //FIXME WriteStdOut( buff );
            }
        } while( compare != block->data );
    }
    if( some_unfreed ) {
        //FIXME WriteNLStdOut();
    }
}
#endif

void CarveDestroy( carve_t cv )
{
    blk_t *cur;
    blk_t *next;

    if( cv != NULL ) {
        if( cv->blk_map != NULL ) {
            free( cv->blk_map );
        }
        cur = cv->blk_list;
        while( cur != NULL ) {
            next = cur->next;
            free( cur );
            cur = next;
        }
        free( cv );
    }
}

void *CarveAlloc( carve_t cv )
{
    void *      p;

    if( cv->free_list == NULL ) {
        MakeFreeList( cv, newBlk( cv ), 0 );
    }
    _REMOVE_FROM_FREE( cv, p );
    //FIXME DbgZapAlloc( p, cv->elm_size );
    return p;
}

void *CarveZeroAlloc( carve_t cv )
{
    void *v;
    unsigned *p;
    int i;

    if( cv->free_list == NULL ) {
        MakeFreeList( cv, newBlk( cv ), 0 );
    }
    _REMOVE_FROM_FREE( cv, v );
    p = (unsigned *)v;
#if 0 
    if ( (cv->elm_size / sizeof(*p)) > 16 )
        DEBUG(( DBG_OLD, "CarveZeroAlloc: elm_size=%h sizeof(*p)=%h", cv->elm_size, sizeof(*p) ));
#endif
#if 0
    DbgAssert( ( cv->elm_size / sizeof(*p) ) <= 16 );
    switch( cv->elm_size / sizeof(*p) ) {
    case 16:
        p[15] = 0;
    case 15:
        p[14] = 0;
    case 14:
        p[13] = 0;
    case 13:
        p[12] = 0;
    case 12:
        p[11] = 0;
    case 11:
        p[10] = 0;
    case 10:
        p[9] = 0;
    case 9:
        p[8] = 0;
    case 8:
        p[7] = 0;
    case 7:
        p[6] = 0;
    case 6:
        p[5] = 0;
    case 5:
        p[4] = 0;
    case 4:
        p[3] = 0;
    case 3:
        p[2] = 0;
    case 2:
        p[1] = 0;
    case 1:
        p[0] = 0;
    }
#else
    for ( i = (cv->elm_size / sizeof(*p)) - 1; i >= 0; i-- )
        p[i] = 0;
#endif
    return p;
}


#ifndef NDEBUG
void CarveDebugFree( carve_t cv, void *elm )
{
    free_t *check;
    blk_t *block;
    char *compare;
    char *start;
    size_t esize;

    for( check = cv->free_list; check != NULL; check = check->next_free ) {
        if( elm == (void *) check ) {
            //FIXME LnkFatal( "carve: freed object was previously freed" );
        }
    }
    for( block = cv->blk_list; block != NULL; block = block->next ) {
        start = block->data;
        compare = start + cv->blk_top;
#if ! ( defined(__COMPACT__) || defined(__LARGE__) )
        if( elm < (void *)start || elm > (void *)compare ) {
            continue;
        }
#endif
        esize = cv->elm_size;
        for(;;) {
            if( compare == start ) break;
            compare -= esize;
            if( elm == compare ) break;
        }
        if( elm == compare ) break;
    }
    if( block == NULL ) {
        //FIXME LnkFatal( "carve: freed object was never allocated" );
    }
    //FIXME DbgZapFreed( elm, cv->elm_size );
}
#else
#define CarveDebugFree( cv, elm )
#endif

void CarveFree( carve_t cv, void *elm )
{
    if( elm == NULL ) {
        return;
    }
    CarveDebugFree( cv, elm );
    _ADD_TO_FREE( cv->free_list, elm );
}

void *CarveGetIndex( carve_t cv, void *elm )
{
    blk_t *     block;
    unsigned    block_index;

    if( elm == NULL ) {
        return( (void *) CARVE_NULL_INDEX );
    }
    block_index = cv->blk_count;
    block = cv->blk_list;
    while( elm < (void *)block ) {
        --block_index;
        block = block->next;
    }
    //FIXME DbgAssert( block != NULL );
    return (void *) MK_INDEX( block_index, ((char *)elm) - block->data );
}

void CarveWalkBlocks( carve_t cv, void (*cbfn)(carve_t, void *, void *),
                      void * cookie )
{
    blk_t *     block;

    block = cv->blk_list;
    while( block != NULL ) {
        cbfn( cv, block, cookie );
        block = block->next;
    }
}

bool CarveBlockModified( void *blk )
{
    return ((blk_t *)blk)->modified;
}

void CarveBlockScan( carve_t cv, void *blk, void (*rtn)(void *, void *),
                     void *data )
{
    char *      compare;
    char *      end;
    size_t      esize;

    esize = cv->elm_size;
    compare = ((blk_t *)blk)->data;
    end = compare + cv->blk_top;
    do {
        (*rtn)( compare, data );
        compare += esize;
    } while( compare != end );
}

unsigned CarveBlockSize( carve_t cv )
{
    return cv->blk_size;
}

void * CarveBlockData( void *block )
{
    return ((blk_t *)block)->data;
}

bool CarveSizeChanged( carve_t cv )
{
    return cv->size_chg;
}

unsigned CarveNumElements( carve_t cv )
{
    return cv->blk_count * cv->elm_count;
}

void CarveWalkAllFree( carve_t cv, void (*rtn)( void * ) )
{
    free_t *check;

    for( check = cv->free_list; check != NULL; check = check->next_free ) {
#ifndef NDEBUG
        free_t *check_next = check->next_free;
        (*rtn)( check );
        if( check->next_free != check_next ) {
            //FIXME LnkFatal( "carve walk free routine destroyed links" );
        }
#else
        (*rtn)( check );
#endif
    }
}

void CarveWalkAll( carve_t cv, void (*rtn)( void *, void * ), void *data )
{
    blk_t *     block;

    for( block = cv->blk_list; block != NULL; block = block->next ) {
        CarveBlockScan( cv, block, rtn, data );
    }
}

void CarveRestart( carve_t cv, unsigned num )
{
    unsigned    numblks;
    unsigned    remainder;
    unsigned    index;
    blk_t *     block;

    if( num == 0 ) return;
    numblks = (num + cv->elm_count - 1) / cv->elm_count;
    for( index = 0; index < numblks; index++ ) {
        newBlk( cv );
    }
	cv->blk_map = (blk_t**)malloc(numblks * sizeof(blk_t*));
    //_ChkAlloc( blk_t **, cv->blk_map, numblks * sizeof( blk_t * ) );
    index = numblks - 1;
    for( block = cv->blk_list; block != NULL; block = block->next ) {
        cv->blk_map[ index ] = block;
        index -= 1;
    }
    remainder = num % cv->elm_count;
    if( remainder != 0 ) {
        MakeFreeList( cv, cv->blk_map[0], remainder * cv->elm_size );
    }
    cv->insert = NULL;
}

static void CarveZapBlock( carve_t cv, void *blk, void *dummy )
{
    dummy = dummy;
    MakeFreeList( cv, (blk_t *)blk, 0 );
}

void CarvePurge( carve_t cv )
{
    cv->free_list = NULL;
    CarveWalkBlocks( cv, CarveZapBlock, NULL );
}

void CarveInsertFree( carve_t cv, void *data )
{
    free_t *    freeblk;

    freeblk = (free_t *)data;
    if( cv->insert == NULL ) {
        freeblk->next_free = cv->free_list;
        cv->free_list = freeblk;
    } else {
        freeblk->next_free = cv->insert->next_free;
        cv->insert->next_free = freeblk;
    }
    cv->insert = freeblk;
}

void *CarveMapIndex( carve_t cv, void *aindex )
{
    unsigned long index = (unsigned long)aindex;
    blk_t *     block;
    blk_t **    block_map;
    unsigned    block_index;
    unsigned    block_offset;

    if( index == CARVE_NULL_INDEX ) {
        return( NULL );
    }
	//DEBUG((DBG_OLD, "CarveMapIndex(aindex=%p)", aindex ));
    block_index = GET_BLOCK( index );
    block_offset = GET_OFFSET( index );
    block_map = cv->blk_map;
    block = block_map[ block_index - 1 ];
    return( &(block->data[ block_offset ]) );
}
