from CodeFile import *
from Cheetah.Template import Template
import json

_cpp_types_infos = None

def CPP_Load_ByteArray_Template(dirpath):
    global _cpp_types_infos
    _cpp_types_infos = {}
    KFDataType.GetTypeID("")
    pass

def CPP_ByteArray_export_Header(kfddata,kfdtabel,dirpath,includes,cpp_template,export_api):

    context = {"data":kfddata,"NS":kfdtabel.kfd_ns,"typeids":KFDataType.Type_to_ids,"baseids":KFDataType.Base_Type_ids}
    context["info"] = kfdtabel.info
    context["export_api"] = export_api

    headfilestr = cpp_template["h"]
    clsname = kfddata["class"]

    #写头文件
    #把生成的文件包含进去
    includes.append("%sF.h" % (clsname,))
    headfilestr = str(Template(headfilestr, searchList=context))

    headpath = dirpath + ("/%sF.h" % (clsname,))
    SaveFile(headpath,headfilestr,True)


    pass
def CPP_ByteArray_export_CPP(kfddatas,kfdtabel,dirpath,includes,cpp_template, cppFileName):

    context = {"datas":kfddatas,"includes":includes,"NS":kfdtabel.kfd_ns,"typeids":KFDataType.Type_to_ids,"baseids":KFDataType.Base_Type_ids}
    context["info"] = kfdtabel.info

    cppfilestr = cpp_template["cpp"]
    # 写CPP文件
    logging.debug("cppfile %s", cppFileName);
    cppfilestr = str(Template(cppfilestr, searchList=context))

    ##logging.debug("after>>>\n%s\n", cppfilestr);

    cpppath = dirpath + "/" + cppFileName
    SaveFile(cpppath, cppfilestr, True)

    pass