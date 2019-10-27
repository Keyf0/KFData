#include "KFDataFormat.h"


NS_KF_BEGIN


KFByteArray KFDataFormat::ClearUseBuff;
kfMap<KFDName, KF::KFDSerialize> KF::KFDataFormat::SerializeAnys;


void KFDataFormat::SkipValue(KFByteArray& bytearr)
{
	ReadValue(bytearr, true);
}

void KFDataFormat::SkipObject(KFByteArray& bytearr)
{
	ReadObjectValue(bytearr,OT_OBJECT,0,"",true);
}

KFDValue* KFDataFormat::ReadValue(KFByteArray& bytearr
	, bool skip /*= false*/
	, KFDValue* val /*= nullptr*/)
{
	KFDValue* retval = val;

	int32 size = bytearr.AvailableSize();

	if (size > 0)
	{
		uint8 valueType = bytearr.ReadUByte();
		
		if (	val != nullptr
			&&	val->valueType != valueType)
		{
			skip = true;
		}

		if (valueType <= OT_UINT64)
		{
			///基础数据类型
			return ReadBaseValue(bytearr,valueType,skip,val);
		}
		else
		{
			switch (valueType)
			{
				case OT_ARRAY :
				case OT_MIXARRAY:
					retval = ReadArrayValue(bytearr, valueType, skip ,val);
					break;
				case OT_OBJECT:
					retval = ReadObjectValue(bytearr, valueType, 0,"", skip ,val);
					break;
				case OT_MIXOBJECT:
				{
					///读取一个类型ID
					uint32 classid = bytearr.ReadVarUInt();
					///如果CLASSID == 1 则需要读取一个字符串
					kfstr clssname;
					if (classid == 1)
					{	
						bytearr.ReadString(clssname);
					}

					retval = ReadObjectValue(bytearr, valueType, classid, clssname, skip, val);
				}
					break;
			default:
				break;
			}
		}
	}

	return retval;
}

#pragma region 基础数据读取

KFDValue* KFDataFormat::ReadBaseValue(KFByteArray& bytearr
	, uint8 valtype
	, bool skip /*= false*/
	, KFDValue* val /*= nullptr*/)
{
	KFDValue* retval = val;

	switch (valtype)
	{
		case OT_UNKNOW :
			break;
		case OT_INT8:
			if (skip) { bytearr.Skip(1); }
			else
			{
				KFDNumber1* knum1 = nullptr;
				
				if (retval != nullptr)
					knum1 = AS_CLASS(KFDNumber1,retval);
				
				if (knum1 == nullptr)
					knum1 = new KFDNumber1();
				
				knum1->numval.intval = bytearr.ReadByte();
				retval = knum1;
			}
			break;
		case OT_UINT8:
			if (skip) { bytearr.Skip(1); }
			else
			{
				KFDNumber1* knum1 = nullptr;

				if (retval != nullptr)
					knum1 = AS_CLASS(KFDNumber1, retval);

				if(knum1 == nullptr)
					knum1 = new KFDNumber1();
				
				knum1->numval.intval = bytearr.ReadUByte();
				retval = knum1;
			}
			break;
		case OT_INT16:
			if (skip) { bytearr.Skip(2); }
			else
			{
				KFDNumber1* knum1 = nullptr;

				if (retval != nullptr)
					knum1 = AS_CLASS(KFDNumber1, retval);

				if (knum1 == nullptr)
					knum1 = new KFDNumber1();

				knum1->numval.intval = bytearr.ReadShort();
				retval = knum1;
			}
			break;
		case OT_UINT16:
			if (skip) { bytearr.Skip(2); }
			else
			{
				KFDNumber1* knum1 = nullptr;

				if (retval != nullptr)
					knum1 = AS_CLASS(KFDNumber1, retval);

				if (knum1 == nullptr)
					knum1 = new KFDNumber1();

				knum1->numval.intval = bytearr.ReadUShort();
				retval = knum1;
			}
			break;
		case OT_INT32:
			if (skip) { bytearr.Skip(4); }
			else
			{
				KFDNumber1* knum1 = nullptr;

				if (retval != nullptr)
					knum1 = AS_CLASS(KFDNumber1, retval);

				if (knum1 == nullptr)
					knum1 = new KFDNumber1();

				knum1->numval.intval = bytearr.ReadInt();
				retval = knum1;
			}
			break;
		case OT_UINT32:
			if (skip) { bytearr.Skip(4); }
			else
			{
				KFDNumber1* knum1 = nullptr;

				if (retval != nullptr)
					knum1 = AS_CLASS(KFDNumber1, retval);

				if (knum1 == nullptr)
					knum1 = new KFDNumber1();

				knum1->numval.intval = bytearr.ReadUInt();
				retval = knum1;
			}
			break;
		case OT_FLOAT:
			if (skip){bytearr.Skip(4);}
			else 
			{ 
				KFDNumber1* knum1 = nullptr;

				if (retval != nullptr)
					knum1 = AS_CLASS(KFDNumber1, retval);

				if (knum1 == nullptr)
					knum1 = new KFDNumber1();

				knum1->numval.fval = bytearr.ReadFloat();
				retval = knum1;
			}
			break;
		case OT_DOUBLE:
			if (skip) { bytearr.Skip(8); }
			else
			{
				KFDNumber2* knum2 = nullptr;
				if (retval != nullptr)
					knum2 = AS_CLASS(KFDNumber2, retval);
				if (knum2 == nullptr)
					knum2 = new KFDNumber2();
				knum2->numval.fval = bytearr.ReadDouble();
				retval = knum2;
			}
			break;
		case OT_STRING:
			if (skip) { bytearr.SkipString();}
			else
			{
				KFDkfstr* strval = nullptr;
				if (retval != nullptr)
					strval = AS_CLASS(KFDkfstr, retval);
				if(strval == nullptr)
					strval = new KFDkfstr();

				bytearr.ReadString(strval->val);
				retval = strval;
			}

			break;
		case OT_NULL:
			break;
		case OT_BYTES:
			if (skip) { bytearr.SkipString(); }
			else
			{
				KFDkfBytes* strval = nullptr;
				if (retval != nullptr)
					strval = AS_CLASS(KFDkfBytes, retval);
				if (strval == nullptr)
					strval = new KFDkfBytes();
				bytearr.ReadkfBytes(strval->val);
				retval = strval;
			}
			break;
		case OT_BOOL:
			if (skip) { bytearr.Skip(1); }
			else
			{
				KFDBool* boolval = nullptr;
				if (retval != nullptr)
					boolval = AS_CLASS(KFDBool, retval);
				if(boolval == nullptr)
					boolval = new KFDBool();
				boolval->val = bytearr.ReadBool();
				retval = boolval;
			}
			break;
		case OT_VARUINT:
			if (skip) { bytearr.ReadVarUInt();}
			else
			{
				KFDNumber1* knum1 = nullptr;

				if (retval != nullptr)
					knum1 = AS_CLASS(KFDNumber1, retval);

				if (knum1 == nullptr)
					knum1 = new KFDNumber1();

				knum1->numval.intval = bytearr.ReadVarUInt();
				retval = knum1;
			}
			break;
		case OT_INT64:
			if (skip) { bytearr.Skip(8);}
			else
			{
				KFDNumber2* knum2 = nullptr;
				if (retval != nullptr)
					knum2 = AS_CLASS(KFDNumber2, retval);
				if (knum2 == nullptr)
					knum2 = new KFDNumber2();

				knum2->numval.intval = bytearr.ReadInt64();
				retval = knum2;
			}
			break;
		case OT_UINT64:
			if (skip) { bytearr.Skip(8); }
			else
			{
				KFDNumber2* knum2 = nullptr;

				if (retval != nullptr)
					knum2 = AS_CLASS(KFDNumber2, retval);
				if (knum2 == nullptr)
					knum2 = new KFDNumber2();

				knum2->numval.intval = bytearr.ReadUInt64();
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

KFDValue* KFDataFormat::ReadObjectValue(
	KFByteArray& bytearr
	, uint8 valtype
	, uint32 classid
	, const kfstr& clsname
	, bool skip /*= false*/
	, KFDValue* val /*= nullptr*/)
{
	KFDObject* retval = AS_CLASS(KFDObject,val);

	if (skip)
	{
		int32 deep = 0;

		while (true)
		{
			///读取PID
			uint32 pid = bytearr.ReadVarUInt();
			
			if (pid == OBJ_PROP_ID_BEGIN)
			{
				deep += 1;
			}
			else if (pid == OBJ_PROP_ID_END)
			{
				deep -= 1;
				if (deep <= 0){break;}
			}
			else
			{
				///读取值
				ReadValue(bytearr, true);
			}
		}
	}
	else
	{
		int32 deep = 0;
		KFDObject* obj = nullptr;

		if (val != nullptr)
			obj = AS_CLASS(KFDObject,val);

		if (obj == nullptr)
		{
			obj = new KFDObject();
			obj->classid = classid;
			obj->classname = clsname;
			obj->valueType = valtype;
		}

		KFDObject* currobj = obj;

		kfVector<KFDObject*> stack;

		while (true)
		{
			///读取PID
			uint32 pid = bytearr.ReadVarUInt();

			if (pid == OBJ_PROP_ID_BEGIN)
			{
				KFDObject* child = nullptr;
				if (deep != 0)
				{
					child = currobj->extendval;
					if (child == nullptr)
					{
						child = new KFDObject();
						child->valueType = OT_OBJECT;
						currobj->extendval = child;
					}
					stack.push_back(currobj);
					currobj = child;
				}

				///读取对象开始
				///obj->val.push_back(new KFDObjProp(pid));
				deep += 1;
			}
			else if (pid == OBJ_PROP_ID_END)
			{
				///读取对象结束
				deep -= 1;

				//obj->val.push_back(new KFDObjProp(pid));
				if (deep <= 0) { break; }
				else
				{
					currobj = stack.back();
					stack.pop_back();
				}
			}
			else
			{
				///读普通的属性
				auto prop = currobj->FindPropByID(pid);
				
				if (prop == nullptr)
				{
					prop = new KFDPropertyValue(pid);
					currobj->val.push_back(prop);
				}

				prop->val = ReadValue(bytearr,false, prop->val);
			}
		}

		retval = obj;
	}

	return retval;
}

#pragma endregion

#pragma region 读取数组

KFDValue* KFDataFormat::ReadArrayValue(KFByteArray& bytearr
	, uint8 valtype
	, bool skip /*= false*/
	, KFDValue* val /*= nullptr*/)
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
			uint32 size = bytearr.ReadVarUInt();
			uint8 oType = bytearr.ReadUByte();

			if (oType <= OT_UINT64) 
			{
				if (skip)
				{
					///如果是普通的数据类型，则直接略过
					for (uint32 i = 0; i < size; i++)
					{
						ReadBaseValue(bytearr, oType, true);
					}
				}
				else
				{
					KFDObject* objarr = retval;
					
					if (objarr == nullptr)
					{
						objarr = new KFDObject();
						objarr->valueType = valtype;
						objarr->SetOType(oType);
					}
					
					for (uint32 i = 0; i < size; i++)
					{
						objarr->val.push_back(ReadBaseValue(bytearr, oType));
					}

					retval = objarr;
				}
			}
			else
			{
				switch (oType)
				{
				case OT_ARRAY:
				case OT_MIXARRAY:
					if (skip)
					{
						for (uint32 i = 0; i < size; i++)
						{
							ReadArrayValue(bytearr, oType, true);
						}
					}
					else
					{
						KFDObject* objarr = retval;

						if (objarr == nullptr)
						{
							objarr = new KFDObject();
							objarr->valueType = valtype;
							objarr->SetOType(oType);
						}

						for (uint32 i = 0; i < size; i++)
						{
							objarr->val.push_back(ReadArrayValue(bytearr, oType));
						}

						retval = objarr;
					}
					break;
				case OT_OBJECT:
				case OT_MIXOBJECT:
				{
					///读取一个类型ID
					uint32 classid = 0;
					kfstr classname;
					///只有MIXOBJECT有实例ID的写入
					if (oType == OT_MIXOBJECT)
					{
						kfWarning("普通数组不支持一个MIXOBJECT对象\n");
						//classid = bytearr.ReadVarUInt();
					}

					if (skip)
					{
						for (uint32 i = 0; i < size; i++)
						{
							ReadObjectValue(bytearr, oType, classid, "", true);
						}
					}
					else
					{
						KFDObject* objarr = retval;

						if (objarr == nullptr)
						{
							objarr = new KFDObject();
							objarr->valueType = valtype;
							objarr->SetOType(oType);
						}

						for (uint32 i = 0; i < size; i++)
						{
							objarr->val.push_back(ReadObjectValue(bytearr, oType, classid, classname));
						}

						retval = objarr;
					}
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
			uint32 size = bytearr.ReadVarUInt();
			if (skip)
			{
				for (uint32 i = 0; i < size; i++)
				{
					ReadValue(bytearr, true);
				}
			}
			else
			{
				KFDObject* objarr = retval;

				if (objarr == nullptr)
				{
					objarr = new KFDObject();
					objarr->valueType = valtype;
				}

				for (uint32 i = 0; i < size; i++)
				{
					objarr->val.push_back(ReadValue(bytearr, false));
				}
				retval = objarr;
			}
		}
	default:
		break;
	}

	return retval;
}

#pragma endregion


#pragma region 向缓存中写入一个值

KFDObject* KFDataFormat::ReadAsObject(KFByteArray& bytearr
	, bool skip /*= false */
	, KFDObject* value /*= nullptr*/)
{
	auto valobj = ReadValue(bytearr, skip, value);
	return AS_CLASS(KFDObject,valobj);
}

void KFDataFormat::WriteValue(KFByteArray& bytearr, KFDValue* val, bool updatewrite/* = false*/)
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
			bytearr.WriteByte(valueType);
			WriteBaseValue(bytearr, valueType, val);
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

						bytearr.WriteByte(OT_OBJECT);
						bytearr.WriteVarUInt(OBJ_PROP_ID_BEGIN);

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
								bytearr.WriteVarUInt(pid);
								WriteValue(bytearr, arritem, updatewrite);
							}
						}

						bytearr.WriteVarUInt(OBJ_PROP_ID_END);
					}
				}
				if(writeflag)
				{
					bytearr.WriteByte(valueType);
					WriteArrayValue(bytearr, valueType, obj);
				}

				val->updateFlag = KFDValue::KFDValue_UPDATE_FLAG_ANY;
			}
				break;
			case OT_OBJECT:
			{
				bytearr.WriteByte(valueType);
				WriteObjectValue(bytearr, valueType, obj, updatewrite);
			}
				break;
			case OT_MIXOBJECT:
			{
				bytearr.WriteByte(valueType);

				///写入类ID吧
				auto kfd = obj->GetKFD();
				if (   kfd != nullptr 
					|| obj->classname.empty() == false)
				{
					bytearr.WriteVarUInt(1);
					bytearr.WriteString(
						kfd != nullptr ?
						kfd->clsname
					: obj->classname);
				}
				else
				{
					bytearr.WriteVarUInt(obj->classid > 1 ?
						obj->classid : 0);
				}
				///写入所有属性吧
				WriteObjectValue(bytearr, valueType , obj, updatewrite);
			}
			break;
			default:
				break;
			}
		}
	}
	else
	{
		bytearr.WriteByte(OT_NULL);
	}
}

void KFDataFormat::WriteBaseValue(KFByteArray& bytearr
	, uint8 dataType
	, KFDValue* val)
{
	val->updateFlag = KFDValue::KFDValue_UPDATE_FLAG_ANY;

	switch (dataType)
	{
	case OT_UNKNOW:
		break;
	case OT_INT8:
		int8Write(bytearr, AS_CLASS(KFDNumber1, val)->numval.intval);
		break;
	case OT_UINT8:
		uint8Write(bytearr, AS_CLASS(KFDNumber1, val)->numval.intval);
		break;
	case OT_INT16:
		int16Write(bytearr, AS_CLASS(KFDNumber1, val)->numval.intval);
		break;
	case OT_UINT16:
		uint16Write(bytearr, AS_CLASS(KFDNumber1, val)->numval.intval);
		break;
	case OT_INT32:
		int32Write(bytearr, AS_CLASS(KFDNumber1, val)->numval.intval);
		break;
	case OT_UINT32:
		uint32Write(bytearr, AS_CLASS(KFDNumber1, val)->numval.intval);
		break;
	case OT_FLOAT:
		num1Write(bytearr, AS_CLASS(KFDNumber1, val)->numval.fval);
		break;
	case OT_DOUBLE:
		num2Write(bytearr, AS_CLASS(KFDNumber2, val)->numval.fval);
		break;
	case OT_STRING:
		kfstrWrite(bytearr, AS_CLASS(KFDkfstr, val)->val);
		break;
	case OT_NULL:
		break;
	case OT_BYTES:
		kfBytesWrite(bytearr, AS_CLASS(KFDkfBytes, val)->val);
		break;
	case OT_BOOL:
		boolWrite(bytearr, AS_CLASS(KFDBool, val)->val);
		break;
	case OT_VARUINT:
		varuintWrite(bytearr, AS_CLASS(KFDNumber1, val)->numval.intval);
		break;
	case OT_INT64:
		int64Write(bytearr, AS_CLASS(KFDNumber2, val)->numval.intval);
		break;
	case OT_UINT64:
		uint64Write(bytearr, AS_CLASS(KFDNumber2, val)->numval.intval);
		break;
	default:
		kfError("不支持的类型[%d]\n", dataType);
		break;
	}
}



void KFDataFormat::WriteObjectValue(KFByteArray& bytearr
	, uint8 dataType
	, KFDObject* val
	, bool updatewrite/* = false*/)
{
	bytearr.WriteVarUInt(OBJ_PROP_ID_BEGIN);
	///写入继承的属性
	if (val->extendval != nullptr)
	{	
		bool writeflag = true;

		if (updatewrite)
		{
			if (val->extendval->GetUpdateFlag() == KFDValue::KFDValue_UPDATE_FLAG_ANY)
			{
				writeflag = false;
			}
		}

		if (writeflag)
		{
			WriteObjectValue(bytearr, dataType, val->extendval, updatewrite);
		}
	}

	///写自己的属性吧
	auto& valarr = val->val;
	for (auto itr = valarr.begin(); itr != valarr.end(); itr ++)
	{
		auto propval = AS_CLASS(KFDPropertyValue, *itr);
		auto pid = propval->pid;

		if (pid != OBJ_PROP_ID_BEGIN
			&& pid != OBJ_PROP_ID_END)
		{
			bool writeflag = true;
			auto valobj = propval->val;

			if (updatewrite)
			{
				if (valobj->GetUpdateFlag() == KFDValue::KFDValue_UPDATE_FLAG_ANY)
				{
					writeflag = false;
				}
			}

			if (writeflag)
			{
				bytearr.WriteVarUInt(pid);
				WriteValue(bytearr, valobj, updatewrite);
			}
		}		
	}
	
	bytearr.WriteVarUInt(OBJ_PROP_ID_END);
	
	///清空标志位
	val->updateFlag = KFDValue::KFDValue_UPDATE_FLAG_ANY;
}

void KFDataFormat::WriteArrayValue(KFByteArray& bytearr
	, uint8 valtype
	, KFDObject* val)
{
	auto&  arrval = val->val;
	uint32 arrsize = kfSize2U(arrval.size());
	bytearr.WriteVarUInt(arrsize);

	uint8 oType = val->GetOType();

	switch (valtype)
	{
		case OT_ARRAY:
		
			if (oType <= OT_UINT64)
			{
				bytearr.WriteByte(oType);
				for (auto vali = arrval.begin()
					; vali != arrval.end()
					; vali += 1)
				{
					WriteBaseValue(bytearr, oType, *vali);
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
					bytearr.WriteByte(oType);
					if (oType == OT_MIXOBJECT)
					{
						///获取第一个元素做为类型ID
						kfWarning("普通数组不支持MIXOBJECT对象\n");
					}

					for (auto vali = arrval.begin()
						; vali != arrval.end()
						; vali += 1)
					{
						auto valobj = AS_CLASS(KFDObject, *vali);
						WriteObjectValue(bytearr, oType, valobj);
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
				WriteValue(bytearr,*vali);
			}

			break;
		default:
			break;
	}
}



#pragma endregion


kfAny KFDataFormat::ReadAny(KFByteArray& bytearr, const KFDName& clsname, kfAny obj /*= nullptr*/)
{
	auto index = SerializeAnys.find(clsname);
	if (index != SerializeAnys.end())
	{
		return index->second.Read(bytearr, obj);
	}

	return obj;
}

bool KFDataFormat::WriteAny(KFByteArray& bytearr, const KFDName& clsname, kfAny obj /*= nullptr*/)
{
	auto index = SerializeAnys.find(clsname);
	if (index != SerializeAnys.end())
	{
		index->second.Write(bytearr, obj);
		return true;
	}
	return false;
}



bool KF::KFDataFormat::DeleteAny(const KFDName& clsname, kfAny obj /*= nullptr*/)
{
	auto index = SerializeAnys.find(clsname);
	if (index != SerializeAnys.end())
	{
		index->second.DeleteAny(obj);
		return true;
	}
	return false;
}

bool KF::KFDataFormat::WriteAnyObject(KFByteArray& buffarr
	, const kfstr& clsname
	, kfAny obj /*= nullptr*/)
{
	if (obj == NULL)
	{
		buffarr.WriteByte(OT_NULL);
		return true;
	}

	buffarr.WriteByte(OT_MIXOBJECT);///objecttype
	buffarr.WriteVarUInt(1);
	buffarr.WriteString(clsname);
	KFDataFormat::WriteAny(buffarr, clsname, obj);

	return true;
}

kfAny KF::KFDataFormat::ReadAnyObject(KFByteArray& buffarr
	, const kfstr& clsname
	, kfAny obj /*= nullptr*/)
{
	uint8 valueType = buffarr.ReadUByte(); /// type is object
	if (valueType == OT_NULL || valueType == 0)
	{
		return nullptr;
	}
	else if (valueType != OT_MIXOBJECT)
	{
		buffarr.Skip(-1);
		KFDataFormat::SkipValue(buffarr);
		return nullptr;
	}

	if (buffarr.ReadVarUInt() == 1)
	{
		kfstr clsname = "";
		buffarr.ReadString(clsname);
		auto anyObj = KFDataFormat::ReadAny(buffarr, clsname, obj);
		return anyObj;
	}
	else
	{
		KFDataFormat::SkipObject(buffarr);
	}

	return NULL;
}

KFDataFormat::KFDataFormat()
	: VER(0)
	, Value(nullptr)
{
}

KFDataFormat::~KFDataFormat()
{
	kfDel(Value);
}

NS_KF_END









