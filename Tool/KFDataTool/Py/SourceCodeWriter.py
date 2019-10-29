
from  SCDefine import *

class SourceCodeWriter(object):
    def __init__(self,kfdTabel = None):
        self.kfd_tabel = kfdTabel
        pass

    def export_code_path(self,export_info):
        if export_info is None:
            return None
        else:
            export_path =trystr(export_info,"export_code_path")
            if export_path == "":
                return None
            return export_path
        pass

    #exts: 扩展名列表比如 [".cpp", ".h"]
    def clearDir(self, dirname, exts):
        for fname in os.listdir(dirname):
            if os.path.splitext(fname)[1] in exts:
                logging.debug("remove file %s", fname)
                os.remove(os.path.join(dirname,fname))

    #在导出之前先清空掉文件夹
    def clearExportDir(self, setting):

        exts = []
        if setting.code_type == "cpp":
            exts = [".cpp", ".h"]

        self.clearDir(setting.export_code_path, exts)

        for ico in setting.import_code_objs:
            export_info = tryobj(ico, "export_info")
            if export_info is not None:
                export_code_path = tryobj(export_info, "export_code_path")
                if export_code_path != "":
                    self.clearDir(export_code_path, exts)



    def export(self,setting):

        #先写类型列表
        if self.kfd_tabel is None:
            return

        self.clearExportDir(setting)

        obj = self.kfd_tabel.get_file_data()

        export_kfd_path = setting.export_kfd_path
        path = export_kfd_path + "/" + setting.export_kfd_table
        SaveConfigFromObject(path,obj)

        #根据类型导出所有的类型或解析类

        code_type = setting.code_type

        load_code_template(code_type,setting.template_path,setting.data_type)
        exportKFD = get_kfd_export(code_type)

        kfds = self.kfd_tabel.import_KFDs
        count = len(kfds)
        i = 0

        includes = []
        export_code_path = setting.export_code_path

        while i < count:
            kfd = kfds[i]
            # 导出路径的定义
            exportCodePath = self.export_code_path(kfd.export_info)
            if exportCodePath is None:
                exportCodePath = export_code_path
            #如果不需要导出结构体则需要输出一个KFD文件
            if not kfd.exportstruct:
                kfd.exportKFD(export_kfd_path)
            elif not kfd.isinclude:
                kfd.exportStruct(exportCodePath)
            #导出解析文件吧
            if exportKFD is not None and not kfd.isinclude:
                #导出数据
                exportKFD(kfd,self.kfd_tabel,exportCodePath,includes)
            i += 1

        #最后写入一些统计类信息
        exportEnd = get_export_end(code_type)

        if exportEnd is not None:

            kfdtabel = self.kfd_tabel
            for clsname in kfdtabel.typedef_map:
                typedef = kfdtabel.typedef_map[clsname]
                export_dirpath = ""
                #先从EXPORTINFO中查看是否有导出TYPECODE的路径
                export_info = tryobj(typedef,"export_info")
                if export_info is not None:
                    export_dirpath = trystr(export_info,"export_typecode_path")
                    #然后从定义中找是否有导出代码的路径
                    if export_dirpath == "":
                        export_dirpath = trystr(export_info, "export_code_path")
                #最后用默认路径导出代码
                if export_dirpath == "":
                    export_dirpath = export_code_path
                exportEnd(typedef,self.kfd_tabel,export_dirpath,includes)
        pass
    pass
