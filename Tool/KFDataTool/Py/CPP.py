from CPPByteArrayExport import *
from CPPTypeDefExport import *
from CPPAnyExport import  *
from CPPDirUtilExport import  *

class CPP(CodeFile):

    def __init__(self, srcreader):
        CodeFile.__init__(self,srcreader,"///KFD(")
        pass

    def parse_class_str(self,currstr,kfd):

        classname = ""
        extendname = ""
        # TPTEST_API Cls : public ExtendCls0,public ExtendCls1
        # Cls
        # 去掉public关键字
        currstr = currstr.replace("public ", "")
        # 判定是否有 :
        extendindex = currstr.find(':')
        str0 = ""
        str1 = ""

        if extendindex == -1:
            str0 = currstr
        else:
            str0 = currstr[0:extendindex]
            str1 = currstr[extendindex + 1:]
            pass

        ##查找类的名称
        tmparr = str0.split(" ")
        clsindex = len(tmparr) - 1
        while clsindex >= 0:
            clsstr = tmparr[clsindex]
            if clsstr != "":
                classname = clsstr
                break
            clsindex -= 1
            pass
        pass

        ##查找扩展类名
        if str1 != "":
            str1 = str1.split(",")[0]
            extendname = str1.replace(" ","")
            pass

        kfd["class"] = classname
        kfd["extend"] = extendname

        return classname,extendname
        pass

    def class_parse(self, codestr, kfd):

        classname = trystr(kfd,"class")
        extendname = ""
        nameindex = 0
        hasclassend = False

        if classname == '':
            clsstr = "class"
            clsindex = codestr.find(clsstr)

            if clsindex == -1:
                clsstr = "struct"
                clsindex = codestr.find(clsstr)
                if clsindex == -1:
                    clsstr = "namespace"
                    clsindex = codestr.find(clsstr)
                #"namespace"

            if clsindex == -1:
                return CodeFile.NEXT_CLASS_BEGIN
            else:
                nameindex = clsindex + len(clsstr)

        codelen = len(codestr)
        currstr = classname

        while(nameindex < codelen):

            currchar = codestr[nameindex]
            # maybe tab
            if ord(currchar) == 9:
                currchar = " "

            if currchar == '{':
                hasclassend = True
                classname,extendname = self.parse_class_str(currstr,kfd)
            else:
                if currchar == '\r' or currchar == '\n':
                    currchar = ""
                elif currchar == " ":
                    #保证间隔最多一个空格
                    lastindex = len(currstr) - 1
                    lastchar = " "
                    if lastindex > -1:
                        lastchar = currstr[lastindex]
                    if lastchar == " " or lastchar == ":":
                        currchar = ""

                currstr += currchar

            nameindex += 1

        if hasclassend:
            kfd["propertys"] = []
            logging.debug(">>class:%s extend:%s",classname,extendname)
            return CodeFile.NEXT_UNKNOW

        #记住临时的字符串
        kfd["class"] = currstr
        return CodeFile.NEXT_CLASS_BEGIN
        pass

    def find_arr_name(self,ptype):
        arrname = 'kfVector<'
        if ptype.find(arrname) == -1:
            arrname = 'vector<'
            if ptype.find(arrname) == -1:
                arrname = 'kfVector<'
                pass
            pass
        return arrname
        pass
    pass

    def var_parse(self, typestr, obj, isparam = False):

        # 去掉空格
        typestr = typestr.strip()
        # 去掉一些修饰符
        typestr = typestr.replace("const ", "")
        typestr = typestr.replace("static ", "")
        typestr = typestr.replace("std::","")

        #有赋值的
        if typestr.find('=') != -1:
            arrstrs = typestr.split("=")
            typestr = arrstrs[0]
            defstr = arrstrs[1]
            #先去掉两头的空格再去掉"
            defstr = defstr.strip().replace('"', "")
            obj["default"] = defstr

        #去掉空格
        typestr = typestr.strip()
        endindex = len(typestr) - 1
        #int32 *a
        starindex = typestr.rfind(" ")

        if starindex != -1:

            ptype = typestr[0:starindex]
            pname = typestr[starindex + 1:endindex + 1]

            if pname.find('*') != -1:
                pname = pname.replace('*','')
                ptype += '*'

            if isparam:
                #如果是解析参数且包含有&则是引用参数
                if ptype.find('&') != -1 or pname.find('&') != -1:
                    obj["id"] = 1
                pass

            ptype = ptype.replace('&','')
            pname = pname.replace('&','')

            obj["name"] = pname
            # 指定使用的类型
            USETYPE = trystr(obj, "USETYPE")
            arrtypename = self.find_arr_name(ptype)

            # 自动判定指定类型
            if USETYPE == '':

                hasStar = (ptype.find('*') != -1)
                hasArr = (ptype.find(arrtypename) != -1)

                if hasArr and hasStar:
                    USETYPE = "mixarr"
                elif hasArr:
                    USETYPE = "arr"
                elif hasStar:
                    USETYPE = "mixobject"
                elif not pyKFDataType.Is_BaseTypeStr(ptype):
                    USETYPE = "object"
                pass

            else:
                del obj["USETYPE"]

            if USETYPE != "":

                if USETYPE == "arr" or USETYPE == "mixarr":
                    ptype = ptype.replace(arrtypename, "")
                    ptype = ptype.replace(">", "")
                    pass

                ptype = ptype.replace('*', '')
                obj["otype"] = ptype
                obj["type"] = USETYPE
                pass
            else:
                obj["type"] = ptype
        else:
            obj["type"] = ""
            obj["name"] = ""
        pass
    pass

    def accept_str_end(self,codestr,accept_str):

        haspropend = False
        index = 0
        charcount = len(codestr)

        while (index < charcount):
            cchar = codestr[index]
            # maybe tab
            if ord(cchar) == 9:
                cchar = " "
            if cchar == ';' or cchar == '}':
                haspropend = True
                break
                pass
            elif cchar != '\r' and cchar != '\n':
                if cchar == " ":
                    if accept_str != "" and accept_str[len(accept_str) - 1] != " ":
                        accept_str += cchar
                else:
                    accept_str += cchar
            index += 1
        return accept_str,haspropend
        pass

    def property_parse(self, codestr, obj):

        typestr = trystr(obj,"type")
        typestr,haspropend = self.accept_str_end(codestr,typestr)

        if haspropend:
            self.var_parse(typestr, obj)
            #logging.debug(">>id:%d type:%s name:%s<<",trynum(obj,"id"), trystr(obj,"type"), trystr(obj,"name"))
            return CodeFile.NEXT_UNKNOW
        else:
            obj["type"] = typestr

        return CodeFile.NEXT_PROPERTY
        pass

    def method_parse(self,codestr,method):
        ##方法字符串
        methodstr = trystr(method, "name")
        methodstr,hasmethodend = self.accept_str_end(codestr,methodstr)

        if hasmethodend:

            if methodstr.startswith("static"):
                method["isstatic"] = True

            methodstr = methodstr.replace("virtual ","")
            methodstr = methodstr.replace("const ","")
            methodstr = methodstr.replace("static ", "")

            firstblank = methodstr.find(" ")
            returnstr = methodstr[0:firstblank]

            lpindex = methodstr.find("(")
            rpindex = methodstr.find(")")
            methodname = methodstr[firstblank + 1:lpindex]

            if methodname.startswith("*"):
                methodname = methodname[1:len(methodname)]
                returnstr += "*"
            elif methodname.startswith("&"):
                methodname = methodname[1:len(methodname)]
                returnstr += "&"

            paramsstr = methodstr[lpindex + 1:rpindex]

            logging.debug("returnstr:%s,methodname:%s,paramsstr:%s",returnstr,methodname,paramsstr)

            method["name"] = methodname

            if returnstr == "":
                returnstr = "void return"
            elif returnstr.find(" ") == -1:
                returnstr += " return"
            retparam = {}
            method["retparam"] = retparam
            self.var_parse(returnstr, retparam, True)

            params = method["params"]

            if paramsstr != '' and paramsstr != "void":
                varstrarr = paramsstr.split(",")
                varsize = len(varstrarr)
                i = 0
                while i < varsize:
                    varstr = varstrarr[i]
                    if varstr.find(" ") == -1:
                        varstr += (" param_%d" % (i,))
                    varobj = {}
                    self.var_parse(varstr, varobj, True)
                    params.append(varobj)
                    i += 1
                pass

            return CodeFile.NEXT_UNKNOW
        else:
            method["name"] = methodstr

        return CodeFile.NEXT_METHOD
        pass
    pass


_cpp_template = None
_cpp_template_dir = None
_cpp_data_type = ""

def CPP_Load_Template(dirpath ,data_type):

    global _cpp_template
    global _cpp_template_dir
    global _cpp_data_type

    _cpp_data_type = data_type

    if _cpp_template is not None:
        return

    _cpp_template_dir = dirpath
    _cpp_template = {}
    default_template = {}

    datatmplpath = _cpp_template_dir

    if _cpp_data_type != "":
        datatmplpath += "/" + _cpp_data_type

    default_template["h"] = LoadFile(datatmplpath + "/{{CLASS}}F.h")
    default_template["cpp"] = LoadFile(datatmplpath + "/{{CLASS}}F.cpp")

    _cpp_template["default"] = default_template

    #枚举生成定义
    typedefTMPL = {}
    typedefTMPL["h"] = LoadFile(_cpp_template_dir + "/{{CLASS}}TypeDef.h")
    typedefTMPL["cpp"] = LoadFile(_cpp_template_dir + "/{{CLASS}}TypeDef.cpp")
 
    _cpp_template["typedefTMPL"] = typedefTMPL

    anyTMPL = {}
    anyTMPL["cpp"] = LoadFile(_cpp_template_dir + "/{{CLASS}}Any.cpp")
    anyTMPL["h"] = LoadFile(_cpp_template_dir + "/{{CLASS}}Any.h")
    _cpp_template["anyTMPL"] = anyTMPL

    utilTMPL = {}
    utilTMPL["cpp"] = LoadFile(_cpp_template_dir + "/{{CLASS}}Util.cpp")
    utilTMPL["h"] = LoadFile(_cpp_template_dir + "/{{CLASS}}Util.h")
    _cpp_template["utilTMPL"] = utilTMPL

    CPP_Load_ByteArray_Template(dirpath)
    pass


def CPP_Template_Name(name,typedef):

    global _cpp_template
    global _cpp_template_dir

    if name == "":
        return tryobj(_cpp_template, "default")

        """
        if typedef == 0:
            return tryobj(_cpp_template, "default")
        else:
            return tryobj(_cpp_template,  "default2") 输出同一个模版了
        """

    template = tryobj(_cpp_template,name)

    if template is None:

        global _cpp_data_type

        datatmplpath = _cpp_template_dir
        if _cpp_data_type != "":
            datatmplpath += "/" + _cpp_data_type

        template = {}
        template["fh"] = LoadFile("%s/%s.h" % (datatmplpath,name))
        template["fcpp"] = LoadFile("%s/%s.cpp" % (datatmplpath,name))
        _cpp_template[name] = template
        pass

    return template
    pass

# _dir_group_dicts = None   #记录每一个文件夹所包含的group
def CPP_KFD_export(kfd,kfdtabel,dirpath,includes):
    kfd_datas = kfd.kfd_datas

    count = len(kfd_datas)
    i = 0
    cpptemplate = None
    cppkfddatas = [];
    cppincludes = [];
    export_api = trystr(kfd.export_info, "export_api")
    #使用kfd名字作为合并的c++的类名
    cppFileName = kfd.kfd_filename.replace(".kfd", "MERGED.cpp")
    while i < count:

        kfddata = kfd_datas[i]
        nof = trynum(kfddata,"nof")

        if nof != 1:
            # 检测扩展类是否存在
            extend = trystr(kfddata, "extend")
            if extend != "" and not kfdtabel.has_cls(extend):
                kfddata["extend"] = ""

            typedef = trynum(kfddata,"typedef")
            tmplname = trystr(kfddata, "template")
            template = CPP_Template_Name(tmplname,typedef)
            if template and not cpptemplate:
                cpptemplate = template

            CPP_ByteArray_export_Header(kfddata,kfdtabel,dirpath,includes,template,export_api)

            #cpp 信息汇总 用于CPP合并输出
            cppkfddatas.append(kfddata)
            if kfddata["class"] != "":
                cppincludes.append("%sF.h" % (kfddata["class"],))
            if typedef == 1:
                cppincludes.append("%sTypeDef.h" % (kfddata["class"],))
        i += 1
    #导出合并的cpp
    if cpptemplate:
        CPP_ByteArray_export_CPP(cppkfddatas, kfdtabel, dirpath, cppincludes, cpptemplate, cppFileName)

    #导出Any
    #取group的名称为类名
    anyClassName = kfd.kfd_filename.replace(".kfd", "")
    CPP_Export_Any(cppkfddatas, kfdtabel, dirpath, cppincludes, anyClassName,export_api)
    pass

def CPP_Export_Any(cppkfddatas, kfdtabel, dirpath, cppincludes, anyClassName,export_api):
    global _cpp_template
    tmpl = _cpp_template["anyTMPL"]
    CPP_Any_export(cppkfddatas, kfdtabel, dirpath, cppincludes, anyClassName, tmpl, export_api)

def CPP_Export_Dir_Util(kfdtable):
    global _cpp_template
    tmpl = _cpp_template["utilTMPL"]
    CPP_Dir_Util_Export(kfdtable, tmpl)


def CPP_Export_End(typedef,kfdtabel,dirpath,includes):
    #先导出定义的一些typedef
    global _cpp_template
    tmpl = _cpp_template["typedefTMPL"]
    CPP_Typedef_export(typedef,kfdtabel,dirpath,includes,tmpl)
    CPP_Export_Dir_Util(kfdtabel)
    pass

#定义CPP导输信息
CPP_Code_info = {       "suffix":".h"
                        ,"reader":CPP
                        ,"export":CPP_KFD_export
                        ,"load_template":CPP_Load_Template
                        ,"end":CPP_Export_End
                }
