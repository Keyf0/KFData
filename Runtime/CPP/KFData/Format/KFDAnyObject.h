#pragma once

#include "KFDValue.h"
#include "KFD.h"

NS_KF_BEGIN

///KFD(C)
class KFDAnyObject
{
public:

	///类型名称
	///KFD(P=1)
	kfstr clsname;
	///原始对象
	///KFD(P=2,NAME=object,TYPE=kfBytes,READ=ReadAny,WRITE=WriteAny)
	kfAny object;
	///KFD(*)
public:
	KFDAnyObject() {}
	~KFDAnyObject();

	KFDAnyObject(const KFDAnyObject& from)
	{
		CopyFrom(const_cast<KFDAnyObject&>(from));
	}

	inline KFDAnyObject& operator=(const KFDAnyObject& from)
	{
		CopyFrom(const_cast<KFDAnyObject&>(from));
		return *this;
	}

	inline void CopyFrom(KFDAnyObject& from)
	{
		clsname = from.clsname;
		object = from.object;
		from.object = nullptr;
	}

public:

	void ReadAny(KFByteArray& bytearr,int32 bytecount);
	void WriteAny(KFByteArray& bytearr);

};

NS_KF_END
