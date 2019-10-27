#include "KFDValue.h"

NS_KF_BEGIN


const KFD* KFDObject::GetKFD()
{
	if (kfd == nullptr)
	{
		if (valueType == OT_MIXOBJECT)
		{
			if (false == classname.empty())
			{
				kfd = KFDTable::TB.GetKFD(classname);
			}

			if (kfd == nullptr && classid > 1)
			{
				kfd = KFDTable::TB.GetKFDByClsID(classid);
			}
		}

		if (kfd == nullptr)
		{
			kfError("找不到相应数据描述文件\n");
			return nullptr;
		}

		if (classname.empty())
		{
			classname = kfd->clsname;
		}
	}

	return kfd;
}

bool KF::KFDObject::AutoCreatePropertys()
{
	if (	valueType == OT_MIXOBJECT 
		||	valueType == OT_OBJECT)
	{
		auto tmpkfd = GetKFD();
		if (tmpkfd != nullptr)
		{
			auto& props = tmpkfd->propertys;
			auto propindex = props.begin();
			
			while (propindex != props.end())
			{
				auto propval = GetPropByName(propindex->name);

				auto propobj = propval != nullptr ? propval->ValueAsObject() : nullptr;
				if (propobj != nullptr)
					propobj->AutoCreatePropertys();

				propindex++;
			}

			auto extval = GetExtend(true);
			if (extval != nullptr)
			{
				extval->AutoCreatePropertys();
			}

			return true;
		}
	}
	return false;
}

KFDPropertyValue* KFDObject::GetPropByName(const kfstr& name
	, bool create /*= true*/
	, bool emptyval /*= false*/)
{
	if( GetKFD() == nullptr)
		return nullptr;
	
	KFDPropertyValue* prop = nullptr;

	if (   valueType == OT_OBJECT
		|| valueType == OT_MIXOBJECT)
	{
		///从描述文件中找
		auto propinfo = kfd->FindProperty(name);

		if (propinfo != nullptr)
		{
			uint32 pid = propinfo->id;
			for (auto iter = val.begin();
			iter < val.end();
				iter += 1)
			{
				auto item = AS_CLASS(KFDPropertyValue, *iter);
				if (item != nullptr
					&&	item->pid == pid)
				{
					prop = item;
					break;
				}
			}

			if (prop != nullptr)
			{
				auto index = KFDTable::KFDataTypeMap.find(propinfo->type);
				if (	index == KFDTable::KFDataTypeMap.end() 
					||	index->second != prop->ValueType())
				{
					///类型对不上吧
					RemovePropVal(pid);
					prop = nullptr;
				}
			}

			///创建一个属性
			if (prop == nullptr)
			{
				if (create)
				{
					prop = new KFDPropertyValue(propinfo->id);
					prop->propinfo = propinfo;
					
					///创建值吧...
					if (emptyval == false)
					{
						prop->val = KFDObject::CreateValue(*propinfo);
					}

					val.push_back(prop);
				}
			}
			else if(prop->propinfo == nullptr)
			{
				prop->propinfo = propinfo;
			}
		}
		else
		{
			if (extendval == nullptr)
			{
				auto extkfd = KFDTable::TB.GetKFD(kfd->extname);
				if (create && extkfd != nullptr)
				{
					extendval = new KFDObject();
					extendval->valueType = OT_OBJECT;
					extendval->kfd = extkfd;
				}
			}
			else
			{
				if (extendval->kfd == nullptr)
				{
					extendval->kfd
						= KFDTable::TB.GetKFD(kfd->extname);
				}
			}

			if (extendval != nullptr)
			{
				prop = extendval->GetPropByName(name, create);
			}
		}
	}

	return prop;
}

KFDPropertyValue* KFDObject::GetDescrPropValue(const kfstr& name
	, bool create /*= true*/
	, bool emptyval /*= false*/)
{
	auto propval = GetPropByName(name, create, emptyval);
	if (propval == nullptr)
	{
		auto extval = GetExtend(create);
		if (extval != nullptr)
			propval = extval->GetDescrPropValue(name, create, emptyval);
	}

	return propval;
}

KFDObject* KFDObject::GetExtend(bool create /*= true*/)
{
	if (kfd == nullptr)
	{
		return this->extendval;
	}

	auto& extendcls = kfd->extname;
	const KFD* extendKFD = extendcls == "" ? nullptr : KFDTable::TB.GetKFD(extendcls);;

	if (extendKFD == nullptr)
	{
		if (extendKFD == nullptr)
		{
			kfDel(this->extendval);
			return nullptr;
		}
	}

	if (extendval == nullptr && create)
	{
		extendval = (KFDObject*)KFDObject::CreateValue(OT_OBJECT, extendcls)->asKFDObject();
	}

	if (extendval != nullptr)
		extendval->kfd = extendKFD;

	return extendval;
}


KFDValue* KFDObject::AddArrayVal(const int32 dataType, const kfstr& otype)
{
	if (	valueType == OT_ARRAY
		||	valueType == OT_MIXARRAY)
	{
		auto typeInt = (KFDataType)dataType;
		if (typeInt != OT_UNKNOW)
		{
			auto arrval = KFDObject::CreateValue(typeInt, otype);
			if (arrval != nullptr)
			{
				AddVal(arrval);
			}
			return arrval;
		}
	}

	return nullptr;
}

KFDPropertyValue* KFDObject::FindPropByID(uint32 pid)
{
	KFDPropertyValue* prop = nullptr;
	if (	valueType == OT_OBJECT
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
				return item;
			}
		}
	}
	return prop;
}

KFDObject* KFDObject::ValueAsObject(int32 index)
{
	KFDObject* obj = nullptr;
	auto value = GetValueAt(index);
	if (value != nullptr)
	{
		obj = AS_CLASS(KFDObject, value);
		if (	obj != nullptr
			&&	obj->HasDescription() == false)
		{
			///如果没有描述文件
			if (valueType == OT_ARRAY)
			{
				///如果是普通数组的话数组的
				///KFD即是对象需要的描述文件
				///MIXARR 则不需要，因为每个OBJECT自身有一个CLASSID
				obj->kfd = kfd;
			}
		}
	}

	return obj;
}


KFDValue* KFDObject::CreateValue(const KFDProperty& propinfo)
{
	KFDValue* retval = nullptr;

	auto index = KFDTable::KFDataTypeMap.find(propinfo.type);
	if (index != KFDTable::KFDataTypeMap.end())
	{
		const kfstr* defval = nullptr;
		auto dataType = index->second;
		if (dataType <= OT_UINT64)
		{
			///找出默认值
			if (!propinfo.defval.empty())
			{
				if (!propinfo.enumcls.empty())
				{
					auto kfd = KFDTable::TB.GetKFD(propinfo.enumcls);
					if (kfd != nullptr)
					{
						defval = kfd->GetEnumDefault(propinfo.defval);
						///没有找到还是以本身的数值为准
						if(defval == nullptr)
							defval = &propinfo.defval;
					}
				}
				else
					defval = &propinfo.defval;
			}
		}

		retval = CreateValue(dataType,propinfo.otype, defval);
	}

	return retval;
}


KFDValue* KFDObject::CreateValue(
		KFDataType dataType
	,	const kfstr& otype
	,	const kfstr* defval)
{
	KFDValue* retval = nullptr;
	switch (dataType)
	{
	case OT_UNKNOW:
		break;
	case OT_INT8:
	case OT_UINT8:
	case OT_INT16:
	case OT_UINT16:
	case OT_INT32:
	case OT_UINT32:
	{
		auto newval = new KFDNumber1();
		if (defval != nullptr)
		{
			newval->numval.intval = atoi(defval->c_str());
		}
		retval = newval;
	}
		break;
	case OT_FLOAT:
		retval = new KFDNumber1();
		break;
	case OT_DOUBLE:
		retval = new KFDNumber2();
		break;
	case OT_STRING:
	{
		auto newval = new KFDkfstr();
		if (defval != nullptr)
		{
			newval->val = *defval;
		}
		retval = newval;
	}
		break;
	case OT_NULL:
		break;
	case OT_BYTES:
		retval = new KFDkfBytes();
		break;
	case OT_BOOL:
		retval = new KFDBool();
		break;
	case OT_VARUINT:
		retval = new KFDNumber1();
		break;
	case OT_INT64:
		retval = new KFDNumber2();
		break;
	case OT_UINT64:
		retval = new KFDNumber2();
		break;
	case OT_ARRAY:
	case OT_MIXARRAY:
	{
		auto arrobj = new KFDObject();
			 arrobj->valueType = dataType;

		auto index = KFDTable::KFDataTypeMap.find(otype);

		if (index == KFDTable::KFDataTypeMap.end())
		{
			///不支持数组里嵌套数组
			///不在基础类型的话
			arrobj->kfd = KFDTable::TB.GetKFD(otype);
			arrobj->classid =
				(dataType == OT_MIXARRAY ? OT_MIXOBJECT : OT_OBJECT);
		}
		else
		{
			arrobj->SetOType(index->second);
		}

		retval = arrobj;
		break; 
	}
	case OT_OBJECT:
	case OT_MIXOBJECT:
	{	
		auto obj = new KFDObject();
		auto kfd = KFDTable::TB.GetKFD(otype);

		if (kfd != nullptr)
		{
			obj->classid = kfd->clsid;
			obj->kfd = kfd;
			
			///有承继的类
			if (KFDTable::TB.GetKFD(kfd->extname) != nullptr)
			{
				auto extendval = CreateValue(dataType, kfd->extname);
				obj->extendval = AS_CLASS(KFDObject, extendval);
			}
		}
		
		retval = obj;
		break;
	}
	default:
		kfError("不支持的类型[%d]\n", dataType);
		break;
	}

	if (retval != nullptr)
	{
		retval->valueType = dataType;
	}
	
	return retval;
}

void KFDObject::DeleteValue(KFDValue* value)
{
	kfDel(value);
}

KFDObject* KFDPropertyValue::ValueAsObject()
{
	KFDObject* valobj = nullptr;
	if (	val != nullptr
		&&	val->valueType > OT_UINT64)
	{
		valobj = AS_CLASS(KFDObject, val);
		if (	valobj != nullptr
			&& 	valobj->HasDescription() == false)
		{ 
			///如果是数组除非普通类型，否则也是需要一个类型定义
			if (propinfo != nullptr)
			{
				valobj->kfd = KFDTable::TB.GetKFD(propinfo->otype);
			}
		}
	}
	return valobj;	
}


bool KFDPropertyValue::SetPropValue(KFDValue* newval)
{
	if (newval != nullptr && newval != val)
	{
		///属性不匹配设置不成功
		if (val != nullptr)
		{
			if (val->valueType != newval->valueType)
			{
				kfError("valueType is Error");
				return false;
			}
		}
		else
		{
			auto index = KFDTable::KFDataTypeMap.find(propinfo->type);
			if (index == KFDTable::KFDataTypeMap.end()
				|| index->second != newval->valueType)
			{
				kfError("valueType is Error");
				return false;
			}
		}
	}

	if (newval != val)
	{
		if (val != nullptr)
		{
			kfDel(val);
		}

		this->val = newval;
	}

	return true;
}


NS_KF_END









