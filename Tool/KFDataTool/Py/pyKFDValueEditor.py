
from pyKFD.pyKFDataFormat import *

from tkinter import *
from tkinter.simpledialog import askstring, askinteger, askfloat
from tkinter import ttk
from tkinter import filedialog
from tkinter import messagebox
from KFDataDefine import *
from KFDTable import *

from pyKFDValueEditDialog import *
from pyKFDValueEditEnum import *

import logging

'''

弹出添加属性窗口


    def show(self,editor):
        
        
'''

def AddPropertyWin(editor,master):

    value = editor.selectvalue
    iid = value.treeinfo["iid"]
    valueType = value.valueType
    propnames = []

    if valueType == pyKFDataType.OT_OBJECT or valueType == pyKFDataType.OT_MIXOBJECT:
        propnames = value.get_can_add_props()
        pass
    elif valueType == pyKFDataType.OT_ARRAY:
        childval = None
        if "otype" in value.propinfo:
            childval = value.add_arrval(value.propinfo["otype"],True, None, value.propinfo)
        else:
            logging.warning("arr not found otype:%s",value.propinfo["name"])
        if childval is not None:
            #self,kfdvalue,parent,index,isextendval = False
            editor.make_tree_item(childval,iid,"end")
            pass
        return
    elif valueType == pyKFDataType.OT_MIXARRAY:
        #寻找所有子类集合
        baseclass = value.propinfo["otype"]
        typedefinfo = tryobj(pyKFDValue.kfdTB.typedef_map,baseclass)
        if not typedefinfo is None:
            kfddatas = tryobj(typedefinfo,"kfddatas")
            for kfddata in kfddatas:
                propnames.append(kfddata["class"])
            pass
        pass
    pass

    proplen = len(propnames)
    if proplen == 0:
        return
    retobj = {}
    StringListDialog(master,propnames,retobj)

    if "return" in retobj:
        propname = retobj["return"]
        logging.info("add %s",propname)

        if valueType == pyKFDataType.OT_MIXARRAY:
            propval = value.add_arrval(propname,True)
        else:
            propval = value.add_propval(propname,True)

        if propval is not None:
            #self,kfdvalue,parent,index,isextendval = False
            editor.make_tree_item(propval,iid,"end")
            pass
        pass

    pass

'''



'''

class KFDValueEditor(object):

    def __init__(self,frm2):

        self.mx = 0
        self.my = 0
        self.kfdvalue = None
        self.kfdvalue_path = None
        self.treevalues = {}
        self.selectvalue = None

        columns = ['1']

        self.treeview = ttk.Treeview(frm2, height=30, columns=columns)
        self.treeview.column("1", width=500)
        self.treeview.heading("1", text="属性值")

        self.treeview.bind("<Double-1>", self.item_double_click)
        #self.treeview.bind("<ButtonRelease-1>", self.item_on_click)
        self.treeview.bind("<Button-3>",self.item_right_click)

        self.menu = None
        pass

    def delete_tree(self):
        if self.kfdvalue is None:
            return

        self.kfdvalue = None
        self.kfdvalue_path = None

        all_children = self.treeview.get_children()
        for item in all_children:
            self.treeview.delete(item)
        pass

    def make_tree(self,kfdvalue,path):

        self.delete_tree()

        self.kfdvalue_path = path
        self.kfdvalue = kfdvalue

        logging.info("value:%d path:%s",kfdvalue.valueType,path)
        self.make_tree_item(self.kfdvalue,"",0)
        pass

    def make_tree_item(self,kfdvalue,parent,index,isextendval = False):

        valueType = kfdvalue.get_valueType()

        if valueType <= pyKFDataType.OT_UINT64:

            valueTypeText = ("%s" % (trystr(KFDataType.ID_to_types,valueType),))

            if hasattr(kfdvalue,"pid"):
                valueTypeText = "(%d)%s:%s" % (kfdvalue.pid,kfdvalue.get_name(),valueTypeText)

            valueStr = kfdvalue.get_val_str()
            itemid = self.treeview.insert(parent, index, text=valueTypeText, values=(valueStr,))
            kfdvalue.treeinfo = {"iid": itemid,"parent":parent}
            self.treevalues[itemid] = kfdvalue
            pass
        else:
            valueTypeText = "%s" % (kfdvalue.get_name(),)

            if valueType == pyKFDataType.OT_OBJECT or valueType == pyKFDataType.OT_MIXOBJECT:
                pass
            elif valueType == pyKFDataType.OT_ARRAY or valueType == pyKFDataType.OT_MIXARRAY:
                valueTypeText = "%s:%s" % (valueTypeText,trystr(KFDataType.ID_to_types,valueType))
                pass

            if isextendval:
                valueTypeText = "extendval:%s" % (valueTypeText,)
            elif hasattr(kfdvalue, "pid"):
                valueTypeText = "(%d)%s" % (kfdvalue.pid,valueTypeText)

            itemid = self.treeview.insert(parent,index,text=valueTypeText,values=("",))
            kfdvalue.treeinfo = {"iid": itemid,"parent":parent}
            self.treevalues[itemid] = kfdvalue

            childi = 0

            #显示继承的方法
            if hasattr(kfdvalue,"extendval") and kfdvalue.extendval is not None:
                self.make_tree_item(kfdvalue.extendval,itemid,childi,True)
                childi += 1
                pass

            valarr = kfdvalue.val

            if valarr is not None:
                for valobj in valarr:
                    if valobj is not None:
                        self.make_tree_item(valobj,itemid,childi)
                    childi += 1
                    pass
                pass
            else:
                logging.info("valarr is null")
            pass
        pass

    def item_on_click(self, event):
        logging.info("item_on_click")
        pass

    def item_double_click(self, event):
        self.check_tree_selection()
        self.menu_commnd_edit(False)
        pass

    def check_tree_selection(self):
        selarr = self.treeview.selection()
        index = len(selarr) - 1
        if index >= 0:
            siid = selarr[index]
            self.selectvalue = tryobj(self.treevalues, siid)
        else:
            self.selectvalue = None

    def item_right_click(self,event):
        self.check_tree_selection()
        if self.selectvalue is not None:
            self.show_menu(event.x_root, event.y_root)
        pass

    def menu_commnd_addchild(self):
        value = self.selectvalue
        if value is None or value.get_valueType() <= pyKFDataType.OT_UINT64:
            messagebox.showinfo("提示", "基础属性不能添加子元素")
            return
        AddPropertyWin(self,self.treeview.master)
        pass

    def menu_commnd_edit(self,ERROR_SHOW = True):

        value = self.selectvalue

        if  value is None or value.get_valueType() > pyKFDataType.OT_UINT64:
            if ERROR_SHOW:
                messagebox.showinfo("提示", "只有基础属性才能编辑")
            return

        valueType = value.get_valueType()
        val = value.get_val()
        iid = value.treeinfo["iid"]
        res = None

        ##支持丰枚举的编辑
        enumKFDData = value.get_enum_data()

        if enumKFDData is not None:

            res = pyKFDValueEditEnum.enum_value_editor(self,valueType, val, enumKFDData)

            if res == "ENTER_YOURSELF":
                res = None
            elif res is None:
                res = "ENTER_YOURSELF"

        if res is None:
            if KFDataType.Is_Str(valueType):
                res = askstring("输入字符串", "", initialvalue=val)
                pass
            elif KFDataType.Is_Bool(valueType):
                valbool = 0
                if val :
                    valbool = 1
                valbool = askinteger("输入布尔值", "非0为真", minvalue=0, maxvalue=1, initialvalue=valbool)
                if valbool is not None:
                    res = False
                    if valbool == 1:
                        res = True
                pass
            elif KFDataType.Is_numFloat(valueType):
                res = askfloat("输入浮点数", "", initialvalue=val)
                pass
            elif KFDataType.Is_numInt(valueType):
                res = askinteger("输入整数", "", initialvalue=val)
                pass

        if res is not None and res != "ENTER_YOURSELF":
            val = res
            value.set_val(val)
            self.treeview.item(iid,values=(value.get_val_str(),))
        pass

    def menu_commnd_remove(self):
        value = self.selectvalue
        if value is None or value.treeinfo["parent"] == "":
            messagebox.showinfo("提示", "不能删除此对象")
            return

        parent = value.treeinfo["parent"]
        parentval = tryobj(self.treevalues,parent)

        if parentval.remove_val(value):
            self.selectvalue = None
            iid = value.treeinfo["iid"]
            self.treeview.delete(iid)
        pass

    def show_menu(self,x,y):
        self.mx = x;
        self.my = y;
        if self.menu is None:
            menu = Menu(self.treeview.master, tearoff=0)
            menu.add_command(label="添加", command=self.menu_commnd_addchild)
            menu.add_separator()
            menu.add_command(label="删除", command=self.menu_commnd_remove)
            menu.add_separator()
            menu.add_command(label="编辑", command=self.menu_commnd_edit)
            self.menu = menu
        self.menu.post(x,y)
        pass
    pass

###导入一些个性化的定义###
from pyKFDValueEditViewObject import *
