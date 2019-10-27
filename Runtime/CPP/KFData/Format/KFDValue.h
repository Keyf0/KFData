#pragma once

#include "KFD.h"
#include "Utils/KFDefine.h"

NS_KF_BEGIN

///KFD值
class KFDValue
{
public:

	enum KFDValue_UPDATE_FLAG
	{
		///普通状态
		KFDValue_UPDATE_FLAG_ANY	= 0,
		///子集有被修改
		KFDValue_UPDATE_FLAG_NEXT	= 1,
		///自己被修改过了
		KFDValue_UPDATE_FLAG_MODIFY = 2,
	};

	KFDValue() :
		  valueType(OT_UNKNOW)
		, updateFlag(KFDValue_UPDATE_FLAG_ANY)
	{}

	virtual ~KFDValue() {}

	virtual bool EqualVal(const kfstr& val) { return false; }
	virtual bool AutoCreatePropertys() { return false; }
	virtual KFDValue_UPDATE_FLAG GetUpdateFlag() { return updateFlag;}

		DEFINE_AS_CLASS_NULL(KFDNumber1)
		DEFINE_AS_CLASS_NULL(KFDNumber2)

		DEFINE_AS_CLASS_NULL(KFDNumber32)
		DEFINE_AS_CLASS_NULL(KFDNumber64)
	///
		DEFINE_AS_CLASS_NULL(KFDBool)
	//
		DEFINE_AS_CLASS_NULL(KFDkfstr)
		DEFINE_AS_CLASS_NULL(KFDkfBytes)
			
		DEFINE_AS_CLASS_NULL(KFDPropertyValue)
		DEFINE_AS_CLASS_NULL(KFDObject)	
public:
	uint8 valueType;
	KFDValue_UPDATE_FLAG updateFlag;
};

class KFDNumber32 : public KFDValue
{
public:
	KFDNumber32() { numval.intval = 0; }
	virtual ~KFDNumber32() {}

	void* asKFDNumber1() override
	{
		return static_cast<KFDNumber32*>(this);
	}

	DEFINE_AS_CLASS(KFDNumber32)

	bool EqualVal(const kfstr& val) override
	{
		return atoi(val.c_str()) == numval.intval;
	}
public:
	num1_buff numval;
};

class KFDNumber64 : public KFDValue
{
public:
	KFDNumber64() { numval.intval = 0; }
	virtual ~KFDNumber64() {}

	void* asKFDNumber2() override
	{
		return static_cast<KFDNumber64*>(this);
	}

	DEFINE_AS_CLASS(KFDNumber64)
public:
	num2_buff numval;
};

typedef KFDNumber32 KFDNumber1;
typedef KFDNumber64 KFDNumber2;


class KFDBool : public KFDValue
{
public:
	KFDBool() { val = false; }
	virtual ~KFDBool() {}
	DEFINE_AS_CLASS(KFDBool)
public:
	bool val;
};


class KFDkfstr : public KFDValue
{
public:
	KFDkfstr() {}
	virtual ~KFDkfstr() {}
	DEFINE_AS_CLASS(KFDkfstr)

	bool EqualVal(const kfstr& valstr) override
	{
		return this->val == valstr;
	}
public:
	kfstr val;
};

class KFDkfBytes : public KFDValue
{
public:
	KFDkfBytes() {}
	virtual ~KFDkfBytes() {}
	DEFINE_AS_CLASS(KFDkfBytes)
public:
	KFByteArray val;
};

class KFDPropertyValue : public KFDValue
{
public:
	KFDPropertyValue(uint32 id = 0)
		: pid(id)
		, val(nullptr) 
		, propinfo(nullptr)
	{}

	virtual ~KFDPropertyValue()
	{
		pid = 0;
		kfDel(val);
		propinfo = nullptr;
	}

	DEFINE_AS_CLASS(KFDPropertyValue)

	KFDPropertyValue(KFDPropertyValue& from)
	{
		CopyFrom(from);
	}

	inline KFDPropertyValue& operator=(KFDPropertyValue& from)
	{
		CopyFrom(from);
		return *this;
	}

	inline void CopyFrom(KFDPropertyValue& from)
	{
		kfDel(val);

		pid = from.pid;
		val = from.val;
		propinfo = from.propinfo;

		from.val = nullptr;
	}

public:

	class KFDObject* ValueAsObject();

	KFDkfBytes* ValueAsBytes()
	{
		return AS_CLASS(KFDkfBytes, val);
	}

	///设置属性的VALUE值
	bool SetPropValue(KFDValue* newval);


	/*获取INT值*/
	int32 ValueInt() 
	{
		auto value = AS_CLASS(KFDNumber1, val);
		if (value == nullptr)
			return  0;
		return value->numval.intval;
	}

	int32 ValueType()
	{
		if (val == nullptr)
			return OT_UNKNOW;
		return val->valueType;
	}

	void SetValueInt(int32 arg)
	{
		auto value = AS_CLASS(KFDNumber32, val);
		if (value == nullptr)
			return;
		value->numval.intval = arg;
	}

	num1 ValueNum1()
	{
		auto value = AS_CLASS(KFDNumber32, val);
		if (value == nullptr)
			return  0;
		return value->numval.fval;
	}


	void SetValueNum1(num1 arg)
	{
		auto value = AS_CLASS(KFDNumber32, val);
		if (value == nullptr)
			return;
		value->numval.fval = arg;
	}

	num2 ValueNum2()
	{
		auto value = AS_CLASS(KFDNumber64, val);
		if (value == nullptr)
			return  0;
		return value->numval.fval;
	}


	void SetValueNum2(num2 arg)
	{
		auto value = AS_CLASS(KFDNumber64, val);
		if (value == nullptr)
			return;
		value->numval.fval = arg;
	}

	int64 ValueInt64()
	{
		auto value = AS_CLASS(KFDNumber64, val);
		if (value == nullptr)
			return  0;
		return value->numval.intval;
	}

	void SetValueInt64(int64 arg)
	{
		auto value = AS_CLASS(KFDNumber64, val);
		if (value == nullptr)
			return;
		value->numval.intval = arg;
	}

	bool ValueBool() 
	{
		auto value = AS_CLASS(KFDBool, val);
		if (value == nullptr)
			return  false;
		return value->val;
	}

	void SetValueBool(bool arg)
	{
		auto value = AS_CLASS(KFDBool, val);
		if (value == nullptr)
			return;
		value->val = arg;
	}

	kfstr* ValueString()
	{
		auto value = AS_CLASS(KFDkfstr, val);
		if (value == nullptr)
			return  nullptr;
		return &value->val;
	}

	void SetValueString(const kfstr& arg)
	{
		auto value = AS_CLASS(KFDkfstr, val);
		if (value == nullptr)
			return;
		value->val = arg;
	}

	const kfstr* Name()
	{
		if (propinfo == nullptr)
			return nullptr;
		return &propinfo->name;
	}

public:
	///属性ID
	uint32 pid;
	///值
	KFDValue* val;
	///属性描述信息
	const KFDProperty* propinfo;
};

class KFDObject : public KFDValue
{
public:
	KFDObject() 
	:	  classid(0)
		, extendval(nullptr)
		, kfd(nullptr)
	{

	}

	virtual ~KFDObject()
	{
		ClearVals(true);
	}

	DEFINE_AS_CLASS(KFDObject)

	KFDObject(KFDObject& from)
	{
		MoveFrom(from);
	}



	inline void ClearVals(bool delinstance = true)
	{
		if (delinstance)
		{
			for (kfVector<KFDValue*>::iterator it = val.begin()
				; it != val.end()
				; it++)
			{
				if (NULL != *it)
				{
					kfDel(*it);
				}
			}

			kfDel(extendval);
			val.clear();
		}
		else
		{
			extendval = nullptr;
			val.clear();
		}

		//kfd = nullptr;
	}

	inline KFDObject& operator=(KFDObject& from)
	{
		MoveFrom(from);
		return *this;
	}

	inline void MoveFrom(KFDObject& from)
	{
		ClearVals(true);

		val.reserve(from.val.size());
		for (KFDValue* tmp:from.val)
		{
			val.push_back(tmp);
		}
		//kfCopy(&val[0], &from.val[0], sizeof(KFDValue*));
		
		classid = from.classid;
		extendval = from.extendval;
		kfd = from.kfd;

		from.ClearVals(false);
	}

	inline void AddVal(KFDValue* v)
	{
		val.push_back(v);
	}

	inline void InsertVal(KFDValue* v, int32 index)
	{
		val.insert(val.begin() + index, v);
	}

	KFDValue* AddArrayVal(const int32 dataType,const kfstr& otype);

	int32 FindVal(KFDValue* vali) 
	{
		int32 i = 0;
		int32 size = ValSize();
		for (;i < size;i += 1)
		{	
			auto item = val[i];
			if (	item != nullptr
				&&	item == vali)
			{
				return i;
			}
		}
		return -1;
	}

	inline void RemoveVal(KFDValue* _val)
	{
		auto i = FindVal(_val);
		if (i >= 0)
			RemoveVal(i);
	}

	inline void RemoveVal(int32 i = -1)
	{
		int32 index = kfSize2I(val.size()) - 1;
		if (index >= 0)
		{
			if (i == -1)
			{
				i = index;
			}
			else if (i > index || i < 0)
			{
				///如果访问超出边界
				return;
			}

			auto v = val[i];
			val.erase(val.begin() + i);
			kfDel(v);
		}
	}

	inline void RemovePropVal(int32 pid)
	{
		if (valueType == OT_OBJECT
			|| valueType == OT_MIXOBJECT)
		{
			for (auto iter = val.begin();
				iter < val.end();
				iter += 1)
			{
				auto item = AS_CLASS(KFDPropertyValue, *iter);
				if (item != nullptr
					&&	item->pid == pid)
				{
					auto v = *iter;
					val.erase(iter);
					kfDel(v);
					break;
				}
			}
		}
	}

	inline void Swapval(int32 i,int32 j)
	{
		std::swap(val[i], val[j]);
	}
	
	const KFD* GetKFD();

	///自动初始化属性
	bool AutoCreatePropertys() override;

	///获取属性
	KFDPropertyValue* GetPropByName(const kfstr& name, bool create = true,bool emptyval = false);

	///获取描述属性 遍历子属性
	KFDPropertyValue* GetDescrPropValue(const kfstr& name, bool create = true, bool emptyval = false);

	///获取继续关系
	KFDObject* GetExtend(bool create = true);

	///通过ID寻找属性
	KFDPropertyValue* FindPropByID(uint32 pid);

	///数组时可用
	uint32 GetOType() 
	{ 
		if(		valueType == OT_ARRAY
			||	valueType == OT_MIXARRAY)
		return classid;
		kfError("不是数组不能获取OTYPE\n");
		return OT_UNKNOW;
	}
	
	///数组时可用
	void SetOType(uint32 type) 
	{
		if (	valueType == OT_ARRAY
			||	valueType == OT_MIXARRAY)
		{
			classid = type;
		}
		else
		{
			kfError("不是数组OBJECT不能设置OTYPE\n");
		}
	}

#pragma region 数组访问形式

	KFDObject* ValueAsObject(int32 index);

	inline KFDValue* GetValueAt(int32 index)
	{
		int32 size = ValSize();
		if (index == -1)
			index = size - 1;
		
		if (index >= 0 && index < size)
		{
			auto vali = val[index];
			
			if (	vali != nullptr 
				&&	valueType == OT_ARRAY
				&&  vali->valueType == OT_OBJECT
				&&	kfd != nullptr)
			{
				auto objval = AS_CLASS(KFDObject, vali);
				if (objval != nullptr)
				{
					objval->kfd = kfd;
				}
			}

			return vali;
		}

		return nullptr;
	}

	/*获取INT值*/
	int32 ValueInt(int32 index)
	{
		auto value = AS_CLASS(KFDNumber32, GetValueAt(index));
		if (value == nullptr)
			return  0;
		return value->numval.intval;
	}

	void SetValueInt(int32 index,int32 arg)
	{
		auto value = AS_CLASS(KFDNumber32, GetValueAt(index));
		if (value == nullptr)
			return;
		value->numval.intval = arg;
	}

	num1 ValueNum1(int32 index)
	{
		auto value = AS_CLASS(KFDNumber32, GetValueAt(index));
		if (value == nullptr)
			return  0;
		return value->numval.fval;
	}


	void SetValueNum1(int32 index,num1 arg)
	{
		auto value = AS_CLASS(KFDNumber32, GetValueAt(index));
		if (value == nullptr)
			return;
		value->numval.fval = arg;
	}

	num2 ValueNum2(int32 index)
	{
		auto value = AS_CLASS(KFDNumber64, GetValueAt(index));
		if (value == nullptr)
			return  0;
		return value->numval.fval;
	}


	void SetValueNum2(int32 index,num2 arg)
	{
		auto value = AS_CLASS(KFDNumber64, GetValueAt(index));
		if (value == nullptr)
			return;
		value->numval.fval = arg;
	}

	int64 ValueInt64(int32 index)
	{
		auto value = AS_CLASS(KFDNumber64, GetValueAt(index));
		if (value == nullptr)
			return  0;
		return value->numval.intval;
	}

	void SetValueInt64(int32 index,int64 arg)
	{
		auto value = AS_CLASS(KFDNumber64, GetValueAt(index));
		if (value == nullptr)
			return;
		value->numval.intval = arg;
	}

	bool ValueBool(int32 index)
	{
		auto value = AS_CLASS(KFDBool, GetValueAt(index));
		if (value == nullptr)
			return  false;
		return value->val;
	}

	void SetValueBool(int32 index,bool arg)
	{
		auto value = AS_CLASS(KFDBool, GetValueAt(index));
		if (value == nullptr)
			return;
		value->val = arg;
	}

	kfstr* ValueString(int32 index)
	{
		auto value = AS_CLASS(KFDkfstr, GetValueAt(index));
		if (value == nullptr)
			return  nullptr;
		return &value->val;
	}

	void SetValueString(int32 index,const kfstr& arg)
	{
		auto value = AS_CLASS(KFDkfstr, GetValueAt(index));
		if (value == nullptr)
			return;
		value->val = arg;
	}
#pragma endregion

#pragma region 对象访问形式

	KFDObject* ValueAsObject(const kfstr& index)
	{
		auto prop = GetPropByName(index);
		return prop == nullptr ? nullptr : prop->ValueAsObject();
	}

	KFDkfBytes* ValueAsBytes(const kfstr& index)
	{
		auto prop = GetPropByName(index);
		return prop == nullptr ? nullptr : prop->ValueAsBytes();
	}

	/*获取INT值*/
	int32 ValueInt(const kfstr& index)
	{
		auto value = GetPropByName(index);
		if (value == nullptr)
			return  0;
		return value->ValueInt();
	}

	void SetValueInt(const kfstr& index, int32 arg)
	{
		auto value = GetPropByName(index);
		if (value == nullptr)
			return;
		value->SetValueInt(arg);
	}

	num1 ValueNum1(const kfstr& index)
	{
		auto value = GetPropByName(index);
		if (value == nullptr)
			return  0;
		return value->ValueNum1();
	}


	void SetValueNum1(const kfstr& index, num1 arg)
	{
		auto value = GetPropByName(index);
		if (value == nullptr)
			return;
		value->SetValueNum1(arg);
	}

	num2 ValueNum2(const kfstr& index)
	{
		auto value = GetPropByName(index);
		if (value == nullptr)
			return  0;
		return value->ValueNum2();
	}


	void SetValueNum2(const kfstr& index, num2 arg)
	{
		auto value = GetPropByName(index);
		if (value == nullptr)
			return;
		value->SetValueNum2(arg);
	}

	int64 ValueInt64(const kfstr& index)
	{
		auto value = GetPropByName(index);
		if (value == nullptr)
			return  0;
		return value->ValueInt64();
	}

	void SetValueInt64(const kfstr& index, int64 arg)
	{
		auto value = GetPropByName(index);
		if (value == nullptr)
			return;
		value->SetValueInt64(arg);
	}

	bool ValueBool(const kfstr& index)
	{
		auto value = GetPropByName(index);
		if (value == nullptr)
			return  false;
		return value->ValueBool();
	}

	void SetValueBool(const kfstr& index, bool arg)
	{
		auto value = GetPropByName(index);
		if (value == nullptr)
			return;
		value->SetValueBool(arg);
	}

	kfstr* ValueString(const kfstr& index)
	{
		auto value = GetPropByName(index);
		if (value == nullptr)
			return  nullptr;
		return value->ValueString();
	}

	void SetValueString(const kfstr& index, const kfstr& arg)
	{
		auto value = GetPropByName(index);
		if (value == nullptr)
			return;
		value->SetValueString(arg);
	}

#pragma endregion



	///创建一个属性值
	static KFDValue* CreateValue(const KFDProperty& propinfo);
	static KFDValue* CreateValue(KFDataType dataType
		,const kfstr& otype
		,const kfstr* defval = nullptr);
	static void DeleteValue(KFDValue* value);

	int32 ValSize() { return kfSize2I(val.size());}

	/*是否有描述文件*/
	inline bool HasDescription()
	{
		if (kfd != nullptr)
			return true;
		if (valueType == OT_ARRAY && classid != 0 && classid <= OT_UINT64)
			return true;
		
		return false;
	}

	virtual KFDValue_UPDATE_FLAG GetUpdateFlag() override 
	{
		if (updateFlag > KFDValue::KFDValue_UPDATE_FLAG_ANY)
		{
			return updateFlag;
		}

		auto iter = val.begin();
		while (iter != val.end())
		{
			auto item = *iter;
			
			if (item->GetUpdateFlag() == KFDValue_UPDATE_FLAG_MODIFY)
			{
				updateFlag = KFDValue::KFDValue_UPDATE_FLAG_NEXT;
				break;
			}

			iter++;
		}

		return updateFlag;
	}

public:

	/*值列表*/
	kfVector<KFDValue*> val;
	
	/*类型id 如果是 arr 则为类型OTYPE*/
	uint32 classid;

	/*类的名称*/
	kfstr classname;

	/*扩展值*/
	KFDObject* extendval;

	/*此类的描述文件*/
	const KFD* kfd;
};

/****
属性路径
****/
class KFDid
{
public:
	KFDid(int32 id = 0)
		: Pid(id)
		, Nextid(NULL)
	{}

	~KFDid()
	{
		Pid = 0;
		kfDel(Nextid);
	}

public:
	///指定一个属性ID
	int32 Pid = 0;
	///子属性描述
	KFDid* Nextid = nullptr;
};

///////定义基础数据定义////////

#define int8Read(buff,prop) prop = buff.ReadByte()
#define uint8Read(buff,prop) prop = buff.ReadUByte()
#define boolRead(buff,prop) prop = buff.ReadBool()
#define int16Read(buff,prop) prop = buff.ReadShort()
#define uint16Read(buff,prop) prop = buff.ReadUShort()
#define int32Read(buff,prop) prop = buff.ReadInt()
#define uint32Read(buff,prop) prop = buff.ReadUInt()
#define int64Read(buff,prop) prop = buff.ReadInt64()
#define uint64Read(buff,prop) prop = buff.ReadUInt64()
#define kfstrRead(buff,prop) buff.ReadString(prop)
#define kfnameRead(buff,prop) {kfstr rs;buff.ReadString(rs);prop=rs;}
#define kfBytesRead(buff,prop) buff.ReadkfBytes(prop)
#define varuintRead(buff,prop) prop = buff.ReadVarUInt()
#define num1Read(buff,prop) prop = buff.ReadFloat()
#define num2Read(buff,prop) prop = buff.ReadDouble()
#define intRead(buff,prop) prop = buff.ReadInt();
#define floatRead(buff,prop) prop = buff.ReadFloat();
#define doubleRead(buff,prop) prop = buff.ReadDouble();

#define int8Write(buff,prop) buff.WriteByte(prop)
#define uint8Write(buff,prop) buff.WriteByte(prop)
#define boolWrite(buff,prop) buff.WriteBool(prop)
#define int16Write(buff,prop) buff.WriteShort(prop)
#define uint16Write(buff,prop) buff.WriteShort(prop)
#define int32Write(buff,prop) buff.WriteInt(prop)
#define uint32Write(buff,prop) buff.WriteUInt(prop)
#define int64Write(buff,prop) buff.WriteInt64(prop)
#define uint64Write(buff,prop) buff.WriteUInt64(prop)
#define kfstrWrite(buff,prop) buff.WriteString(prop)
#define kfnameWrite(buff,prop) buff.WriteString(prop.ToString())
#define kfBytesWrite(buff,prop) buff.WritekfBytes(prop)
#define varuintWrite(buff,prop) buff.WriteVarUInt(prop)
#define num1Write(buff,prop) buff.WriteFloat(prop)
#define num2Write(buff,prop) buff.WriteDouble(prop)
#define intWrite(buff,prop) buff.WriteInt(prop)
#define floatWrite(buff,prop) buff.WriteFloat(prop)
#define doubleWrite(buff,prop) buff.WriteDouble(prop)

#define int8WriteVal(buff,prop) buff.WriteByte(OT_INT8);buff.WriteByte(prop)
#define uint8WriteVal(buff,prop) buff.WriteByte(OT_UINT8);buff.WriteByte(prop)
#define boolWriteVal(buff,prop) buff.WriteByte(OT_BOOL);buff.WriteBool(prop)
#define int16WriteVal(buff,prop) buff.WriteByte(OT_INT16);buff.WriteShort(prop)
#define uint16WriteVal(buff,prop) buff.WriteByte(OT_UINT16);buff.WriteShort(prop)
#define int32WriteVal(buff,prop) buff.WriteByte(OT_INT32);buff.WriteInt(prop)
#define uint32WriteVal(buff,prop) buff.WriteByte(OT_UINT32);buff.WriteUInt(prop)
#define int64WriteVal(buff,prop) buff.WriteByte(OT_INT64);buff.WriteInt64(prop)
#define uint64WriteVal(buff,prop) buff.WriteByte(OT_UINT64);buff.WriteUInt64(prop)
#define kfstrWriteVal(buff,prop) buff.WriteByte(OT_STRING);buff.WriteString(prop)
#define kfnameWriteVal(buff,prop) buff.WriteByte(OT_STRING);buff.WriteString(prop.ToString())
#define kfBytesWriteVal(buff,prop) buff.WriteByte(OT_BYTES);buff.WritekfBytes(prop)
#define varuintWriteVal(buff,prop) buff.WriteByte(OT_VARUINT);buff.WriteVarUInt(prop)
#define num1WriteVal(buff,prop) buff.WriteByte(OT_FLOAT);buff.WriteFloat(prop)
#define num2WriteVal(buff,prop) buff.WriteByte(OT_DOUBLE);buff.WriteDouble(prop)
#define intWriteVal(buff,prop) buff.WriteByte(OT_INT32);buff.WriteInt(prop)
#define floatWriteVal(buff,prop) buff.WriteByte(OT_FLOAT);buff.WriteFloat(prop)
#define doubleWriteVal(buff,prop) buff.WriteByte(OT_DOUBLE);buff.WriteDouble(prop)


//////









NS_KF_END
