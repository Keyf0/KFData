import {KFByteArray} from "../Utils/FKByteArray";
import {KFDataType} from "./KFD";
import {KFDTable} from "./KFDTable";

export class KFDJson
{
    private static _read_base_value(bytearr:KFByteArray
                                    , valtype:number
                                    , skip:boolean=false
                                    , value:any=null):any
    {
        let retval = value;
        switch (valtype)
        {
            case KFDataType.OT_INT8:
            {
                if (skip)
                    bytearr.Skip(1);
                else
                    retval = bytearr.readByte();
            }
                break;
        case KFDataType.OT_UINT8:
            {
                if(skip)
                    bytearr.Skip(1);
                else
                    retval = bytearr.readUnsignedByte();
            }
            break;
        case KFDataType.OT_INT16:
            if(skip)
                bytearr.Skip(2);
            else
                retval = bytearr.readShort();
            break;
            case KFDataType.OT_UINT16:
            if (skip)
                bytearr.Skip(2);
            else
                retval = bytearr.readUnsignedShort();
                break;
            case KFDataType.OT_INT32:
                if(skip)
                    bytearr.Skip(4);
                else
                    retval = bytearr.readInt();
                break;
            case KFDataType.OT_UINT32:
                if(skip)
                    bytearr.Skip(4);
                else
                    retval = bytearr.readUnsignedInt();
                break;
            case KFDataType.OT_FLOAT:
                if(skip)
                    bytearr.Skip(4);
                else
                    retval = bytearr.readFloat();
                break;
            case KFDataType.OT_DOUBLE:
                if(skip)
                    bytearr.Skip(8);
                else
                    retval = bytearr.readDouble();
                break;
            case KFDataType.OT_STRING:
                if(skip)
                    bytearr.skipstring();
                else
                    retval = bytearr.readstring();
                break;
            case KFDataType.OT_BYTES:
                if(skip)
                    bytearr.skipstring();
                else
                    retval = bytearr.readkfbytes();
                break;
            case KFDataType.OT_BOOL:
                if(skip)
                    bytearr.Skip(1);
                else
                    retval = bytearr.readBoolean();
                break;
            case KFDataType.OT_VARUINT:
                    retval = bytearr.readvaruint();
                break;
            case KFDataType.OT_INT64:
                if(skip)
                    bytearr.Skip(8);
                else
                    retval = bytearr.readInt64();
                break;
            case KFDataType.OT_UINT64:
                if(skip)
                    bytearr.Skip(8);
                break;
            default:
                    retval = bytearr.readUInt64();
        }
        return retval
    }

    private static _read_object_value(bytearr:KFByteArray
                                      , valtype:number
                                      , kfddata:any = null
                                      , skip:boolean = false
                                      , val:any = null):any
    {
        let retval = val;
        if(skip)
        {
            let deep = 0;
            while(true)
            {
                let pid = bytearr.readvaruint();
                if (pid == KFDataType.OBJ_PROP_ID_BEGIN)
                    deep += 1;
                else if(pid == KFDataType.OBJ_PROP_ID_END)
                {
                    deep -= 1;
                    if(deep <= 0)
                        break
                }
                else {
                        KFDJson.read_value(bytearr,true);
                    }
            }
        }
        else
        {
            let deep = 0;
            let obj = null;
            if(val != null)
                obj = val;
            if(obj == null)
                obj = {};

            let currKFDData = kfddata;
            let stack = [];

            while (true)
            {
                let pid = bytearr.readvaruint();
                if(pid == KFDataType.OBJ_PROP_ID_BEGIN)
                {
                    if (deep != 0)
                    {
                        let child = KFDTable.find_extend_kfddata(currKFDData);
                        stack.push(currKFDData);
                        currKFDData = child;
                    }
                    deep += 1;
                }
                else if(pid == KFDataType.OBJ_PROP_ID_END)
                {
                    deep -= 1;
                    if(deep <= 0)
                        break;
                    else
                        currKFDData = stack.pop();
                }else
                {
                    let pinfo = KFDTable.find_prop_info(currKFDData, pid);
                    if(pinfo != null)
                    {
                        let pname = pinfo["name"];
                        let propobj = null;

                        if(obj != null && obj[pname])
                        {
                            propobj = obj[pname];
                        }
                        obj[pname] = KFDJson.read_value(bytearr,false, propobj, pinfo);
                    }
                    else
                    {
                        KFDJson.read_value(bytearr,true);
                    }
                }
            }
            retval = obj;
        }
        return retval;
    }


    private static _read_array_value(bytearr:KFByteArray
                                     , valtype:number
                                     , skip:boolean = false
                                     , val:any = null
                                     , propinfo:any = null):any
    {
        let retval = val;
        if(valtype == KFDataType.OT_ARRAY)
        {
            let size = bytearr.readvaruint();
            let otype = bytearr.readUnsignedByte();
            if(otype <= KFDataType.OT_UINT64)
            {
                if(skip){
                    while (size > 0)
                    {
                        KFDJson._read_base_value(bytearr, otype, true);
                        size -= 1;
                    }
                }else
                {
                    let objarr = [];
                    while (size > 0)
                    {
                        objarr.push(KFDJson._read_base_value(bytearr, otype));
                        size -= 1;
                    }
                    retval = objarr;
                }
            }
            else
            {
                if(otype == KFDataType.OT_ARRAY || otype == KFDataType.OT_MIXARRAY)
                {
                    if(skip)
                    {
                        while (size > 0)
                        {
                            KFDJson._read_array_value(bytearr, otype, true);
                            size -= 1;
                        }
                    }
                    else
                    {
                        let arrobj = [];
                        while (size > 0)
                        {
                            arrobj.push(KFDJson._read_array_value(bytearr, otype));
                            size -= 1;
                        }
                        retval = arrobj;
                    }
                }
                else if(otype == KFDataType.OT_OBJECT || otype == KFDataType.OT_MIXOBJECT)
                {
                    if(skip)
                    {
                        while (size > 0)
                        {
                            KFDJson._read_object_value(bytearr, otype,null , true);
                            size -= 1;
                        }
                    }
                    else
                    {
                        let kfddata = null;
                        if (otype == KFDataType.OT_OBJECT && propinfo)
                        {
                            kfddata = KFDTable.kfdTB.get_kfddata(propinfo["otype"]);
                        }

                        let objarr = [];
                        while (size > 0)
                        {
                            objarr.push(KFDJson._read_object_value(bytearr, otype, kfddata));
                            size -= 1;
                        }
                        retval = objarr;
                    }
                }
            }

        }
        else if(valtype == KFDataType.OT_MIXARRAY)
        {
            let size = bytearr.readvaruint();
            if(skip)
            {
                while (size > 0)
                {
                    KFDJson.read_value(bytearr,true);
                    size -= 1;
                }
            }
            else
            {
                let arrobj = [];
                while (size > 0)
                {
                    arrobj.push(KFDJson.read_value(bytearr,false));
                    size -= 1;
                }
                retval = arrobj;
            }
        }
        return retval;
    }


    public static read_value(bytearr:KFByteArray
                             , skip:boolean = false
                             , jsonobj:any = null
                             , propinfo:any = null):any
    {
        let retval = jsonobj;
        let size = bytearr.bytesAvailable;

        if(size > 0)
        {
            let valueType = bytearr.readUnsignedByte();
            if(valueType <= KFDataType.OT_UINT64)
            {
                retval = KFDJson._read_base_value(bytearr, valueType, skip, jsonobj);
            }
            else
            {
                if(valueType == KFDataType.OT_ARRAY || valueType == KFDataType.OT_MIXARRAY)
                {
                    retval = KFDJson._read_array_value(bytearr, valueType, skip, jsonobj, propinfo);
                }
                else if(valueType == KFDataType.OT_OBJECT)
                {
                    let kfddata = null;
                    if(propinfo != null)
                        kfddata = KFDTable.kfdTB.get_kfddata(propinfo["otype"]);
                    retval = KFDJson._read_object_value(bytearr, valueType, kfddata, skip, jsonobj);
                }
                else if(valueType == KFDataType.OT_MIXOBJECT)
                {
                    let classid = bytearr.readvaruint();
                    let classname = null;
                    if (classid == 1)
                    {
                        classname = bytearr.readstring();
                    }
                    let kfddata = null;
                    if ( classname && classname != "")
                        kfddata = KFDTable.kfdTB.get_kfddata(classname);
                    retval = KFDJson._read_object_value(bytearr, valueType, kfddata, skip, jsonobj);
                    //设置类名吧
                    if(retval && kfddata)
                    {
                        retval["__cls__"] = kfddata["class"];
                    }
                }
            }
        }

        return retval;
    }

    public static write_value(bytearr:KFByteArray
                              , jsonobj:any
                              , propinfo:any = null)
    {
        if(jsonobj != null)
        {
           let valueType = KFDataType.OT_NULL;
           let kfddata = null;

           if(propinfo)
           {
               valueType = KFDataType.GetTypeID(propinfo["type"]);
           }

           if(jsonobj && jsonobj["__cls__"])
           {
               kfddata = KFDTable.kfdTB.get_kfddata(jsonobj["__cls__"]);
               valueType = KFDataType.OT_MIXOBJECT;
           }

           bytearr.writeByte(valueType);

           if(valueType == KFDataType.OT_NULL){}
           else if(valueType <= KFDataType.OT_UINT64)
           {
               KFDJson._write_base_value(bytearr, valueType, jsonobj);
           }else
               {
                   if(valueType == KFDataType.OT_ARRAY || valueType == KFDataType.OT_MIXARRAY)
                   {
                       KFDJson._write_array_value(bytearr, valueType, jsonobj, propinfo);
                   }else if(valueType == KFDataType.OT_OBJECT)
                   {
                       kfddata = KFDTable.kfdTB.get_kfddata(propinfo["otype"]);
                       KFDJson._write_object_value(bytearr, valueType, jsonobj, kfddata);
                   }else if(valueType == KFDataType.OT_MIXOBJECT)
                   {
                       if(kfddata)
                       {
                           bytearr.writevaruint(1);
                           bytearr.writestring(kfddata["class"]);
                       }else
                           bytearr.writevaruint(0);

                       KFDJson._write_object_value(bytearr, valueType, jsonobj, kfddata);
                   }
               }
        }
        else
        {
            bytearr.writeByte(KFDataType.OT_NULL);
        }
    }


    private static _write_base_value(bytearr:KFByteArray
                                     , dataType:number
                                     , valObject:any)
    {
        if(dataType == KFDataType.OT_UNKNOW)return;

        switch (dataType)
        {
            case KFDataType.OT_INT8:
            case KFDataType.OT_UINT8:
                bytearr.writeByte(valObject);
                break;
            case KFDataType.OT_INT16:
                bytearr.writeShort(valObject);
                break;
            case KFDataType.OT_UINT16:
                bytearr.writeUnsignedShort(valObject);
                break;
            case KFDataType.OT_INT32:
                bytearr.writeInt(valObject);
                break;
            case KFDataType.OT_UINT32:
                bytearr.writeUnsignedInt(valObject);
                break;
            case KFDataType.OT_INT64:
                bytearr.writeInt64(valObject);
                break;
            case KFDataType.OT_UINT64:
                bytearr.writeUInt64(valObject);
                break;
            case KFDataType.OT_FLOAT:
                bytearr.writeFloat(valObject);
                break;
            case KFDataType.OT_DOUBLE:
                bytearr.writeDouble(valObject);
                break;
            case KFDataType.OT_STRING:
                bytearr.writestring(valObject);
                break;
            case KFDataType.OT_BYTES:
                bytearr.writeBytes(valObject);
                break;
            case KFDataType.OT_BOOL:
                bytearr.writeBoolean(valObject);
                break;
            case KFDataType.OT_VARUINT:
                bytearr.writevaruint(valObject);
                break;

        }
    }

    private static _write_array_value(bytearr:KFByteArray
                                      , valtype:number
                                      , val:any
                                      , propinfo:any)
    {
        let arrval = val
        let arrsize = arrval.length;
        bytearr.writevaruint(arrsize);

        let oType = 0;
        let kfddata = null;

        if(propinfo && propinfo["otype"])
        {
            let otypestr = propinfo["otype"]
            oType = KFDataType.GetTypeID(otypestr);

            if(oType == 0)
            {
                kfddata = KFDTable.kfdTB.get_kfddata(otypestr)
                if (kfddata)
                {
                    if (valtype == KFDataType.OT_ARRAY)
                        oType = KFDataType.OT_OBJECT
                    else
                        oType = KFDataType.OT_MIXOBJECT
                }
            }
        }

        if(valtype == KFDataType.OT_ARRAY)
        {
            if(oType != 0 && oType <= KFDataType.OT_UINT64)
            {
                bytearr.writeByte(oType);
                for (let item of arrval) {
                    KFDJson._write_base_value(bytearr, oType, item);
                }
            }else
            {
                if(oType == 0 || oType == KFDataType.OT_ARRAY || oType == KFDataType.OT_MIXARRAY)
                {
                    ///不支持
                }else if(oType == KFDataType.OT_OBJECT || oType == KFDataType.OT_MIXOBJECT)
                {
                    bytearr.writeByte(oType);
                    if(oType == KFDataType.OT_MIXOBJECT){}
                    for (let item of arrval)
                    {
                        KFDJson._write_object_value(bytearr, oType, item, kfddata);
                    }
                }
            }

        }
        else if(valtype == KFDataType.OT_MIXARRAY)
        {
            for (let item of arrval)
            {
                KFDJson.write_value(bytearr, item)
            }
        }
    }

    private static _write_object_value(bytearr:KFByteArray
                                       , dataType:number
                                       , objectval:any
                                       , kfddata:any)
    {
        bytearr.writevaruint(KFDataType.OBJ_PROP_ID_BEGIN);
        if(kfddata)
        {
             let extendcls = kfddata["extend"];
             if(extendcls)
             {
                 let extenddata = KFDTable.kfdTB.get_kfddata(extendcls);
                 if(extenddata != null)
                    KFDJson._write_object_value(bytearr, dataType, objectval, extenddata);
             }

             let valarr = kfddata["propertys"];
            for (let item of valarr)
            {
                let pid = item["pid"];
                let name = item["name"];

                if(objectval.hasOwnProperty(name) &&
                    pid != KFDataType.OBJ_PROP_ID_BEGIN &&
                    pid != KFDataType.OBJ_PROP_ID_END)
                {
                    bytearr.writevaruint(pid)
                    KFDJson.write_value(bytearr, objectval[name], item)
                }
            }
        }
        bytearr.writevaruint(KFDataType.OBJ_PROP_ID_END);
    }

}