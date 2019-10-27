
from .pyKFDataType import *

class pyKFDTable(object):

    kfdTB = None

    @staticmethod
    def find_json_kfddata(jsonobj):
        kfddata = None
        if ("__cls__" not in jsonobj):
            if ("__clsid__" not in jsonobj):
                return None
            else:
                kfddata = pyKFDTable.kfdTB.get_kfddata_clsid(jsonobj["__clsid__"])
            pass
        else:
            kfddata = pyKFDTable.kfdTB.get_kfddata(jsonobj["__cls__"])
            pass
        return kfddata

    @staticmethod
    def find_extend_kfddata(kfddata):
        if kfddata is None:
            return None
        __extend__ = None
        if "__extend__" in kfddata:
            __extend__ = kfddata["__extend__"]
            pass
        if __extend__ is None and "extend" in kfddata:
            __extend__ = pyKFDTable.kfdTB.get_kfddata(kfddata["extend"])
            if __extend__ is not None:
                kfddata["__extend__"] = __extend__
            pass
        return __extend__
        pass

    @staticmethod
    def find_prop_info(kfddata,pid):
        if kfddata is None:
            return
        __ids__ = None
        if "__ids__" in kfddata:
            __ids__ = kfddata["__ids__"]
            pass
        if __ids__ is None:
            __ids__ = {}
            kfddata["__ids__"] = __ids__
            propertys = kfddata["propertys"]
            proplen = len(propertys)
            i = 0
            while i < proplen:
                prop = propertys[i]
                __ids__[prop["id"]] = prop
                i += 1
                pass
            pass
        if pid in __ids__:
            return  __ids__[pid]
            pass
        return  None
        pass


    def __init__(self):
        self.kfddata_maps = {}
        pass

    def get_kfddata(self,clsname):
        if clsname in self.kfddata_maps:
            return self.kfddata_maps[clsname]
        return None

    def get_kfddata_clsid(self, clsid):
        return None

    def has_cls(self,clsname):
        return clsname in self.kfddata_maps

    def has_otype(self,otype):
        if otype == "":
            return True
        if pyKFDataType.Is_BaseTypeStr(otype):
            return True
        return self.has_cls(otype)
        pass

    def get_propkey(self,kfddata):
        propertys = kfddata["propertys"]
        count = len(propertys)
        i = 0
        while i < count:
            pinfo = propertys[i]
            if "KEY" in pinfo and pinfo["KEY"] != "":
                return pinfo
            i += 1
        return None
        pass

    pass
