#include "pch.h"
#include "structs.h"
#include "command.h"
#include "cmdline.h"
#include "cmdqnx.h"
#include "cmdos2.h"
#include "globals.h"
#include "exeos2.h"
#include "cmdall.h"
#include "cmd16m.h"
#include "cmdnov.h"
#include "cmdelf.h"
#include "cmdphar.h"
#include "cmddos.h"
#include "util.h"
#include "start.h"
#include "ntio.h"
#include "fileio.h"
#include "loadqnx.h"
#include "exeqnx.h"

bool ProcQNX(void)
{
	if (!ProcOne(QNXFormats, SEP_NO, FALSE))
	{
		HintFormat(MK_QNX_16);             
	}
	return(TRUE);
}

bool ProcQNXFlat(void)
{
	return(TRUE);
}

void SetQNXFmt(void)
{
	Extension = E_QNX;
	FmtData.u.qnx.flags = 0;
	FmtData.u.qnx.priv_level = 0;
	FmtData.u.qnx.seg_flags = NULL;
	FmtData.u.qnx.heapsize = 4096;
	FmtData.u.qnx.gen_seg_relocs = TRUE;
	FmtData.u.qnx.gen_linear_relocs = FALSE;
	ChkBase(4 * 1024);
}

void FreeQNXFmt(void)
{
	//FIXME FreeSegFlags((seg_flags*)FmtData.u.qnx.seg_flags);
}

void CmdQNXFini(void)
{
	if (!FmtData.u.qnx.gen_linear_relocs && !FmtData.u.qnx.gen_seg_relocs)
	{
		LinkState &= ~MAKE_RELOCS;
	}
}

static bool GetQNXSegFlags(void)
{
	bool            isclass;
	qnx_seg_flags* entry;

	Token.thumb = REJECT;
	isclass = ProcOne(QNXSegDesc, SEP_NO, FALSE);
	if (!GetToken(SEP_NO, TOK_INCLUDE_DOT))
	{
		return(FALSE);
	}
	entry = (qnx_seg_flags*)malloc(sizeof(qnx_seg_flags));
	//_ChkAlloc(qnx_seg_flags*, entry, sizeof(qnx_seg_flags));
	entry->flags = 0;
	entry->name = tostring();
	entry->isclass = isclass;
	entry->next = FmtData.u.qnx.seg_flags;
	FmtData.u.qnx.seg_flags = entry;
	return(ProcOne(QNXSegModel, SEP_NO, FALSE));
}

bool ProcQNXSegment(void)
{
	return(ProcArgList(GetQNXSegFlags, TOK_INCLUDE_DOT));
}

bool ProcQNXClass(void)
{
	return(TRUE);
}

bool ProcQNXExecuteonly(void)
{
	FmtData.u.qnx.seg_flags->flags = QNX_EXEC_ONLY;
	return(TRUE);
}

bool ProcQNXExecuteread(void)
{
	FmtData.u.qnx.seg_flags->flags = QNX_EXEC_READ;
	return(TRUE);
}

bool ProcQNXReadOnly(void)
{
	FmtData.u.qnx.seg_flags->flags = QNX_READ_ONLY;
	return(TRUE);
}

bool ProcQNXReadWrite(void)
{
	FmtData.u.qnx.seg_flags->flags = QNX_READ_WRITE;
	return(TRUE);
}

bool ProcLongLived(void)
{
	FmtData.u.qnx.flags |= _TCF_LONG_LIVED;
	return(TRUE);
}

bool ProcQNXNoRelocs(void)
{
	FmtData.u.qnx.gen_seg_relocs = FALSE;
	return(TRUE);
}

bool ProcLinearRelocs(void)
{
	FmtData.u.qnx.gen_linear_relocs = TRUE;
	return(TRUE);
}

bool ProcQNXHeapSize(void)
{
	ord_state           ret;
	uint32_t             value;

	if (!HaveEquals((tokcontrol)0)) return(FALSE);
	ret = getatol(&value);
	if (ret != ST_IS_ORDINAL || value == 0)
	{
		//LnkMsg(LOC + LINE + WRN + MSG_VALUE_INCORRECT, "s", "heapsize");
	}
	else
	{
		FmtData.u.qnx.heapsize = value;
	}
	return(TRUE);
}

bool ProcQNXPrivilege(void)
{
	ord_state           ret;
	uint16_t         value;

	if (!HaveEquals((tokcontrol)0)) return(FALSE);
	ret = getatoi(&value);
	if (ret != ST_IS_ORDINAL || value > 3)
	{
		//LnkMsg(LOC + LINE + WRN + MSG_VALUE_INCORRECT, "s", "privilege");
	}
	else
	{
		FmtData.u.qnx.priv_level = value;
	}
	return(TRUE);
}
