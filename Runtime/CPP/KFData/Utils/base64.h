//
//  base64 encoding and decoding with C++.
//  Version: 1.01.00
//

#ifndef BASE64_H_C0CE2A47_D10E_42C9_A27C_C883944E704A
#define BASE64_H_C0CE2A47_D10E_42C9_A27C_C883944E704A

#include <string>
#include "KFByteArray.h"
NS_KF_BEGIN

void base64_encode(KFByteArray& inbytearr
	, std::string& outstr);
void base64_decode(const char* str,int32 slen
	, KFByteArray& outbytearr);

NS_KF_END

#endif /* BASE64_H_C0CE2A47_D10E_42C9_A27C_C883944E704A */
