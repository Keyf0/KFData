#include "KFDAnyObject.h"
#include "KFDataFormat.h"

NS_KF_BEGIN

KFDAnyObject::~KFDAnyObject()
{
	if (object != nullptr)
	{
		KFDataFormat::DeleteAny(clsname, object);
		object = nullptr;
	}
}

void KFDAnyObject::ReadAny(KFByteArray& bytearr
	, int32 bytecount)
{
	if (bytecount > 0)
	{
		object = KFDataFormat::ReadAny(bytearr, clsname, object);
	}
}

void KFDAnyObject::WriteAny(KFByteArray& bytearr)
{
	if (object != nullptr)
	{
		KFDataFormat::WriteAny(bytearr, clsname, object);
	}
}

NS_KF_END


