#pragma once 

#ifdef _DOS16M
bool ProcMemory16M(void);

bool ProcTryExtended(void);

bool ProcTryLow(void);

bool ProcForceExtended(void);

bool ProcForceLow(void);

bool ProcTransparent(void);

bool ProcTStack(void);;

bool ProcTData(void);

bool ProcKeyboard(void);

bool ProcOverload(void);

bool ProcInt10(void);

bool ProcInit00(void);

bool ProcInitFF(void);

bool ProcRotate(void);

bool ProcSelectors(void);

bool ProcAuto(void);

bool ProcBuffer(void);

bool ProcGDTSize(void);

bool ProcRelocs(void);

bool Proc16MNoRelocs(void);

bool ProcSelStart(void);

bool ProcExtended(void);

bool ProcDataSize(void);

void SetD16MFmt(void);

void FreeD16MFmt(void);

bool Proc16M(void);
#endif

