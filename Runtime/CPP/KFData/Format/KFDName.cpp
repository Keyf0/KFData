#include "KFDName.h"

NS_KF_BEGIN

KFDNameStrings::KFDNameStrings()
{
	_ID2Strings.push_back("");
}

int32 KFDNameStrings::GetNameID(const kfstr& namestr)
{
	if(namestr.empty()) return 0;

	auto index = _Strings2ID.find(namestr);
	if (index == _Strings2ID.end())
	{
		return __NewNameID(namestr);
	}

	return index->second;
}

int32 KFDNameStrings::__NewNameID(const kfstr& namestr)
{
	_ID2Strings.push_back(namestr);
	///ID=数组的长度-1
	int32 nameid = kfSize2I(_ID2Strings.size()) - 1;
	_Strings2ID.insert(kfPair<kfstr, int32>(
		namestr,nameid
		));
	return nameid;
}

const kfstr& KFDNameStrings::GetNameStr(int32 nameid)
{
	if (nameid < _ID2Strings.size())
	{
		return _ID2Strings[nameid];
	}
	return NULLSTR;
}


KF::KFDNameStrings& KFDName::NameStrings()
{
	static KFDNameStrings _NameStrings;
	return _NameStrings;
}

NS_KF_END





