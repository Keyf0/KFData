from  pyKFDValueEditDialog import *
from  KFDataToolSetting import *

class pyKFDValueEditEnum:

    Edit_def = {}

    @staticmethod
    def enum_value_editor(editor, valueType, val, kfddata):
        clsname = trystr(kfddata,"class")
        enum_edit_func = tryobj(pyKFDValueEditEnum.Edit_def,clsname)
        if not enum_edit_func:
            uimaster = editor.treeview.master
            return pyKFDValueEditEnum.normal_enum_edit(uimaster,valueType,val,kfddata)
        else:
            return enum_edit_func(editor, valueType, val, kfddata)
        pass

    @staticmethod
    def normal_enum_edit(master, valueType, val, kfddata, ENTER_YOURSELF=True):

        res = None
        retobj = {}

        propnames = []
        props = {}
        propertys = kfddata["propertys"]

        for prop in propertys:

            pname = trystr(prop,"cname")
            if pname =="":
                pname = prop["name"]

            pdefval = prop["default"]
            pname = "%s(%s)" % (pname, pdefval)
            props[pname] = pdefval

            if str(val) == pdefval:
                retobj["selection"] = pname

            propnames.append(pname)
            pass
        if ENTER_YOURSELF:
            propnames.append("ENTER_YOURSELF")
        #弹出对话框选择
        StringListDialog(master, propnames, retobj)

        if "return" in retobj:
            propname = retobj["return"]
            pdefval = tryobj(props,propname)

            if propname == "ENTER_YOURSELF":
                res = propname;

            if pdefval is not None:
                if KFDataType.Is_Str(valueType):
                    res = pdefval
                    pass
                elif KFDataType.Is_Bool(valueType):
                    res = pdefval == "true"
                    pass
                elif KFDataType.Is_numFloat(valueType):
                    res = float(pdefval)
                    pass
                elif KFDataType.Is_numInt(valueType):
                    res = int(pdefval)
                    pass
                pass
        return res
        pass
    pass


def chg_path_to(abs,currentPath):
    if currentPath:
        if abs:
            if not os.path.isabs(currentPath):
                currentPath = os.path.abspath(currentPath)
        else:
            if os.path.isabs(currentPath):
                currentPath = os.path.relpath(currentPath)
        currentPath = currentPath.replace('\\','/')
        pass
    return currentPath
    pass

def edit_KFDataToolFileStr(editor, valueType, val, kfddata):

    uimaster = editor.treeview.master

    if val is None or val == "":
        val = pyKFDValueEditEnum.normal_enum_edit(uimaster, valueType, val, kfddata,False)
        pass

    if val is None:
        return None

    ##默认绝对路径 如果是相对路径则相对于当前编辑文件
    currentPath = None
    Absolute = True
    IsFile = False

    if not KFDataToolFileStr.IsEnumType(val):
        currentPath = val
        if val != "" and val.find(":") == -1:
            Absolute = False
        pass
    else:
        IsFile = KFDataToolFileStr.IsFile(val)
        Absolute = KFDataToolFileStr.IsAbs(val)

    ## 文件夹目录
    pathdir = None
    if editor.kfdvalue_path is not None:
        pathdir = os.path.dirname(editor.kfdvalue_path)
        pass

    EditorPath = os.path.abspath('.')
    if pathdir is None:
        pathdir = EditorPath

    #切换工作目录
    os.chdir(pathdir)
    #切换当前路径
    currentPath = chg_path_to(Absolute,currentPath)
    if currentPath is not None and currentPath != "":
        IsFile = os.path.isfile(currentPath)
        pass

    if IsFile:
        filename = filedialog.askopenfilename(initialdir=currentPath, title="Select file")
    else:
        filename = filedialog.askdirectory(initialdir=currentPath)

    if filename is not None and filename != "":
        currentPath = chg_path_to(Absolute,filename)
        pass
    else:
        currentPath = None

    #切换回来
    os.chdir(EditorPath)

    return currentPath
    pass


pyKFDValueEditEnum.Edit_def["KFDataToolFileStr"] = edit_KFDataToolFileStr







