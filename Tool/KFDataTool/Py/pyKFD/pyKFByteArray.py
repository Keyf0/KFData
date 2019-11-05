
import struct
from enum import Enum

class Endian(Enum):
    UNKNOW = 0,
    BIG_ENDIAN = 1,
    LITTLE_ENDIAN = 2

class KFByteArray(object):

    _system_endian = 0

    @staticmethod
    def get_system_endian():
        if KFByteArray._system_endian != 0:
            return KFByteArray._system_endian

        test_buff = KFByteArray()
        test_buff.write_int(1)
        if test_buff.buffer[0] == 0:
            KFByteArray._system_endian = Endian.BIG_ENDIAN
        else:
            KFByteArray._system_endian = Endian.LITTLE_ENDIAN
        return KFByteArray._system_endian
        pass

    def __init__(self, buffbytes = None):
        self.buffer = buffbytes
        if self.buffer is None:
            self.buffer = b''
            pass
        self.read_pos = 0

    def get_byte_size(self):
        return len(self.buffer)

    def available_size(self):
        return len(self.buffer) - self.read_pos

    def get_position(self):
        return self.read_pos

    def set_position(self, pos):
        self.read_pos = pos

    def skip(self, count):
        self.read_pos += count

    def skip_string(self):
        len = self.read_varuint()
        self.read_pos += len

    def read_byte(self):
        _s = struct.Struct('>b')
        _r = _s.unpack_from(self.buffer, self.read_pos)[0]
        self.read_pos += _s.size
        return _r

    def read_ubyte(self):
        _s = struct.Struct('>B')
        _r = _s.unpack_from(self.buffer, self.read_pos)[0]
        self.read_pos += _s.size
        return _r

    def read_short(self):
        _s = struct.Struct('>h')
        _r = _s.unpack_from(self.buffer, self.read_pos)[0]
        self.read_pos += _s.size
        return _r

    def read_ushort(self):
        _s = struct.Struct('>H')
        _r = _s.unpack_from(self.buffer, self.read_pos)[0]
        self.read_pos += _s.size
        return _r

    def read_int(self):
        _s = struct.Struct('>i')
        _r = _s.unpack_from(self.buffer, self.read_pos)[0]
        self.read_pos += _s.size
        return _r

    def read_uint(self):
        _s = struct.Struct('>I')
        _r = _s.unpack_from(self.buffer, self.read_pos)[0]
        self.read_pos += _s.size
        return _r

    def read_float(self):
        _s = struct.Struct('>f')
        _r = _s.unpack_from(self.buffer, self.read_pos)[0]
        self.read_pos += _s.size
        return _r

    def read_double(self):
        _s = struct.Struct('>d')
        _r = _s.unpack_from(self.buffer, self.read_pos)[0]
        self.read_pos += _s.size
        return _r

    def read_string(self):
        len = self.read_varuint()  # int32
        if len > 0:
            str = self.buffer[self.read_pos:self.read_pos + len]
            self.read_pos += len
            return bytes.decode(str,encoding='utf-8')
        return ""

    def read_bytesbuff(self):
        len = self.read_varuint()  # int32
        if len > 0:
            buff = self.buffer[self.read_pos:self.read_pos + len]
            self.read_pos += len
            return buff
        return None
        pass


    def read_bool(self):
        _s = struct.Struct('>?')
        _r = _s.unpack_from(self.buffer, self.read_pos)[0]
        self.read_pos += _s.size
        return _r

    def read_varuint(self):
        B = 128  # uint32
        nextval = self.read_ubyte()  # uint32
        varval = nextval & 127  # uint32
        readtimes = 0  # uint32

        while nextval >= B and readtimes < 4:
            readtimes += 1
            nextval = self.read_ubyte()
            varval = varval | ((nextval & 127) << (7 * readtimes))
        return varval

    def read_int64(self):
        _s = struct.Struct('>q')
        _r = _s.unpack_from(self.buffer, self.read_pos)[0]
        self.read_pos += _s.size
        return _r

    def read_uint64(self):
        _s = struct.Struct('>Q')
        _r = _s.unpack_from(self.buffer, self.read_pos)[0]
        self.read_pos += _s.size
        return _r

    def write_byte(self, val):
        if val > 127:
            val = (256 - val) * (-1)
            pass
        self.buffer += struct.pack('>b', val)

    def write_ubyte(self, val):
        self.buffer += struct.pack('>B', val)

    def write_short(self, val):
        self.buffer += struct.pack('>h', val)

    def write_ushort(self, val):
        self.buffer += struct.pack('>H', val)

    def write_int(self, val):
        self.buffer += struct.pack('>i', val)

    def write_uint(self, val):
        self.buffer += struct.pack('>I', val)

    def write_float(self, val):
        self.buffer += struct.pack('>f', val)

    def write_double(self, val):
        self.buffer += struct.pack('>d', val)

    def write_string(self, val):
        if val is None:
            self.write_varuint(0)  # 写入变长字节的字符串长度
        else:
            website_bytes_utf8 = val.encode(encoding="utf-8")
            # 支持一个32位长度的字符串
            count = len(website_bytes_utf8)
            # 写入变长字节的字符串长度
            self.write_varuint(count)
            self.buffer += website_bytes_utf8

    def write_bytesbuff(self,buff):
        if buff is None:
            self.write_varuint(0)  # 写入变长字节的字符串长度
        else:
            # 支持一个32位长度的字符串
            count = len(buff)
            # 写入变长字节的字符串长度
            self.write_varuint(count)
            self.buffer += buff
        pass

    def write_bool(self, val):
        self.buffer += struct.pack('>?', val)

    def write_varuint(self, val):
        bit7 = 1 << 7  # uint32
        bit14 = 1 << 14  # uint32
        bit21 = 1 << 21  # uint32
        bit28 = 1 << 28  # uint32
        B = 128  # uint32
        if val < bit7:
            self.write_ubyte(val)
        elif val < bit14:
            self.write_ubyte((val | B) % 256)
            self.write_ubyte((val >> 7) % 256)
        elif val < bit21:
            self.write_ubyte((val | B) % 256)
            self.write_ubyte(((val >> 7) | B) % 256)
            self.write_ubyte((val >> 14) % 256)
        elif val < bit28:
            self.write_ubyte((val | B) % 256)
            self.write_ubyte(((val >> 7) | B) % 256)
            self.write_ubyte(((val >> 14) | B) % 256)
            self.write_ubyte((val >> 21) % 256)
        else:
            self.write_ubyte((val | B) % 256)
            self.write_ubyte(((val >> 7) | B) % 256)
            self.write_ubyte(((val >> 14) | B) % 256)
            self.write_ubyte(((val >> 21) | B) % 256)
            self.write_ubyte((val >> 28) % 256)

    def write_int64(self, val):
        self.buffer += struct.pack('>q', val)

    def write_uint64(self, val):
        self.buffer += struct.pack('>Q', val)



