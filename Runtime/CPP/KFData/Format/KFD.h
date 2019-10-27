#pragma once

#include "Utils/KFDataType.h"
#include "Utils/KFByteArray.h"

NS_KF_BEGIN

///对象类型 最大255
enum KFDataType
{
	OT_UNKNOW = 0,
	OT_INT8 = 0x01,
	OT_UINT8 = 0x02,
	OT_INT16 = 0x03,
	OT_UINT16 = 0x04,
	OT_INT32 = 0x05,
	OT_UINT32 = 0x06,
	OT_FLOAT = 0x07,
	OT_DOUBLE = 0x08,
	OT_STRING = 0x09,
	OT_NULL = 0x0A,
	OT_BYTES = 0x0B,
	OT_BOOL = 0x0C,
	OT_VARUINT = 0x0D,
	OT_INT64 = 0x0E,
	OT_UINT64 = 0x0F,
	OT_ARRAY = 0x10,//固定数组
	OT_MIXARRAY = 0x11,//混合数组
	OT_OBJECT = 0x12,//固定对象
	OT_MIXOBJECT = 0x13,//混合对象
};

enum KFDPropertyID
{
	OBJ_PROP_ID_BEGIN = 0x7F,
	OBJ_PROP_ID_END = 0,
};

///KFD(C,CLASS=KFDataTypeDef,NOF=1)
///KFD(P=1,NAME=int8,TYPE=kfstr,DEFAULT=int8)
///KFD(P=2,NAME=uint8,TYPE=kfstr,DEFAULT=uint8)
///KFD(P=3,NAME=int16,TYPE=kfstr,DEFAULT=int16)
///KFD(P=4,NAME=uint16,TYPE=kfstr,DEFAULT=uint16)
///KFD(P=5,NAME=int32,TYPE=kfstr,DEFAULT=int32)
///KFD(P=6,NAME=int,TYPE=kfstr,DEFAULT=int)
///KFD(P=7,NAME=uint32,TYPE=kfstr,DEFAULT=uint32)
///KFD(P=8,NAME=num1,TYPE=kfstr,DEFAULT=num1)
///KFD(P=9,NAME=float,TYPE=kfstr,DEFAULT=float)
///KFD(P=10,NAME=num2,TYPE=kfstr,DEFAULT=num2)
///KFD(P=11,NAME=double,TYPE=kfstr,DEFAULT=double)
///KFD(P=12,NAME=kfstr,TYPE=kfstr,DEFAULT=kfstr)
///KFD(P=13,NAME=null,TYPE=kfstr,DEFAULT=null)
///KFD(P=14,NAME=kfBytes,TYPE=kfstr,DEFAULT=kfBytes)
///KFD(P=15,NAME=bool,TYPE=kfstr,DEFAULT=bool)
///KFD(P=16,NAME=varuint,TYPE=kfstr,DEFAULT=varuint)
///KFD(P=17,NAME=int64,TYPE=kfstr,DEFAULT=int64)
///KFD(P=18,NAME=uint64,TYPE=kfstr,DEFAULT=uint64)
///KFD(P=19,NAME=arr,TYPE=kfstr,DEFAULT=arr)
///KFD(P=20,NAME=mixarr,TYPE=kfstr,DEFAULT=mixarr)
///KFD(P=21,NAME=object,TYPE=kfstr,DEFAULT=object)
///KFD(P=22,NAME=mixobject,TYPE=kfstr,DEFAULT=mixobject)
///KFD(*)

/*联动枚举*/
///KFD(C,NOF=1)
class KFDAttachEnum
{
public:
	///KFD(P=1)
	kfstr pname;
	///KFD(P=2)
	kfstr pval;
	///KFD(P=3,NAME=enum,TYPE=kfstr)
	kfstr enumcls;
	///KFD(*)
};


///KFD(C,NOF=1)
class KFDTag
{
public:
	///KFD(P=1)
	kfstr tag;
	///KFD(P=2)
	kfstr val;
	///KFD(*)
};


/*属性定义*/

///KFD(C,NOF=1)
class KFDProperty
{
public:
	KFDProperty() 
		: id(0)
		, OVERRIDE(0)
	{

	}
	~KFDProperty() {}

	const kfstr& GetPropDesName() const
	{
		if (cname.empty())
			return name;
		return cname;
	}

	const KFDTag* GetTag(const kfstr& tag) const
	{
		auto iter = unknowtags.begin();
		while (iter != unknowtags.end())
		{
			if (iter->tag == tag)
			{
				return &(*iter);
			}
			iter += 1;
		}
		return nullptr;
	}

	const kfstr& GetTagVal(const kfstr& tag) const
	{
		auto tagobj = GetTag(tag);
		if (tagobj == nullptr)
		{
			return NULLSTR;
		}
		return tagobj->val;
	}


public:
	/*如果是函数参数 id = 1 引用参数*/
	///KFD(P=1)
	uint32 id;
	///KFD(P=2)
	kfstr name;
	///KFD(P=3)
	kfstr cname;
	///KFD(P=4)
	kfstr des;
	///KFD(P=5,ENUM=KFDataTypeDef)
	kfstr type;
	///KFD(P=6,ENUM=KFDataTypeDef)
	kfstr otype;
	///KFD(P=7,NAME=default,TYPE=kfstr)
	kfstr defval;
	///KFD(P=8)
	kfstr KEY;
	///KFD(P=9,NAME=OR,TYPE=int8)
	int8 OVERRIDE;
	///KFD(P=10,NAME=enum,TYPE=kfstr)
	kfstr enumcls;

	///KFD(P=11,NAME=enumattach,TYPE=arr,OTYPE=KFDAttachEnum)
	kfVector<KFDAttachEnum> attachEnums;

	///KFD(P=11,NAME=unknowtags,TYPE=arr,OTYPE=KFDTag)
	kfVector<KFDTag> unknowtags;

	///KFD(*)
};


/*方法定定义*/

///KFD(C,NOF=1)
class KFDMethod
{
public:

	///方法名称
	///KFD(P=1)
	kfstr name;

	///描述名字
	///KFD(P=2)
	kfstr cname;

	///说明
	///KFD(P=3)
	kfstr des;

	///调用参数
	///KFD(P=4)
	kfVector<KFDProperty> params;

	///返回回参数
	///KFD(P=5)
	KFDProperty retparam;

	///数据结构模版
	///KFD(P=6)
	kfstr templ;

	///KFD(P=7)
	kfstr extend;

	///KFD(P=8)
	bool isstatic = false;

	///KFD(P=9,NAME=unknowtags,TYPE=arr,OTYPE=KFDTag)
	kfVector<KFDTag> unknowtags;


public:

	const KFDTag* GetTag(const kfstr& tag) const
	{
		auto iter = unknowtags.begin();
		while (iter != unknowtags.end())
		{
			if (iter->tag == tag)
			{
				return &(*iter);
			}
			iter += 1;
		}
		return nullptr;
	}

	const kfstr& GetTagVal(const kfstr& tag) const
	{
		auto tagobj = GetTag(tag);
		if (tagobj == nullptr)
		{
			return NULLSTR;
		}
		return tagobj->val;
	}

	///KFD(*)
};

///KFD(C,NOF=1)
class KFD {

public:

	KFD();
	~KFD();

public:

	/*类的名称*/
	///KFD(P=1,NAME=class,TYPE=kfstr)
	kfstr clsname;
	
	/*继承的类*/
	///KFD(P=2,NAME=extend,TYPE=kfstr)
	kfstr extname;

	///KFD(P=3)
	kfstr cname;

	///KFD(P=4)
	kfstr des;
	
	/*可指定一个单独的导出模版*/
	///KFD(P=5,NAME=template,TYPE=kfstr)
	kfstr templ;
	
	/*是否需要导出一个类型说明文件*/
	///KFD(P=6,NAME=typedef,TYPE=int8)
	int8  typed = 0;
	
	/*指定一个ID 可赋值给属性中的KEY*/
	///KFD(P=7,NAME=typeid,TYPE=int32)
	int32 typedid = 0;
	
	/*包含的头文件*/
	///KFD(P=8)
	kfVector<kfstr> includes;

	/*所有的属性定义*/
	///KFD(P=9)
	kfVector<KFDProperty> propertys;
	
	/*重载父级的描述的定义*/
	///KFD(P=10)
	kfVector<KFDProperty> orprops;

	///KFD(P=11)
	uint32 clsid = 0;

	/*不导出解析文件*/
	///KFD(P=12)
	int8 nof = 0;

	///KFD(P=13)
	kfVector<KFDMethod> methods;

	///KFD(P=14,NAME=unknowtags,TYPE=arr,OTYPE=KFDTag)
	kfVector<KFDTag> unknowtags;

	///KFD(*)

public:
	
	const kfstr& GetClassDesName() const
	{
		if (cname.empty())
			return clsname;
		return cname;
	}


	const KFDTag* GetTag(const kfstr& tag) const;

	const kfstr& GetTagVal(const kfstr& tag) const
	{
		auto tagobj = GetTag(tag);
		if (tagobj == nullptr)
		{
			return NULLSTR;
		}
		return tagobj->val;
	}

	///寻找真实的属性
	const KFDProperty* FindProperty(const kfstr& name) const;
	const KFDProperty* FindProperty(const int32 pid) const;

	///获取描述相关属性
	const KFDProperty* GetDescrProperty(const kfstr& name) const;
	const kfstr& GetDescrName(const kfstr& name) const;

	const kfstr* GetEnumDefault(const kfstr& defval) const;
	const int32 GetEnumDefaultInt(const kfstr& defval) const;

	const KFDProperty* GetEnumProperty(int32 value) const;
	const KFDProperty* GetEnumProperty(const kfstr& value) const;

	const kfstr* GetEnumDesName(int32 value) const;
	const kfstr* GetEnumDesName(const kfstr& value) const;

	KFDProperty* AddDynamicProperty(const kfstr& name
		, const kfstr& defval
		, const kfstr& cname = "");

	///获取所有属性的名称
	void GetAllPropNames(kfVector<kfstr>& AllPropNames
		, kfMap<kfstr, bool>* igNames = NULL) const;
};

///数据结构集合
class KFDTable
{
private:

	///所有的数据
	kfMap<kfstr, KFD> _kfds;
	///一个类ID的字典
	kfMap<uint32, KFD*> _clsKFDs;

public:

	~KFDTable() 
	{
		_clsKFDs.clear();
		_kfds.clear(); 
	}
	///解析
	bool DecodeKFDsJson(const kfstr& jsonstr);
	///通过名称获取KFD
	const KFD* GetKFD(const kfstr& clsname);
	///通过CLASSID获取
	const KFD* GetKFDByClsID(const uint32 clsid);
	///
	void AddKFD(const KFD& kfd);

	///找到所有子类定义
	void FindAllChildClass(const kfstr& name,kfVector<const KFD*>& allKDFs);

	bool IsExtend(const kfstr& name, const KFD& kfd);
	bool IsExtend(const kfstr& extendcls, const kfstr& basecls)
	{
		auto kfd = GetKFD(basecls);
		return kfd != nullptr ? IsExtend(extendcls, *kfd) : false;
	}

	///获取第一个
	KFD* Frist();

	const kfMap<uint32, KFD*>& GetKFDMap() { return _clsKFDs; }
	
public:

	static KFDataType GetDataType(const kfstr& tname);

	///所有kfd集合
	static KFDTable TB;
	///字符串对应类型ID
	static kfMap<kfstr, KFDataType> KFDataTypeMap;
};

NS_KF_END
