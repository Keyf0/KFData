#pragma once

#include <cstdlib>
#include <cstdio>
#include <cstdint>
#include <cstring>
#include <string>
#include <map>
#include <set>
#include <vector>
#include <queue>
#include <memory>
#include <functional>

#ifndef KF_DEFINE
#define KF_DEFINE

#define NS_KF_BEGIN namespace KF {
#define NS_KF_END }

#define USING_KF using namespace KF

#define kfNew(Type, ...) new Type(__VA_ARGS__)
#define kfDel(obj) if(obj != nullptr){delete obj;obj = nullptr;}
#define kfDelArr(arr) if(arr != nullptr){delete[] arr;arr = nullptr;}

#define kfInline inline

#define kfIsBaseOf(Base,Der) std::is_base_of<Base,Der>::value
#define kfAssert(expr) assert(expr)
#define kfNewMeta(T) kfNew(T::Meta)
#define null nullptr

#endif //!KF_DEFINE

#ifndef KF_LOG_FUNC
#define KF_LOG_FUNC


#define kfTrace(MSG,...)																	\
{																							\
	std::string msgbuff;																	\
	kfStringFormat(msgbuff,MSG,##__VA_ARGS__);												\
	if(KFLogger__::kfLogDebug__ == NULL)													\
		printf("[debug][%d][%s()] %s\n",__LINE__,__FUNCTION__,msgbuff.c_str());	\
	else																					\
		KFLogger__::kfLogDebug__(msgbuff,__FILE__,__LINE__,__FUNCTION__);													\
}

#define kfWarning(MSG,...)																				\
{																										\
	std::string msgbuff;																				\
	kfStringFormat(msgbuff,MSG,##__VA_ARGS__);															\
	if(KFLogger__::kfLogWarning__ == NULL)																\
		printf("[Warning][%s:%d][%s] %s\n",__FILE__,__LINE__,__FUNCTION__,msgbuff.c_str());				\
	else																								\
		KFLogger__::kfLogWarning__(msgbuff,__FILE__,__LINE__,__FUNCTION__);															\
}

#define kfError(MSG,...)																	\
{																							\
	std::string msgbuff;																	\
	kfStringFormat(msgbuff,MSG,##__VA_ARGS__);												\
	if(KFLogger__::kfLogError__ == NULL)													\
		printf("[Error][%s:%d][%s] %s\n",__FILE__,__LINE__,__FUNCTION__,msgbuff.c_str());	\
	else																					\
		KFLogger__::kfLogError__(msgbuff,__FILE__,__LINE__,__FUNCTION__);													\
}


NS_KF_BEGIN

class KFLogger__
{
public:
	typedef std::function<void(std::string& msg, const char*, int, const char*)> kfLogHandler__;
	static kfLogHandler__ kfLogDebug__;
	static kfLogHandler__ kfLogWarning__;
	static kfLogHandler__ kfLogError__;
};

NS_KF_END

#endif // !KF_LOG

#ifndef KF_RTTI
#define KF_RTTI

#define kfCast(T,obj) reinterpret_cast<T>(obj)
#define kfSCast(T,obj) static_cast<T>(obj)
#define kfRCast(T,obj) reinterpret_cast<T>(obj)
#define kfDCast(T,obj) reinterpret_cast<T*>(obj->As(T::Meta::Type()))

///增加一个另类定义,后续如果有新的RTTI方式
#define kf_d_cast(T,instance) instance->As<T>()
#define kf_d0_cast(T,instance) kf_d_cast(T*,instance)

#define kfSize2U(size) static_cast<uint32>(size)
#define kfSize2I(size) static_cast<int32>(size)
#define kf_2_uint32(size) kfSize2U(size)
#define kf_2_int32(size) kfSize2I(size)


#define DEFINE_AS_CLASS(T) \
void* as##T() override{return static_cast<T*>(this);}

#define DEFINE_AS_CLASS_NULL(T) \
virtual void* as##T(){return 0;}

#define DEFINE_AS_INTERFACE(T) \
virtual void* as##T() = 0;

#define DEFINE_AS_INTERFACE_NULL(T) \
virtual void* as##T() override{return 0;}

#define AS_CLASS(T,obj) (obj != NULL?((T*)(obj)->as##T()):NULL)


#endif // !KF_RTTI

#ifndef KF_MALLOC
#define KF_MALLOC

#define kfMalloc(size) std::malloc(size)
#define kfFree(block) if(block != NULL){std::free(block);block = NULL;}
#define kfCalloc(count,size) std::calloc(count,size)
#define kfRealloc(block,size) std::realloc(block,size)

#define kfMallocTo(size,t) kfRCast(t,kfMalloc(size))

#endif // !KF_MALLOC


#ifndef KF_STRING_FUNC
#define KF_STRING_FUNC

#define kfCopy(dst,src,size) std::memcpy(dst,src,size)
#define kfCmp(buf1,buf2,size) std::memcmp(buf1,buf2,size)
#define kfSet(dst,val,size) std::memset(dst,val,size)
#define kfMove(dst,src,size) std::memmove(dst,src,size)


#ifndef KFENGINERT_API
#define KFENGINERT_API
#endif

NS_KF_BEGIN

KFENGINERT_API std::string& kfStringFormat(std::string& buff
	, const char* fmt_str, ...);

NS_KF_END


#endif // !KF_STRING_FUNC







