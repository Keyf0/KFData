from KFDataDefine import *

#KFD文件定义

class KFD(object):

    def __init__(self,kfdtable,exportstruct = True,kfd_datas = None):
        self.exportstruct = exportstruct
        self.methodKFDData = None
        self.kfd_table = kfdtable
        self.isdirty = True
        self.kfd_filename = ""
        self.group = ""
        self.export_info = None
        if isinstance(kfd_datas,list):
            self.kfd_datas = kfd_datas
        else:
            self.kfd_datas = None
        pass

    def add_datas(self,kfd_datas):
        if self.kfd_datas is None:
            self.kfd_datas = kfd_datas
        else:
            self.kfd_datas.extend(kfd_datas)
        pass

    def add_one_data(self,kfd_data):
        if self.kfd_datas is None:
            self.kfd_datas = []
        self.kfd_datas.append(kfd_data)
        pass

    def _make_one_method(self,methodKFD, kfddata, method):

        methoddata = {}
        includes = []

        methoddata["includes"] = includes
        clsname = ("%s_%s" % (kfddata["class"] ,method["name"]))
        methoddata["class"] = clsname
        methoddata["extend"] = trystr(method,"extend")

        ##生成文件的头文件
        includes.append("%s_method.h" % (kfddata["class"],))

        propertys = []

        #不要加入了把这个功能放在他的子类里实现
        #加入对实例的引用对象__instance_ref
        #propinfo = {}

        #propinfo["id"] = 1
        #propinfo["name"] = "__instance__"
        #propinfo["type"] = "int32"

        #propertys.append(propinfo)

        params = tryobj(method,"params")

        if params != None:
            paramscount = len(params)
            i = 0
            while i < paramscount:
                param = params[i]
                paramotype = trystr(param, "otype")

                if self.kfd_table.has_otype(paramotype):
                    pid = i + 2
                    if i >= 126:
                        pid = i + 3
                        pass

                    propinfo = {}

                    propinfo["id"] = pid
                    propinfo["name"] = trystr(param, "name")
                    propinfo["type"] = trystr(param, "type")
                    propinfo["otype"] = paramotype

                    propertys.append(propinfo)
                    pass

                i += 1
                pass
            pass

        methoddata["propertys"] = propertys
        methodKFD.add_one_data(methoddata)

        pass

    def make_methodKFDs(self):
        i = 0
        kfdCount = 0
        if self.kfd_datas is not None:
            kfdCount = len(self.kfd_datas)
            pass
        while i < kfdCount:
            kfddata = self.kfd_datas[i]
            methods = tryobj(kfddata,"methods")
            if methods is not None:

                size = len(methods)
                if size > 0:
                    j = 0
                    methodKFD = KFD(self.kfd_table,True)
                    methodKFD.kfd_filename = self.kfd_filename
                    methodKFD.export_info = self.export_info
                    methodKFD.methodKFDData = kfddata

                    while j < size:
                        self._make_one_method(methodKFD,kfddata, methods[j])
                        j += 1
                        pass

                    self.kfd_table.add_KFD(methodKFD)

                    pass
                pass
            i += 1
            pass
        pass

    def exportMethodStruct(self,dirpath):

        pass


    def load(self,path):
        #加载数据文件
        jsondata = LoadConfigFromJson(path)

        if isinstance(jsondata,list) == False:
            if jsondata is not None:
                self.add_one_data(jsondata)
        elif jsondata is not None:
            self.add_datas(jsondata)
            pass

        self.kfd_table.add_KFD(self)
        logging.info("load kfd path:%s",path)
        pass

    def exportStruct(self,dirpath):
        if self.methodKFDData is not None:
            self.exportMethodStruct(dirpath)
            pass
        else:
            pass
        pass

    def exportKFD(self,dirpath):
        path = dirpath + "/" + self.kfd_filename
        SaveConfigFromObject(path,self.kfd_datas)
        pass

    pass


