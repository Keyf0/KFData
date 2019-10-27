#pragma once

#include "KFDValue.h"
#include "KFD.h"
#include "KFDName.h"
#include "Utils/zzzjson.h"

NS_KF_BEGIN

class KFENGINERT_API KFDataJson
{

public:

	static KFDValue* ReadValue(const kfstr& jsonstr, KFDValue* value = nullptr);
	static KFDObject* ReadAsObject(const kfstr& jsonstr, KFDValue* value = nullptr);
	static void WriteValue(kfstr& outjsonstr, KFDValue* value, bool updatewrite = false);

private:

	///读取一个KFD的值
	static KFDValue* ReadValue(Value *next
		, KFDValue* value = nullptr
		, const KFDProperty* property = nullptr);

	///读取基础数据|不包括类型部分
	static KFDValue* ReadBaseValue(Value *next
		, uint8 valtype
		, KFDValue* value = nullptr);

	///读取数组数据|不包括类型部分
	static KFDValue* ReadArrayValue(Value *next
		, uint8 valtype
		, KFDValue* val = nullptr
		, const KFDProperty* property = nullptr);

	///读取一个KFD的对象|不包括类型部分
	static KFDValue* ReadObjectValue(
		Value *next
		, uint8 valtype
		, KFDValue* val = nullptr
		, const KFD* kfd = nullptr
		, kfMap<kfstr, Value*>* valuemaps = nullptr);

	///写入一个数值
	static void WriteValue(Value *next, KFDValue* val, bool updatewrite = false);
	static void WriteBaseValue(Value *next, uint8 dataType, KFDValue* val);
	static void WriteObjectValue(Value *next, uint8 dataType, KFDObject* val, bool updatewrite = false);
	static void WriteArrayValue(Value *next, uint8 valtype, KFDObject* val);

};

NS_KF_END
