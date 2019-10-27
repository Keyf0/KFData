#pragma once

#include "Utils/KFDataType.h"

NS_KF_BEGIN

/*

二进制数据处理类

*/

class KFENGINERT_API KFByteArray
{

public:

	enum KFEndian
	{
		KF_UNKNOW,
		KF_BIG_ENDIAN,
		KF_LITTLE_ENDIAN
	};

	KFByteArray(const int32 buffsize = 0, const bytes buff = NULL);
	KFByteArray(kfBytes& kfbuff);

	void UnAttach(kfBytes& kfbuff);

	virtual ~KFByteArray();

	KFByteArray(const KFByteArray& from)
	{
		CopyFrom(from);
	}

	inline KFByteArray& operator=(const KFByteArray& from)
	{
		CopyFrom(from);
		return *this;
	}

	inline void CopyFrom(const KFByteArray& from)
	{
		this->ResetBuffSize(
			  from._writePos - from._writePos
			, from._buff);
	}

	kfInline void SetEndian(KFEndian endian) { _endian = endian; }
	kfInline KFEndian GetEndian() { return _endian; }

	kfInline bytes GetBuff() { return _buff; }
	kfInline bytes GetWriteBuff() { return _buff + _writePos;}
	kfInline bytes GetReadBuff() { return _buff + _readPos;}
	kfInline int32 GetBuffSize() { return _buffSize; }
	kfInline int32 GetByteSize() { return _writePos; }
	kfInline void SetByteSize(int32 value) { _writePos = value; }
	kfInline int32 GetBuffAvailable() { return _buffSize - _writePos;}

	kfInline bool IncrByteCount(int32 count)
	{
		if (_buffSize - _writePos >= count)
		{
			_writePos += count;
			return true;
		}
		return false;
	}
	kfInline int32 AvailableSize() { return _writePos - _readPos;}

///判定BUFF长度 自动增长BUFF
#define IF_ENOUGH_SIZE(count)							\
bool EnoughSize = true;														\
if(_buffSize - _writePos < count ){EnoughSize = ExtendBuff(count);}	\
if(EnoughSize){
//写入完成的回调
#define ELSE_WRITE_RENTRUN return _writePos;} else return -1;

	kfInline int32 GetPosition() { return _readPos; }
	kfInline void SetPosition(int32 pos) { if (pos <= _writePos) { _readPos = pos;}}
	kfInline void Skip(int32 count) { if (count <= _writePos - _readPos) { _readPos += count;}}


	kfInline int32 WriteBool(bool value)
	{
		IF_ENOUGH_SIZE(1)
			_buff[_writePos] = value ? 1 : 0;
		_writePos += 1;
		ELSE_WRITE_RENTRUN
	}

	kfInline int32 WriteByte(int32 value)
	{
		IF_ENOUGH_SIZE(1)
		_buff[_writePos] = value & 0xff;
		_writePos += 1;
		ELSE_WRITE_RENTRUN
	}


	kfInline int32 WriteNumByteTo(int32 count, int32 value
		,bytes* startbuf = NULL)
	{
		IF_ENOUGH_SIZE(count)
		if (startbuf != NULL)
		{
			*startbuf = _buff + _writePos;
		}
		while (count > 0)
		{
			_buff[_writePos] = value & 0xff;
			_writePos += 1;
			count -= 1;
		}
		ELSE_WRITE_RENTRUN
	}

	kfInline int32 WriteShort(int32 value)
	{
		IF_ENOUGH_SIZE(2)
		if (_endian == KF_BIG_ENDIAN)
		{
			_buff[_writePos] = (value >> 8) & 0xff;
			_buff[_writePos + 1] = value & 0xff;
		}
		else
		{
			_buff[_writePos] = value & 0xff;
			_buff[_writePos + 1] = (value >> 8) & 0xff;
		}
		_writePos += 2;
		ELSE_WRITE_RENTRUN
	}
	
	kfInline int32 WriteDouble(num2 value) 
	{
		IF_ENOUGH_SIZE(8)

		num2_buff valuebuff;
		valuebuff.fval = value;

		if (_endian == SYSTEM_ENDIAN)
		{
			for (int32 i = 0; i < 8; i++)
			{
				_buff[_writePos + i] = valuebuff.buff[i];
			}
		}
		else
		{
			for (int32 i = 0; i < 8; i++)
			{
				_buff[_writePos + i] = valuebuff.buff[7 - i];
			}
		}

		_writePos += 8;
		ELSE_WRITE_RENTRUN
	}

	kfInline int32 WriteFloat(num1 value)
	{
		IF_ENOUGH_SIZE(4)

		num1_buff valuebuff;
		valuebuff.fval = value;

		if (_endian == SYSTEM_ENDIAN)
		{
			for (int32 i = 0; i < 4; i++)
			{
				_buff[_writePos + i] = valuebuff.buff[i];
			}
		}
		else
		{
			for (int32 i = 0; i < 4; i++)
			{
				_buff[_writePos + i] = valuebuff.buff[3 - i];
			}
		}

		_writePos += 4;
		ELSE_WRITE_RENTRUN
	}

	kfInline int32 WriteInt(int32 value)
	{
		return WriteUInt(value);
	}

	kfInline int32 GetVarUIntSize(uint32 value)
	{
		uint32 bit7 = 1 << 7;
		uint32 bit14 = 1 << 14;
		uint32 bit21 = 1 << 21;
		uint32 bit28 = 1 << 28;

		if (value < bit7) return 1;
		else if (value < bit14) return 2;
		else if (value < bit21) return 3;
		else if (value < bit28) return 4;
		else return 5;
	}


	kfInline int32 WriteVarUInt(uint32 value)
	{
		uint32 bit7  = 1 << 7;
		uint32 bit14 = 1 << 14;
		uint32 bit21 = 1 << 21;
		uint32 bit28 = 1 << 28;
		uint32 B = 128;

		if (value < bit7)
		{
			IF_ENOUGH_SIZE(1)
			_buff[_writePos] = value;
			_writePos += 1;
			ELSE_WRITE_RENTRUN
		}
		else if (value < bit14)
		{
			IF_ENOUGH_SIZE(2)
			_buff[_writePos] = value | B;
			_buff[_writePos + 1] = value >> 7;
			_writePos += 2;
			ELSE_WRITE_RENTRUN
		}
		else if (value < bit21)
		{
			IF_ENOUGH_SIZE(3)
			_buff[_writePos] = value | B;
			_buff[_writePos + 1] = (value >> 7) | B;
			_buff[_writePos + 2] = value >> 14;
			_writePos += 3;
			ELSE_WRITE_RENTRUN

		}else if(value < bit28)
		{
			IF_ENOUGH_SIZE(4)
			_buff[_writePos] = value | B;
			_buff[_writePos + 1] = (value >> 7) | B;
			_buff[_writePos + 2] = (value >> 14) | B;
			_buff[_writePos + 3] = value >> 21;
			_writePos += 4;
			ELSE_WRITE_RENTRUN
		}
		else
		{
			IF_ENOUGH_SIZE(5)
			_buff[_writePos] = value | B;
			_buff[_writePos + 1] = (value >> 7) | B;
			_buff[_writePos + 2] = (value >> 14) | B;
			_buff[_writePos + 3] = (value >> 21) | B;
			_buff[_writePos + 4] = value >> 28;
			_writePos += 5;
			ELSE_WRITE_RENTRUN
		}
	}

	kfInline int32 WriteUInt(uint32 value)
	{
		IF_ENOUGH_SIZE(4)
			if (_endian == KF_BIG_ENDIAN)
			{
				_buff[_writePos] = (value >> 24) & 0xff;
				_buff[_writePos + 1] = (value >> 16) & 0xff;
				_buff[_writePos + 2] = (value >> 8) & 0xff;
				_buff[_writePos + 3] = value & 0xff;
			}
			else
			{
				_buff[_writePos] = value & 0xff;
				_buff[_writePos + 1] = (value >> 8) & 0xff;
				_buff[_writePos + 2] = (value >> 16) & 0xff;
				_buff[_writePos + 3] = (value >> 24) & 0xff;
			}
		_writePos += 4;
		ELSE_WRITE_RENTRUN
	}

	kfInline int32 WriteInt64(int64 value)
	{
		return WriteUInt64(value);
	}

	kfInline int32 WriteUInt64(uint64 value)
	{
		IF_ENOUGH_SIZE(8)
			if (_endian == KF_BIG_ENDIAN)
			{
				_buff[_writePos] = (value >> 56) & 0xff;
				_buff[_writePos + 1] = (value >> 48) & 0xff;
				_buff[_writePos + 2] = (value >> 40) & 0xff;
				_buff[_writePos + 3] = (value >> 32) & 0xff;

				_buff[_writePos + 4] = (value >> 24) & 0xff;
				_buff[_writePos + 5] = (value >> 16) & 0xff;
				_buff[_writePos + 6] = (value >> 8) & 0xff;
				_buff[_writePos + 7] = value & 0xff;
			}
			else
			{
				_buff[_writePos] = value & 0xff;
				_buff[_writePos + 1] = (value >> 8) & 0xff;
				_buff[_writePos + 2] = (value >> 16) & 0xff;
				_buff[_writePos + 3] = (value >> 24) & 0xff;

				_buff[_writePos + 4] = (value >> 32) & 0xff;
				_buff[_writePos + 5] = (value >> 40) & 0xff;
				_buff[_writePos + 6] = (value >> 48) & 0xff;
				_buff[_writePos + 7] = (value >> 56) & 0xff;
			}

		_writePos += 8;
		ELSE_WRITE_RENTRUN
	}


	int32 WriteString(const kfstr& str);
	int32 WritekfBytes(const kfBytes& kfbuff);
	int32 WritekfBytes(KFByteArray& kfbuff);

	kfInline int32 WriteBytes(bytes buff, int32 count, int32 offset = 0)
	{
		IF_ENOUGH_SIZE(count)
		/*for (int32 i = 0; i < count; i++)
		{
			_buff[_writePos + i] = buff[offset + i];
		}
		*/
		kfCopy(_buff + _writePos, buff + offset, count);
		_writePos += count;

		ELSE_WRITE_RENTRUN
	}

	kfInline int32 WriteBytes(KFByteArray& byteArr, int32 count, int32 offset = 0)
	{
		return WriteBytes(byteArr.GetBuff(), count, offset);
	}

	///判定是否可以读
#define CAN_READ_BYTES(count) _writePos - _readPos >= count

	kfInline int8 ReadByte()
	{
		int8 val = 0;
		if (CAN_READ_BYTES(1))
		{
			val = _buff[_readPos];
			_readPos += 1;
		}
		return val;
	}

	kfInline bool ReadBool()
	{
		int8 val = 0;
		if (CAN_READ_BYTES(1))
		{
			val = _buff[_readPos];
			_readPos += 1;
		}
		return val == 1;
	}

	kfInline uint8 ReadUByte()
	{
		uint8 val = 0;
		if (CAN_READ_BYTES(1))
		{
			val = _buff[_readPos];
			_readPos += 1;
		}
		return val;
	}

	kfInline int16 ReadShort()
	{
		int16 val = 0;
		if (CAN_READ_BYTES(2))
		{
			if (_endian == KF_BIG_ENDIAN)
			{
				val = (_buff[_readPos] << 8) | _buff[_readPos + 1];
			}
			else
			{
				val = _buff[_readPos] | (_buff[_readPos + 1] << 8);
			}

			_readPos += 2;
		}
		return val;
	}

	kfInline uint16 ReadUShort()
	{
		uint16 val = 0;
		if (CAN_READ_BYTES(2))
		{
			if (_endian == KF_BIG_ENDIAN)
			{
				val = (_buff[_readPos] << 8) | _buff[_readPos + 1];
			}
			else
			{
				val = _buff[_readPos] | (_buff[_readPos + 1] << 8);
			}

			_readPos += 2;
		}
		return val;
	}

	kfInline num2 ReadDouble()
	{
		num2_buff valbuff;
		valbuff.fval = 0.0f;

		if (CAN_READ_BYTES(8))
		{
			if (_endian == SYSTEM_ENDIAN)
			{
				kfCopy(valbuff.buff, _buff + _readPos, 8);
			}
			else
			{
				for (int32 i = 0; i < 8; i++)
				{
					valbuff.buff[i] = _buff[_readPos + 7 - i];
				}
			}
			_readPos += 8;
		}

		return valbuff.fval;
	}

	kfInline num1 ReadFloat()
	{
		num1_buff valbuff;
		valbuff.fval = 0.0f;

		if (CAN_READ_BYTES(4))
		{
			if (_endian == SYSTEM_ENDIAN)
			{
				kfCopy(valbuff.buff, _buff + _readPos, 4);
			}
			else
			{
				for (int32 i = 0; i < 4; i++)
				{
					valbuff.buff[i] = _buff[_readPos + 3 - i];
				}
			}
			_readPos += 4;
		}

		return valbuff.fval;
	}

	kfInline int32 ReadInt()
	{
		return ReadUInt();
	}

	kfInline uint32 ReadUInt()
	{
		uint32 val = 0;
		if (CAN_READ_BYTES(4))
		{
			if (_endian == KF_BIG_ENDIAN)
			{
				val = ((uint32)_buff[_readPos] << 24)
					| ((uint32)_buff[_readPos + 1] << 16)
					| ((uint32)_buff[_readPos + 2] << 8)
					| ((uint32)_buff[_readPos + 3]);
			}
			else
			{
				val = ((uint32)_buff[_readPos + 3] << 24)
					| ((uint32)_buff[_readPos + 2] << 16)
					| ((uint32)_buff[_readPos + 1] << 8)
					| ((uint32)_buff[_readPos]);
			}

			_readPos += 4;
		}
		return val;
	}

	kfInline int64 ReadInt64()
	{
		return ReadUInt64();
	}

	kfInline uint64 ReadUInt64()
	{
		uint64 val = 0;
		if (CAN_READ_BYTES(8))
		{
			if (_endian == KF_BIG_ENDIAN)
			{
				val = ((uint64)_buff[_readPos] << 56)
					| ((uint64)_buff[_readPos + 1] << 48)
					| ((uint64)_buff[_readPos + 2] << 40)
					| ((uint64)_buff[_readPos + 3] << 32)
					| ((uint64)_buff[_readPos + 4] << 24)
					| ((uint64)_buff[_readPos + 5] << 16)
					| ((uint64)_buff[_readPos + 6] << 8)
					| ((uint64)_buff[_readPos + 7]);
			}
			else
			{
				val = (uint64)(_buff[_readPos])
					| ((uint64)_buff[_readPos + 1] << 8)
					| ((uint64)_buff[_readPos + 2] << 16)
					| ((uint64)_buff[_readPos + 3]<< 24)
					| ((uint64)_buff[_readPos + 4] << 32)
					| ((uint64)_buff[_readPos + 5] << 40)
					| ((uint64)_buff[_readPos + 6] << 48)
					| ((uint64)_buff[_readPos + 7] << 56);
			}

			_readPos += 8;
		}

		return val;
	}


	kfInline uint32 ReadVarUInt()
	{
		uint32 B = 128;
		uint32 nextval = ReadUByte();
		uint32 varval = nextval & 127;
		uint32 readtimes = 0;

		while (nextval >= B && readtimes < 4)
		{	
			readtimes += 1;
			nextval = ReadUByte();
			varval = varval | ((nextval & 127) << (7 * readtimes));
		}

		return varval;
	}

	/*还回字符串的长度*/
	int32 ReadString(kfstr& str);
	int32 ReadkfBytes(kfBytes& kfbuff);
	int32 ReadkfBytes(KFByteArray& kfbuff);
	///此方法会叠加到原来字符的后面
	int32 ReadStringLen(kfstr& str,int32 strlen);
	int32 SkipString();

	/*还回是否读取成功*/
	kfInline bool ReadBytes(bytes buff, int32 count, int32 offset = 0)
	{
		if (CAN_READ_BYTES(count))
		{
			kfCopy(buff + offset, _buff + _readPos, count);
			_readPos += count;
		}
		
		return false;
	}
	/*还回是否读取成功 offset = -1 直接向byteArr中写入字节*/
	kfInline bool ReadBytes(KFByteArray& byteArr, int32 count, int32 offset = -1)
	{
		if (offset != -1)
		{
			ReadBytes(byteArr.GetBuff(), count, offset);
		}
		else
		{
			if (CAN_READ_BYTES(count))
			{
				offset = byteArr.GetByteSize();

				if (byteArr.IncrByteCount(count))
				{
					ReadBytes(byteArr.GetBuff(), count, offset);
				}
				else
					return false;
			}
		}

		return true;
	}

	void Clear();
	
	void PrintBuff();

	bool ExtendBuff(int32 count);
	void ResetBuffSize(const int32 buffsize , const bytes buff = NULL);

	static KFEndian GetSystemEndian();

public:
	static int32 DEFAULT_MAX_SIZE;

private:
	void _Init(const int32 buffsize, const bytes buff,const int32 copysize = 0);
private:
	static KFEndian SYSTEM_ENDIAN;

	bytes _buff = nullptr;
	int32 _buffSize = 0;

	int32 _writePos = 0;
	int32 _readPos = 0;
	KFEndian _endian = KFEndian::KF_BIG_ENDIAN;
};

NS_KF_END