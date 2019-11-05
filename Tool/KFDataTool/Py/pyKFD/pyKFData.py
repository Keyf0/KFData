import logging

from .pyKFDataType import *
from .pyKFDTable import *


def find_prop_info(kfdata,pid):
    for info in kfdata["propertys"]:
        if info["id"] == pid:
            return info
    return None
    pass

def find_prop_info_name(kfdata,name):
    for info in kfdata["propertys"]:
        if info["name"] == name:
            return info
    return None
    pass

def get_enum_default(enumkfddata,pdefault):

    sindex = pdefault.find("::")
    if sindex != -1:
        pname = pdefault[sindex + 2:]
        propinfo = find_prop_info_name(enumkfddata,pname)
        if propinfo is None:
            return None
        if "default" not in propinfo:
            return None
        return propinfo["default"]
        pass
    return pdefault
    pass

def default_val(prop,valuetype):

    defval = None
    if prop is not None and "default" in prop:
        pdefault = prop["default"]
        if "enum" in prop:
            enumcls = prop["enum"]
            enumkfddata = pyKFDTable.kfdTB.get_kfddata(enumcls)
            if enumkfddata is not None:
                defval = get_enum_default(enumkfddata,pdefault)
                pass
            pass
        else:
            defval = pdefault
        pass
    pass


    if valuetype > pyKFDataType.OT_UINT64:
        return []
    elif pyKFDataType.Is_Str(valuetype):
        if defval is None:
            return ""
        else:
            return defval
        pass
    elif pyKFDataType.Is_numFloat(valuetype):
        if defval is None:
            return 0.0
        else:
            try:
                return float(defval)
            except:
                return 0
        pass
    elif pyKFDataType.Is_Bool(valuetype):
        if defval is None:
            return False
        else:
            return defval == "true"
        pass
    elif pyKFDataType.Is_numInt(valuetype):
        if defval is None:
            return 0
        else:
            if defval.isdigit():
                return int(defval)
            else:
                return 0
        pass
    return None
    pass

Style_ViewObject = {}

def Create_ViewObject(otype,valueObject):
    global Style_ViewObject
    if otype in Style_ViewObject:
        return Style_ViewObject[otype](valueObject)
    return None
    pass

#####

class pyKFDValue(object):

    #get_kfddata_clsid
    def __init__(self):
        self.valueType = pyKFDataType.OT_UNKNOW
        self.val = None
        #self.pid = 0
        #self.oType = 0
        #self.classid = 0
        #self.classname = ""
        #self.kfddata = None
        #self.propinfo = None
        #self.extendval = None
        pass

    def get_propval(self,propname,CREATE=True):

        for pval in self.val:
            if hasattr(pval, "propinfo") and pval.propinfo["name"] == propname:
                return pval
            pass

        if CREATE:
            return self.add_propval(propname,True)
        return None
        pass

    def add_propval(self,propname,CREATE=False,jsonobj=None):

        proparrs = self.get_can_add_props(propname)

        if(len(proparrs) != 0):

            prop = proparrs[0]
            typename = prop["type"]

            ovType = pyKFDataType.Type_to_ids[typename]
            valitem = None

            if ovType == pyKFDataType.OT_MIXARRAY or ovType == pyKFDataType.OT_ARRAY:
                valitem = pyKFDValue()
                arrotype = prop["otype"]
                if pyKFDataType.Is_BaseTypeStr(arrotype):
                    valitem.oType = pyKFDataType.Base_Type_ids[arrotype]
                else:
                    valitem.oType = pyKFDataType.OT_OBJECT
                pass
            elif ovType == pyKFDataType.OT_OBJECT or ovType == pyKFDataType.OT_MIXOBJECT:
                valitem = pyKFDValue()
                pass
            else:
                valitem = pyKFDValue()
                pass

            valitem.pid = prop["id"]
            valitem.val = default_val(prop, ovType)
            valitem.valueType = ovType

            valitem.bind_propinfo(prop,CREATE,jsonobj)

            self.val.append(valitem)

            return valitem
            pass
        return None
        pass

    def add_arrval(self,otype,CREATE=False,jsonobj=None, propinfo=None):
        valitem = None
        if not pyKFDataType.Is_BaseTypeStr(otype):
            okfddata = pyKFDTable.kfdTB.get_kfddata(otype)
            if okfddata is not None:

                valitem = pyKFDValue()
                valitem.val = []

                valitem.valueType = pyKFDataType.OT_OBJECT
                if self.valueType == pyKFDataType.OT_MIXARRAY:
                    valitem.valueType = pyKFDataType.OT_MIXOBJECT

                valitem.bind_kfddata(okfddata,CREATE,jsonobj)
                self.val.append(valitem)
            pass
        else:
            #基础数据类型
            ovType = pyKFDataType.Base_Type_ids[otype]
            valitem = pyKFDValue()
            valitem.valueType = ovType
            valitem.val = default_val(None, ovType)
            #为了增加ENUM的支持 临时增加没有详细测试 2019.17.24
            valitem.propinfo = propinfo

            if jsonobj is not None and CREATE:
                valitem.val = jsonobj

            self.val.append(valitem)
        return valitem
        pass

    def remove_val(self,kfdvalue):
        val = self.val
        if val is None:
            return False
        i = val.index(kfdvalue)
        if i >= 0:
            val.remove(kfdvalue)
            return True
        return False
        pass

    def get_enum_data(self):
        if hasattr(self,"propinfo") and "enum" in self.propinfo:
            if hasattr(self,"enumKFDData"):
                return self.enumKFDData
            enumClass = self.propinfo["enum"]
            if enumClass != "":
                self.enumKFDData = pyKFDTable.kfdTB.get_kfddata(enumClass)
                return self.enumKFDData
        return None
        pass

    def get_name(self):

        if hasattr(self, "ViewObject"):
            return self.ViewObject.get_name()
        name = ""
        if hasattr(self,"propinfo"):
            cname = ""
            if "cname" in self.propinfo:
                cname = self.propinfo["cname"]
            if cname == "":
                name += self.propinfo["name"]
            else:
                name += cname
        if hasattr(self,"kfddata") and self.kfddata:
            cname = ""
            if "cname" in self.kfddata:
                cname = self.kfddata["cname"]
            if cname == "":
                name = name + ":" + self.kfddata["class"]
            elif pyKFDataType.DEBUG_SHOW:
                name = cname + ":" + name + ":" + self.kfddata["class"]
            else:
                name = cname
        return name
        pass

    def get_valueType(self):
        if hasattr(self,"ViewObject"):
            return self.ViewObject.get_valueType()
        else:
            return self.valueType;
        pass

    def set_val(self,val):
        if hasattr(self, "ViewObject"):
            self.ViewObject.set_val(val)
        else:
            self.val = val
        pass

    def get_val(self):
        if hasattr(self, "ViewObject"):
           return self.ViewObject.get_val()
        else:
            return self.val
        pass

    def get_val_str(self):
        enumKFDData = self.get_enum_data()
        if enumKFDData is not None:
            propertys = enumKFDData["propertys"]

            for prop in propertys:
                pname = prop["name"]
                pdefval = prop["default"]
                if str(self.get_val()) == pdefval:
                    return pname
            pass
        return self.get_val()
        pass

    def get_kfddata(self,notthenfind = False):
        if hasattr(self,"kfddata"):
            return self.kfddata
        if notthenfind:
            okfddata = None
            if hasattr(self,"classname") and self.classname:
                okfddata = pyKFDTable.kfdTB.get_kfddata(self.classname)
                pass
            if okfddata is None and hasattr(self,"classid") and self.classid != 0:
                okfddata = pyKFDTable.kfdTB.get_kfddata_clsid(self.classid)
                pass
            self.kfddata = okfddata
            return okfddata
        return None

    def has_prop_id(self,pid):
        if self.val is None:
            return False
        for pval in self.val:
            if pval.pid == pid:
                return True
        return False
        pass

    def get_can_add_props(self,propname = None):
        kfddata = None
        if hasattr(self, "kfddata"):
            kfddata = self.kfddata
        if kfddata is None:
            return []
        propernames = []
        propertys = kfddata["propertys"]

        for prop in propertys:
            if not self.has_prop_id(prop["id"]):
                if propname is None:
                    propernames.append(prop["name"])
                elif propname == prop["name"]:
                    propernames.append(prop)
                    break
                pass
            pass
        return propernames
        pass

    def auto_bind(self,CREATE = False,jsonobj = None):
        if not hasattr(self, "kfddata"):
            self.bind_kfddata(self.get_kfddata(True),CREATE,jsonobj)
            pass
        pass

    def bind_kfddata(self,kfddata,CREATE = False,jsonobj = None):

        self.kfddata = kfddata

        if kfddata is None:
            #寻找一个KFDDATA数据
            if CREATE == False or (jsonobj is None):
                return

            kfddata = pyKFDTable.find_json_kfddata(jsonobj)
            if kfddata is None:
                return
            self.kfddata = kfddata

        if self.valueType == pyKFDataType.OT_MIXOBJECT:
            if not hasattr(self,"classname"):
                self.classname = kfddata["class"]
                pass
            pass

        if "class" in kfddata:
            VO = Create_ViewObject(kfddata["class"],self)
            if VO is not None:
                self.ViewObject = VO

        #看继承关系
        if "extend" in kfddata:
            extend = kfddata["extend"]
            if not extend == "":
                extdata = pyKFDTable.kfdTB.get_kfddata(extend)
                if extdata is not None:
                    if not hasattr(self, "extendval"):
                        self.extendval = None
                    #有继承关系但是没有对象则创建一个
                    if CREATE and self.extendval is None:
                        valitem = pyKFDValue()
                        valitem.val = []
                        valitem.valueType = pyKFDataType.OT_OBJECT
                        self.extendval = valitem
                        pass

                    if self.extendval is not None:
                        if extdata is not None:
                            self.extendval.bind_kfddata(extdata,CREATE,jsonobj)
                            pass
                        pass
                    pass
                pass
            pass
        pass

        #看属性
        proparr = self.val
        if proparr is not None:
            has_pids = {}
            for propval in proparr:
                pid = propval.pid
                propinfo = find_prop_info(kfddata, pid)
                if propinfo and propval.valueType == KFDataType.GetTypeID(propinfo["type"]):
                    propval.bind_propinfo(propinfo,CREATE)
                has_pids[pid] = True
                pass
            pass

            if CREATE:
                propertys = kfddata["propertys"]
                for prop in propertys:
                    pid = prop["id"]
                    if pid not in has_pids:
                        propname = prop["name"]
                        usepropval = None
                        if jsonobj is not None and propname in jsonobj:
                            usepropval = jsonobj[propname]
                        self.add_propval(propname,CREATE,usepropval)
                        pass
                    pass
                pass
            pass
        pass

    def bind_propinfo(self,propinfo,CREATE = False,jsonobj=None):

        if propinfo is None:
            return

        self.propinfo = propinfo

        if self.valueType == pyKFDataType.OT_OBJECT:
            otype = propinfo["otype"]
            okfddata = pyKFDTable.kfdTB.get_kfddata(otype)
            if okfddata is not None:
                self.bind_kfddata(okfddata,CREATE,jsonobj)
            pass
        elif self.valueType == pyKFDataType.OT_MIXOBJECT:
            okfddata = self.get_kfddata(True)
            self.bind_kfddata(okfddata,CREATE,jsonobj)
            pass
        elif self.valueType == pyKFDataType.OT_ARRAY:
            if "otype" in propinfo:
                otype = propinfo["otype"]
                if not pyKFDataType.Is_BaseTypeStr(otype):
                    okfddata = pyKFDTable.kfdTB.get_kfddata(otype)
                    #数组绑定下
                    self.bind_valarr(okfddata,False,CREATE,jsonobj)
                else:
                    #普通数组也需要绑定
                    proparr = self.val
                    if proparr is not None:
                        i = 0
                        size = len(proparr)
                        while i < size:
                            propval = proparr[i]
                            if propval is not None:
                                propval.propinfo = propinfo
                                pass
                            i += 1
                            pass
                    #普通数组的创建过程
                    if CREATE and jsonobj is not None:
                        #普通通数组的话
                        i = 0
                        size = len(jsonobj)
                        while i < size:
                            ival = jsonobj[i]
                            self.add_arrval(otype,CREATE,ival,propinfo)
                            i += 1
                        pass
                    pass

            else:
                logging.warning("propinfo not found otype:%s",propinfo["name"])
        elif self.valueType == pyKFDataType.OT_MIXARRAY:
            self.bind_valarr(None,True,CREATE,jsonobj)
            pass
        elif CREATE and jsonobj is not None:
            #如果是基础数据且为创建过程中
            self.val = jsonobj
            pass
        pass

    def bind_valarr(self, kfddata , autofind = False,CREATE = False,jsonobj=None):

        if kfddata is None and autofind == False:
            return

        proparr = self.val
        if proparr is not None:
            i = 0
            size = len(proparr)
            while i < size:
                propval = proparr[i]
                if propval is not None:
                    if autofind:
                        kfddata = propval.get_kfddata(True)
                    if kfddata is not None:
                        propval.bind_kfddata(kfddata,CREATE)
                        pass
                i += 1
                pass
            ###########
            ##CREATE ARR ITEMVAL
            if CREATE and jsonobj is not None:
                i = 0
                size = len(jsonobj)

                while i < size:

                    data = jsonobj[i]
                    otype = None
                    kfdd = None

                    if autofind:
                        kfdd = pyKFDTable.find_json_kfddata(data)
                    else:
                        kfdd = kfddata

                    if kfdd is not None:
                        otype = kfdd["class"]

                    if otype is not None:
                        self.add_arrval(otype,CREATE,data)

                    i += 1
                    pass
                pass
            pass

        pass
    pass
