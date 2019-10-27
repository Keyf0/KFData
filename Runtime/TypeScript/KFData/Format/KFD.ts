

export class KFDataType
{
    public static OT_UNKNOW	= 0;
    public static OT_INT8	= 0x01;
    public static OT_UINT8	= 0x02;
    public static OT_INT16	= 0x03;
    public static OT_UINT16	= 0x04;
    public static OT_INT32	= 0x05;
    public static OT_UINT32	= 0x06;
    public static OT_FLOAT	= 0x07;
    public static OT_DOUBLE	= 0x08;
    public static OT_STRING	= 0x09;
    public static OT_NULL	= 0x0A;
    public static OT_BYTES	= 0x0B;
    public static OT_BOOL       = 0x0C;
    public static OT_VARUINT    = 0x0D;
    public static OT_INT64      = 0x0E;
    public static OT_UINT64     = 0x0F;
    public static OT_ARRAY	    = 0x10;
    public static OT_MIXARRAY   = 0x11;
    public static OT_OBJECT	    = 0x12;
    public static OT_MIXOBJECT  = 0x13;

    public static OBJ_PROP_ID_BEGIN = 0X7F;
    public static OBJ_PROP_ID_END   = 0;


    public static ID_to_types:any = null;
    public static Base_Type_ids:any = null;
    public static Type_to_ids:any = null;

    public static NumIntEnum:any  = {}
    public static NumFloatEnum:any  = {}

    public static Is_numInt(type:number):boolean
    {
        return KFDataType.NumIntEnum[type];
    }

    public static Is_BaseTypeStr(typestr:string):boolean
    {
        return KFDataType.Base_Type_ids[typestr];
    }

    public static Is_numFloat(type:number):boolean
    {
        return type == KFDataType.OT_FLOAT
            || type == KFDataType.OT_DOUBLE;
    }

    public static GetTypeID(type):number
    {
        if (KFDataType.Type_to_ids == null)
        {
            let Type_to_ids = {};
            Type_to_ids["int8"] = KFDataType.OT_INT8
            Type_to_ids["uint8"] = KFDataType.OT_UINT8
            Type_to_ids["int16"] = KFDataType.OT_INT16
            Type_to_ids["uint16"] = KFDataType.OT_UINT16
            Type_to_ids["int"] = KFDataType.OT_INT32
            Type_to_ids["int32"] = KFDataType.OT_INT32
            Type_to_ids["uint32"] = KFDataType.OT_UINT32
            Type_to_ids["uint"] = KFDataType.OT_UINT32
            Type_to_ids["float"] = KFDataType.OT_FLOAT
            Type_to_ids["double"] = KFDataType.OT_DOUBLE
            Type_to_ids["num1"] = KFDataType.OT_FLOAT
            Type_to_ids["num2"] = KFDataType.OT_DOUBLE
            Type_to_ids["kfstr"] = KFDataType.OT_STRING
            Type_to_ids["kfname"] = KFDataType.OT_STRING
            Type_to_ids["null"] = KFDataType.OT_NULL
            Type_to_ids["kfBytes"] = KFDataType.OT_BYTES
            Type_to_ids["bool"] = KFDataType.OT_BOOL
            Type_to_ids["varuint"] = KFDataType.OT_VARUINT
            Type_to_ids["int64"] = KFDataType.OT_INT64
            Type_to_ids["uint64"] = KFDataType.OT_UINT64
            Type_to_ids["arr"] = KFDataType.OT_ARRAY
            Type_to_ids["mixarr"] = KFDataType.OT_MIXARRAY
            Type_to_ids["object"] = KFDataType.OT_OBJECT
            Type_to_ids["mixobject"] = KFDataType.OT_MIXOBJECT

            let ID_to_types = {};

            for( let keystr in Type_to_ids)
            {
                ID_to_types[Type_to_ids[keystr]] = keystr;
            }

            let Base_Type_ids = {}

            Base_Type_ids["int8"] = KFDataType.OT_INT8
            Base_Type_ids["uint8"] = KFDataType.OT_UINT8
            Base_Type_ids["int16"] = KFDataType.OT_INT16
            Base_Type_ids["uint16"] = KFDataType.OT_UINT16
            Base_Type_ids["int"] = KFDataType.OT_INT32
            Base_Type_ids["int32"] = KFDataType.OT_INT32
            Base_Type_ids["uint32"] = KFDataType.OT_UINT32
            Base_Type_ids["uint"] = KFDataType.OT_UINT32
            Base_Type_ids["float"] = KFDataType.OT_FLOAT
            Base_Type_ids["double"] = KFDataType.OT_DOUBLE
            Base_Type_ids["num1"] = KFDataType.OT_FLOAT
            Base_Type_ids["num2"] = KFDataType.OT_DOUBLE
            Base_Type_ids["kfstr"] = KFDataType.OT_STRING
            Base_Type_ids["kfname"] = KFDataType.OT_STRING
            Base_Type_ids["null"] = KFDataType.OT_NULL
            Base_Type_ids["kfBytes"] = KFDataType.OT_BYTES
            Base_Type_ids["bool"] = KFDataType.OT_BOOL
            Base_Type_ids["varuint"] = KFDataType.OT_VARUINT
            Base_Type_ids["int64"] = KFDataType.OT_INT64
            Base_Type_ids["uint64"] = KFDataType.OT_UINT64

            KFDataType.Base_Type_ids = Base_Type_ids
            KFDataType.Type_to_ids = Type_to_ids
            KFDataType.ID_to_types = ID_to_types

            let NumIntEnum = {};
            NumIntEnum[KFDataType.OT_INT8] = true
            NumIntEnum[KFDataType.OT_UINT8] = true
            NumIntEnum[KFDataType.OT_INT16] = true
            NumIntEnum[KFDataType.OT_UINT16] = true
            NumIntEnum[KFDataType.OT_INT32] = true
            NumIntEnum[KFDataType.OT_UINT32] = true
            NumIntEnum[KFDataType.OT_INT64] = true
            NumIntEnum[KFDataType.OT_UINT64] = true

            KFDataType.NumIntEnum = NumIntEnum
        }

        let ret = KFDataType.Type_to_ids[type];
        return ret == null ? 0: ret;
    }

}