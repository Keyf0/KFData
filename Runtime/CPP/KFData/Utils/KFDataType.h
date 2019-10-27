#pragma once

#include "Utils/KFDefine.h"
#include <list>
#include <vector>
#include <queue>
#include <map>
#include <stack>
#include <array>
#include <unordered_map>

#ifndef KF_DATA_TYPE
#define KF_DATA_TYPE

typedef unsigned char byte;
typedef unsigned char* bytes;

//typedef std::int64_t int64;
//typedef std::uint64_t uint64;

typedef signed long long int64;
typedef unsigned long long	uint64;


typedef std::uint32_t uint32;
typedef std::int32_t int32;
typedef std::uint32_t varuint;
typedef std::uint16_t uint16;
typedef std::int16_t int16;
typedef std::uint8_t uint8;
typedef std::int8_t  int8;

typedef std::string kfstr;



#define kfToString(v) std::to_string(v)

typedef float num1;
typedef float num32;
typedef double num2;
typedef double num64;


///用int32代替单精度浮点
typedef std::int32_t kfnum1;
///用int64代替双精度浮点
typedef std::int64_t kfnum2;
/// long long
typedef std::size_t kfsize;

///用kfMap代替
#define kfMap std::unordered_map
#define kfOMap std::map
#define kfVector std::vector
#define kfArray std::array
#define kfQueue std::queue
#define kfList std::list
#define kfPair std::pair
#define kfStack std::stack


#define kfPtr std::shared_ptr
#define kfPtrDefine(cls) \
	class cls;\
	typedef kfPtr<cls> cls##Ptr;


///定义一个对象
typedef void* kfAny;


///浮点和INT32互转
union num1_buff
{
	num1 fval;
	byte buff[4];
	int32 intval;
};

///双精度浮点和INT64互转
union num2_buff
{
	num2 fval;
	byte buff[8];
	int64 intval;
};

///定义一个二进制数据格式
struct kfBytes
{
public:

	kfBytes(int32 size = 0)
	{
		buffsize = size;
		buffcapacity = size;

		ResetSize(buffsize);
	}

	~kfBytes() 
	{
		kfFree(buff);
		buffsize = 0;
		buffcapacity = 0;
	}

	kfBytes(const kfBytes& from)
	{
		CopyFrom(from);
	}

	inline kfBytes& operator=(kfBytes& from)
	{
		CopyFrom(from);
		return *this;
	}

	inline void CopyFrom(const kfBytes& from)
	{
		ResetSize(from.buffsize);
		kfCopy(buff, from.buff, buffsize);
	}

	void ResetSize(int32 size)
	{
		if (size > buffcapacity)
		{
			kfFree(buff);
			
			buffcapacity = size;

			if (buffcapacity > 0)
			{
				buff = kfMallocTo(buffcapacity, bytes);
			}
		}

		buffsize = size;
	}

	void Attach(bytes tbuff, int32 size)
	{
		kfFree(buff);

		this->buff = tbuff;
		this->buffsize = size;
		this->buffcapacity = size;
	}

	void UnAttach()
	{
		buff = NULL;
		buffsize = 0;
		buffcapacity = 0;
	}

public:

	bytes buff = NULL;
	int32 buffsize = 0;
	int32 buffcapacity = 0;
};

///类型ID
///KFD(C)
class kfTypeid
{
public:
	///KFD(P=1)
	int32 id = 0;
	///KFD(P=2)
	kfstr name;
	///KFD(*)
};


static kfstr NULLSTR = "";
#define kfstrNULL NULLSTR

#endif