
class pyKFDataType():

    OT_UNKNOW	= 0
    OT_INT8		= 0x01
    OT_UINT8	= 0x02
    OT_INT16	= 0x03
    OT_UINT16	= 0x04
    OT_INT32	= 0x05
    OT_UINT32	= 0x06
    OT_FLOAT	= 0x07
    OT_DOUBLE	= 0x08
    OT_STRING	= 0x09
    OT_NULL		= 0x0A
    OT_BYTES	= 0x0B
    OT_BOOL     = 0x0C
    OT_VARUINT  = 0x0D
    OT_INT64    = 0x0E
    OT_UINT64   = 0x0F
    OT_ARRAY	= 0x10
    OT_MIXARRAY = 0x11
    OT_OBJECT	= 0x12
    OT_MIXOBJECT= 0x13

    OBJ_PROP_ID_BEGIN = 0X7F
    OBJ_PROP_ID_END = 0

    ID_to_types = None
    Base_Type_ids = None
    Type_to_ids = None

    NumIntEnum = {}
    NumFloatEnum = {}

    @staticmethod
    def Is_numInt(type):
        return type in pyKFDataType.NumIntEnum
        pass

    @staticmethod
    def Is_BaseTypeStr(typestr):
        return typestr in pyKFDataType.Base_Type_ids
        pass

    @staticmethod
    def Is_numFloat(type):
        return type == pyKFDataType.OT_FLOAT or type == pyKFDataType.OT_DOUBLE
        pass

    @staticmethod
    def Is_Bool(type):
        return type == pyKFDataType.OT_BOOL
        pass

    @staticmethod
    def Is_Str(type):
        return type == pyKFDataType.OT_STRING
        pass

    DEBUG_SHOW = False
    pass

class KFDataType(pyKFDataType):

    @staticmethod
    def GetTypeID(type):
        if KFDataType.Type_to_ids is None:
            Type_to_ids = {}
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

            ID_to_types = {}

            for key in Type_to_ids:
                ID_to_types[Type_to_ids[key]] = key

            Base_Type_ids = {}

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

            pyKFDataType.Base_Type_ids = Base_Type_ids
            pyKFDataType.Type_to_ids = Type_to_ids
            pyKFDataType.ID_to_types = ID_to_types

            NumIntEnum = {}
            NumIntEnum[KFDataType.OT_INT8] = True
            NumIntEnum[KFDataType.OT_UINT8] = True
            NumIntEnum[KFDataType.OT_INT16] = True
            NumIntEnum[KFDataType.OT_UINT16] = True
            NumIntEnum[KFDataType.OT_INT32] = True
            NumIntEnum[KFDataType.OT_UINT32] = True
            NumIntEnum[KFDataType.OT_INT64] = True
            NumIntEnum[KFDataType.OT_UINT64] = True

            pyKFDataType.NumIntEnum = NumIntEnum

        if type in KFDataType.Type_to_ids:
            return pyKFDataType.Type_to_ids[type]
        return 0
        pass