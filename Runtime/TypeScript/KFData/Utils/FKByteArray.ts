
///二进制处理器

export class Endian
{
    public static LITTLE_ENDIAN: string = "littleEndian";
    public static BIG_ENDIAN: string = "bigEndian";
}

export const enum EndianConst
{
    LITTLE_ENDIAN = 1,
    BIG_ENDIAN = 2
}

const enum ByteArraySize
{
    SIZE_OF_BOOLEAN = 1,

    SIZE_OF_INT8 = 1,

    SIZE_OF_INT16 = 2,

    SIZE_OF_INT32 = 4,

    SIZE_OF_UINT8 = 1,

    SIZE_OF_UINT16 = 2,

    SIZE_OF_UINT32 = 4,

    SIZE_OF_INT64 = 8,

    SIZE_OF_UINT64 = 8,

    SIZE_OF_FLOAT32 = 4,

    SIZE_OF_FLOAT64 = 8
}


let ByteException = function ()
{
    console.exception("ByteException");
}

let ByteError = function (code:number,...args)
{
    console.error("ByteError:" + code);
}


export class KFByteArray
{
    protected bufferExtSize = 0;//Buffer expansion size
    protected data: DataView;
    protected _bytes: Uint8Array;
    protected _position: number;
    protected write_position: number;

    public get endian() {
        return this.$endian == EndianConst.LITTLE_ENDIAN ? Endian.LITTLE_ENDIAN : Endian.BIG_ENDIAN;
    }

    public set endian(value: string) {
        this.$endian = value == Endian.LITTLE_ENDIAN ? EndianConst.LITTLE_ENDIAN : EndianConst.BIG_ENDIAN;
    }

    protected $endian: EndianConst;

    constructor(buffer?: ArrayBuffer | Uint8Array, bufferExtSize = 0) {
        if (bufferExtSize < 0) {
            bufferExtSize = 0;
        }
        this.bufferExtSize = bufferExtSize;
        let bytes: Uint8Array, wpos = 0;
        if (buffer) {//有数据，则可写字节数从字节尾开始
            let uint8: Uint8Array;
            if (buffer instanceof Uint8Array) {
                uint8 = buffer;
                wpos = buffer.length;
            } else {
                wpos = buffer.byteLength;
                uint8 = new Uint8Array(buffer);
            }
            if (bufferExtSize == 0) {
                bytes = new Uint8Array(wpos);
            }
            else {
                let multi = (wpos / bufferExtSize | 0) + 1;
                bytes = new Uint8Array(multi * bufferExtSize);
            }
            bytes.set(uint8);
        } else {
            bytes = new Uint8Array(bufferExtSize);
        }
        this.write_position = wpos;
        this._position = 0;
        this._bytes = bytes;
        this.data = new DataView(bytes.buffer);
        this.endian = Endian.BIG_ENDIAN;
    }

    public get GetBuffAvailable()
    {
        return this.write_position - this._position;
    }

    public get buffer(): ArrayBuffer {
        return this.data.buffer.slice(0, this.write_position);
    }

    public get rawBuffer(): ArrayBuffer {
        return this.data.buffer;
    }

    public set buffer(value: ArrayBuffer) {
        let wpos = value.byteLength;
        let uint8 = new Uint8Array(value);
        let bufferExtSize = this.bufferExtSize;
        let bytes: Uint8Array;
        if (bufferExtSize == 0) {
            bytes = new Uint8Array(wpos);
        }
        else {
            let multi = (wpos / bufferExtSize | 0) + 1;
            bytes = new Uint8Array(multi * bufferExtSize);
        }
        bytes.set(uint8);
        this.write_position = wpos;
        this._bytes = bytes;
        this.data = new DataView(bytes.buffer);
    }

    public GetBuff(): Uint8Array
    {
        return this._bytes;
    }

    public get dataView(): DataView {
        return this.data;
    }

    public set dataView(value: DataView) {
        this.buffer = value.buffer;
    }

    public get bufferOffset(): number {
        return this.data.byteOffset;
    }

    public GetPosition():number
    {
        return this._position;
    }

    public SetPosition(value: number)
    {
        this._position = value;
        if (value > this.write_position)
        {
            this.write_position = value;
        }
    }

    public Skip(count:number)
    {
        let nextpos = this._position + count;
        if (nextpos < this.write_position)
        {
            this._position = nextpos;
        }
    }

    public get length(): number
    {
        return this.write_position;
    }
    public GetByteSize():number{return this.write_position;}

    public set length(value: number) {
        this.write_position = value;
        if (this.data.byteLength > value) {
            this._position = value;
        }
        this._validateBuffer(value);
    }

    protected _validateBuffer(value: number) {
        if (this.data.byteLength < value) {
            let be = this.bufferExtSize;
            let tmp: Uint8Array;
            if (be == 0) {
                tmp = new Uint8Array(value);
            }
            else {
                let nLen = ((value / be >> 0) + 1) * be;
                tmp = new Uint8Array(nLen);
            }
            tmp.set(this._bytes);
            this._bytes = tmp;
            this.data = new DataView(tmp.buffer);
        }
    }

    public get bytesAvailable(): number {
        return this.data.byteLength - this._position;
    }

    public clear(): void {
        let buffer = new ArrayBuffer(this.bufferExtSize);
        this.data = new DataView(buffer);
        this._bytes = new Uint8Array(buffer);
        this._position = 0;
        this.write_position = 0;
    }


    public readBoolean(): boolean {
        if (this.validate(ByteArraySize.SIZE_OF_BOOLEAN)) {
            let ret = !!this._bytes[this._position];
            this.SetPosition(this._position + 1);
            return ret;
        }
    }

    public readByte(): number {
        if (this.validate(ByteArraySize.SIZE_OF_INT8)) {
            let ret = this.data.getInt8(this._position);
            this.SetPosition(this._position + 1);
            return ret;
        }
    }

    public readBytes(bytes: KFByteArray, offset: number = 0, length: number = 0): void {
        if (!bytes) {//由于bytes不返回，所以new新的无意义
            return;
        }
        let pos = this._position;
        let available = this.write_position - pos;
        if (available < 0) {
            ByteError(1025);
            return;
        }
        if (length == 0) {
            length = available;
        }
        else if (length > available) {
            ByteError(1025);
            return;
        }
        const position = bytes._position;
        bytes._position = 0;
        bytes.validateBuffer(offset + length);
        bytes._position = position;
        bytes._bytes.set(this._bytes.subarray(pos, pos + length), offset);
        this.SetPosition(this._position + length);
    }

    public readDouble(): number {
        if (this.validate(ByteArraySize.SIZE_OF_FLOAT64)) {
            let value = this.data.getFloat64(this._position, this.$endian == EndianConst.LITTLE_ENDIAN);
            this.SetPosition(this._position + ByteArraySize.SIZE_OF_FLOAT64);
            return value;
        }
    }


    public readFloat(): number {
        if (this.validate(ByteArraySize.SIZE_OF_FLOAT32)) {
            let value = this.data.getFloat32(this._position, this.$endian == EndianConst.LITTLE_ENDIAN);
            this.SetPosition(this._position + ByteArraySize.SIZE_OF_FLOAT32);
            return value;
        }
    }

    public readvaruint():number
    {
        let B = 128;
        let nextval = this.readUnsignedByte();
        let varval = nextval & 127;
        let readtimes = 0;

        while (nextval >= B && readtimes < 4)
        {
            readtimes += 1;
            nextval = this.readUnsignedByte();
            varval = varval | ((nextval & 127) << (7 * readtimes));
        }

        return varval;
    }

    public readstring():string
    {
        let len = this.readvaruint();
        return this.readUTFBytes(len);
    }

    public readkfbytes(kfbuff:KFByteArray = null):KFByteArray
    {
        let bytesize = this.readvaruint();
        if(kfbuff == null) kfbuff = new KFByteArray();
        this.readBytes(kfbuff, 0, bytesize);
        return kfbuff;
    }

    public skipstring()
    {
        let strlen = this.readvaruint();
        this.Skip(strlen);
    }

    public readInt(): number {
        if (this.validate(ByteArraySize.SIZE_OF_INT32)) {
            let value = this.data.getInt32(this._position, this.$endian == EndianConst.LITTLE_ENDIAN);
            this.SetPosition(this._position + ByteArraySize.SIZE_OF_INT32);
            return value;
        }
    }

    public readShort(): number {
        if (this.validate(ByteArraySize.SIZE_OF_INT16)) {
            let value = this.data.getInt16(this._position, this.$endian == EndianConst.LITTLE_ENDIAN);
            this.SetPosition(this._position + ByteArraySize.SIZE_OF_INT16);
            return value;
        }
    }


    public readInt64():number
    {
        ByteException();
        return 0;
    };
    public readUInt64():number
    {
        ByteException();
        return 0;
    };

    public readUnsignedByte(): number
    {
        if (this.validate(ByteArraySize.SIZE_OF_UINT8))
        {
            let ret = this._bytes[this._position];
            this.SetPosition(this._position + 1);
            return ret;
        }
    }

    public readUnsignedInt(): number {
        if (this.validate(ByteArraySize.SIZE_OF_UINT32)) {
            let value = this.data.getUint32(this._position, this.$endian == EndianConst.LITTLE_ENDIAN);
            this.SetPosition(this._position + ByteArraySize.SIZE_OF_UINT32);
            return value;
        }
    }

    public readUnsignedShort(): number {
        if (this.validate(ByteArraySize.SIZE_OF_UINT16)) {
            let value = this.data.getUint16(this._position, this.$endian == EndianConst.LITTLE_ENDIAN);
            this.SetPosition(this._position + ByteArraySize.SIZE_OF_UINT16);
            return value;
        }
    }

    public readUTF(): string {
        let length = this.readUnsignedShort();
        if (length > 0) {
            return this.readUTFBytes(length);
        } else {
            return "";
        }
    }

    public readUTFBytes(length: number): string {
        if (!this.validate(length)) {
            return;
        }
        let data = this.data;
        let bytes = new Uint8Array(data.buffer, data.byteOffset + this._position, length);
        this.SetPosition(this._position + length);
        return this.decodeUTF8(bytes);
    }

    public writeBoolean(value: boolean): void {
        this.validateBuffer(ByteArraySize.SIZE_OF_BOOLEAN);
        this._bytes[this._position] = +value;
        this.SetPosition(this._position + 1);
    }

    public writeByte(value: number): void
    {
        this.validateBuffer(ByteArraySize.SIZE_OF_INT8);
        this._bytes[this._position] = value & 0xff;
        this.SetPosition(this._position + 1);
    }

    public writeBytes(bytes: KFByteArray, offset: number = 0, length: number = 0): void {
        let writeLength: number;
        if (offset < 0) {
            return;
        }
        if (length < 0) {
            return;
        } else if (length == 0) {
            writeLength = bytes.length - offset;
        } else {
            writeLength = Math.min(bytes.length - offset, length);
        }
        if (writeLength > 0) {
            this.validateBuffer(writeLength);
            this._bytes.set(bytes._bytes.subarray(offset, offset + writeLength), this._position);
            this.SetPosition(this._position + writeLength);
        }
    }

    public writeDouble(value: number): void {
        this.validateBuffer(ByteArraySize.SIZE_OF_FLOAT64);
        this.data.setFloat64(this._position, value, this.$endian == EndianConst.LITTLE_ENDIAN);
        this.SetPosition(this._position + ByteArraySize.SIZE_OF_FLOAT64);
    }

    public writeFloat(value: number): void {
        this.validateBuffer(ByteArraySize.SIZE_OF_FLOAT32);
        this.data.setFloat32(this._position, value, this.$endian == EndianConst.LITTLE_ENDIAN);
        this.SetPosition(this._position + ByteArraySize.SIZE_OF_FLOAT32);
    }

    public writeInt(value: number): void
    {
        this.validateBuffer(ByteArraySize.SIZE_OF_INT32);
        this.data.setInt32(this._position, value, this.$endian == EndianConst.LITTLE_ENDIAN);
        this.SetPosition(this._position + ByteArraySize.SIZE_OF_INT32);
    }

    public GetVarUIntSize(value:number) : number
    {
        let bit7 = 1 << 7;
        let bit14 = 1 << 14;
        let bit21 = 1 << 21;
        let bit28 = 1 << 28;

        if (value < bit7) return 1;
        else if (value < bit14) return 2;
        else if (value < bit21) return 3;
        else if (value < bit28) return 4;
        else return 5;
    }

    public writevaruint(value:number):void
    {
        let bit7  = 1 << 7;
        let bit14 = 1 << 14;
        let bit21 = 1 << 21;
        let bit28 = 1 << 28;
        let B = 128;

        if (value < bit7)
        {
            this.validateBuffer(1);
            this._bytes[this._position] = value & 0xff;
            this.SetPosition(this._position + 1);
        }
        else if (value < bit14)
        {
            this.validateBuffer(2);
            this._bytes[this._position] = value | B;
            this._bytes[this._position + 1] = value >> 7;
            this.SetPosition(this._position + 2);
        }
        else if (value < bit21)
        {
            this.validateBuffer(3);
            this._bytes[this._position] = value | B;
            this._bytes[this._position + 1] = (value >> 7) | B;
            this._bytes[this._position + 2] = value >> 14;
            this.SetPosition(this._position + 3);

        }else if(value < bit28)
        {
            this.validateBuffer(4)
            this._bytes[this._position] = value | B;
            this._bytes[this._position + 1] = (value >> 7) | B;
            this._bytes[this._position + 2] = (value >> 14) | B;
            this._bytes[this._position + 3] = value >> 21;
            this.SetPosition(this._position + 4);

        }
        else
        {
            this.validateBuffer(5)
            this._bytes[this._position] = value | B;
            this._bytes[this._position + 1] = (value >> 7) | B;
            this._bytes[this._position + 2] = (value >> 14) | B;
            this._bytes[this._position + 3] = (value >> 21) | B;
            this._bytes[this._position + 4] = value >> 28;
            this.SetPosition(this._position + 5);
        }
    }

    public writeInt64(value:number):void
    {
        ByteException();
    };
    public writeUInt64(value:number):void
    {
        ByteException();
    };

    public writeShort(value: number): void {
        this.validateBuffer(ByteArraySize.SIZE_OF_INT16);
        this.data.setInt16(this._position, value, this.$endian == EndianConst.LITTLE_ENDIAN);
        this.SetPosition(this._position + ByteArraySize.SIZE_OF_INT16);
    }

    public writeUnsignedInt(value: number): void {
        this.validateBuffer(ByteArraySize.SIZE_OF_UINT32);
        this.data.setUint32(this._position, value, this.$endian == EndianConst.LITTLE_ENDIAN);
        this.SetPosition(this._position + ByteArraySize.SIZE_OF_UINT32);
    }

    public writeUnsignedShort(value: number): void {
        this.validateBuffer(ByteArraySize.SIZE_OF_UINT16);
        this.data.setUint16(this._position, value, this.$endian == EndianConst.LITTLE_ENDIAN);
        this.SetPosition(this._position + ByteArraySize.SIZE_OF_UINT16);
    }

    public writeUTF(value: string): void {
        let utf8bytes: ArrayLike<number> = this.encodeUTF8(value);
        let length: number = utf8bytes.length;
        this.validateBuffer(ByteArraySize.SIZE_OF_UINT16 + length);
        this.data.setUint16(this._position, length, this.$endian == EndianConst.LITTLE_ENDIAN);
        this.SetPosition(this._position + ByteArraySize.SIZE_OF_UINT16);
        this._writeUint8Array(utf8bytes, false);
    }

    public writeUTFBytes(value: string): void
    {
        this._writeUint8Array(this.encodeUTF8(value));
    }

    public writestring(value:string)
    {
        let strbuff = this.encodeUTF8(value);
        this.writevaruint(strbuff.length);
        this._writeUint8Array(strbuff);
    }

    public toString(): string
    {
        return "[ByteArray] length:" + this.length + ", bytesAvailable:" + this.bytesAvailable;
    }

    public _writeUint8Array(bytes: Uint8Array | ArrayLike<number>, validateBuffer: boolean = true): void {
        let pos = this._position;
        let npos = pos + bytes.length;
        if (validateBuffer) {
            this.validateBuffer(npos);
        }
        this._bytes.set(bytes, pos);
        this.SetPosition(npos);
    }

    public validate(len: number): boolean {
        let bl = this._bytes.length;
        if (bl > 0 && this._position + len <= bl) {
            return true;
        } else {
            ByteError(1025);
        }
    }


    protected validateBuffer(len: number): void {
        this.write_position = len > this.write_position ? len : this.write_position;
        len += this._position;
        this._validateBuffer(len);
    }


    private encodeUTF8(str: string): Uint8Array {
        let pos: number = 0;
        let codePoints = this.stringToCodePoints(str);
        let outputBytes = [];

        while (codePoints.length > pos) {
            let code_point: number = codePoints[pos++];

            if (this.inRange(code_point, 0xD800, 0xDFFF)) {
                this.encoderError(code_point);
            }
            else if (this.inRange(code_point, 0x0000, 0x007f)) {
                outputBytes.push(code_point);
            } else {
                let count, offset;
                if (this.inRange(code_point, 0x0080, 0x07FF)) {
                    count = 1;
                    offset = 0xC0;
                } else if (this.inRange(code_point, 0x0800, 0xFFFF)) {
                    count = 2;
                    offset = 0xE0;
                } else if (this.inRange(code_point, 0x10000, 0x10FFFF)) {
                    count = 3;
                    offset = 0xF0;
                }

                outputBytes.push(this.div(code_point, Math.pow(64, count)) + offset);

                while (count > 0) {
                    let temp = this.div(code_point, Math.pow(64, count - 1));
                    outputBytes.push(0x80 + (temp % 64));
                    count -= 1;
                }
            }
        }
        return new Uint8Array(outputBytes);
    }

    private decodeUTF8(data: Uint8Array): string {
        let fatal: boolean = false;
        let pos: number = 0;
        let result: string = "";
        let code_point: number;
        let utf8_code_point = 0;
        let utf8_bytes_needed = 0;
        let utf8_bytes_seen = 0;
        let utf8_lower_boundary = 0;

        while (data.length > pos) {

            let _byte = data[pos++];

            if (_byte == this.EOF_byte) {
                if (utf8_bytes_needed != 0) {
                    code_point = this.decoderError(fatal);
                } else {
                    code_point = this.EOF_code_point;
                }
            } else {

                if (utf8_bytes_needed == 0) {
                    if (this.inRange(_byte, 0x00, 0x7F)) {
                        code_point = _byte;
                    } else {
                        if (this.inRange(_byte, 0xC2, 0xDF)) {
                            utf8_bytes_needed = 1;
                            utf8_lower_boundary = 0x80;
                            utf8_code_point = _byte - 0xC0;
                        } else if (this.inRange(_byte, 0xE0, 0xEF)) {
                            utf8_bytes_needed = 2;
                            utf8_lower_boundary = 0x800;
                            utf8_code_point = _byte - 0xE0;
                        } else if (this.inRange(_byte, 0xF0, 0xF4)) {
                            utf8_bytes_needed = 3;
                            utf8_lower_boundary = 0x10000;
                            utf8_code_point = _byte - 0xF0;
                        } else {
                            this.decoderError(fatal);
                        }
                        utf8_code_point = utf8_code_point * Math.pow(64, utf8_bytes_needed);
                        code_point = null;
                    }
                } else if (!this.inRange(_byte, 0x80, 0xBF)) {
                    utf8_code_point = 0;
                    utf8_bytes_needed = 0;
                    utf8_bytes_seen = 0;
                    utf8_lower_boundary = 0;
                    pos--;
                    code_point = this.decoderError(fatal, _byte);
                } else {

                    utf8_bytes_seen += 1;
                    utf8_code_point = utf8_code_point + (_byte - 0x80) * Math.pow(64, utf8_bytes_needed - utf8_bytes_seen);

                    if (utf8_bytes_seen !== utf8_bytes_needed) {
                        code_point = null;
                    } else {

                        let cp = utf8_code_point;
                        let lower_boundary = utf8_lower_boundary;
                        utf8_code_point = 0;
                        utf8_bytes_needed = 0;
                        utf8_bytes_seen = 0;
                        utf8_lower_boundary = 0;
                        if (this.inRange(cp, lower_boundary, 0x10FFFF) && !this.inRange(cp, 0xD800, 0xDFFF)) {
                            code_point = cp;
                        } else {
                            code_point = this.decoderError(fatal, _byte);
                        }
                    }

                }
            }
            //Decode string
            if (code_point !== null && code_point !== this.EOF_code_point) {
                if (code_point <= 0xFFFF) {
                    if (code_point > 0) result += String.fromCharCode(code_point);
                } else {
                    code_point -= 0x10000;
                    result += String.fromCharCode(0xD800 + ((code_point >> 10) & 0x3ff));
                    result += String.fromCharCode(0xDC00 + (code_point & 0x3ff));
                }
            }
        }
        return result;
    }

    private encoderError(code_point) {
        ByteError(1026, code_point);
    }

    private decoderError(fatal, opt_code_point?): number {
        if (fatal) {
            ByteError(1027);
        }
        return opt_code_point || 0xFFFD;
    }


    private EOF_byte: number = -1;

    private EOF_code_point: number = -1;

    private inRange(a, min, max) {
        return min <= a && a <= max;
    }

    private div(n, d) {
        return Math.floor(n / d);
    }

    private stringToCodePoints(string)
    {
        /** @type {Array.<number>} */
        let cps = [];
        // Based on http://www.w3.org/TR/WebIDL/#idl-DOMString
        let i = 0, n = string.length;
        while (i < string.length) {
            let c = string.charCodeAt(i);
            if (!this.inRange(c, 0xD800, 0xDFFF)) {
                cps.push(c);
            } else if (this.inRange(c, 0xDC00, 0xDFFF)) {
                cps.push(0xFFFD);
            } else { // (inRange(c, 0xD800, 0xDBFF))
                if (i == n - 1) {
                    cps.push(0xFFFD);
                } else {
                    let d = string.charCodeAt(i + 1);
                    if (this.inRange(d, 0xDC00, 0xDFFF)) {
                        let a = c & 0x3FF;
                        let b = d & 0x3FF;
                        i += 1;
                        cps.push(0x10000 + (a << 10) + b);
                    } else {
                        cps.push(0xFFFD);
                    }
                }
            }
            i += 1;
        }
        return cps;
    }
}


