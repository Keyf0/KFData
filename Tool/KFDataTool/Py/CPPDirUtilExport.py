from CodeFile import *
from Cheetah.Template import Template

def DirnameToClsname(dirname):
    normalPath = os.path.normpath(dirname)
    lst = normalPath.split(os.sep)
    return "Any%s%sUtil" % (lst[-2], lst[-1])

def GetExoprtApi(groups):
    if len(groups) == 0:
        return ""
    export_info = tryobj(groups[0],"export_info")
    if export_info != None:
        return trystr(export_info, "export_api")
    return ""

#查找目录下是否有Any文件 没有就不在生成Util类
def AnyFileExist(dirpath, groups):
    for g in groups:
        groupname = trystr(g, "group")
        if groupname == "":
            continue
        path = dirpath + ("/Any%s.cpp" % (groupname,))
        if os.path.exists(path):
            return True
    return False


def CPP_Dir_Util_Export_Inner(kfdtabel, clsname, groups,dirpath, tmpl):
    context = { "groups": groups, "NS": kfdtabel.kfd_ns, "CLASS": clsname}
    context["export_api"] = GetExoprtApi(groups)

    if not AnyFileExist(dirpath, groups):
        return

    #写h文件
    headfilestr = tmpl["h"]
    headfilestr = str(Template(headfilestr, searchList=context))
    headpath = dirpath + ("/%s.h" % (clsname,))
    SaveFile(headpath, headfilestr, True)

    # 写CPP文件
    cppfilestr = tmpl["cpp"]
    cppfilestr = str(Template(cppfilestr, searchList=context))
    cpppath = dirpath + ("/%s.cpp" % (clsname,))
    SaveFile(cpppath, cppfilestr, True)

def CPP_Dir_Util_Export(kfdtabel, tmpl):
    clsname_2_group = {}
    for k, v in kfdtabel.dir_2_group.items():
        clsname = DirnameToClsname(k)
        clsname_2_group[clsname] = {"groups":v, "path":k}
        CPP_Dir_Util_Export_Inner(kfdtabel, clsname, v, k, tmpl)
    pass


