#include "pch.h"
#include "objstrip.h"
#include "lsymtab.h"
#include "globals.h"

typedef struct edgelist
{
	struct edgelist* next;
	union
	{
		segdata* seg;
		symbol* sym;
	} u;
	unsigned            issym : 1;           
	unsigned            reverse_dir : 1;       
} edgelist;

static edgelist* FreedEdges;


void ResetObjStrip(void)
{
	FreedEdges = NULL;
}

static edgelist* AllocEdge(void)
{
	edgelist* edge;

	if (FreedEdges != NULL)
	{
		edge = FreedEdges;
		FreedEdges = edge->next;
	}
	else
	{
		edge = (edgelist*)malloc(sizeof(edgelist));
		//_Pass1Alloc(edgelist*, edge, sizeof(edgelist));
	}
	memset(edge, 0, sizeof * edge);
	return(edge);
}

static void FreeEdge(edgelist* edge)
{
	edge->next = FreedEdges;
	FreedEdges = edge;
}

static void FreeEdgeList(edgelist* edge)
{
	edgelist* listend;

	if (edge == NULL) return;
	listend = edge;
	while (listend->next != NULL)
	{
		listend = listend->next;
	}
	listend->next = FreedEdges;
	FreedEdges = edge;
}

static void PruneNonSymEdges(symbol* sym)
{
	edgelist* list;
	edgelist* next;

	list = (edgelist*)sym->p.edges;
	sym->p.edges = NULL;
	while (list != NULL)
	{
		next = list->next;
		if (list->issym)
		{
			list->next = (edgelist*)sym->p.edges;
			sym->p.edges = list;
		}
		else
		{
			FreeEdge(list);
		}
		list = next;
	}
}

void RefSeg(segdata* seg)
{
	edgelist* edge;
	edgelist* next;

	if (seg->isrefd || seg->visited) return;
	seg->isrefd = TRUE;
	seg->visited = TRUE;
	edge = (edgelist*)seg->a.refs;
	while (edge != NULL)
	{
		//DbgAssert(edge->issym == 0);
		next = edge->next;
		RefSeg(edge->u.seg);
		FreeEdge(edge);
		edge = next;
	}
	seg->a.refs = NULL;
	seg->visited = FALSE;
}

void DataRef(symbol* sym)
{
	if (IS_PPC_PE)
	{
		unsigned    len = strlen(sym->name) + 3;
		char* s = (char*)malloc(len);

		s[0] = s[1] = '.';
		strcpy(s + 2, sym->name);
		sym = SymOp(ST_FIND, s, len - 1);
	}
	if (sym->info & SYM_DEFINED && !IS_SYM_IMPORTED(sym))
	{
		RefSeg(sym->p.seg);
	}
	DO_OR_EQ(sym_info, sym->info, |=, SYM_DCE_REF);
}

void AddEdge(segdata* seg, symbol* sym)
{
	edgelist* edge;

	if (sym->info & SYM_DEFINED)
	{
		if (!IS_SYM_IMPORTED(sym) && seg != sym->p.seg && sym->p.seg != NULL)
		{
			if (seg->isrefd || !seg->iscode)
			{
				RefSeg(sym->p.seg);
			}
			else if (!sym->p.seg->isrefd)
			{
				edge = AllocEdge();
				edge->u.seg = sym->p.seg;
				edge->issym = FALSE;
				edge->next = (edgelist*)seg->a.refs;
				seg->a.refs = edge;
			}
		}
	}
	else
	{        
		if (seg->isrefd || !seg->iscode)
		{
			DO_OR_EQ(sym_info, sym->info, |=, SYM_DCE_REF);
			PruneNonSymEdges(sym);
		}
		else
		{
			edge = AllocEdge();
			edge->u.seg = seg;
			edge->next = (edgelist*)sym->p.edges;
			edge->issym = FALSE;
			edge->reverse_dir = TRUE;
			sym->p.edges = edge;
		}
	}
}

void AddSymEdge(symbol* srcsym, symbol* targsym)
{
	edgelist* edge;

	if (srcsym->info & SYM_DEFINED)
	{
		AddEdge(srcsym->p.seg, targsym);
	}
	else
	{
		edge = AllocEdge();
		edge->u.sym = targsym;
		edge->issym = TRUE;
		edge->next = (edgelist*)srcsym->p.edges;
		srcsym->p.edges = edge;
	}
}

void AddSymSegEdge(symbol* srcsym, segdata* targsdata)
{
	edgelist* edge;

	if (IS_SYM_IMPORTED(srcsym))
	{
		RefSeg(targsdata);
		return;
	}
	edge = AllocEdge();
	edge->u.seg = targsdata;
	edge->issym = FALSE;
	if (srcsym->info & SYM_DEFINED)
	{
		edge->next = (edgelist*)targsdata->a.refs;
		targsdata->a.refs = edge;
	}
	else
	{
		edge->next = (edgelist*)srcsym->p.edges;
		srcsym->p.edges = edge;
	}
}

void DefStripSym(symbol* sym, segdata* seg)
{
	edgelist* list;
	edgelist* next;

	if (sym->info & SYM_DCE_REF)
	{
		RefSeg(seg);
	}
	list = (edgelist*)sym->p.edges;
	while (list != NULL)
	{
		next = list->next;
		if (list->issym)
		{
			//DbgAssert(list->reverse_dir == 0);      
			AddEdge(seg, list->u.sym);
			FreeEdge(list);
		}
		else
		{
			segdata* src, * trg;
			if (list->reverse_dir)
			{
				src = list->u.seg;
				trg = seg;
			}
			else
			{
				src = seg;
				trg = list->u.seg;
			}
			if (trg->isrefd)
			{          
				FreeEdge(list);
			}
			else
			{
				if (src->isrefd)
				{
					RefSeg(trg);
					FreeEdge(list);          
				}
				else
				{
					list->reverse_dir = 0;
					list->u.seg = trg;        
					list->next = (edgelist*)src->a.refs;          
					src->a.refs = list;
				}
			}
		}
		list = next;
	}
	sym->p.seg = seg;
}

void DefStripImpSym(symbol* sym)
{
	edgelist* list;
	edgelist* next;

	list = (edgelist*)sym->p.edges;
	while (list != NULL)
	{
		next = list->next;
		if (list->reverse_dir == 0)
		{
			if (list->issym)
			{
				DO_OR_EQ(sym_info, list->u.sym->info, |=, SYM_DCE_REF);
			}
			else
			{
				RefSeg(list->u.seg);
			}
		}
		FreeEdge(list);
		list = next;
	}
	sym->p.edges = NULL;
}

void CleanStripInfo(symbol* sym)
{
	FreeEdgeList((edgelist*)sym->p.edges);
	sym->p.edges = NULL;
}
