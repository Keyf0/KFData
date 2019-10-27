from SourceCodeWriter import *

'''
{
  "import_code_paths":[	
, {     "path":"需要扫描的目录"
        ,"group":"KFD可合并到此分组"#优先级高于导出目录
        ,"headpath":"引入头文件上加入此目录"
        ,"export_info":
        {
             "export_code_path":"指定代码导出目录"
         ,   "export_typecode_path":"类型管理导出目录"#与基类所在的分组有关
         }
  }
] ,
  "import_kfd_path":  "导出预先生成的KFD", 
  "export_code_path": "默认导出的代码目录",
  "export_kfd_path":  "导出的KFD目录",
  "export_kfd_table": "KFDTABLE的文件名",
  "template_path": "模版的目录",
  "namespace":"生成代码的命名空间",
  "code_type":"导出的代码语言",
  "data_type":"文件类型，二进制或其他",
  "info":{"TYPE":1}///[过期]个性化的模版文件数据 请用配置info_props配置
  "info_props":[{"key":"名称","value":"值","type":"类型"}] ///合并到info里去
  "typeDefTypes":[{"key":"目标类型","value":"个性类型","type":"类型"}] 
  ///可将一个自定义类型对应到内置类型上
}


以上路径都是相对于当前配置文件的路径
'''

'''
#新的文件格式
{
"import_groups":
[
    {
        "group":"KFD可合并到此分组"#优先级高于导出目录"
        ,"export_info":
        {
            "export_code_path":"指定代码导出目录"
         ,  "export_typecode_path":"类型管理导出目录"#与基类所在的分组有关
         ,  "export_api":"导出的字符串定义"
        }
        ,"import_code_paths":
        [
            {"path":"需要扫描的目录"
            ,"headpath":"引入头文件上加入此目录"
            }
        ]
    }
]

,
  "import_kfd_path":  "导出预先生成的KFD", 
  "export_code_path": "默认导出的代码目录",
  "export_kfd_path":  "导出的KFD目录",
  "export_kfd_table": "KFDTABLE的文件名",
  "template_path": "模版的目录",
  "namespace":"生成代码的命名空间",
  "code_type":"导出的代码语言",
  "data_type":"文件类型，二进制或其他",
  "info":{"TYPE":1}///[过期]个性化的模版文件数据 请用配置info_props配置
  "info_props":[{"key":"名称","value":"值","type":"类型"}] ///合并到info里去
  "typeDefTypes":[{"key":"目标类型","value":"个性类型","type":"类型"}] 
  ///可将一个自定义类型对应到内置类型上
}


以上路径都是相对于当前配置文件的路径
'''



class KFDataToolFileStr:
    Relative = "Relative"
    RelativeFile = "RelativeFile"
    Absolute = "Absolute"
    AbsoluteFile = "AbsoluteFile"

    @staticmethod
    def IsEnumType(val):
        return val == KFDataToolFileStr.Relative or val == KFDataToolFileStr.Absolute \
               or val == KFDataToolFileStr.RelativeFile or val == KFDataToolFileStr.AbsoluteFile
        pass

    @staticmethod
    def IsFile(val):
        return val == KFDataToolFileStr.RelativeFile or val == KFDataToolFileStr.AbsoluteFile

    @staticmethod
    def IsAbs(val):
        return val == KFDataToolFileStr.Absolute or val == KFDataToolFileStr.AbsoluteFile
    pass


class ToolSettingFormat(object):

    @staticmethod
    def ChangeToGroupFirst(configpath):

        settingobj = LoadConfigFromJson(configpath)

        import_code_paths = tryobj(settingobj, "import_code_paths")

        index = -1
        if import_code_paths is not None:
            index = len(import_code_paths) - 1

        group_maps = {}
        def_group = {}
        def_group["import_code_paths"] = []

        while index >= 0:
            codepathobj = import_code_paths[index]
            groupobj = def_group

            group = trystr(codepathobj,"group")

            if group != "":
                groupobj = tryobj(group_maps,group)
                if groupobj is None:
                    groupobj = {}
                    groupobj["group"] = group
                    groupobj["import_code_paths"] = []
                    group_maps[group] = groupobj
                pass

            # 生成目录路径
            importpathobj = {}

            importpathobj["path"] = trystr(codepathobj,"path")
            importpathobj["headpath"] = trystr(codepathobj,"headpath")
            export_info = tryobj(codepathobj,"export_info")

            if export_info is not None:
                groupobj["export_info"] = export_info
                pass

            groupobj["import_code_paths"].append(importpathobj)

            index -= 1
        pass

        del settingobj["import_code_paths"]
        import_groups = []
        import_groups.append(def_group)

        for key in group_maps:
            import_groups.append(group_maps[key])
            pass

        settingobj["import_groups"] = import_groups

        SaveConfigFromObject(configpath,settingobj)

        print("==>",configpath)

        pass

    @staticmethod
    def LoadJsonConfig(configpath):

        settingobj = LoadConfigFromJson(configpath)
        import_groups = tryobj(settingobj,"import_groups")
        groupcount = 0
        if import_groups is not None:
            groupcount = len(import_groups)
            pass

        import_code_paths = []
        index = 0
        while index < groupcount:
            groupobj = import_groups[index]
            group = trystr(groupobj,"group")
            export_info = tryobj(groupobj,"export_info")
            group_code_paths = tryobj(groupobj,"import_code_paths")
            codepathcount = 0
            if group_code_paths is not None:
                codepathcount = len(group_code_paths)
                pass
            index0 = 0
            while index0 < codepathcount:
                importpathobj = group_code_paths[index0]
                if group != "":
                    importpathobj["group"] = group
                if export_info is not None:
                    importpathobj["export_info"] = export_info
                    pass
                import_code_paths.append(importpathobj)
                index0 += 1
                pass
            index += 1
            pass
        del settingobj["import_groups"]
        settingobj["import_code_paths"] = import_code_paths
        return settingobj
        pass

    @staticmethod
    def ChangePathFrist(configpath):
        configobj = ToolSettingFormat.LoadJsonConfig(configpath)
        SaveConfigFromObject(configpath,configobj)
        pass

    pass


class KFDataToolSetting(object):

    def __init__(self,configpath):

        self.initSucc = False
        self.errorMsg = "Unknow"

        #setting = LoadConfigFromJson(configpath)
        setting = ToolSettingFormat.LoadJsonConfig(configpath)

        if setting is not None:
            self.import_code_objs = []
            import_code_paths = tryobj(setting,"import_code_paths")

            index = -1
            if import_code_paths is not None:
                index = len(import_code_paths) - 1

            while index >= 0:
                codepathobj = import_code_paths[index]
                #生成目录路径
                codepathobj["path"] = abspath(codepathobj["path"])
                export_code_path = ""
                #导出的文件夹处理方式
                if "export_info" in codepathobj:
                    export_info = codepathobj["export_info"]
                    export_code_path = trystr(export_info,"export_code_path")
                    if export_code_path != "":

                        if os.path.exists(export_code_path):
                            export_info["export_code_path"] = abspath(export_code_path)
                        else:
                            self.errorMsg = "export_code_path[%s] not exists" % (export_code_path,)
                            return
                        pass
                    export_typecode_path = trystr(export_info,"export_typecode_path")
                    if export_typecode_path != "":
                        export_info["export_typecode_path"] = export_typecode_path
                self.import_code_objs.append(codepathobj)
                logging.info("in=%s,out=%s",codepathobj["path"],export_code_path)
                index -= 1

            self.import_kfd_path = abspath(trystr(setting,"import_kfd_path"))
            self.export_code_path = abspath(trystr(setting,"export_code_path"))
            self.export_kfd_path = abspath(trystr(setting,"export_kfd_path"))

            logging.info("==>import_kfd_path=%s\n==>export_code_path=%s\n==>export_kfd_path=%s\n" %
                         ( self.import_kfd_path
                          ,self.export_code_path
                          ,self.export_kfd_path))

            self.template_path = abspath(trystr(setting,"template_path"))
            logging.info("==>template_path=%s",self.template_path)

            self.export_kfd_table = trystr(setting,"export_kfd_table")

            if self.export_kfd_table == "":
                self.export_kfd_table = "KFDTable.txt"

            self.namespace = trystr(setting,"namespace")
            self.code_type = trystr(setting,"code_type")
            self.data_type = trystr(setting,"data_type")

            #有些个性化的模版可以自定义一些传入的参数
            self.info = tryobj(setting,"info")
            if self.info is None:
                self.info = {}
            info_props = tryobj(setting,"info_props")
            if info_props is not None:
                for infoprop in info_props:
                    propkey = trystr(infoprop,"key")
                    if propkey != "":
                        typestr = trystr(infoprop,"type")
                        propval = infoprop["value"]

                        if typestr == "":
                            typestr = "kfstr"

                        typeid = KFDataType.GetTypeID(typestr)

                        if typeid <= KFDataType.OT_UINT32:
                            propval = int(propval)
                            pass
                        elif typeid == KFDataType.OT_FLOAT:
                            propval = float(propval)

                        self.info[propkey] = propval
                        pass
                    pass
                pass
            pass
            #有些可以定义一些默认参数转换的方式
            self.typeDefTypes = []
            typeDefTypes = tryobj(setting,"typeDefTypes")
            if typeDefTypes is not None:
                Base_Type_ids = KFDataType.Base_Type_ids
                Type_to_ids = KFDataType.Type_to_ids

                for typeDef in typeDefTypes:
                    typename = trystr(typeDef,"key")
                    customdef = trystr(typeDef,"value")
                    typeid = KFDataType.GetTypeID(typename)
                    if typeid != pyKFDataType.OT_UNKNOW and typeid <= pyKFDataType.OT_UINT64:
                        Base_Type_ids[customdef] = typeid
                        Type_to_ids[customdef] = typeid
                        self.typeDefTypes.append(typeDef)
                        pass
                    pass
                pass
            pass

            self.initSucc = True
        pass
    pass

if __name__ == '__main__':
    configpath = abspath(sys.argv[1])
    workdir = os.path.dirname(configpath)
    ##切换工作目录吧
    os.chdir(workdir)
    ToolSettingFormat.ChangeToGroupFirst(configpath)
    pass