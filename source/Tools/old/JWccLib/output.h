#pragma once

#ifndef _DEBUG
#define debug_printf (void)
#endif

#ifdef _DEBUG
int debug_printf(char* msg);
#endif