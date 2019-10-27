from CPP import *

#定义源文件的后缀
#export func(kfd,kfdtabel,dirpath,includes)
#export CPP_Export_End(typedef,kfdtabel,dirpath,includes)
'''
                        {"suffix":".h"
                        ,"reader":CPP
                        ,"export":CPP_KFD_export
                        ,"load_template":CPP_Load_Template
                        ,"end":CPP_Export_End
                        }

'''

_codetype_def = {
    "cpp": CPP_Code_info
                 }


def get_sc_suffix(code_type):
    codeinfo = tryobj(_codetype_def,code_type)
    if codeinfo is not None:
        return trystr(codeinfo,"suffix")
    return ".h"
    pass

def get_code_reader(code_type,screader):
    codeinfo = tryobj(_codetype_def, code_type)
    if codeinfo is not None:
        cls = tryobj(codeinfo,"reader")
        return cls(screader)
    return CPP(screader)
    pass

def load_code_template(code_type,dirpath,data_type):
    dirpath = dirpath + "/" + code_type
    codeinfo = tryobj(_codetype_def, code_type)
    if codeinfo is not None:
        load_func = tryobj(codeinfo,"load_template")
        if load_func is not None:
            load_func(dirpath,data_type)
            return
    CPP_Load_Template(dirpath,data_type)


def get_kfd_export(code_type):
    codeinfo = tryobj(_codetype_def, code_type)
    if codeinfo is not None:
        return tryobj(codeinfo, "export")
    return CPP_KFD_export
    pass


def get_export_end(code_type):
    codeinfo = tryobj(_codetype_def, code_type)
    if codeinfo is not None:
        return tryobj(codeinfo, "end")
    return CPP_Export_End
    pass