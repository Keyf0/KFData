#pragma once

#include "KFJson.h"

NS_KF_BEGIN


void jGetKeyStr(kfstr& str, Value *next)
{
	zzz_SIZE len;
	const char* keychar = GetKeyFast(next, &len);
	str.append(keychar, len);
}

void jGetValStr(kfstr& str, Value *next)
{
	zzz_SIZE len;
	const char* keychar = GetStrFast(next, &len);
	str.append(keychar, len);
}

void jGetValInt32(int32& val, Value *next)
{
	zzz_SIZE len;
	const char* num = GetNumFast(next, &len);
	val = atoi(num);
}

void jGetValUInt32(uint32& val, Value *next)
{
	zzz_SIZE len;
	const char* num = GetNumFast(next, &len);
	val = atoll(num) & 0xffffffff;
}

void jGetValFloat(num1& val,Value *next)
{
	val = *GetDouble(next);
}

void jGetValBool(bool& val, Value *next)
{
	auto boolptr = GetBool(next);
	val = boolptr == NULL ? false : ((*boolptr) == True);
}

void jGetObjectValues(
		kfMap<kfstr, Value*>& objvalues
	,	Value *srcv)
{
	const JSONType *t;
	t = Type(srcv);

	if (*t == JSONTYPEOBJECT)
	{
		Value *next = Begin(srcv);
		while (next != 0)
		{
			kfstr key;
			jGetKeyStr(key, next);
			objvalues.insert(kfPair<kfstr, Value*>(key, next));
			next = Next(next);
		}
	}
}

Value* jFindObjectValue(const kfstr& key
	, Value *srcv
	, kfMap<kfstr, Value*> *valuesmap)
{
	if (valuesmap == nullptr)
	{
		const JSONType *t;
		t = Type(srcv);

		if (*t == JSONTYPEOBJECT)
		{
			Value *next = Begin(srcv);
			while (next != 0)
			{
				kfstr name;
				jGetKeyStr(name, next);
				if (name == key)
					return next;
				next = Next(next);
			}
		}
		return nullptr;
	}
	else
	{
		jGetObjectValues(*valuesmap, srcv);
		auto index = valuesmap->find(key);

		if (index != valuesmap->end())
			return index->second;
		else
			return nullptr;
	}
}

NS_KF_END









