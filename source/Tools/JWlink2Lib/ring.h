

#ifndef __RING_H__

#ifndef RING_H_RECURSE
#define RING_H_RECURSE
#include "ring.h"
#undef RINGHNAME
#define RINGHNAME( name ) Ring##name
#define __RING_H__
#else
#undef RINGHNAME
#define RINGHNAME( name ) Ring2##name
#endif


#include "carve.h"

void *RINGHNAME(Alloc) (             
    void *hdr,                           
    size_t size )                            
;
void* RINGHNAME(CarveAlloc) (         
    carve_t carver,                  
    void *hdr )                          
;
void RINGHNAME(CarveFree) (            
    carve_t carver,                  
    void *hdr )                          
;
void RINGHNAME(Append) (            
    void *hdr,                           
    void *element )                      
;
int RINGHNAME(Count) (               
    void *hdr )                      
;
void RINGHNAME(Dealloc) (           
    void *hdr,                           
    void *element )                      
;
void RINGHNAME(Free) (                
    void *hdr )                          
;
void* RINGHNAME(Promote) (            
    void *hdr,                           
    void *elt,                           
    void *prv )                          
;
void RINGHNAME(Insert) (            
    void *hdr,                           
    void *element,                       
    void *insert )                           
;
void *RINGHNAME(Lookup) (               
    void *hdr,                       
    bool (*compare_rtn)              
        ( void *element,             
          void *comparand ),         
    void *comparand )              
;
void *RINGHNAME(Pop) (                
    void *hdr )                          
;
void *RINGHNAME(Last) (               
    void *hdr )
;
void *RINGHNAME(First) (              
    void *hdr )
;
void *RINGHNAME(Pred) (               
    void *hdr,                       
    void *element )                
;
void *RINGHNAME(Prune) (             
    void *hdr,                           
    void *element )                      
;
void *RINGHNAME(PruneWithPrev) (         
    void *hdr,                           
    void *element,                       
    void *prv )                          
;
void* RINGHNAME(Push) (               
    void *hdr,                           
    void *element )                      
;
void RINGHNAME(Walk) (            
    void *hdr,                       
    void (*rtn)                      
        (void * curr) )                  
;
void *RINGHNAME(Step) (                   
    void *hdr,                       
    void *elt )                            
;
#endif
