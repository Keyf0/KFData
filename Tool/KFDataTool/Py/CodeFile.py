from  KFDataDefine import *

class CodeFile(object):

    NEXT_UNKNOW = 0
    NEXT_CLASS_BEGIN = 1
    NEXT_PROPERTY   = 2
    CURRENT_CLASS_BEGIN = 3
    CURRENT_PROPERTY = 4
    CURRENT_METHOD = 5
    NEXT_METHOD = 6
    NEXT_CLASS_END  = -1

    def __init__(self,srcreader,SIGN):
        self.srcreader = srcreader
        self.kfd_datas = []
        self.FIND_SIGN = SIGN
        self.SIGN_SIZE = len(self.FIND_SIGN)
        self.include = ""
        self.filename = ""
        self.group = None
        self.export_info = None
        pass

    def load(self,codepath,includePath):

        codelines = None

        self.kfd_datas = []
        self.include = includePath

        #logging.debug("LOOK:%s",codepath)

        codelines = LoadFileLines(codepath)
        if codelines is not None:
            self.read(codelines)
        else:
            logging.info("%s:is empty",codepath)
    #如果发现了KFDDATA
    def parse(self,linecount,codelines):

        i = 0
        tmpkfds = []
        kfd = None
        nextflag = CodeFile.NEXT_UNKNOW
        nextobj = None
        haskfd = False

        while i < linecount:
            codestr = codelines[i]
            if nextflag == CodeFile.NEXT_UNKNOW:
                sign_index = codestr.find(self.FIND_SIGN)
                if sign_index != -1:
                    nextflag, nextobj = self.key_parse(codestr, sign_index + self.SIGN_SIZE)
                    #解析到一个类型或者已经解析完成了
                    if nextflag == CodeFile.NEXT_CLASS_BEGIN or nextflag == CodeFile.CURRENT_CLASS_BEGIN:
                        if kfd is not None:
                            tmpkfds.append(kfd)
                        kfd = nextobj
                        #如果对象已经解析完成也需要等NEXT_CLASS_END
                        if nextflag == CodeFile.CURRENT_CLASS_BEGIN:
                            nextflag = CodeFile.NEXT_UNKNOW
                    #类型解析结束
                    elif nextflag == CodeFile.NEXT_CLASS_END:
                        if kfd is not None:
                            self.kfd_datas.append(kfd)
                            haskfd = True
                        nextflag = CodeFile.NEXT_UNKNOW
                        kfd = None
                        tmpindex = len(tmpkfds) - 1
                        if tmpindex >= 0:
                            kfd = tmpkfds[tmpindex]
                            tmpkfds.remove(kfd)
                    elif nextflag == CodeFile.CURRENT_PROPERTY:
                        nextflag = CodeFile.NEXT_UNKNOW
                        self.add_prop(kfd, nextobj)
                        nextobj = None
                        pass
                    elif nextflag == CodeFile.CURRENT_METHOD:
                        nextflag = CodeFile.NEXT_UNKNOW
                        self.add_method(kfd, nextobj)
                        nextobj = None
                        pass
                    pass
            elif nextflag == CodeFile.NEXT_CLASS_BEGIN:
                #logging.debug("VALUE=%s", codestr)
                nextflag = self.class_parse(codestr,kfd)
                pass
            elif nextflag == CodeFile.NEXT_PROPERTY:
                nextflag = self.property_parse(codestr,nextobj)
                if nextflag == CodeFile.NEXT_UNKNOW:
                    self.add_prop(kfd,nextobj)
                    nextobj = None
                pass
            elif nextflag == CodeFile.NEXT_METHOD:
                nextflag = self.method_parse(codestr,nextobj)
                if nextflag == CodeFile.NEXT_UNKNOW:
                    self.add_method(kfd,nextobj)
                    nextobj = None
                pass

            i += 1
        return haskfd
        pass

    def add_prop(self,kfd,nextobj):
        if kfd is not None:
            # 检测下属性ID != 0x7F or = 0
            propid = trynum(nextobj, "id")
            if propid != KFDataType.OBJ_PROP_ID_BEGIN and propid != KFDataType.OBJ_PROP_ID_END:

                OVERRIDE = trynum(nextobj,"OR")
                if OVERRIDE == 0:
                    kfd["propertys"].append(nextobj)
                else:
                    orprops = tryobj(kfd,"orprops")
                    if orprops is None:
                        orprops = []
                        kfd["orprops"] = orprops
                    orprops.append(nextobj)

                # 如果发现是一个自定义的类型
                if "otype" in nextobj:
                    otype = nextobj["otype"]
                    if KFDataType.GetTypeID(otype) == 0:
                        clsinc = ("%sF.h" % (otype,))
                        includes = kfd["includes"]
                        if clsinc not in includes:
                            includes.append(clsinc)
                        pass
                    pass
            else:
                logging.error("class =%s prop id != 0x7f or 0", kfd["class"])
        pass

    def add_method(self,kfd,method):
        if kfd is not None:
            methods = None
            if "methods" in kfd:
                methods = kfd["methods"]
            else:
                methods = []
                kfd["methods"] = methods
            methods.append(method)
            pass
        pass

    def class_parse(self,codestr,kfd):
        return CodeFile.NEXT_CLASS_BEGIN
        pass

    def property_parse(self,codestr,obj):
        return CodeFile.NEXT_PROPERTY
        pass

    def var_parse(self,codestr,obj,isparam = False):
        pass

    def method_parse(self,codestr,obj):
        return CodeFile.NEXT_METHOD
        pass

    def key_cls_parse(self,codestr,sign_index,strcount):
        kfddata = {"includes": [self.include]}
        pairstr = ""
        while sign_index < strcount:
            cstr = codestr[sign_index]
            if cstr == ',' or cstr == ')':
                # 解析下pairstr
                if pairstr != "" and pairstr.find("=") != -1:
                    #去掉开头和结尾的字符
                    pairstr = pairstr.strip()
                    pairs = pairstr.split('=')
                    pairkey = pairs[0].upper()

                    if pairkey == 'CLASS':
                        kfddata["class"] = pairs[1]
                    elif pairkey == 'EXTEND':
                        kfddata["extend"] = pairs[1]
                    elif pairkey == 'TMPL':
                        kfddata["template"] = pairs[1]
                    elif pairkey == "TYPEDEF":
                        kfddata["typedef"] = int(pairs[1])
                    elif pairkey == "TYPEID":
                        kfddata["typeid"] = int(pairs[1])
                    elif pairkey == "DES":
                        kfddata["des"] = pairs[1]
                    elif pairkey == "CNAME":
                        kfddata["cname"] = pairs[1]
                    elif pairkey == "CTYPEIDS":
                        kfddata["CTYPEIDS"] = pairs[1].split('|')
                    elif pairkey == "NOF":
                        kfddata["nof"] = int(pairs[1])
                    elif pairkey == "NEW":
                        kfddata["newfunc"] = pairs[1]
                    else:
                        unknowtags = None
                        if "unknowtags" not in kfddata:
                            unknowtags = []
                            kfddata["unknowtags"] = unknowtags
                        else:
                            unknowtags = kfddata["unknowtags"]
                        tag = {"tag": pairs[0], "val": pairs[1]}
                        unknowtags.append(tag)
                pairstr = ""
                if cstr == ')':
                    break
            elif cstr == '\r' or cstr == '\n' or cstr == '/':
                ## 过滤掉换行以及/最后一个与语言有关
                pass
            elif cstr != '':
                pairstr += cstr
            sign_index += 1
            pass

        if "class" in kfddata:
            kfddata["propertys"] = []
            return CodeFile.CURRENT_CLASS_BEGIN, kfddata
        else:
            return CodeFile.NEXT_CLASS_BEGIN, kfddata
        pass

    def key_prop_parse(self,codestr,sign_index,strcount):
        # 发现一个属性
        pobj = {}
        pairstr = ""

        while sign_index < strcount:
            cstr = codestr[sign_index]
            if cstr == ',' or cstr == ')':
                # 去掉pairstr左右的空格吧
                pairstr = pairstr.strip()
                # 解析下pairstr
                pairs = pairstr.split('=')
                pairkey = pairs[0].upper()

                if pairkey == 'P':
                    pid = int(pairs[1])
                    if pid == pyKFDataType.OBJ_PROP_ID_BEGIN:
                        errormsg = ("filename=%s ,pid != %d", self.filename, pid)
                        raise RuntimeError(errormsg)
                        pass
                    pobj["id"] = pid
                elif pairkey == "KEY":
                    pobj["KEY"] = pairs[1]
                elif pairkey == "USETYPE":
                    pobj["USETYPE"] = pairs[1]
                elif pairkey == "NAME":
                    pobj["name"] = pairs[1]
                elif pairkey == "TYPE":
                    pobj["type"] = pairs[1]
                elif pairkey == "OTYPE":
                    pobj["otype"] = pairs[1]
                elif pairkey == "DES":
                    pobj["des"] = pairs[1]
                elif pairkey == "CNAME":
                    pobj["cname"] = pairs[1]
                elif pairkey == "OR":
                    pobj["OR"] = int(pairs[1])
                elif pairkey == "CALL":
                    pobj["call"] = pairs[1]
                elif pairkey == "ITEMCALL":#貌似这个没有用处了？
                    pobj["itemcall"] = pairs[1]
                elif pairkey == "CLEAR":
                    pobj["clear"] = pairs[1]
                elif pairkey == "ARRCLEAR":
                    pobj["arrclear"] = pairs[1]
                elif pairkey == "ARRSIZE":
                    pobj["arrsize"] = pairs[1]
                elif pairkey == "ARRPUSH":
                    pobj["arrpush"] = pairs[1]
                elif pairkey == "ARRBACK":
                    pobj["arrback"] = pairs[1]
                elif pairkey == "READ":
                    pobj["read"] = pairs[1]
                elif pairkey == "WRITE":
                    pobj["write"] = pairs[1]
                elif pairkey == "ENUM":
                    pobj["enum"] = pairs[1]
                elif pairkey == "ENUMATTACH":
                    ##关联连动
                    ##ENUM_ATTACH=TYPE|1|XXX#XX|2|XXX
                    ENUM_ATTACH = []
                    argsstr = pairs[1]
                    args = [argsstr]
                    if argsstr.find("#") != -1:
                        args = argsstr.split("#")
                    for arg in args:
                        argparams = arg.split("|")
                        attachobj = {}
                        attachobj["pname"] = argparams[0]
                        attachobj["pval"] = argparams[1]
                        attachobj["enum"] = argparams[2]
                        ENUM_ATTACH.append(attachobj)
                        pass
                    pobj["enumattach"] = ENUM_ATTACH
                elif pairkey == "DEFAULT":
                    pobj["default"] = pairs[1]
                else:
                    unknowtags = None
                    if "unknowtags" not in pobj:
                        unknowtags = []
                        pobj["unknowtags"] = unknowtags
                    else:
                        unknowtags = pobj["unknowtags"]

                    tag = {"tag": pairs[0], "val": pairs[1]}
                    unknowtags.append(tag)
                    pass
                pass

                pairstr = ""
                if cstr == ')':
                    break

            elif cstr == '\r' or cstr == '\n' or cstr == '/':
                ## 过滤掉换行以及/最后一个与语言有关
                pass
            elif cstr != '':
                pairstr += cstr
            sign_index += 1
            pass

        # 如果KFD里定义了 name 则不用分析源代码了
        if "name" in pobj:
            return CodeFile.CURRENT_PROPERTY, pobj
        else:
            return CodeFile.NEXT_PROPERTY, pobj
        pass

    def key_method_parse(self, codestr, sign_index, strcount):

        method = {}

        params = []
        method["params"] = params
        pairstr = ""

        while sign_index < strcount:
            cstr = codestr[sign_index]
            if cstr == ',' or cstr == ')':
                # 去掉pairstr左右的空格吧
                pairstr = pairstr.strip()
                if pairstr != "":
                    # 解析下pairstr
                    pairs = pairstr.split('=')
                    pairkey = pairs[0].upper()

                    if pairkey == "NAME":
                        method["name"] = pairs[1]
                        pass
                    elif pairkey == "CNAME":
                        method["cname"] = pairs[1]
                        pass
                    elif pairkey == "DES":
                        method["des"] = pairs[1]
                        pass
                    elif pairkey == 'TMPL':
                        method["templ"] = pairs[1]
                    elif pairkey == 'EXTEND':
                        method["extend"] = pairs[1]
                    elif pairkey == "RETURN":

                        retparamstr = params[1]
                        if retparamstr.find(" ") == -1:
                            retparamstr += " return"

                        retparam = {}
                        method["retparam"] = retparam
                        self.var_parse(retparamstr,retparam,True)
                        pass
                    elif pairkey == "PARAMS":
                        paramsstrArr = None
                        paramsstr = pairs[1]
                        if paramsstr.find("|") != -1:
                            paramsstrArr = paramsstr.split("|")
                            paramsstr = paramsstrArr[0]
                            pass
                        varstrarr = paramsstr.split(";")
                        varsize = len(varstrarr)
                        i = 0
                        while i < varsize:
                            varstr = varstrarr[i]
                            if varstr.find(" ") == -1:
                                varstr += (" param_%d" % (i,))
                            varobj = {}
                            self.var_parse(varstr,varobj,True)
                            params.append(varobj)
                            i += 1
                        pass
                pairstr = ""
                if cstr == ')':
                    break
            elif cstr != '':
                pairstr += cstr
            sign_index += 1
            pass

        if "name" in method:
            return CodeFile.CURRENT_METHOD, method
        else:
            return CodeFile.NEXT_METHOD, method

        pass

    def key_parse(self,codestr,sign_index):
        strcount = len(codestr)
        #发现了一个类型
        if sign_index < strcount:
            nextchar = codestr[sign_index]
            if nextchar == 'C':
                return self.key_cls_parse(codestr,sign_index + 1,strcount)
            elif nextchar == '*':
                return CodeFile.NEXT_CLASS_END,None
            elif nextchar == 'M':
                return  self.key_method_parse(codestr,sign_index + 1,strcount)
            pass
        return self.key_prop_parse(codestr,sign_index,strcount)
        pass

    def read(self,codelines):

        if codelines is None:
            logging.debug("CODE IS EMPTY")
            return
        linecount = len(codelines)
        if linecount == 0:
            logging.debug("CODE IS EMPTY")
            return

        if self.parse(linecount,codelines):
            self.srcreader.addCodeFile(self)
            pass
        pass
    pass
