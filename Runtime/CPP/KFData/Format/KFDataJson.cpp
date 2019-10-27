
#include "KFDataJson.h"
#include "Utils/KFJson.h"
#include "Utils/base64.h"


NS_KF_BEGIN

KFDValue* KFDataJson::ReadValue(
	  Value *next
	, KFDValue* val /*= nullptr*/
	, const KFDProperty* property /*= nullptr*/)
{
	KFDValue* retval = val;
	uint8 valueType = OT_UNKNOW;
	const KFD* kfd = nullptr;

	if (property != nullptr)
	{
		valueType = KFDTable::GetDataType(property->type);
		if (valueType == OT_OBJECT)
		{
			///如果是普通OBJECT则找出KFD
			kfd = KFDTable::TB.GetKFD(property->otype);
			if (kfd == nullptr)
			{
				valueType = OT_UNKNOW;
			}
		}
	}

	if (	valueType == OT_UNKNOW 
		&&	next != nullptr)
	{
		valueType = OT_MIXOBJECT;
	}

	if (valueType != OT_UNKNOW)
	{
		if (valueType <= OT_UINT64)
		{
			///基础数据类型
			return ReadBaseValue(next,valueType,val);
		}
		else
		{
			switch (valueType)
			{
				case OT_ARRAY :
				case OT_MIXARRAY:
					retval = ReadArrayValue(next, valueType , val, property);
					break;
				case OT_OBJECT:
					retval = ReadObjectValue(next, valueType , val, kfd);
					break;
				case OT_MIXOBJECT:
				{
					///找出__cls__
					kfMap<kfstr, Value*> objvaluesmap;
					auto __cls__ = jFindObjectValue("__cls__", next, &objvaluesmap);
					if (__cls__ != nullptr)
					{
						kfstr clsname;
						jGetValStr(clsname, __cls__);
						kfd = KFDTable::TB.GetKFD(clsname);
						if (kfd != nullptr)
						{
							retval = ReadObjectValue(next
								, valueType
								, val
								, kfd
								, &objvaluesmap);
						}
					}
				}
					break;
			default:
				break;
			}
		}
	}

	return retval;
}

KFDObject* KFDataJson::ReadAsObject(const kfstr& jsonstr
	, KFDValue* value /*= nullptr*/)
{
	auto valobj = ReadValue(jsonstr, value);
	return AS_CLASS(KFDObject, valobj);
}

KFDValue* KFDataJson::ReadValue(const kfstr& jsonstr
	, KFDValue* value /*= nullptr*/)
{
	Allocator *A = NewAllocator();
	Value *src_v = NewValue(A);
	zzz_BOOL ret = ParseFast(src_v, jsonstr.c_str());
	if (ret != True)
	{
		kfError("DecodeKFDsJson error!\n");
		return value;
	}
	value = ReadValue(src_v, value);
	ReleaseAllocator(A);
	return value;
}

void KFDataJson::WriteValue(kfstr& outjsonstr
	, KFDValue* value
	, bool updatewrite)
{
	Allocator *A = NewAllocator();
	Value *v = NewValue(A);
	WriteValue(v, value, updatewrite);
	outjsonstr = Stringify(v);
	ReleaseAllocator(A);
}


#pragma region 基础数据读取

KFDValue* KFDataJson::ReadBaseValue(
		Value *next
	, uint8 valtype
	, KFDValue* val /*= nullptr*/)
{
	KFDValue* retval = val;

	switch (valtype)
	{
		case OT_UNKNOW :
			break;
		case OT_INT8:
			{
				KFDNumber1* knum1 = nullptr;
				
				if (retval != nullptr)
					knum1 = AS_CLASS(KFDNumber1,retval);
				
				if (knum1 == nullptr)
					knum1 = new KFDNumber1();

				jGetValInt32(knum1->numval.intval, next);
				retval = knum1;
			}
			break;
		case OT_UINT8:
			{
				KFDNumber1* knum1 = nullptr;

				if (retval != nullptr)
					knum1 = AS_CLASS(KFDNumber1, retval);

				if(knum1 == nullptr)
					knum1 = new KFDNumber1();
				
				jGetValInt32(knum1->numval.intval, next);
				retval = knum1;
			}
			break;
		case OT_INT16:
			{
				KFDNumber1* knum1 = nullptr;

				if (retval != nullptr)
					knum1 = AS_CLASS(KFDNumber1, retval);

				if (knum1 == nullptr)
					knum1 = new KFDNumber1();

				jGetValInt32(knum1->numval.intval, next);
				retval = knum1;
			}
			break;
		case OT_UINT16:
			{
				KFDNumber1* knum1 = nullptr;

				if (retval != nullptr)
					knum1 = AS_CLASS(KFDNumber1, retval);

				if (knum1 == nullptr)
					knum1 = new KFDNumber1();

				jGetValInt32(knum1->numval.intval, next);
				retval = knum1;
			}
			break;
		case OT_INT32:
			{
				KFDNumber1* knum1 = nullptr;

				if (retval != nullptr)
					knum1 = AS_CLASS(KFDNumber1, retval);

				if (knum1 == nullptr)
					knum1 = new KFDNumber1();

				jGetValInt32(knum1->numval.intval, next);
				retval = knum1;
			}
			break;
		case OT_UINT32:
			{
				KFDNumber1* knum1 = nullptr;

				if (retval != nullptr)
					knum1 = AS_CLASS(KFDNumber1, retval);

				if (knum1 == nullptr)
					knum1 = new KFDNumber1();

				jGetValInt32(knum1->numval.intval, next);
				retval = knum1;
			}
			break;
		case OT_FLOAT:
			{ 
				KFDNumber1* knum1 = nullptr;

				if (retval != nullptr)
					knum1 = AS_CLASS(KFDNumber1, retval);

				if (knum1 == nullptr)
					knum1 = new KFDNumber1();

				 jGetValFloat(knum1->numval.fval, next);
				retval = knum1;
			}
			break;
		case OT_DOUBLE:
			{
				KFDNumber2* knum2 = nullptr;
				if (retval != nullptr)
					knum2 = AS_CLASS(KFDNumber2, retval);
				if (knum2 == nullptr)
					knum2 = new KFDNumber2();
				knum2->numval.fval = *GetDouble(next);
				retval = knum2;
			}
			break;
		case OT_STRING:
			{
				KFDkfstr* strval = nullptr;
				if (retval != nullptr)
					strval = AS_CLASS(KFDkfstr, retval);
				if(strval == nullptr)
					strval = new KFDkfstr();

				jGetValStr(strval->val, next);
				retval = strval;
			}

			break;
		case OT_NULL:
			break;
		case OT_BYTES:
			{
			KFDkfBytes* strval = nullptr;
				if (retval != nullptr)
					strval = AS_CLASS(KFDkfBytes, retval);
				if (strval == nullptr)
					strval = new KFDkfBytes();


				zzz_SIZE len;
				const char* keychar = GetStrFast(next, &len);
				base64_decode(keychar, len, strval->val);
				
				retval = strval;
				///不支持JOSN的BYTES
			}
			break;
		case OT_BOOL:
			{
				KFDBool* boolval = nullptr;

				if (retval != nullptr)
					boolval = AS_CLASS(KFDBool, retval);
				if(boolval == nullptr)
					boolval = new KFDBool();

				boolval->val = static_cast<bool>(*GetBool(next));
				retval = boolval;
			}
			break;
		case OT_VARUINT:
			{
				KFDNumber1* knum1 = nullptr;

				if (retval != nullptr)
					knum1 = AS_CLASS(KFDNumber1, retval);

				if (knum1 == nullptr)
					knum1 = new KFDNumber1();
				uint32 uval = 0;
				jGetValUInt32(uval, next);
				knum1->numval.intval = uval;
				retval = knum1;
			}
			break;
		case OT_INT64:
			{
				KFDNumber2* knum2 = nullptr;
				if (retval != nullptr)
					knum2 = AS_CLASS(KFDNumber2, retval);
				if (knum2 == nullptr)
					knum2 = new KFDNumber2();

				knum2->numval.intval = *GetLongLong(next);
				retval = knum2;
			}
			break;
		case OT_UINT64:
			{
				KFDNumber2* knum2 = nullptr;

				if (retval != nullptr)
					knum2 = AS_CLASS(KFDNumber2, retval);
				if (knum2 == nullptr)
					knum2 = new KFDNumber2();

				knum2->numval.intval = *GetLongLong(next);
				retval = knum2;
			}
			break;
	default:
		kfError("不支持的类型[%d]\n", valtype);
		break;
	}

	if (retval != nullptr)
		retval->valueType = valtype;

	return retval;
}

#pragma endregion

#pragma region 读取对象的数据

KFDValue* KFDataJson::ReadObjectValue(
	Value *next
	, uint8 valtype
	, KFDValue* val /*= nullptr*/
	, const KFD* kfd /*= nullptr*/
	, kfMap<kfstr, Value*>* valuemaps /*= nullptr*/)
{
	if (kfd == nullptr)
		return val;

	int32 deep = 0;
	KFDObject* obj = nullptr;
	if (val != nullptr)
	{
		obj = AS_CLASS(KFDObject, val);
	}

	if (obj == nullptr)
	{
		obj = new KFDObject();
		//obj->classid = kfd->clsid; 以后不需要clsid来判定类型
		obj->classname = kfd->clsname;
		obj->valueType = valtype;
	}
	
	kfMap<kfstr, Value*> tmpvalues;
	if (valuemaps == nullptr)
	{
		jGetObjectValues(tmpvalues, next);
		valuemaps = &tmpvalues;
	}

	///========================================
	///读取下父级的
	if (kfd->extname.empty() == false)
	{
		auto extendkfd = KFDTable::TB.GetKFD(kfd->extname);
		if (extendkfd != nullptr)
		{
			auto extendval = ReadObjectValue(next, OT_OBJECT, obj->extendval, extendkfd, valuemaps);
			obj->extendval = AS_CLASS(KFDObject, extendval);
		}
	}

	auto& propertys = kfd->propertys;
	auto endindex = valuemaps->end();

	int32 i = 0;
	while (i < propertys.size())
	{
		auto& propinfo = propertys[i];
		auto pindex = valuemaps->find(propinfo.name);

		if (endindex != pindex)
		{
			///读普通的属性
			auto prop = obj->GetPropByName(propinfo.name);
			if (prop == nullptr)
			{
				prop = new KFDPropertyValue(propinfo.id);
				obj->val.push_back(prop);
			}
			prop->val = ReadValue(pindex->second, prop->val, &propinfo);
		}
		i += 1;
	}

	return obj;
}

#pragma endregion



#pragma region 读取数组

KFDValue* KFDataJson::ReadArrayValue(
		Value *next
	, uint8 valtype
	, KFDValue* val/* = nullptr*/
	, const KFDProperty* property/* = nullptr*/)
{
	KFDObject* retval =  nullptr;
	if (val != nullptr)
	{
		retval = AS_CLASS(KFDObject,val);
	}

	switch (valtype)
	{
		case OT_ARRAY:
		{
			uint8 oType = OT_UNKNOW;
			const KFD* kfd = nullptr;

			if (property != nullptr)
			{
				oType = KFDTable::GetDataType(property->otype);
				if (oType == OT_UNKNOW)
				{
					kfd = KFDTable::TB.GetKFD(property->otype);
					if (kfd != nullptr)
					{
						oType = OT_OBJECT;
					}
				}
			}

			if(oType == OT_UNKNOW)
			{

			}
			else if (oType <= OT_UINT64) 
			{
				KFDObject* objarr = retval;

				if (objarr == nullptr)
				{
					objarr = new KFDObject();
					objarr->valueType = valtype;
					objarr->SetOType(oType);
				}

				const JSONType *t;
				t = Type(next);
				if (t != 0)
				{
					if (*t == JSONTYPEARRAY)
					{
						///解析KFD数组
						Value *nexti = Begin(next);
						while (nexti != 0)
						{
							objarr->val.push_back(ReadBaseValue(nexti, oType));
							nexti = Next(nexti);
						}
					}
				}

				retval = objarr;
			}
			else
			{
				switch (oType)
				{
				case OT_ARRAY:
				case OT_MIXARRAY:
					{
						KFDObject* objarr = retval;

						if (objarr == nullptr)
						{
							objarr = new KFDObject();
							objarr->valueType = valtype;
							objarr->SetOType(oType);
						}

						const JSONType *t;
						t = Type(next);
						if (t != 0)
						{
							if (*t == JSONTYPEARRAY)
							{
								///解析KFD数组
								Value *nexti = Begin(next);
								while (nexti != 0)
								{
									objarr->val.push_back(ReadArrayValue(nexti, oType));
									nexti = Next(nexti);
								}
							}
						}
						
						retval = objarr;
					}
					break;
				case OT_OBJECT:
				case OT_MIXOBJECT:
				{
					KFDObject* objarr = retval;

					if (objarr == nullptr)
					{
						objarr = new KFDObject();
						objarr->valueType = valtype;
						objarr->SetOType(oType);
					}

					const JSONType *t;
					t = Type(next);
					if (t != 0)
					{
						if (*t == JSONTYPEARRAY)
						{
							///解析KFD数组
							Value *nexti = Begin(next);
							while (nexti != 0)
							{
								objarr->val.push_back(ReadObjectValue(nexti, oType, nullptr, kfd));
								nexti = Next(nexti);
							}
						}
					}

					retval = objarr;
				}
				break;
				default:
					break;
				}
			}
		}
		break;
		case OT_MIXARRAY:
		{
			KFDObject* objarr = retval;

			if (objarr == nullptr)
			{
				objarr = new KFDObject();
				objarr->valueType = valtype;
			}

			const JSONType *t;
			t = Type(next);
			if (t != 0)
			{
				if (*t == JSONTYPEARRAY)
				{
					///解析KFD数组
					Value *nexti = Begin(next);
					while (nexti != 0)
					{
						objarr->val.push_back(ReadValue(nexti));
						nexti = Next(nexti);
					}
				}
			}

			retval = objarr;
		}
	default:
		break;
	}

	return retval;
}


#pragma endregion

#pragma region 向缓存中写入一个值


void KFDataJson::WriteValue(
		Value *next
	, KFDValue* val
	, bool updatewrite)
{
	if (val != nullptr)
	{
		KFDValue::KFDValue_UPDATE_FLAG FlAG = KFDValue::KFDValue_UPDATE_FLAG_ANY;

		if (updatewrite)
		{
			FlAG = val->GetUpdateFlag();

			if (FlAG == KFDValue::KFDValue_UPDATE_FLAG_ANY)
			{
				return;
			}
		}

		auto valueType = val->valueType;
		
		if (valueType <= OT_UINT64)
		{
			WriteBaseValue(next, valueType, val);
		}
		else
		{
			auto obj = AS_CLASS(KFDObject, val);

			switch (valueType)
			{
			case OT_ARRAY:
			case OT_MIXARRAY:
			{
				bool writeflag = true;
				if (updatewrite)
				{
					if (FlAG == KFDValue::KFDValue_UPDATE_FLAG_NEXT)
					{
						writeflag = false;
						
						///写入一个修改数组定义
						///定义一个虚拟的OBJECT对象

						SetObj(next);

						auto& arrval = obj->val;
						auto arrsize = kfSize2I(arrval.size());

						for (int32 index = 0
							; index < arrsize
							; index += 1)
						{
							auto arritem = arrval[index];
							if (	arritem->GetUpdateFlag()
								!=	KFDValue::KFDValue_UPDATE_FLAG_ANY)
							{
								///把PID错位过OBJ_PROP_ID_BEGIN
								int32 pid = index + 1;
								if (pid >= OBJ_PROP_ID_BEGIN)
									pid += 1;

								kfstr idstr;
								kfStringFormat(idstr, "%d", pid);

								auto propv = NewValue(next->A);
								SetKey(propv, idstr.c_str());
								WriteValue(propv, arritem, updatewrite);

								ObjAddFast(next, propv);
							}
						}
					}
				}
				if(writeflag)
				{
					SetArray(next);
					WriteArrayValue(next, valueType, obj);
				}

				val->updateFlag = KFDValue::KFDValue_UPDATE_FLAG_ANY;
			}
				break;
			case OT_OBJECT:
			{
				SetObj(next);
				WriteObjectValue(next, valueType, obj, updatewrite);
			}
				break;
			case OT_MIXOBJECT:
			{
				SetObj(next);

				///写入类名
				auto __cls__ = NewValue(next->A);
				SetKeyFast(__cls__, "__cls__");

				auto kfd = obj->GetKFD();
				if (   kfd != nullptr)
				{
					SetStrFast(__cls__, kfd->clsname.c_str());
				}
				ObjAddFast(next, __cls__);
				///写入所有属性吧
				WriteObjectValue(next, valueType , obj, updatewrite);
			}
			break;
			default:
				break;
			}
		}
	}
	else
	{
		SetNull(next);
	}
}


void KFDataJson::WriteBaseValue(
		Value *next
	, uint8 dataType
	, KFDValue* val)
{
	val->updateFlag = KFDValue::KFDValue_UPDATE_FLAG_ANY;

	switch (dataType)
	{
	case OT_UNKNOW:
		break;
	case OT_INT8:
		SetInt(next, AS_CLASS(KFDNumber1, val)->numval.intval);
		break;
	case OT_UINT8:
		SetInt(next, AS_CLASS(KFDNumber1, val)->numval.intval);
		break;
	case OT_INT16:
		SetInt(next, AS_CLASS(KFDNumber1, val)->numval.intval);
		break;
	case OT_UINT16:
		SetInt(next, AS_CLASS(KFDNumber1, val)->numval.intval);
		break;
	case OT_INT32:
		SetInt(next, AS_CLASS(KFDNumber1, val)->numval.intval);
		break;
	case OT_UINT32:
		SetLong(next, (uint32)(AS_CLASS(KFDNumber1, val)->numval.intval));
		break;
	case OT_FLOAT:
		SetDouble(next, AS_CLASS(KFDNumber1, val)->numval.fval);
		break;
	case OT_DOUBLE:
		SetDouble(next, AS_CLASS(KFDNumber2, val)->numval.fval);
		break;
	case OT_STRING:
		SetStrEscape(next, AS_CLASS(KFDkfstr, val)->val.c_str());
		break;
	case OT_NULL:
		break;
	case OT_BYTES:
	{
		kfstr basestr;
		base64_encode(AS_CLASS(KFDkfBytes, val)->val, basestr);
		SetStr(next, basestr.c_str());
	}
		break;
	case OT_BOOL:
		SetBool(next, AS_CLASS(KFDBool, val)->val);
		break;
	case OT_VARUINT:
		SetLong(next, (uint32)(AS_CLASS(KFDNumber1, val)->numval.intval));
		break;
	case OT_INT64:
		SetLongLong(next, AS_CLASS(KFDNumber2, val)->numval.intval);
		break;
	case OT_UINT64:
		SetLongLong(next, (uint64)(AS_CLASS(KFDNumber2, val)->numval.intval));
		break;
	default:
		kfError("不支持的类型[%d]\n", dataType);
		break;
	}
}


void KFDataJson::WriteObjectValue(
	Value *next
	, uint8 dataType
	, KFDObject* val
	, bool updatewrite)
{
	///写入继承的属性
	auto extendval = val->GetExtend(false);
	if (extendval != nullptr)
	{	
		bool writeflag = true;

		if (updatewrite)
		{
			if (extendval->GetUpdateFlag() == KFDValue::KFDValue_UPDATE_FLAG_ANY)
			{
				writeflag = false;
			}
		}

		if (writeflag)
		{
			WriteObjectValue(next
				, dataType
				, extendval
				, updatewrite);
		}
	}

	///写自己的属性吧
	auto kfd = val->GetKFD();
	auto& valarr = kfd->propertys;
	
	for (auto itr = valarr.begin(); itr != valarr.end(); itr ++)
	{
		auto& propinfo = *itr;		
		auto propval = val->GetPropByName(propinfo.name, false);
		
		if (propval != nullptr)
		{
			KFDValue* valobj = propval->val;

			if (propval->ValueType() > OT_UINT64)
			{
				valobj = propval->ValueAsObject();
			}

			bool writeflag = true;

			if (updatewrite)
			{
				if (valobj->GetUpdateFlag() == KFDValue::KFDValue_UPDATE_FLAG_ANY)
				{
					writeflag = false;
				}
			}

			if (writeflag)
			{
				auto propjson = NewValue(next->A);

				SetKeyFast(propjson, propinfo.name.c_str());
				WriteValue(propjson, valobj, updatewrite);

				ObjAddFast(next, propjson);
			}
		}	
	}
	
	///清空标志位
	val->updateFlag = KFDValue::KFDValue_UPDATE_FLAG_ANY;
}

void KFDataJson::WriteArrayValue(
	Value *next
	, uint8 valtype
	, KFDObject* val)
{
	auto&  arrval = val->val;
	uint32 arrsize = kfSize2U(arrval.size());

	uint8 oType = val->GetOType();

	switch (valtype)
	{
		case OT_ARRAY:
		
			if (oType <= OT_UINT64)
			{
				for (auto vali = arrval.begin()
					; vali != arrval.end()
					; vali += 1)
				{
					auto itemval = NewValue(next->A);
					WriteBaseValue(itemval, oType, *vali);
					ArrayAddFast(next, itemval);
				}
			}
			else
			{
				switch (oType)
				{
				case OT_ARRAY:
				case OT_MIXARRAY:
					kfError("暂时不支持多维数组\n");
					break;
				case OT_OBJECT:
				case OT_MIXOBJECT:
				{
					if (oType == OT_MIXOBJECT)
					{
						///获取第一个元素做为类型ID
						kfWarning("普通数组不支持MIXOBJECT对象\n");
					}

					auto arrkfd = val->GetKFD();

					for (auto vali = arrval.begin()
						; vali != arrval.end()
						; vali += 1)
					{
						auto jsonobj = NewValue(next->A);
						SetObj(jsonobj);

						auto valobj = AS_CLASS(KFDObject, *vali);
						valobj->kfd = arrkfd;

						WriteObjectValue(jsonobj, oType, valobj);
						ArrayAddFast(next, jsonobj);
					}
				}
					break;
				default:
					break;
				}
			}

			break;
		case OT_MIXARRAY:

			for (auto vali = arrval.begin()
				; vali != arrval.end()
				; vali += 1)
			{
				auto jsonobj = NewValue(next->A);
				SetObj(jsonobj);

				WriteValue(jsonobj, *vali);
				ArrayAddFast(next, jsonobj);
			}

			break;
		default:
			break;
	}
}


#pragma endregion

NS_KF_END









