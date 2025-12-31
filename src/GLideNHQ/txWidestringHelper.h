#pragma once
#include <string>
#include <algorithm>

#define tx_swprintf	snprintf
#define wst(A) A
#define wccmp(A, B) A[0] == B[0]

typedef std::string tx_wstring;

inline
void removeColon(tx_wstring& _s)
{
	std::replace(_s.begin(), _s.end(), ':', '-');
	std::replace(_s.begin(), _s.end(), '/', '-');
}
