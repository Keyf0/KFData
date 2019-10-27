'''

PYTHON运行时序列化与反序列化 KFDATA


'''

from .pyKFByteArray import *
from .pyKFDataType import *
from .pyKFDTable import *

class pyKFDJson(object):

    @staticmethod
    def _read_base_value(bytearr, valtype, skip=False, value=None):
        retval = value

        if valtype == pyKFDataType.OT_INT8:
            if skip:
                bytearr.skip(1)
            else:
                retval = bytearr.read_byte()
        elif valtype == pyKFDataType.OT_UINT8:
            if skip:
                bytearr.skip(1)
            else:
                retval = bytearr.read_ubyte()
        elif valtype == pyKFDataType.OT_INT16:
            if skip:
                bytearr.skip(2)
            else:
                retval = bytearr.read_short()
        elif valtype == pyKFDataType.OT_UINT16:
            if skip:
                bytearr.skip(2)
            else:
                retval = bytearr.read_ushort()
        elif valtype == pyKFDataType.OT_INT32:
            if skip:
                bytearr.skip(4)
            else:
                retval = bytearr.read_int()
        elif valtype == pyKFDataType.OT_UINT32:
            if skip:
                bytearr.skip(4)
            else:
                retval = bytearr.read_uint()
        elif valtype == pyKFDataType.OT_FLOAT:
            if skip:
                bytearr.skip(4)
            else:
                retval = bytearr.read_float()
        elif valtype == pyKFDataType.OT_DOUBLE:
            if skip:
                bytearr.skip(8)
            else:
                retval = bytearr.read_double();
        elif valtype == pyKFDataType.OT_STRING:
            if skip:
                bytearr.skip_string()
            else:
                retval = bytearr.read_string()
        elif valtype == pyKFDataType.OT_BYTES:
            if skip:
                bytearr.skip_string()
            else:
                retval = bytearr.read_bytesbuff()
        elif valtype == pyKFDataType.OT_BOOL:
            if skip:
                bytearr.skip(1)
            else:
                retval = bytearr.read_bool()
        elif valtype == pyKFDataType.OT_VARUINT:
            if skip:
                bytearr.read_varuint()
            else:
                retval = bytearr.read_varuint()
        elif valtype == pyKFDataType.OT_INT64:
            if skip:
                bytearr.skip(8)
            else:
                retval = bytearr.read_int64()
        elif valtype == pyKFDataType.OT_UINT64:
            if skip:
                bytearr.skip(8)
            else:
                retval = bytearr.read_uint64()
        return retval

    @staticmethod
    def _read_object_value(bytearr, valtype, kfddata = None, skip = False, val=None):
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
                    # 读取值
                    pyKFDJson.read_value(bytearr, True)
        else:
            # int32
            deep = 0
            obj = None
            #赋值一个需要填充的对象
            if val != None:
                obj = val
            #如果为空则创建一个对象
            if obj == None:
                obj = {}

            currKFDData = kfddata
            stack = []  # KFDATA STACK

            while True:
                pid = bytearr.read_varuint()  # 读取PID
                if pid == pyKFDataType.OBJ_PROP_ID_BEGIN:
                    # child = None
                    if deep != 0:
                        child = pyKFDTable.find_extend_kfddata(currKFDData)
                        stack.append(currKFDData)
                        currKFDData = child
                    deep += 1
                elif pid == pyKFDataType.OBJ_PROP_ID_END:
                    deep -= 1
                    if deep <= 0:
                        break
                    else:
                        currKFDData = stack.pop()
                else:
                    # 读普通的属性
                    pinfo = pyKFDTable.find_prop_info(currKFDData, pid)
                    if pinfo is not None:
                        pname = pinfo["name"]
                        propobj = None
                        if obj is not None and pname in obj:
                            propobj = obj[pname]
                        obj[pname] = pyKFDJson.read_value(bytearr, False, propobj, pinfo)
                        pass
                    else:
                        pyKFDJson.read_value(bytearr, True)
            retval = obj
        return retval

    @staticmethod
    def _read_array_value(bytearr, valtype, skip = False, val = None, propinfo = None):
        retval = val
        if valtype == pyKFDataType.OT_ARRAY:
            size = bytearr.read_varuint()  # uint32
            oType = bytearr.read_ubyte()  # uint8
            if oType <= pyKFDataType.OT_UINT64:
                if skip:
                    i = 0
                    while i < size:
                        pyKFDJson._read_base_value(bytearr, oType, True)
                        i = i + 1
                else:
                    objarr = []
                    i = 0
                    while i < size:
                        objarr.append(pyKFDJson._read_base_value(bytearr, oType))
                        i = i + 1
                    retval = objarr
                    pass
            else:
                if oType == pyKFDataType.OT_ARRAY or oType == pyKFDataType.OT_MIXARRAY:
                    if skip:
                        i = 0
                        while i < size:
                            pyKFDJson._read_array_value(bytearr, oType, True)
                            i = i + 1
                    else:
                        arrobj = []
                        while i < size:
                            arrobj.append(pyKFDJson._read_array_value(bytearr, oType, False))
                            i = i + 1
                        retval = arrobj
                        pass
                elif oType == pyKFDataType.OT_OBJECT or oType == pyKFDataType.OT_MIXOBJECT:
                    kfddata = None
                    if oType == pyKFDataType.OT_OBJECT and propinfo is not None:
                        if "otype" in propinfo:
                            kfddata = pyKFDTable.kfdTB.get_kfddata(propinfo["otype"])
                            pass
                        pass
                    if skip:
                        i = 0
                        while i < size:
                            pyKFDJson._read_object_value(bytearr, oType, None, True)
                            i = i + 1
                    else:
                        objarr = []
                        i = 0
                        while i < size:
                            objarr.append(pyKFDJson._read_object_value(bytearr, oType, kfddata))
                            i = i + 1
                        retval = objarr
        elif valtype == pyKFDataType.OT_MIXARRAY:
            # uint32
            size = bytearr.read_varuint()
            if skip:
                i = 0
                while i < size:
                    pyKFDJson.read_value(bytearr, True)
                    i = i + 1
            else:
                arrobj = []
                i = 0
                while i < size:
                    arrobj.append(pyKFDJson.read_value(bytearr, False))
                    i = i + 1
                retval = arrobj
        return retval


    @staticmethod
    def read_value(bytearr, skip = False, jsonobj = None, propinfo = None):
        retval = jsonobj
        size = bytearr.available_size()
        if size > 0:
            # uint8
            valueType = bytearr.read_ubyte()
            if valueType <= pyKFDataType.OT_UINT64:
                return pyKFDJson._read_base_value(bytearr, valueType, skip, jsonobj)  # 基础数据类型
            else:
                if valueType == pyKFDataType.OT_ARRAY or valueType == pyKFDataType.OT_MIXARRAY:
                    retval = pyKFDJson._read_array_value(bytearr, valueType, skip, jsonobj, propinfo)
                elif valueType == pyKFDataType.OT_OBJECT:
                    retval = pyKFDJson._read_object_value(bytearr, valueType, propinfo, skip, jsonobj)
                elif valueType == pyKFDataType.OT_MIXOBJECT:
                    # 读取一个类型ID uint32
                    classid = bytearr.read_varuint()
                    classname = ""
                    if classid == 1:
                        classname = bytearr.read_string()
                        pass
                    kfddata = None
                    if classname != "":
                        kfddata = pyKFDTable.kfdTB.get_kfddata(classname)
                    else:
                        kfddata = pyKFDTable.kfdTB.get_kfddata_clsid(classid)

                    retval = pyKFDJson._read_object_value(bytearr, valueType, kfddata, skip, jsonobj)
                    #设置类名吧
                    if retval is not None and kfddata is not None:
                        retval["__cls__"] = kfddata["class"]
                        pass
                    pass
        return retval
        pass

    @staticmethod
    def _write_base_value(bytearr, dataType, valObject):
        if dataType == pyKFDataType.OT_UNKNOW:
            return
        elif dataType == pyKFDataType.OT_INT8:
            bytearr.write_byte(valObject)
        elif dataType == pyKFDataType.OT_UINT8:
            bytearr.write_ubyte(valObject)
        elif dataType == pyKFDataType.OT_INT16:
            bytearr.write_short(valObject)
        elif dataType == pyKFDataType.OT_UINT16:
            bytearr.write_ushort(valObject)
        elif dataType == pyKFDataType.OT_INT32:
            bytearr.write_int(valObject)
        elif dataType == pyKFDataType.OT_UINT32:
            bytearr.write_uint(valObject)
        elif dataType == pyKFDataType.OT_FLOAT:
            bytearr.write_float(valObject)
        elif dataType == pyKFDataType.OT_DOUBLE:
            bytearr.write_double(valObject)
        elif dataType == pyKFDataType.OT_STRING:
            bytearr.write_string(valObject)
        elif dataType == pyKFDataType.OT_NULL:
            return
        elif dataType == pyKFDataType.OT_BYTES:
            bytearr.write_bytesbuff(valObject)
        elif dataType == pyKFDataType.OT_BOOL:
            bytearr.write_bool(valObject)
        elif dataType == pyKFDataType.OT_VARUINT:
            bytearr.write_varuint(valObject)
        elif dataType == pyKFDataType.OT_INT64:
            bytearr.write_int64(valObject)
        elif dataType == pyKFDataType.OT_UINT64:
            bytearr.write_uint64(valObject)
        else:
            raise Exception("不支持的类型" + str(dataType))

    @staticmethod
    def _write_array_value(bytearr, valtype, val, propinfo):
        arrval = val
        arrsize = len(arrval)
        bytearr.write_varuint(arrsize)
        oType = 0
        kfddata = None
        if propinfo and hasattr(propinfo, "otype"):
            #字符类型
            otypestr = propinfo["otype"]
            oType = KFDataType.GetTypeID(otypestr)

            if oType == 0:
                kfddata = pyKFDTable.kfdTB.get_kfddata(otypestr)
                if kfddata is not None:
                    if valtype == pyKFDataType.OT_ARRAY:
                        oType = pyKFDataType.OT_OBJECT
                    else:
                        oType = pyKFDataType.OT_MIXOBJECT
                pass
            pass

        if valtype == pyKFDataType.OT_ARRAY:
            if oType != 0 and oType <= pyKFDataType.OT_UINT64:
                bytearr.write_byte(oType)
                for item in arrval:
                    pyKFDJson._write_base_value(bytearr, oType, item)
            else:
                if oType == 0 or oType == pyKFDataType.OT_ARRAY or oType == pyKFDataType.OT_MIXARRAY:
                    raise Exception("暂时不支持多维数组")
                elif oType == pyKFDataType.OT_OBJECT or oType == pyKFDataType.OT_MIXOBJECT:
                    bytearr.write_byte(oType)
                    if oType == pyKFDataType.OT_MIXOBJECT:
                        raise Exception("普通数组不支持MIXOBJECT对象")
                    for item in arrval:
                        pyKFDJson._write_object_value(bytearr, oType, item, kfddata)
        elif valtype == pyKFDataType.OT_MIXARRAY:
            for item in arrval:
                pyKFDJson.write_value(bytearr, item)
                pass
            pass
            pass
        pass

    @staticmethod
    def _write_object_value(bytearr, dataType, objectval, kfddata):
        bytearr.write_varuint(pyKFDataType.OBJ_PROP_ID_BEGIN)
        if kfddata is not None:
            # 写入继承的属性
            if "extend" in kfddata:
                extenddata = pyKFDTable.kfdTB.get_kfddata(kfddata["extend"])
                if extenddata is not None:
                    pyKFDJson._write_object_value(bytearr, dataType, objectval, extenddata)
                    pass
                pass
            # 写自己的属性吧
            valarr = kfddata["propertys"]
            for item in valarr:
                pid = item["id"]
                name = item["name"]
                if name in objectval and pid != pyKFDataType.OBJ_PROP_ID_BEGIN and pid != pyKFDataType.OBJ_PROP_ID_END:
                    bytearr.write_varuint(pid)
                    pyKFDJson.write_value(bytearr, objectval[name], item)
                    pass
                pass
            pass
        bytearr.write_varuint(pyKFDataType.OBJ_PROP_ID_END)
        pass

    @staticmethod
    def write_value(bytearr, jsonobj, propinfo = None):
        if jsonobj != None:
            valueType = pyKFDataType.OT_NULL
            kfddata = None
            if propinfo is not None:
                valueType = KFDataType.GetTypeID(propinfo["type"])
                pass
            elif jsonobj is not None and "__cls__" in jsonobj:
                kfddata = pyKFDTable.kfdTB.get_kfddata(jsonobj["__cls__"])
                valueType = pyKFDataType.OT_MIXOBJECT
                pass
            bytearr.write_byte(valueType)
            if valueType == pyKFDataType.OT_NULL:
                pass
            elif valueType <= pyKFDataType.OT_UINT64:
                pyKFDJson._write_base_value(bytearr, valueType, jsonobj)
            else:
                if valueType == pyKFDataType.OT_ARRAY or valueType == pyKFDataType.OT_MIXARRAY:
                    pyKFDJson._write_array_value(bytearr, valueType, jsonobj, propinfo)
                elif valueType == pyKFDataType.OT_OBJECT:
                    kfddata = pyKFDTable.kfdTB.get_kfddata(propinfo["otype"])
                    pyKFDJson._write_object_value(bytearr, valueType, jsonobj, kfddata)
                elif valueType == pyKFDataType.OT_MIXOBJECT:
                    if kfddata is not None:
                        bytearr.write_varuint(1)
                        bytearr.write_string(kfddata["class"])
                    else:
                        # 写入类ID
                        bytearr.write_varuint(0)
                    # 写入所有属性
                    pyKFDJson._write_object_value(bytearr, valueType, jsonobj, kfddata)
                else:
                    return
        else:
            bytearr.write_byte(pyKFDataType.OT_NULL)
        pass
    pass

