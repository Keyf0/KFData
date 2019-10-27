#include "KFD.h"
#include "Utils/KFJson.h"

NS_KF_BEGIN

KFDTable KFDTable::TB;
kfMap<kfstr, KFDataType> KFDTable::KFDataTypeMap = 
{
	{"int8",KFDataType::OT_INT8},
	{"uint8",KFDataType::OT_UINT8 },
	{"int16",KFDataType::OT_INT16 },
	{"uint16",KFDataType::OT_UINT16 },
	{"int32",KFDataType::OT_INT32 },
	{"int",KFDataType::OT_INT32 },
	{"uint32",KFDataType::OT_UINT32 },
	{"num1",KFDataType::OT_FLOAT },
	{"float",KFDataType::OT_FLOAT },
	{"num2",KFDataType::OT_DOUBLE },
	{"double",KFDataType::OT_DOUBLE },
	{"kfstr",KFDataType::OT_STRING },
	{"kfname",KFDataType::OT_STRING },
	{"null",KFDataType::OT_NULL },
	{"kfBytes",KFDataType::OT_BYTES },
	{"bool",KFDataType::OT_BOOL },
	{"varuint",KFDataType::OT_VARUINT },
	{"int64",KFDataType::OT_INT64 },
	{"uint64",KFDataType::OT_UINT64 },
	{"arr",KFDataType::OT_ARRAY },
	{"mixarr",KFDataType::OT_MIXARRAY },
	{"object",KFDataType::OT_OBJECT },
	{"mixobject",KFDataType::OT_MIXOBJECT }
};

KFDataType KFDTable::GetDataType(const kfstr& tname)
{
	auto iter = KFDataTypeMap.find(tname);
	if (iter != KFDataTypeMap.end())
	{
		return iter->second;
	}
	return OT_UNKNOW;
}

KFD::KFD()
	: typed(0)
	, typedid(0)
	, clsid(0)
{

}

KFD::~KFD()
{

}

const KF::KFDTag* KF::KFD::GetTag(const kfstr& tag) const
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

	if (!extname.empty())
	{
		auto extkfd = KFDTable::TB.GetKFD(extname);
		if (extkfd != nullptr)
		{
			return extkfd->GetTag(tag);
		}
	}
	return nullptr;
}

const KFDProperty* KFD::FindProperty(const kfstr& name) const
{
	for (auto iter = propertys.begin();
		iter != propertys.end();
		iter += 1)
	{
		if (iter->name == name)
			return &(*iter);
	}

	return nullptr;
}

const KFDProperty* KFD::FindProperty(const int32 pid) const
{
	for (auto iter = propertys.begin();
		iter != propertys.end();
		iter += 1)
	{
		if (iter->id == pid)
			return &(*iter);
	}

	return nullptr;
}

const KFDProperty* KFD::GetDescrProperty(const kfstr& name) const
{
	for (auto iter = orprops.begin();
		iter != orprops.end();
		iter += 1)
	{
		if (iter->name == name)
			return &(*iter);
	}

	auto prop = FindProperty(name);
	if (prop == nullptr && !extname.empty())
	{
		auto extkfd = KFDTable::TB.GetKFD(extname);
		if (extkfd != nullptr)
			prop = extkfd->GetDescrProperty(name);
	}

	return prop;
}

const kfstr& KFD::GetDescrName(const kfstr& name) const
{
	auto propinfo = GetDescrProperty(name);
	if (propinfo != nullptr)
	{
		auto& _cname = propinfo->cname;
		if (_cname.empty())
			return propinfo->name;
		return _cname;
	}

	return name;
}

const kfstr* KFD::GetEnumDefault(const kfstr& defval) const
{
	auto propname = defval;
	auto index = defval.find("::");
	
	if (index != -1)
	{
		propname = defval.substr(index + 2);
	}

	auto propinfo = FindProperty(propname);

	if (propinfo != nullptr)
	{
		return &propinfo->defval;
	}
	else if (!extname.empty())
	{
		auto extkfd = KFDTable::TB.GetKFD(extname);
		if (extkfd != nullptr)
		{
			return extkfd->GetEnumDefault(defval);
		}
	}

	return nullptr;
}


const int32 KFD::GetEnumDefaultInt(const kfstr& defval) const
{
	const kfstr* tmpstr = GetEnumDefault(defval);
	if (tmpstr == nullptr)
		return 0;
	return atoi(tmpstr->c_str());
}

void KFD::GetAllPropNames(
	  kfVector<kfstr>& AllPropNames
	, kfMap<kfstr, bool>* igNames /*= NULL*/) const
{
	for (auto iter = propertys.begin();
		iter != propertys.end();
		iter += 1)
	{
		auto& pname = iter->name;
		if (	igNames == NULL 
			||	igNames->find(pname) == igNames->end())
		{
			AllPropNames.push_back(iter->name);
		}
	}

	if(!extname.empty())
	{
		auto extkfd = KFDTable::TB.GetKFD(extname);
		if (extkfd != nullptr)
		{
			AllPropNames.push_back("");
			extkfd->GetAllPropNames(AllPropNames,igNames);
		}
	}
}

const kfstr* KFD::GetEnumDesName(int32 value) const
{
	auto prop = GetEnumProperty(value);
	if(prop != nullptr)
	{
		if (prop->cname.empty() == false)
			return &prop->cname;
		return &prop->name;
	}
	return nullptr;
}

const kfstr* KFD::GetEnumDesName(const kfstr& value) const
{
	auto prop = GetEnumProperty(value);
	if (prop != nullptr)
	{
		if (prop->cname.empty() == false)
			return &prop->cname;
		return &prop->name;
	}
	return nullptr;
}

KFDProperty* KFD::AddDynamicProperty(const kfstr& name
	, const kfstr& defval
	, const kfstr& cname/* = ""*/)
{
	if (nullptr == FindProperty(name))
	{
		propertys.push_back(KFDProperty());

		KFDProperty& kfdProperty = propertys.back();

		kfdProperty.name = name;
		kfdProperty.defval = defval;
		kfdProperty.cname = cname;

		return &kfdProperty;
	}

	return nullptr;
}

const KFDProperty* KF::KFD::GetEnumProperty(int32 value) const
{
	for (kfsize i = 0; i < propertys.size(); ++i)
	{
		const KFDProperty* prop = &propertys[i];
		if(atoi(prop->defval.c_str()) == value)
		{
			return prop;
		}
	}
	return nullptr;
}

const KFDProperty* KFD::GetEnumProperty(const kfstr& value) const
{
	for (kfsize i = 0; i < propertys.size(); ++i)
	{
		const KFDProperty* prop = &propertys[i];
		if (prop->defval == value)
		{
			return prop;
		}
	}
	return nullptr;
}

/*KFDTable集合*/

#pragma region JSON解析过程



/*
// 深复制函数，这里命名为GetAndSet更直观
void GetAndSet(Value *srcv, Value *desv)
{
	// 获取值的类型
	const JSONType *t;
	t = Type(srcv);
	if (t == 0)
		return;
	switch (*t)
	{
	case JSONTYPEARRAY:
	{
		// 如果是数组，则把当前值设为数组，然后遍历并复制数组中的每个值
		SetArray(desv);
		Value *next = Begin(srcv);
		while (next != 0)
		{
			Value *v = NewValue(desv->A);
			GetAndSet(next, v);
			if (ArrayAddFast(desv, v) != True)
				return;
			next = Next(next);
		}
		break;
	}
	case JSONTYPEOBJECT:
	{
		// 如果是对象，则把当前值设为对象，然后遍历并复制对象中的每个值
		SetObj(desv);
		Value *next = Begin(srcv);
		while (next != 0)
		{
			Value *v = NewValue(desv->A);
			SetKeyFast(v, GetKey(next));
			GetAndSet(next, v);
			if (ObjAddFast(desv, v) != True)
				return;
			next = Next(next);
		}
		break;
	}
	case JSONTYPEBOOL:
	{
		// 如果是布尔值，则获取并复制该值
		const zzz_BOOL *b = GetBool(srcv);
		if (b == 0)
			return;
		SetBool(desv, *b);
		break;
	}
	case JSONTYPENULL:
	{
		// 如果是空，则复制该值
		if (IsNull(srcv) == False)
			return;
		SetNull(desv);
		break;
	}

	case JSONTYPESTRING:
	{
		// 如果是字符串，则获取并复制该字符串
		const char *str = GetStr(srcv);
		if (str == 0)
			return;
		// 如果需要拷贝字符串，则需要使用SetStr
		if (SetStrFast(desv, str) != True)
			return;
		break;
	}
	case JSONTYPENUMBER:
	{
		// 如果是数字，则获取并复制该数字
		const char *str = GetNumStr(srcv);
		if (str == 0)
			return;
		// 如果需要拷贝数字，则需要使用SetNumStr
		if (SetNumStrFast(desv, str) != True)
			return;
		break;
	}
	}
}

*/

void DecodeStrArr(kfVector<kfstr>& strarr, Value *srcv)
{
	const JSONType *t;
	t = Type(srcv);

	if (*t == JSONTYPEARRAY)
	{
		Value *next = Begin(srcv);
		while (next != 0)
		{
			kfstr str;
			jGetValStr(str, next);
			strarr.push_back(str);
			next = Next(next);
		}
	}
}


bool DecodeKFDTag(KFDTag& tag, Value *srcv)
{
	bool hasProp = false;
	const JSONType *t;
	t = Type(srcv);

	if (*t == JSONTYPEOBJECT)
	{
		Value *next = Begin(srcv);
		while (next != 0)
		{
			kfstr key;
			jGetKeyStr(key, next);

			if (key == "tag")
			{
				jGetValStr(tag.tag, next);
				hasProp = true;
			}
			else if (key == "val")
			{
				jGetValStr(tag.val, next);
			}

			next = Next(next);
		}
	}

	return hasProp;
}

void DecodeKFDTags(kfVector<KFDTag>& tagarr
	, Value *srcv)
{
	const JSONType *t;
	t = Type(srcv);

	if (*t == JSONTYPEARRAY)
	{
		Value *next = Begin(srcv);
		while (next != 0)
		{
			KFDTag tag;

			if (DecodeKFDTag(tag, next))
			{
				tagarr.push_back(tag);
			}

			next = Next(next);
		}
	}
}

bool DecodeKFDAttachEnum(KFDAttachEnum& prop, Value *srcv)
{
	bool hasProp = false;
	const JSONType *t;
	t = Type(srcv);

	if (*t == JSONTYPEOBJECT)
	{
		Value *next = Begin(srcv);
		while (next != 0)
		{
			kfstr key;
			jGetKeyStr(key, next);

			if (key == "pname")
			{
				jGetValStr(prop.pname, next);
				hasProp = true;
			}
			else if (key == "pval")
			{
				jGetValStr(prop.pval, next);
			}
			else if (key == "enum")
			{
				jGetValStr(prop.enumcls, next);
			}
			
			next = Next(next);
		}
	}

	return hasProp;
}


void DecodeKFDAttachEnums(kfVector<KFDAttachEnum>& proparr
	, Value *srcv)
{
	const JSONType *t;
	t = Type(srcv);

	if (*t == JSONTYPEARRAY)
	{
		Value *next = Begin(srcv);
		while (next != 0)
		{
			KFDAttachEnum info;

			if (DecodeKFDAttachEnum(info, next))
			{
				proparr.push_back(info);
			}

			next = Next(next);
		}
	}
}

bool DecodeKFDPropInfo(KFDProperty& prop, Value *srcv)
{
	bool hasProp = false;
	const JSONType *t;
	t = Type(srcv);

	if (*t == JSONTYPEOBJECT)
	{
		Value *next = Begin(srcv);
		while (next != 0)
		{
			kfstr key;
			jGetKeyStr(key, next);

			if (key == "id")
			{
				hasProp = true;
				jGetValUInt32(prop.id, next);
			}
			else if (key == "name")
			{
				jGetValStr(prop.name, next);
			}
			else if (key == "cname")
			{
				jGetValStr(prop.cname, next);
			}
			else if (key == "des")
			{
				jGetValStr(prop.des, next);
			}
			else if (key == "type")
			{
				jGetValStr(prop.type, next);
			}
			else if (key == "otype")
			{
				jGetValStr(prop.otype, next);
			}
			else if (key == "default")
			{
				jGetValStr(prop.defval, next);
			}
			else if (key == "KEY")
			{
				jGetValStr(prop.KEY, next);
			}
			else if (key == "OR")
			{
				int32 ival;
				jGetValInt32(ival, next);
				prop.OVERRIDE = ival & 0xff;
			}
			else if (key == "enum")
			{
				jGetValStr(prop.enumcls, next);
			}
			else if (key == "enumattach")
			{
				DecodeKFDAttachEnums(prop.attachEnums, next);
			}
			else if (key == "unknowtags")
			{
				///解析unknowTags
				DecodeKFDTags(prop.unknowtags, next);
			}

			next = Next(next);
		}
	}

	return hasProp;
}

void DecodeKFDPropertys(kfVector<KFDProperty>& proparr, Value *srcv)
{
	const JSONType *t;
	t = Type(srcv);

	if (*t == JSONTYPEARRAY)
	{
		Value *next = Begin(srcv);
		while (next != 0)
		{
			KFDProperty info;
			
			if (DecodeKFDPropInfo(info, next))
			{
				proparr.push_back(info);
			}

			next = Next(next);
		}
	}
}



bool DecodeKFDMethod(KFDMethod& method, Value *srcv)
{
	bool hasProp = false;
	const JSONType *t;
	t = Type(srcv);

	if (*t == JSONTYPEOBJECT)
	{
		Value *next = Begin(srcv);
		while (next != 0)
		{
			kfstr key;
			jGetKeyStr(key, next);

			if (key == "name")
			{
				jGetValStr(method.name, next);
				hasProp = true;
			}
			else if (key == "cname")
			{
				jGetValStr(method.cname, next);
			}
			else if (key == "des")
			{
				jGetValStr(method.des, next);
			}
			else if (key == "extend")
			{
				jGetValStr(method.extend, next);
			}
			else if (key == "templ")
			{
				jGetValStr(method.templ, next);
			}
			else if (key == "retparam")
			{
				DecodeKFDPropInfo(method.retparam, next);
			}
			else if (key == "params")
			{
				DecodeKFDPropertys(method.params, next);
			}
			else if (key == "isstatic")
			{
				jGetValBool(method.isstatic, next);
			}
			else if (key == "unknowtags")
			{
				///解析unknowTags
				DecodeKFDTags(method.unknowtags, next);
			}

			next = Next(next);
		}
	}

	return hasProp;
}


void DecodeKFDMethods(kfVector<KFDMethod>& methodarr, Value *srcv)
{
	const JSONType *t;
	t = Type(srcv);

	if (*t == JSONTYPEARRAY)
	{
		Value *next = Begin(srcv);
		while (next != 0)
		{
			KFDMethod method;

			if (DecodeKFDMethod(method, next))
			{
				methodarr.push_back(method);
			}

			next = Next(next);
		}
	}
}

bool DecodeKFD(Value *srcv, KFD& kfd)
{
	bool hasClass = false;
	const JSONType *t;
	t = Type(srcv);
	
	if (*t == JSONTYPEOBJECT)
	{
		Value *next = Begin(srcv);
		while (next != 0)
		{
			kfstr key;
			jGetKeyStr(key, next);

			if (key == "class")
			{
				hasClass = true;
				jGetValStr(kfd.clsname,next);
			}
			else if (key == "extend")
			{
				jGetValStr(kfd.extname ,next);
			}
			else if (key == "template")
			{
				jGetValStr(kfd.templ, next);
			}
			else if (key == "typedef")
			{
				int32 intval;
				jGetValInt32(intval, next);
				kfd.typed = intval & 0xff;
			}
			else if (key == "typeid")
			{
				jGetValInt32(kfd.typedid, next);
			}
			else if (key == "includes")
			{
				DecodeStrArr(kfd.includes, next);
			}
			else if (key == "cname")
			{
				jGetValStr(kfd.cname, next);
			}
			else if (key == "des")
			{
				jGetValStr(kfd.des, next);
			}
			else if (key == "propertys")
			{
				DecodeKFDPropertys(kfd.propertys, next);
			}
			else if (key == "orprops")
			{
				DecodeKFDPropertys(kfd.orprops, next);
			}
			else if (key == "methods")
			{
				DecodeKFDMethods(kfd.methods, next);
			}
			else if (key == "clsid")
			{
				jGetValUInt32(kfd.clsid, next);
			}
			else if (key == "nof")
			{
				int32 val = 0;
				jGetValInt32(val, next);
				kfd.nof = val&0xff;
			}
			else if (key == "unknowtags")
			{
				///解析unknowTags
				DecodeKFDTags(kfd.unknowtags, next);
			}

			next = Next(next);
		}
	}

	return hasClass;
}
#pragma endregion


bool KFDTable::DecodeKFDsJson(const kfstr& jsonstr)
{
	Allocator *A = NewAllocator();
	Value *src_v = NewValue(A);
	//Value *des_v = NewValue(A);

	zzz_BOOL ret = ParseFast(src_v, jsonstr.c_str());

	if (ret != True)
	{
		kfError("DecodeKFDsJson error!\n");
		return false;
	}

	// 获取值的类型
	const JSONType *t;
	t = Type(src_v);
	if (t != 0)
	{
		if (*t == JSONTYPEARRAY)
		{	
			///解析KFD数组
			Value *next = Begin(src_v);
			while (next != 0)
			{
				KFD kfd;
				if (DecodeKFD(next, kfd))
				{
					AddKFD(kfd);
				}
				next = Next(next);
			}
		}
		else if (*t == JSONTYPEOBJECT)
		{
			///也可以解析只是一个OBJECT的KFD文件
			Value *next = Begin(src_v);
			if (next != 0)
			{
				KFD kfd;
				if (DecodeKFD(next, kfd))
				{
					AddKFD(kfd);
				}
			}
		}
	}

	ReleaseAllocator(A);
	return true;
}

const KFD* KFDTable::GetKFD(const kfstr& clsname)
{
	if(clsname.empty()) return null;
	auto index = _kfds.find(clsname);
	if(index == _kfds.end())
		return nullptr;
	return &index->second;
}

const KFD* KFDTable::GetKFDByClsID(const uint32 clsid)
{
	auto index = _clsKFDs.find(clsid);
	if (index == _clsKFDs.end())
		return nullptr;
	return index->second;
}

void KFDTable::AddKFD(const KFD& kfd)
{
	_kfds.insert(std::pair<kfstr, KFD>(kfd.clsname, kfd));
	auto kfdptr = &_kfds[kfd.clsname];
	_clsKFDs.insert(std::pair<uint32, KFD*>(kfd.clsid,kfdptr));
}

void KFDTable::FindAllChildClass(
	  const kfstr& name
	, kfVector<const KFD*>& allKDFs)
{
	auto index = _kfds.begin();
	while (index != _kfds.end())
	{
		auto& kfd = index->second;
		if (IsExtend(name, kfd))
		{
			allKDFs.push_back(&kfd);
		}
		index ++;
	}
}

bool  KFDTable::IsExtend(const kfstr& name, const KFD& kfd)
{
	if (kfd.clsname == name)
		return true;

	auto& extname = kfd.extname;
	if (extname.empty()) 
		return false;
	
	auto extkfd = GetKFD(extname);
	if (extkfd != nullptr)
		return IsExtend(name, *extkfd);

	return false;
}

KFD* KFDTable::Frist()
{
	if (_kfds.empty())
		return nullptr;

	return &_kfds.begin()->second;
}

NS_KF_END











