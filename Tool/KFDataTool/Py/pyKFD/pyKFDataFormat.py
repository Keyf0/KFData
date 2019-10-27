
from .pyKFData import *
from .pyKFByteArray import *

class pyKFDataFormat(object):

    @staticmethod
    def skip_value(bytearr):
        pyKFDataFormat.read_value(bytearr, True)

    @staticmethod
    def skip_object(bytearr):
        pyKFDataFormat.read_object_value(bytearr, pyKFDataType.OT_MIXOBJECT, 0, True)

    @staticmethod
    def read_value(bytearr, skip=False, val=None):
        retval = val
        size = bytearr.available_size()
        if size > 0:
            valueType = bytearr.read_ubyte()  # uint8
            if val != None and val.valueType != valueType:
                skip = True

            if valueType <= pyKFDataType.OT_UINT64:
                return pyKFDataFormat.read_base_value(bytearr, valueType, skip, val)  # 基础数据类型
            else:
                if valueType == pyKFDataType.OT_ARRAY or valueType == pyKFDataType.OT_MIXARRAY:
                    retval = pyKFDataFormat.read_array_value(bytearr, valueType, skip, val)
                elif valueType == pyKFDataType.OT_OBJECT:
                    retval = pyKFDataFormat.read_object_value(bytearr, valueType, 0, skip, val)
                elif valueType == pyKFDataType.OT_MIXOBJECT:
                    # 读取一个类型ID uint32
                    classid = bytearr.read_varuint()
                    classname = ""
                    if classid == 1:
                        classname = bytearr.read_string()
                        pass
                    retval = pyKFDataFormat.read_object_value(bytearr, valueType, classid, classname, skip, val)
                    pass

        return retval

    @staticmethod
    def read_object_value(bytearr, valtype, classid, classname, skip=False, val=None):
        retval = val
        if skip:
            deep = 0
            while True:
                pid = bytearr.read_varuint()  # 读取PID
                if pid == pyKFDataType.OBJ_PROP_ID_BEGIN:
                    deep += 1
                elif pid == pyKFDataType.OBJ_PROP_ID_END:
                    deep -= 1
                    if deep <= 0:
                        break
                else:
                    pyKFDataFormat.read_value(bytearr, True)  # 读取值
        else:
            deep = 0  # int32
            obj = None
            if val != None:
                obj = val
            if obj == None:
                obj = pyKFDValue()
                obj.classid = classid
                obj.valueType = valtype
                obj.classname = classname
                obj.val = []
                obj.extendval = None

            currobj = obj
            stack = []  # kfdobject

            while True:
                pid = bytearr.read_varuint()  # 读取PID
                if pid == pyKFDataType.OBJ_PROP_ID_BEGIN:
                    # child = None
                    if deep != 0:
                        child = currobj.extendval
                        if child == None:
                            child = pyKFDValue()
                            child.valueType = pyKFDataType.OT_OBJECT
                            child.val = []
                            currobj.extendval = child
                        stack.append(currobj)
                        currobj = child
                    deep += 1
                elif pid == pyKFDataType.OBJ_PROP_ID_END:
                    deep -= 1
                    if deep <= 0:
                        break
                    else:
                        currobj = stack.pop()
                else:
                    # 读普通的属性
                    # TODOO..
                    # 此处不会去覆盖已经有的属性吗？
                    prop = pyKFDataFormat.read_value(bytearr, False, None)
                    prop.pid = pid
                    currobj.val.append(prop)
            retval = obj
        return retval

    @staticmethod
    def read_base_value(bytearr, valtype, skip=False, value=None):
        retval = value

        if valtype == pyKFDataType.OT_INT8:
            if skip:
                bytearr.skip(1)
            else:
                knum1 = pyKFDValue()
                knum1.val = bytearr.read_byte()
                retval = knum1
        elif valtype == pyKFDataType.OT_UINT8:
            if skip:
                bytearr.skip(1)
            else:
                knum1 = pyKFDValue()
                knum1.val = bytearr.read_ubyte()
                retval = knum1
        elif valtype == pyKFDataType.OT_INT16:
            if skip:
                bytearr.skip(2)
            else:
                knum1 = pyKFDValue()
                knum1.val = bytearr.read_short()
                retval = knum1
        elif valtype == pyKFDataType.OT_UINT16:
            if skip:
                bytearr.skip(2)
            else:
                knum1 = pyKFDValue()
                knum1.val = bytearr.read_ushort()
                retval = knum1
        elif valtype == pyKFDataType.OT_INT32:
            if skip:
                bytearr.skip(4)
            else:
                knum1 = pyKFDValue()
                knum1.val = bytearr.read_int()
                retval = knum1
        elif valtype == pyKFDataType.OT_UINT32:
            if skip:
                bytearr.skip(4)
            else:
                knum1 = pyKFDValue()
                knum1.val = bytearr.read_uint()
                retval = knum1
        elif valtype == pyKFDataType.OT_FLOAT:
            if skip:
                bytearr.skip(4)
            else:
                knum1 = pyKFDValue()
                knum1.val = bytearr.read_float()
                retval = knum1
        elif valtype == pyKFDataType.OT_DOUBLE:
            if skip:
                bytearr.skip(8)
            else:
                knum2 = pyKFDValue()
                knum2.val = bytearr.read_int64()
                retval = knum2
        elif valtype == pyKFDataType.OT_STRING:
            if skip:
                bytearr.skip_string()
            else:
                strval = pyKFDValue()
                strval.val = bytearr.read_string()
                retval = strval
        elif valtype == pyKFDataType.OT_BYTES:
            if skip:
                bytearr.skip_string()
            else:
                knum1 = pyKFDValue()
                knum1.val = bytearr.read_bytesbuff()
                retval = knum1
        elif valtype == pyKFDataType.OT_BOOL:
            if skip:
                bytearr.skip(1)
            else:
                boolval = pyKFDValue()
                boolval.val = bytearr.read_bool()
                retval = boolval
        elif valtype == pyKFDataType.OT_VARUINT:
            if skip:
                bytearr.read_varuint()
            else:
                knum1 = pyKFDValue()
                knum1.val = bytearr.read_varuint()
                retval = knum1
        elif valtype == pyKFDataType.OT_INT64:
            if skip:
                bytearr.skip(8)
            else:
                knum2 = pyKFDValue()
                knum2.val = bytearr.read_int64()
                retval = knum2
        elif valtype == pyKFDataType.OT_UINT64:
            if skip:
                bytearr.skip(8)
            else:
                knum2 = pyKFDValue()
                knum2.val = bytearr.read_uint64()
                retval = knum2

        if retval:
            retval.valueType = valtype
        return retval

    @staticmethod
    def read_array_value(bytearr, valtype, skip=False, val=None):
        retval = val
        if valtype == pyKFDataType.OT_ARRAY:
            size = bytearr.read_varuint()  # uint32
            oType = bytearr.read_ubyte()  # uint8
            if oType <= pyKFDataType.OT_UINT64:
                if skip:
                    i = 0
                    while i < size:
                        pyKFDataFormat.read_base_value(bytearr, oType, True)
                        i = i + 1
                else:

                    objarr = pyKFDValue()
                    objarr.valueType = valtype
                    objarr.oType = oType
                    objarr.val = []

                    i = 0
                    while i < size:
                        objarr.val.append(pyKFDataFormat.read_base_value(bytearr, oType))
                        i = i + 1
                    retval = objarr
            else:
                if oType == pyKFDataType.OT_ARRAY or oType == pyKFDataType.OT_MIXARRAY:
                    if skip:
                        i = 0
                        while i < size:
                            pyKFDataFormat.read_array_value(bytearr, oType, True)
                            i = i + 1
                    else:

                        arrobj = pyKFDValue()
                        arrobj.valueType = valtype
                        arrobj.oType = oType
                        arrobj.val = []

                        while i < size:
                            arrobj.val.append(pyKFDataFormat.read_array_value(bytearr, oType, False))
                            i = i + 1
                        retval = arrobj
                elif oType == pyKFDataType.OT_OBJECT or oType == pyKFDataType.OT_MIXOBJECT:
                    # 读取一个类型ID
                    classid = 0  # uint32
                    # 只有MIXOBJECT有实例ID的写入
                    if oType == pyKFDataType.OT_MIXOBJECT:
                        #classid = bytearr.read_varuint()
                        pass
                    if skip:
                        i = 0
                        while i < size:
                            pyKFDataFormat.read_object_value(bytearr, oType, classid, "", True)
                            i = i + 1
                    else:
                        objarr = pyKFDValue()
                        objarr.valueType = valtype
                        objarr.oType = oType
                        objarr.val = []

                        i = 0
                        while i < size:
                            objarr.val.append(pyKFDataFormat.read_object_value(bytearr, oType, classid, ""))
                            i = i + 1
                        retval = objarr
        elif valtype == pyKFDataType.OT_MIXARRAY:
            size = bytearr.read_varuint()  # uint32
            if skip:
                i = 0
                while i < size:
                    pyKFDataFormat.read_value(bytearr, True)
                    i = i + 1
            else:
                arrobj = pyKFDValue()
                arrobj.valueType = valtype
                arrobj.val = []

                i = 0
                while i < size:
                    arrobj.val.append(pyKFDataFormat.read_value(bytearr, False))
                    i = i + 1
                retval = arrobj
        return retval

    @staticmethod
    def write_object_value(bytearr, dataType, objectval):
        bytearr.write_varuint(pyKFDataType.OBJ_PROP_ID_BEGIN)
        # 写入继承的属性

        if hasattr(objectval,"extendval") and objectval.extendval is not None:
            pyKFDataFormat.write_object_value(bytearr, dataType, objectval.extendval)

        # 写自己的属性吧
        valarr = objectval.val
        for item in valarr:
            pid = item.pid
            if pid != pyKFDataType.OBJ_PROP_ID_BEGIN and pid != pyKFDataType.OBJ_PROP_ID_END:
                bytearr.write_varuint(pid)
                pyKFDataFormat.write_value(bytearr, item)
        bytearr.write_varuint(pyKFDataType.OBJ_PROP_ID_END)

    @staticmethod
    def write_value(bytearr, val):
        if val != None:
            valueType = val.valueType
            bytearr.write_byte(valueType)
            if valueType <= pyKFDataType.OT_UINT64:
                pyKFDataFormat.write_base_value(bytearr, valueType, val)
            else:
                if valueType == pyKFDataType.OT_ARRAY or valueType == pyKFDataType.OT_MIXARRAY:
                    pyKFDataFormat.write_array_value(bytearr, valueType, val)
                elif valueType == pyKFDataType.OT_OBJECT:
                    pyKFDataFormat.write_object_value(bytearr, valueType, val)
                elif valueType == pyKFDataType.OT_MIXOBJECT:
                    vkfd = val.get_kfddata(True)
                    if vkfd is not None or "classname" in val:
                        bytearr.write_varuint(1)
                        if vkfd is None:
                            bytearr.write_string(val.classname)
                        else:
                            bytearr.write_string(vkfd["class"])
                    else:
                        # 写入类ID
                        bytearr.write_varuint(val.classid)
                    # 写入所有属性
                    pyKFDataFormat.write_object_value(bytearr, valueType, val)
                else:
                    return
        else:
            bytearr.write_byte(pyKFDataType.OT_NULL)

    @staticmethod
    def write_array_value(bytearr, valtype, val):
        arrval = val.val
        arrsize = len(arrval)
        bytearr.write_varuint(arrsize)

        oType = 0
        if hasattr(val,"oType"):
            oType = val.oType

        if valtype == pyKFDataType.OT_ARRAY:
            if oType <= pyKFDataType.OT_UINT64:
                bytearr.write_byte(oType)
                for item in arrval:
                    pyKFDataFormat.write_base_value(bytearr, oType, item)
            else:
                if oType == pyKFDataType.OT_ARRAY or oType == pyKFDataType.OT_MIXARRAY:
                    raise Exception("暂时不支持多维数组")
                elif oType == pyKFDataType.OT_OBJECT or oType == pyKFDataType.OT_MIXOBJECT:
                    bytearr.write_byte(oType)
                    if oType == pyKFDataType.OT_MIXOBJECT:
                        raise Exception("普通数组不支持MIXOBJECT对象")
                    for item in arrval:
                        pyKFDataFormat.write_object_value(bytearr, oType, item)
        elif valtype == pyKFDataType.OT_MIXARRAY:
            for item in arrval:
                pyKFDataFormat.write_value(bytearr, item)

    @staticmethod
    def write_base_value(bytearr, dataType, valObject):
        if dataType == pyKFDataType.OT_UNKNOW:
            return
        elif dataType == pyKFDataType.OT_INT8:
            bytearr.write_byte(valObject.val)
        elif dataType == pyKFDataType.OT_UINT8:
            bytearr.write_ubyte(valObject.val)
        elif dataType == pyKFDataType.OT_INT16:
            bytearr.write_short(valObject.val)
        elif dataType == pyKFDataType.OT_UINT16:
            bytearr.write_ushort(valObject.val)
        elif dataType == pyKFDataType.OT_INT32:
            bytearr.write_int(valObject.val)
        elif dataType == pyKFDataType.OT_UINT32:
            bytearr.write_uint(valObject.val)
        elif dataType == pyKFDataType.OT_FLOAT:
            bytearr.write_float(valObject.val)
        elif dataType == pyKFDataType.OT_DOUBLE:
            bytearr.write_double(valObject.val)
        elif dataType == pyKFDataType.OT_STRING:
            bytearr.write_string(valObject.val)
        elif dataType == pyKFDataType.OT_NULL:
            return
        elif dataType == pyKFDataType.OT_BYTES:
            bytearr.write_bytesbuff(valObject.val)
        elif dataType == pyKFDataType.OT_BOOL:
            bytearr.write_bool(valObject.val)
        elif dataType == pyKFDataType.OT_VARUINT:
            bytearr.write_varuint(valObject.val)
        elif dataType == pyKFDataType.OT_INT64:
            bytearr.write_int64(valObject.val)
        elif dataType == pyKFDataType.OT_UINT64:
            bytearr.write_uint64(valObject.val)
        else:
            raise Exception("不支持的类型" + str(dataType))

    @staticmethod
    def int8_read(buff):
        return buff.read_byte()

    @staticmethod
    def uint8_read(buff):
        return buff.read_ubyte()

    @staticmethod
    def int16_read(buff):
        return buff.read_short()

    @staticmethod
    def uint16_read(buff):
        return buff.read_ushort()

    @staticmethod
    def int32_read(buff):
        return buff.read_int()

    @staticmethod
    def uint32_read(buff):
        return buff.read_uint()

    @staticmethod
    def num1_read(buff):
        return buff.read_float()

    @staticmethod
    def num2_read(buff):
        return buff.read_double()

    @staticmethod
    def kfstr_read(buff):
        return buff.read_string()

    @staticmethod
    def bool_read(buff):
        return buff.read_bool()

    @staticmethod
    def varuint_read(buff):
        return buff.read_varuint()

    @staticmethod
    def int64_read(buff):
        return buff.read_int64()

    @staticmethod
    def uint64_read(buff):
        return buff.read_uint64()

    @staticmethod
    def int8_write(buff, prop):
        buff.write_byte(prop)

    @staticmethod
    def uint8_write(buff, prop):
        buff.write_ubyte(prop)

    @staticmethod
    def int16_write(buff, prop):
        buff.write_short(prop)

    @staticmethod
    def uint16_write(buff, prop):
        buff.write_ushort(prop)

    @staticmethod
    def int32_write(buff, prop):
        buff.write_int(prop)

    @staticmethod
    def uint32_write(buff, prop):
        buff.write_uint(prop)

    @staticmethod
    def num1_write(buff, prop):
        buff.write_float(prop)

    @staticmethod
    def num2_write(buff, prop):
        buff.write_double(prop)

    @staticmethod
    def kfstr_write(buff, prop):
        buff.write_string(prop)

    @staticmethod
    def bool_write(buff, prop):
        buff.write_bool(prop)

    @staticmethod
    def varuint_write(buff, prop):
        buff.write_varuint(prop)

    @staticmethod
    def int64_write(buff, prop):
        buff.write_int64(prop)

    @staticmethod
    def uint64_write(buff, prop):
        buff.write_uint64(prop)

    @staticmethod
    def int8_write_val(buff, val):
        buff.write_ubyte(pyKFDataType.OT_INT8)
        buff.write_byte(val)

    @staticmethod
    def uint8_write_val(buff, val):
        buff.write_ubyte(pyKFDataType.OT_UINT8)
        buff.write_ubyte(val)

    @staticmethod
    def int16_write_val(buff, val):
        buff.write_ubyte(pyKFDataType.OT_INT16)
        buff.write_short(val)

    @staticmethod
    def uint16_write_val(buff, val):
        buff.write_ubyte(pyKFDataType.OT_UINT16)
        buff.write_ushort(val)

    @staticmethod
    def int32_write_val(buff, val):
        buff.write_ubyte(pyKFDataType.OT_INT32)
        buff.write_int(val)

    @staticmethod
    def uint32_write_val(buff, val):
        buff.write_ubyte(pyKFDataType.OT_UINT32)
        buff.write_uint(val)

    @staticmethod
    def num1_write_val(buff, val):
        buff.write_ubyte(pyKFDataType.OT_FLOAT)
        buff.write_float(val)

    @staticmethod
    def num2_write_val(buff, val):
        buff.write_ubyte(pyKFDataType.OT_DOUBLE)
        buff.write_double(val)

    @staticmethod
    def kfstr_write_val(buff, val):
        buff.write_ubyte(pyKFDataType.OT_STRING)
        buff.write_string(val)

    @staticmethod
    def bool_write_val(buff, val):
        buff.write_ubyte(pyKFDataType.OT_BOOL)
        buff.write_bool(val)

    @staticmethod
    def varuint_write_val(buff, val):
        buff.write_ubyte(pyKFDataType.OT_VARUINT)
        buff.write_varuint(val)

    @staticmethod
    def int64_write_val(buff, val):
        buff.write_ubyte(pyKFDataType.OT_INT64)
        buff.write_int64(val)

    @staticmethod
    def uint64_write_val(buff, val):
        buff.write_ubyte(pyKFDataType.OT_UINT64)
        buff.write_uint64(val)
    pass






