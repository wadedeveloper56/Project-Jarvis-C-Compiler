// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef PCH_H
#define PCH_H

// add headers that you want to pre-compile here
#include <stdint.h>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <limits>
#include <string>
#include <vector>
#include <cstdlib>
#include <optional>
#include <cctype>
#include <cstring>
#include <iosfwd>
#include <list>
#include <map>
#include <memory>
#include <set>
#include <unordered_map>
#include <stack>
#include <sstream>
#include <stdexcept>
#include <algorithm>
#include <unordered_set>
#include <format>

#ifdef _DEBUG
#define DEBUG_PRINT(x) std::cout << std::format x;
//("{} is {} years old. Pi is {:.2f}\n", name, age, pi);
#else
#define DEBUG_PRINT(x)
#endif

#pragma warning(disable:4267)
#pragma warning(disable:4244)
#pragma warning(disable:4018)
#pragma warning(disable:4311)
#pragma warning(disable:4146)
#pragma warning(disable:4113)
#pragma warning(disable:5286)
#pragma warning(disable:4312)
#pragma warning(disable:4477)
#pragma warning(disable:5287)
#pragma warning(disable:4047)
#pragma warning(disable:4065)
#pragma warning(disable:4005)
#pragma warning(disable:6011)
#pragma warning(disable:26495)

#endif //PCH_H
