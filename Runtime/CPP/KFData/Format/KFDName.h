#pragma once

#include <type_traits>
#include "Utils/KFDataType.h"

NS_KF_BEGIN

/*
* KFDName字符集合
* 线程不安全
*/

class KFENGINERT_API KFDNameStrings
{
public:

	KFDNameStrings();
	~KFDNameStrings() {}

	int32 GetNameID(const kfstr& namestr);
	const kfstr& GetNameStr(int32 nameid);

	///不理解不要直接操作
	int32 __internal_add_string(const kfstr& namestr)
	{
		return __NewNameID(namestr);
	}

private:

	int32 __NewNameID(const kfstr& namestr);

private:

	///找字符串的索引
	kfMap<kfstr,int32> _Strings2ID;

	///所有字符串的集合
	kfVector<kfstr> _ID2Strings;
};

/*
* 线程不安全 切记
* KFDName如果用在多线程读写上需要单独设计
* 
*/

class KFENGINERT_API KFDName
{

public:
	
	friend struct std::hash<KF::KFDName>;

	KFDName() { _name_id = 0;}
	KFDName(const kfstr& namestr)
	{
		_name_id = namestr.size() == 0 ? 0 : NameStrings().GetNameID(namestr);
	}

	KFDName(const char* namestr)
	{
		_name_id = namestr == NULL ? 0 : NameStrings().GetNameID(namestr);
	}

	
	KFDName(const KFDName& from)
	{
		CopyFrom(from);
	}
	
	virtual ~KFDName() {}

	inline KFDName& operator=(const KFDName& from)
	{
		CopyFrom(from);
		return *this;
	}

	

	
	inline const kfstr& ToString() const
	{
		if (_name_id < 0) return NULLSTR;
		return NameStrings().GetNameStr(_name_id);
	}

	inline const int32 ToValue() const{ return _name_id;}
	inline void SetValue(int32 nameid) { _name_id = nameid;}

	inline const char* c_str() const
	{
		return ToString().c_str();
	}

	inline bool empty() const
	{
		return _name_id == 0;
	}

	inline KFDName& operator=(const kfstr& namestr)
	{
		///重新计算ID
		_name_id = NameStrings().GetNameID(namestr);
		return *this;
	}


	inline const char* operator*()
	{
		return c_str();
	}

	inline KFDName& operator=(const char* namestr)
	{
		_name_id = NameStrings().GetNameID(namestr);
		return *this;
	}

	inline bool operator==(const kfstr& from) const
	{
		///不要增加字符串
		return NameStrings().GetNameStr(_name_id) == from;
	}

	inline bool operator!=(const kfstr& from) const
	{
		///不要增加字符串
		return NameStrings().GetNameStr(_name_id) != from;
	}

	inline bool operator==(const KFDName& from) const
	{
		return this->_name_id == from._name_id;
	}

	inline bool operator!=(const KFDName& from) const
	{
		return this->_name_id != from._name_id;
	}

	inline bool operator==(const int32 from) const
	{
		return _name_id == from;
	}

	inline bool operator!=(const int32 from) const
	{
		return _name_id != from;
	}

	inline void CopyFrom(const KFDName& from)
	{
		this->_name_id = from._name_id;
	}

	static KFDNameStrings& NameStrings();

private:

	int32 _name_id = 0;

};


typedef KFDName kfname;

NS_KF_END

///定义一个取HASH值的方法

namespace std
{
	template <>
	struct std::hash<KF::KFDName>
	{
		int operator()(const KF::KFDName& v) const
		{
			return v._name_id;
		}
	};
}
