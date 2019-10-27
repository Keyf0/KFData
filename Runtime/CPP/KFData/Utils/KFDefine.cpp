#include "KFDefine.h"
#include <stdarg.h>

NS_KF_BEGIN
std::string& kfStringFormat(std::string& buff
	, const char* fmt_str, ...)
{
	std::size_t n = 256;

	if (buff.size() < n)
	{
		buff.resize(n);
	}
	else
	{
		n = buff.size();
	}

	//cout<<"------------------------"<<endl;
	while (1)
	{
		//std::cout<<"processing...., n="<<n<<std::endl;
		va_list ap;
		va_start(ap, fmt_str);//TODO Error in ue4
		const int final_n = vsnprintf(&buff[0], n, fmt_str, ap);
		va_end(ap);
		if (final_n < 0) // 文档说了返回负值表示encoding error
		{
			n += static_cast<size_t>(-final_n);
			buff = "encoding error";
			break;
		}

		if (static_cast<size_t>(final_n) >= n)
		{
			n += static_cast<size_t>(final_n) - n + 1;
			/*if (n > 4096) // 免得分配的内存太大不能控制
			{
			buff = "string too long, larger then 4096...";
			break;
			}*/
			buff.resize(n);
		}
		else
		{
			buff[final_n] = '\0';
			buff.resize(final_n);
			break;
		}
	}
	return buff;
}

KFLogger__::kfLogHandler__ KFLogger__::kfLogDebug__ = NULL;
KFLogger__::kfLogHandler__ KFLogger__::kfLogError__ = NULL;
KFLogger__::kfLogHandler__ KFLogger__::kfLogWarning__ = NULL;

NS_KF_END



