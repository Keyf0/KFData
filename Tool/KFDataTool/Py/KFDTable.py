from KFD import *
from pyKFD.pyKFDTable import *

class KFDTable(pyKFDTable):

    def __init__(self, minclsid = 1000):
        pyKFDTable.__init__(self)

        self.cls_def_map = {}
        self.cls_def_list = []
        self.class_2_kfd = {}

        self.typedef_map = {}
        self.dir_2_group = {}

        self.is_loaded = False
        self.min_clsid = minclsid

        if minclsid is not None:
            self.minclsid = minclsid

        self.import_KFDs = []
        self.import_KFDs_Group = {}

        self.kfd_table_file = ""
        self.kfd_ns = ""
        self.info = {}

        pass

    def get_kfddata_clsid(self,clsid):
        if clsid in self.cls_def_map:
            clsdef = self.cls_def_map[clsid]
            return tryobj(self.kfddata_maps,clsdef["class"])
        return None
        pass

    def find_typedef_extend(self,clsname):
        kfddata = tryobj(self.kfddata_maps,clsname)
        if kfddata is not None:
            typedef = trynum(kfddata,"typedef")
            if typedef != 0:
                return kfddata
            extend = trystr(kfddata,"extend")
            if extend == "":
                return None
            return self.find_typedef_extend(extend)
            pass
        return None
        pass

    def make_methoddef(self):
        kfdCount = len(self.import_KFDs)
        i = 0
        while( i < kfdCount):
            kfd = self.import_KFDs[i]
            kfd.make_methodKFDs()
            i += 1
            pass
        pass

    def make_typedef(self):

        self.cls_typedef_map = {}
        clscount = len(self.cls_def_list)
        i = 0
        while i < clscount:
            clsdef = self.cls_def_list[i]
            clsname = trystr(clsdef, "class")
            clskfd = tryobj(self.kfddata_maps, clsname)
            rootkfdData = None

            if clskfd is not None:
                rootkfdData = self.find_typedef_extend(clsname)
            typedef_kfddatas = None

            if rootkfdData is not None:

                typedefcls = trystr(rootkfdData,"class")
                typedefinfo = tryobj(self.typedef_map,typedefcls)

                if typedefinfo is None:
                    typedefinfo = {}
                    typedef_kfddatas = []
                    typedefinfo["class"] = typedefcls
                    typedefinfo["kfddatas"] = typedef_kfddatas
                    rootkfd = self.class_2_kfd[typedefcls]

                    if rootkfd.export_info is not None:
                        typedefinfo["export_info"] = rootkfd.export_info

                    self.typedef_map[typedefcls] = typedefinfo
                    pass
                else:
                    typedef_kfddatas = typedefinfo["kfddatas"]
                    pass
                typedef_kfddatas.append(clskfd)
            i += 1
        pass

    def get_file_data(self):
        return {"classlist":self.cls_def_list}

    def add_KFD(self, kfd, group = None):
        if group is not None and group != "":
            self.import_KFDs_Group[group] = kfd
        kfd_datas = kfd.kfd_datas
        self.add_KFDDatas(kfd_datas,kfd)
        self.import_KFDs.append(kfd)
        pass

    def get_KFD_Group(self,group):
        return tryobj(self.import_KFDs_Group,group)
        pass

    def add_KFDDatas(self,kfd_datas,kfd):
        count = len(kfd_datas)
        i = 0
        while i < count:
            kfddata = kfd_datas[i]
            self.class_2_kfd[kfddata["class"]] = kfd
            self.add_cls_def(kfddata)
            i += 1
        pass

    def add_cls_def(self,kfddata):

        clsname = trystr(kfddata,"class")

        if tryobj(self.kfddata_maps,clsname) is not None:
            logging.error("class(%s) duplicate definition",clsname)
            pass

        self.kfddata_maps[clsname] = kfddata

        kfddataClsid = trynum(kfddata,"clsid")
        clsdef = tryobj(self.cls_def_map, clsname)

        if clsdef is None:

            clsdef = {}
            clsid = kfddataClsid

            if clsid == 0:
                clsid = self.min_clsid
                self.min_clsid += 1

            clsdef["class"] = clsname
            clsdef["id"] = clsid
            kfddataClsid = clsid

            self.cls_def_map[clsid] = clsdef
            self.cls_def_map[clsname] = clsdef

            self.cls_def_list.append(clsdef)

            pass
        else:
            kfddataClsid = clsdef["id"]

        kfddata["clsid"] = kfddataClsid
        pass


    def load(self, setting):
        if self.is_loaded:
            return
        self.info = setting.info
        self.load_table(setting)
        self.load_kfds(setting)
        self.load_dir_group(setting)
        self.is_loaded = True
        pass

    def load_table(self, setting):

        self.kfd_ns = setting.namespace
        # 寻找类型字典的位置
        path = setting.export_kfd_path + "/" + setting.export_kfd_table
        self.kfd_table_file = path

        if self.is_loaded:
            logging.warning("cls_def_list is not None")
            return

        kftable = LoadConfigFromJson(path)
        clslist = tryobj(kftable, "classlist")

        if clslist is not None:
            count = len(clslist)
            i = 0
            while i < count:
                clsdef = clslist[i]
                clsid = trynum(clsdef, "id")
                clsname = trystr(clsdef, "class")
                self.cls_def_map[clsid] = clsdef
                self.cls_def_map[clsname] = clsdef
                if self.min_clsid <= clsid:
                    self.min_clsid = clsid + 1
                i = i + 1
                pass

            self.cls_def_list = clslist
            pass
        pass


    def load_kfds(self, setting):

        if self.is_loaded:
            return
        self.load_kfd_dir(setting.import_kfd_path, True)

        pass

    def load_kfd_dir(self, dirpath, exportstruct, JSONFILE = False):
        if os.path.exists(dirpath):
            files = os.listdir(dirpath)
            for file in files:
                filepath = dirpath + "/" + file
                if not os.path.isdir(filepath):
                    if file.endswith(".kfd") or (JSONFILE and file.endswith(".json")):
                        kfdpath = filepath
                        kfd = KFD(self, exportstruct)
                        kfd.load(kfdpath)
                    pass
                else:
                    self.load_kfd_dir(filepath, exportstruct)
        else:
            logging.error("dirpath[%s] FileNotFoundError",dirpath)
        pass
    pass

    def load_dir_group(self, setting):
        for code_obj in setting.import_code_objs:
            export_info = tryobj(code_obj, "export_info")
            if export_info == None:
                continue
            path = trystr(export_info, "export_code_path")
            if path == "":
                path = setting.export_code_path
            group_list = self.dir_2_group.setdefault(path, [])
            group_list.append(code_obj)
            pass
        pass



