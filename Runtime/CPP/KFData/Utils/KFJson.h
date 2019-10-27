#pragma once

#include "KFDataType.h"
#include "zzzjson.h"

NS_KF_BEGIN

void jGetKeyStr(kfstr& str, Value *next);
void jGetValStr(kfstr& str, Value *next);
void jGetValInt32(int32& val, Value *next);
void jGetValUInt32(uint32& val, Value *next);
void jGetValFloat(num1& val, Value *next);
void jGetValBool(bool& val, Value *next);

void jGetObjectValues(kfMap<kfstr, Value*>& objvalues,Value *srcv);
Value* jFindObjectValue(const kfstr& key, Value *srcv, kfMap<kfstr, Value*> *valuesmap = nullptr);

NS_KF_END







