#pragma once

#include "KFDValue.h"
#include "KFD.h"
#include "KFDName.h"

NS_KF_BEGIN

/**
	KFDF 格式的定义

	值结构体
	-------------
		1byte 类型
		nbyte 内容
		--------------------
				 类型 OBJECT
				 属性的定义1
				 属性的定义N
				 类型 MIXOBJECT
				 ------------
				 1-5byte类型ID
				 类型名OPTION:kfstr[类型ID == 1时表示字符串]
				 属性的定义1
				 属性的定义N
				 =============
				 类型为 ARRAY
				 ------------
				 1-5byte 数组长度
				 1byte 数组元素类型
				 内容
				 =================
				 类型为MIXARRAY
				 --------------
				 1-5byte 数组长度
				 值结构体
				 ================

	属性的定义:
	-----------
		varuint32 属性id
		值结构体
		======================
		属性id = 0x7f 对象读取开始
		属性id = 0    对象读取结束


**/

class KFDSerialize
{
public:
	
	///通用序列化反充列方法
	typedef kfAny(*SerializeRead)(KFByteArray& bytearr, kfAny obj);
	typedef void(*SerializeWrite)(KFByteArray& bytearr, kfAny obj);
	///通用删除方法
	typedef void(*Delete)(kfAny obj);

	SerializeRead Read;
	SerializeWrite Write;
	Delete Del;

public:

	KFDSerialize(SerializeRead rfunc = NULL
		, SerializeWrite wfunc = NULL
		, Delete dfunc = NULL)
	{
		this->Read = rfunc;
		this->Write = wfunc;
		this->Del = dfunc;
	}

	kfAny ReadAny(KFByteArray& bytearr, kfAny obj)
	{
		if (this->Read != NULL)
		{return this->Read(bytearr, obj);}
		return obj;
	}

	void WriteAny(KFByteArray& bytearr, kfAny obj)
	{
		if (this->Write != NULL)
		{this->Write(bytearr, obj);}
	}

	void DeleteAny(kfAny obj)
	{
		if (this->Del != NULL)
		{this->Del(obj);}
	}

};



class KFENGINERT_API KFDataFormat
{
public:

	///跳过指定的值对象
	static void SkipValue(KFByteArray& bytearr);

	///跳过一个OBJECT对象
	///与上面唯一不同的点是
	///上面有类型这个略过直接是一个OBJECT对象
	static void SkipObject(KFByteArray& bytearr);

	///读取一个KFD的值
	static KFDValue* ReadValue(KFByteArray& bytearr
		, bool skip = false
		, KFDValue* value = nullptr);

	static KFDObject* ReadAsObject(KFByteArray& bytearr
		, bool skip = false
		, KFDObject* value = nullptr);

	///读取基础数据|不包括类型部分
	static KFDValue* ReadBaseValue(KFByteArray& bytearr
		, uint8 valtype
		, bool skip = false
		, KFDValue* value = nullptr);

	///读取数组数据|不包括类型部分
	static KFDValue* ReadArrayValue(KFByteArray& bytearr
		, uint8 valtype
		, bool skip = false
		, KFDValue* val = nullptr);

	///读取一个KFD的对象|不包括类型部分
	static KFDValue* ReadObjectValue(KFByteArray& bytearr
		, uint8 valtype
		, uint32 classid
		, const kfstr& clsname
		, bool skip = false
		, KFDValue* val = nullptr);

	///写入一个数值
	static void WriteValue(KFByteArray& bytearr, KFDValue* val, bool updatewrite = false);
	static void WriteBaseValue(KFByteArray& bytearr, uint8 dataType, KFDValue* val);
	static void WriteObjectValue(KFByteArray& bytearr, uint8 dataType, KFDObject* val, bool updatewrite = false);
	static void WriteArrayValue(KFByteArray& bytearr, uint8 valtype, KFDObject* val);


	///通过名称来定义

	static kfAny ReadAny(KFByteArray& bytearr, const KFDName& clsname, kfAny obj = nullptr);
	static bool WriteAny(KFByteArray& bytearr, const KFDName& clsname, kfAny obj = nullptr);
	static bool DeleteAny(const KFDName& clsname, kfAny obj = nullptr);

	static bool WriteAnyObject(KFByteArray& bytearr, const kfstr& clsname, kfAny obj = nullptr);
	static kfAny ReadAnyObject(KFByteArray& bytearr, const kfstr& clsname, kfAny obj = nullptr);

	KFDataFormat();

	virtual ~KFDataFormat();

	inline static void Reg_kfd_any(
		const kfstr& clsname
		, KFDSerialize::SerializeRead rfunc
		, KFDSerialize::SerializeWrite wfunc
		, KFDSerialize::Delete dfunc)
	{
		//kfname nameid;
		//nameid.SetValue(kfname::NameStrings().__internal_add_string(clsname));

		SerializeAnys.insert(kfPair<kfname, KFDSerialize>(
				clsname
			, KFDSerialize(rfunc, wfunc, dfunc)));
	}

public:

	///临时用的数组用之前CLEAR
	static KFByteArray ClearUseBuff;
	///序列化反序列化信息
	static kfMap<kfname, KFDSerialize> SerializeAnys;

	///版本号
	uint16 VER;
	///值
	KFDValue* Value;
};

///注册函数信息
#define REG_KFD_FORMAT_ANY(clsname,rfunc,wfunc,dfunc) \
KFDataFormat::Reg_kfd_any(clsname,rfunc,wfunc,dfunc)

NS_KF_END
