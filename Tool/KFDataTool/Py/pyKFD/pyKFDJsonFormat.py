from .pyKFData import *
from .pyKFByteArray import *

class pyKFDJsonFormat(object):

    @staticmethod
    def ExportJson(kfdValue):
        jsonobj = None
        valueType = kfdValue.valueType
        if valueType <= pyKFDataType.OT_UINT64:
            jsonobj = kfdValue.val
            pass
        else:
            if valueType == pyKFDataType.OT_ARRAY or valueType == pyKFDataType.OT_MIXARRAY:
                jsonobj = []
                valarr = kfdValue.val
                valsize = 0
                if valarr is not None:
                    valsize = len(valarr)
                i = 0
                while i < valsize:
                    valitem = valarr[i]
                    jsonobj.append(pyKFDJsonFormat.ExportJson(valitem))
                    i += 1
                    pass
                pass
            elif valueType == pyKFDataType.OT_OBJECT or valueType == pyKFDataType.OT_MIXOBJECT:
                jsonobj = {}
                pyKFDJsonFormat.ExportPropToJson(kfdValue.val,jsonobj)

                if hasattr(kfdValue,"extendval"):
                    extendval = kfdValue.extendval
                    if extendval is not None:
                        pyKFDJsonFormat.ExportPropToJson(extendval.val,jsonobj)
                        pass
                    pass
                pass

                if valueType == pyKFDataType.OT_MIXOBJECT:
                    ##jsonobj["__clsid__"] = kfdValue.classid
                    if hasattr(kfdValue,"kfddata"):
                        kfddata = kfdValue.kfddata
                        if kfddata and ("class" in kfddata):
                            jsonobj["__cls__"] = kfddata["class"]
                            pass
                        pass
                    pass
                pass
            pass
        return jsonobj
        pass

    @staticmethod
    def ExportPropToJson(valarr,jsonobj):
        valsize = 0
        if valarr is not None:
            valsize = len(valarr)
        i = 0
        while i < valsize:
            valitem = valarr[i]
            if hasattr(valitem, "propinfo"):
                jsonobj[valitem.propinfo["name"]] = pyKFDJsonFormat.ExportJson(valitem)
                pass
            i += 1
            pass
        pass

    @staticmethod
    def ImportJson(jsonobj):

        if jsonobj is None:
            return None

        if ("__clsid__" not in jsonobj) and ("__cls__" not in jsonobj):
            return None

        kfdvalue = pyKFDValue()

        kfdvalue.valueType = pyKFDataType.OT_MIXOBJECT
        kfdvalue.val = []

        kfdvalue.auto_bind(True,jsonobj)

        if kfdvalue.kfddata == None:
            logging.error("%s:NOT FOUND CLASS",jsonobj["__cls__"])
        return kfdvalue

        pass

    pass
