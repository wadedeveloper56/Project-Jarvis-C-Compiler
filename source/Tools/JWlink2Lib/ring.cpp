

#include "pch.h"
//#include "linkstd.h"
#include "ring.h"
//#include "alloc.h"

#ifdef PARAM2
#define RINGNAME( name ) Ring2##name
#else
#define RINGNAME( name ) Ring##name
#endif

typedef struct ring RING;
struct ring                         
{
#ifdef PARAM2
	void* filler;
#endif
	RING* next;                        
};


#define RingIterBeg( h, i ) \
    if( i = h ) { \
        RING* _T = i; \
        do { \
            i = i->next;

#define RingIterEnd( i ) \
        } while( i != _T ); \
    }


#define RingIterBegSafe( h, i ) \
    if( i = h ) { \
        RING* _T = i; \
        RING* _N = _T->next; \
        do { \
            i = _N; \
            _N = i->next;

#define RingIterEndSafe( i ) \
        } while( i != _T ); \
    }

#ifndef NDEBUG
static void verifyNotInRing(RING* ring, RING* elt)
{
	RING* curr;

	RingIterBeg(ring, curr) {
		if (curr == elt)
		{
			//LnkFatal("trying to insert element twice into a ring");
		}
	} RingIterEnd(curr)
}
#else
#define verifyNotInRing( h, r )
#endif


void RINGNAME(Append) (             
	void* hdr,                           
	void* element)                      
{
	RING** rhdr;                     
	RING* relement;                  
	RING* lelement;                        

	rhdr = (RING**)hdr;
	relement = (RING*)element;
	verifyNotInRing(*rhdr, relement);
	lelement = *rhdr;
	if (lelement == NULL)
	{
		relement->next = relement;
	}
	else
	{
		relement->next = lelement->next;
		lelement->next = relement;
	}
	*rhdr = relement;
}


void* RINGNAME(Promote) (             
	void* hdr,                           
	void* elt,                           
	void* prv)                          
{
	RING** rhdr;
	RING* last;
	RING* prev;
	RING* element;

	rhdr = (RING**)hdr;
	prev = (RING*)prv;
	element = (RING*)elt;
	last = *rhdr;
	if (prev == NULL || last == prev)
	{
		return element;
	}
	if (last != element)
	{
		prev->next = element->next;
		element->next = last->next;
		last->next = element;
	}
	else
	{
		last = prev;
	}
	*(RING**)hdr = last;
	return element;
}


void RINGNAME(Insert) (             
	void* hdr,                           
	void* element,                       
	void* insert)                           
{
	RING** rhdr;                     
	RING* relement;                        
	RING* ielement;                      
	RING* lelement;                        

	rhdr = (RING**)hdr;
	relement = (RING*)element;
	verifyNotInRing(*rhdr, relement);
	ielement = (RING*)insert;
	lelement = *rhdr;
	if ((lelement == NULL) || (lelement == ielement))
	{
		RINGNAME(Append)(hdr, element);
	}
	else if (ielement == NULL)
	{       
		relement->next = lelement->next;
		lelement->next = relement;
	}
	else
	{
		relement->next = ielement->next;
		ielement->next = relement;
	}
}


void RINGNAME(Walk) (             
	void* hdr,                       
	void (*rtn)                      
	(void* curr))                   
{
#if 0
	RING* rhdr;                      
	RING* relement;                  
	RING* nelement;                    

	if (hdr != NULL)
	{
		rhdr = hdr;
		nelement = rhdr->next;
		do
		{
			relement = nelement;
			nelement = nelement->next;
			(*rtn)(relement) );
		} while (relement != rhdr);
	}
#else
	RING* relement;                  
	RingIterBegSafe((RING*)hdr, relement) {
		(*rtn)(relement);
	} RingIterEndSafe(relement)
#endif
}


void* RINGNAME(Pred)(                
	void* hdr,                       
	void* element)                
{
	RING* rhdr;                      
	RING* pred;                      
	RING* next;                      

	rhdr = (RING*)hdr;
	if (rhdr == NULL)
	{
		pred = NULL;
	}
	else
	{
		for (pred = rhdr; ; )
		{
			next = pred->next;
			if (element == next) break;
			pred = next;
			if (pred == rhdr)
			{
				pred = NULL;
				break;
			}
		}
	}
	return(pred);
}

void* RINGNAME(PruneWithPrev) (         
	void* hdr,                           
	void* element,                       
	void* prv)                          
{
	RING** rhdr;                         
	RING* relement;                      
	RING* prev;                      

	rhdr = (RING**)hdr;
	relement = (RING*)element;
	prev = (RING*)prv;
	if (prev == NULL)
	{
		prev = *rhdr;
	}
	prev->next = relement->next;
	if (prev == relement)
	{
		*rhdr = NULL;
	}
	else
	{
		if (*rhdr == relement)
		{
			*rhdr = prev;
		}
	}
	relement->next = NULL;
	return(relement);
}


void* RINGNAME(Prune) (              
	void* hdr,                           
	void* element)                      
{
	RING** rhdr;                         
	RING* relement;                      
	RING* prev;                      

	rhdr = (RING**)hdr;
	relement = (RING*)element;
	prev = (RING*)RINGNAME(Pred)(*rhdr, relement);
	return(RINGNAME(PruneWithPrev)(hdr, element, prev));
}


void* RINGNAME(Push) (                
	void* hdr,                           
	void* element)                      
{
	RING** rhdr;                         
	RING* last;                      
	RING* relement;                      

	rhdr = (RING**)hdr;
	last = *rhdr;
	relement = (RING*)element;
	verifyNotInRing(last, relement);
	if (last == NULL)
	{
		relement->next = relement;
		*rhdr = relement;
	}
	else
	{
		relement->next = last->next;
		last->next = relement;
	}
	return relement;
}

void* RINGNAME(Last) (               
	void* hdr)                      
{
	return hdr;
}

void* RINGNAME(First) (              
	void* hdr)                      
{
	return ((RING*)hdr)->next;
}

void* RINGNAME(Pop) (                
	void* hdr)                          
{
	RING** rhdr;                         
	RING* last;                      
	RING* first;                     

	rhdr = (RING**)hdr;
	first = NULL;
	last = *rhdr;
	if (last != NULL)
	{
		first = last->next;
		if (first == last)
		{
			*rhdr = NULL;
		}
		else
		{
			last->next = first->next;
		}
	}
	return(first);
}


void* RINGNAME(Lookup) (           
	void* hdr,                       
	bool (*compare_rtn)              
	(void* element,             
		void* comparand),         
	void* comparand)          
{
	RING* rhdr;                      
	RING* curr;                      

	if (hdr == NULL)
	{
		curr = NULL;
	}
	else
	{
		rhdr = (RING*)hdr;
		curr = rhdr;
		for (; ; )
		{
			curr = curr->next;
			if ((*compare_rtn)(curr, comparand)) break;
			if (curr == rhdr)
			{
				curr = NULL;
				break;
			}
		};
	}
	return(curr);
}


int RINGNAME(Count) (                
	void* hdr)                      
{
	int count;                       
	RING* curr;                      

	count = 0;
	RingIterBeg((RING*)hdr, curr) {
		++count;
	} RingIterEnd(curr)
		return count;
}

void* RINGNAME(Alloc) (              
	void* hdr,                           
	size_t size)                            
{
	void* new_element;               

	new_element = (void *)malloc(size);
	//_ChkAlloc(void*, new_element, size);
	RINGNAME(Append)(hdr, new_element);
	return(new_element);
}


void RINGNAME(Dealloc) (            
	void* hdr,                           
	void* element)                      
{
	RINGNAME(Prune)(hdr, element);
	free(element);
}


void RINGNAME(Free) (                 
	void* hdr)                          
{
	void* elt;

	for (;;)
	{
		elt = RINGNAME(Pop)(hdr);
		if (elt == NULL) break;
		free(elt);
	}
}




#include "carve.h"


void* RINGNAME(CarveAlloc) (          
	carve_t carver,                  
	void* hdr)                          
{
	void* elt;

	elt = CarveAlloc(carver);
	RINGNAME(Append)(hdr, elt);
	return elt;
}


void RINGNAME(CarveFree) (             
	carve_t carver,                  
	void* hdr)                          
{
	void* elt;

	for (;;)
	{
		elt = RINGNAME(Pop)(hdr);
		if (elt == NULL) break;
		CarveFree(carver, elt);
	}
}


void* RINGNAME(Step) (            
	void* hdr,               
	void* elt)                    
{
	RING* rhdr;              
	RING* relt;              

	rhdr = (RING*)hdr;
	if (elt == NULL)
	{
		if (rhdr != NULL)
		{
			elt = rhdr->next;
		}
	}
	else
	{
		relt = (RING*)elt;
		if (relt != rhdr)
		{
			elt = relt->next;
		}
		else
		{
			elt = NULL;
		}
	}
	return(elt);
}
