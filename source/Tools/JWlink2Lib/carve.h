#pragma once

typedef struct blk blk_t;
typedef struct free_t free_t;

typedef struct {
    free_t *    free_list;
    free_t *    insert;
    blk_t *     blk_list;
    blk_t **    blk_map;
    size_t      elm_size;
    size_t      elm_count;
    size_t      blk_top;
    size_t      blk_count;
    size_t      blk_size;
    unsigned    size_chg : 1;
} cv_t, *carve_t;

extern carve_t CarveCreate( size_t elm_size, size_t how_many );
extern void CarveRestart( carve_t cv, unsigned );
extern void CarvePurge( carve_t cv );
extern void CarveVerifyAllGone( carve_t cv, char * );
extern void CarveDestroy( carve_t cv );

extern void *CarveAlloc( carve_t cv );
extern void *CarveZeroAlloc( carve_t cv );
extern void CarveFree( carve_t cv, void *elm );


enum {
    CARVE_NULL_INDEX    = 0,    
    CARVE_INVALID_INDEX = 1,    
};

extern void *   CarveGetIndex( carve_t, void * );
extern void     CarveWalkBlocks( carve_t, void (*)(carve_t, void *, void *),
                                 void * );
extern bool     CarveBlockModified( void * );
extern void     CarveBlockScan( carve_t, void *, void (*)(void *, void *),
                                void *);
extern unsigned CarveBlockSize( carve_t );
extern unsigned CarveNumElements( carve_t );
extern void *   CarveBlockData( void * );
extern void *   CarveMapIndex( carve_t, void * );
extern bool     CarveSizeChanged( carve_t );
extern void     CarveInsertFree( carve_t, void * );

typedef struct {
    unsigned    bindex;
    blk_t *     block;
    free_t **   head;
    carve_t     cv;
} cvinit_t;

extern void CarveWalkAllFree( carve_t gcv, void (*rtn)( void * ) );
extern void CarveWalkAll( carve_t, void (*)(void *, void *), void * );

