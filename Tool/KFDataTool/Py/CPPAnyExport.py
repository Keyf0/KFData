from CodeFile import *
from Cheetah.Template import Template

def CPP_Any_export(kfddatas, kfdtabel, dirpath, includes, clsname, tmpl,export_api):
    if len(kfddatas) == 0:
        return
    context = {"datas": kfddatas, "includes": includes, "NS": kfdtabel.kfd_ns, "CLASS":clsname}
    context["export_api"] = export_api
    headfilestr = tmpl["h"]

    #print(json.dumps(context))

    headfilestr = str(Template(headfilestr, searchList=context))

    headpath = dirpath + ("/Any%s.h" % (clsname,))
    SaveFile(headpath,headfilestr,True)

    cppfilestr = tmpl["cpp"]
    # 写CPP文件
    logging.debug("cppfile %s", clsname);
    cppfilestr = str(Template(cppfilestr, searchList=context))

    ##logging.debug("after>>>\n%s\n", cppfilestr);

    cpppath =  dirpath + ("/Any%s.cpp" % (clsname,))
    SaveFile(cpppath, cppfilestr, True)
    pass
