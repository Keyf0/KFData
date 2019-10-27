#include "KFByteArray.h"

NS_KF_BEGIN

int32 KFByteArray::DEFAULT_MAX_SIZE = 1024;
KFByteArray::KFEndian KFByteArray::SYSTEM_ENDIAN = KFEndian::KF_UNKNOW;

KFByteArray::KFEndian KFByteArray::GetSystemEndian()
{
	if (SYSTEM_ENDIAN == KFEndian::KF_UNKNOW)
	{
		num1_buff valbuff;
		valbuff.intval = 0x1;
		SYSTEM_ENDIAN = valbuff.buff[0] == 0 ? KFEndian::KF_BIG_ENDIAN : KFEndian::KF_LITTLE_ENDIAN;
	}
	return SYSTEM_ENDIAN;
}

KFByteArray::KFByteArray(
		const int32 buffsize/* = 0*/
	,	const bytes buff /*= NULL*/)
{

	if (SYSTEM_ENDIAN == KFEndian::KF_UNKNOW)
	{
		KFByteArray::GetSystemEndian();
	}

	_Init(buffsize, buff);
}

KFByteArray::KFByteArray(kfBytes& kfbuff)
{
	_buff = kfbuff.buff;
	_buffSize = kfbuff.buffcapacity;
	_writePos = kfbuff.buffsize;

	kfbuff.UnAttach();
}

void KF::KFByteArray::UnAttach(kfBytes& kfbuff)
{
	kfbuff.buff = _buff;
	kfbuff.buffsize = _writePos;
	kfbuff.buffcapacity = _buffSize;

	Clear();

	_buff = nullptr;
	_buffSize = 0;
}

void KFByteArray::_Init(const int32 buffsize
	, const bytes buff
	, const int32 copysize /*= 0*/)
{
	if (_buff != nullptr)
	{
		kfWarning("_buff is not null!");
		return;
	}

	_buffSize = buffsize > 0 ? buffsize : DEFAULT_MAX_SIZE;
	_buff = kfMallocTo(_buffSize, bytes);

	///拷贝初始化数据
	if (buff != NULL)
	{
		int32 copyTosize = copysize == 0 ? _buffSize : copysize;

		kfCopy(_buff, buff, copyTosize);
		_writePos = copyTosize;
	}
}


void KFByteArray::ResetBuffSize(const int32 buffsize, const bytes buff /*= NULL*/)
{
	if (_buffSize < buffsize)
	{
		kfFree(_buff);
		_Init(buffsize, buff);
	}
}

bool KFByteArray::ExtendBuff(int32 count)
{
	int32 i = 2;
	int32 targetBuffSize = _buffSize * i;

	while (targetBuffSize - _writePos < count)
	{
		i += 1;
		targetBuffSize = _buffSize * i;
	}

	if (_writePos > 0)
	{
		auto cachebuff = _buff;
		_buff = NULL;
		_Init(targetBuffSize, cachebuff, _writePos);
		kfFree(cachebuff);
	}
	else
	{
		kfFree(_buff);
		_Init(targetBuffSize,NULL);
	}
	
	return true;
}

KFByteArray::~KFByteArray()
{
	kfFree(_buff);
}


int32 KFByteArray::WriteString(const kfstr& str)
{
	///只支持一个32位长度的字符串
	int32 count = (int32) str.length();
	IF_ENOUGH_SIZE(count + GetVarUIntSize(count))
	
	///写入变长字节的字符串长度
	WriteVarUInt(count);
	///写入字节
	WriteBytes((bytes)str.c_str(), count, 0);

	ELSE_WRITE_RENTRUN
}

int32 KFByteArray::WritekfBytes(const kfBytes& kfbuff)
{
	///只支持一个32位长度的字符串
	int32 count = kfbuff.buffsize;
	IF_ENOUGH_SIZE(count + GetVarUIntSize(count))
	///写入变长字节的字符串长度
	WriteVarUInt(count);
	///写入字节
	WriteBytes(kfbuff.buff, count, 0);

	ELSE_WRITE_RENTRUN
}


int32 KFByteArray::WritekfBytes(KFByteArray& kfbuff)
{
	///只支持一个32位长度的字符串
	int32 count = kfbuff.GetByteSize();
	IF_ENOUGH_SIZE(count + GetVarUIntSize(count))
	///写入变长字节的字符串长度
	WriteVarUInt(count);
	///写入字节
	WriteBytes(kfbuff.GetBuff(), count, 0);

	ELSE_WRITE_RENTRUN
}

int32 KFByteArray::ReadString(kfstr& str)
{
	int32 len = ReadVarUInt();
	///需要先清空下字符串
	str = "";
	return ReadStringLen(str, len);
}


int32 KFByteArray::ReadkfBytes(kfBytes& kfbuff)
{
	int32 len = ReadVarUInt();

	if (CAN_READ_BYTES(len))
	{
		kfbuff.ResetSize(len);
		kfCopy(kfbuff.buff, _buff + _readPos, len);
		_readPos += len;
	}
	else
	{
		kfError("BUFF长度不够\n");
		len = 0;
	}

	return len;
}

int32 KF::KFByteArray::ReadkfBytes(KFByteArray& kfbuff)
{
	int32 len = ReadVarUInt();

	kfbuff.Clear();
	kfbuff.ResetBuffSize(len);

	return ReadBytes(kfbuff, len);
}

int32 KFByteArray::ReadStringLen(kfstr& str, int32 strlen)
{
	if (CAN_READ_BYTES(strlen))
	{
		if (strlen > 0)
		{
			///如果STR不为空则填充下字符串
			str.append((const char*)(_buff + _readPos), strlen);
		}

		_readPos += strlen;
	}
	else
	{
		strlen = 0;
		kfError("BUFF长度不够\n");
	}

	return strlen;
}

int32 KFByteArray::SkipString()
{
	int32 len = ReadVarUInt();
	_readPos += len;
	return len;
}

void KFByteArray::Clear()
{
	this->_readPos = 0;
	this->_writePos = 0;
}

void KFByteArray::PrintBuff()
{
	kfTrace("\n===buffbegin===(%d)", _writePos)
	for (int32 j = 0; j < _writePos; j++)
	{
		if ((j % 16) == 0)
		{
			kfTrace(" <%d>\n %02X", j ,_buff[j]);
		}
		else
		{
			kfTrace(" %02X", _buff[j]);
		}
	}

	kfTrace("\n===buffend===\n")
}

NS_KF_END









