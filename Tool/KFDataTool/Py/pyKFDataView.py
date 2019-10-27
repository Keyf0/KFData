import os
import sys

from pyKFD.pyKFDataFormat import *
from pyKFD.pyKFDJsonFormat import *

from tkinter import *
from tkinter.simpledialog import askstring, askinteger, askfloat
from tkinter import ttk
from tkinter import filedialog
from KFDataDefine import *
from KFDTable import *
from pyKFDValueEditor import *


import logging
import windnd

class KFDataViewer(object):

    def __init__(self):

        self.titlestr = "数据编辑器"
        self.FileExt = (("All Files", "*.*"),("kfd json file", "*.json"),("kf block target", ("*.blk","*.say","*.meta")),("kfd data file", "*.data"))
        self.FileExtList = ".json|.blk|.say|.meta|.data"
        self.valueEditor = None

        self.initialdir = sys.path[0]

        self.bytes_data_file = True
        self.current_file_path = ""

        self.dirname =  self.initialdir + "/__initialdir___"
        self.settingdir = self.initialdir

        self.initconfig = LoadConfigFromJson(self.dirname)

        initialdir = None

        if self.initconfig is not None:
            if "initialdir" in self.initconfig:
                initialdir = self.initconfig["initialdir"]
                pass
            if "settingdir" in self.initconfig:
                self.settingdir = self.initconfig["settingdir"]
                pass
        else:
            self.initconfig = {}

        if initialdir is not None and initialdir != "":
            self.initialdir = initialdir
            pass
        pass

    def accept_file(self,filepath):
        typename = os.path.splitext(filepath)[-1]
        return self.FileExtList.find(typename) != -1
        pass

    def on_dragdrop_file(self,ls):
        for idx, i in enumerate(ls):
            filepath = str(i,encoding='gbk')
            if self.accept_file(filepath):
                logging.debug("load===>%s", filepath)
                self.read_byte_onclick(filepath)
                break
            pass
        pass

    def show(self):


        window = Tk()
        window.title(self.titlestr)
        #window.resizable(width=0, height=0)

        frm = Frame(window)
        frm.pack(fill='both', expand='yes')

        frm1 = Frame(frm)
        frm1.pack(fill='both', padx=10, pady=10)

        frm2 = Frame(frm)
        frm2.pack(fill='both', expand='yes',padx=10, pady=10)

        read_byt_btn = Button(frm1, text="读取", command=self.read_byte_onclick)
        read_byt_btn.pack(side='left')

        save_btn = Button(frm1, text="保存", command=self.save_onclick)
        save_btn.pack(side='left')

        saveas_btn = Button(frm1, text="另存...", command=self.saveas_onclick)
        saveas_btn.pack(side='left')

        self.add_btn = Button(frm1, text=" 增加 ", command=self.add_onclick)
        self.add_btn.pack(side='left')

        self.remove_btn = Button(frm1, text="关闭文件", command=self.remove_onclick,
                                    disabledforeground='red')
        self.remove_btn.pack(side='left')

        # 配置当前需要生成的配置

        config_btn = Button(frm1, text="配置", command=self.read_config_onclick)
        config_btn.pack(side='right', padx=10)

        config_btn = Button(frm1, text="生成项目文件", command=self.build_code_onclick)
        config_btn.pack(side='right')

        #columns = ['1']
        #self.treeview = ttk.Treeview(frm2, height=30, columns=columns)
        #self.treeview.column("1", width=500, anchor='w')
        #self.treeview.heading("1", text="属性值")
        #self.treeview.bind("<Double-1>", self.item_double_click)
        #self.treeview.bind("<ButtonRelease-1>", self.item_on_click)

        self.valueEditor = KFDValueEditor(frm2)
        self.treeview = self.valueEditor.treeview
        self.treeview.pack(side='left',fill='both',expand='yes')

        vsb = ttk.Scrollbar(frm2, orient="vertical", command=self.treeview.yview)
        vsb.pack(side='right',fill='y')
        self.treeview.configure(yscrollcommand=vsb.set)

        self.win = window
        # 尝试支持下DRAG DROP FILE 测试功能
        sysarg = str(sys.argv)

        if sysarg.find('-dragdropfile') != -1:
            windnd.hook_dropfiles(window,self.on_dragdrop_file)
            logging.debug("=====>open drag drop file")
        window.mainloop()
        pass


    def add_onclick(self):

        if self.valueEditor.kfdvalue is not None:
            messagebox.showinfo("提示", "当前已经存在一个数据文件")
            return
            pass

        classnames = []

        for clsdef in pyKFDTable.kfdTB.cls_def_list:
            classnames.append(clsdef["class"])

        classnames.sort()

        retobj = {}
        StringListDialog(self.treeview.master, classnames, retobj)

        if "return" in retobj:
            clsname = retobj["return"]
            logging.info("add %s", clsname)
            kfddata = pyKFDTable.kfdTB.get_kfddata(clsname)
            if kfddata is not None:

                kfdvalue = pyKFDValue()

                kfdvalue.valueType = pyKFDataType.OT_MIXOBJECT
                kfdvalue.val = []

                kfdvalue.bind_kfddata(kfddata,True)

                self.valueEditor.make_tree(kfdvalue,None)

            pass
        pass

    def remove_onclick(self):
        self.valueEditor.delete_tree()
        self.bytes_data_file = True
        self.current_file_path = None
        self.win.title(self.titlestr)
        pass

    def start_editor_kfdvalue(self,kfdvalue , filename):

        if kfdvalue is None:
            return False

        self.win.title("%s[%s]"%(self.titlestr,filename))

        if kfdvalue is not None:
            kfdvalue.auto_bind()
            pass

        self.valueEditor.make_tree(kfdvalue, filename)
        return True
        pass

    def build_code_onclick(self):
        if not self.current_file_path:
            return
        if not self.bytes_data_file:
            os.system("python KFDataTool.py %s" % (self.current_file_path,))
        pass

    def read_config_onclick(self):

        filename = filedialog.askopenfilename(initialdir=self.settingdir, title="Select file", filetypes=(("kfd json file", "*.json"),))
        kfdvalue = None
        try:
            if filename.find(".json") != -1:
                jsonobj = LoadConfigFromJson(filename)
                if "__cls__" not in jsonobj:
                    jsonobj["__cls__"] = "KFDataToolSetting"
                kfdvalue = pyKFDJsonFormat.ImportJson(jsonobj)
                self.bytes_data_file = False
        except FileNotFoundError:
            logging.error("FileNotFoundError")
            return

        if not self.start_editor_kfdvalue(kfdvalue, filename):
            return

        self.current_file_path = filename
        newdir = os.path.dirname(filename)

        if self.settingdir != newdir:
            self.settingdir = newdir
            self.initconfig["settingdir"] = newdir
            SaveConfigFromObject(self.dirname, self.initconfig)
            pass
        pass

    def read_byte_onclick(self, path_catch=None):

        kfdvalue = None

        if path_catch:
            filename = path_catch
        else:
            filename = filedialog.askopenfilename(initialdir=self.initialdir, title="Select file", filetypes=self.FileExt)
        try:

            if filename.find(".json") == -1:
                with open(filename, 'rb') as f:

                    kf_byte_arr = KFByteArray()
                    kf_byte_arr.buffer = f.read()
                    kfdvalue = pyKFDataFormat.read_value(kf_byte_arr, False)
                    logging.info("kf_byte_arr available_size:%d",kf_byte_arr.available_size())
                    self.bytes_data_file = True
            else:
                jsonobj = LoadConfigFromJson(filename)
                kfdvalue = pyKFDJsonFormat.ImportJson(jsonobj)
                self.bytes_data_file = False

        except FileNotFoundError:
            return

        if not self.start_editor_kfdvalue(kfdvalue, filename):
            return

        self.current_file_path = filename
        self.valueEditor.make_tree(kfdvalue, filename)

        newdir = os.path.dirname(filename)

        if self.initialdir != newdir:
            self.initialdir = newdir
            self.initconfig["initialdir"] = newdir
            SaveConfigFromObject(self.dirname, self.initconfig)
        pass

    def saveas_onclick(self):
        self.save_onclick(True)
        pass

    def save_onclick(self,isAS=False):

        kfvalue = self.valueEditor.kfdvalue

        if kfvalue is not None:

            filename = ""
            if self.current_file_path:
                filename = self.current_file_path

            dextension = ".data"

            if not self.bytes_data_file:
                dextension = ".json"

            if isAS or filename == "":
                filename = filedialog.asksaveasfilename(initialfile=self.current_file_path,defaultextension=dextension, filetypes=self.FileExt)

            if filename.find(".json") == -1:
                buff = KFByteArray()
                pyKFDataFormat.write_value(buff, kfvalue)
                if filename != '':
                    with open(filename, 'wb') as f:
                        f.write(buff.buffer)
            else:
                jsonobj = pyKFDJsonFormat.ExportJson(kfvalue)
                SaveConfigFromObject(filename,jsonobj)
                pass

    pass

def get_sys_arg_value(argname,args):
    i = 0
    argnamestr = "-%s=" % (argname,)
    while i < len(args):
        argstr = args[i]
        if argstr.startswith(argnamestr):
            return argstr.split("=")[1]
        i += 1
    return None
    pass

def pyKFDataView_main():

    init_logging(logging.DEBUG, True)

    #设置当前运行
    rootpath = (sys.path[0])
    os.chdir(rootpath)
    sys.path.append(rootpath)

    print("===>", rootpath)

    args = sys.argv

    KFDataType.GetTypeID(0)
    pyKFDataType.Type_to_ids = KFDataType.Type_to_ids

    path = get_sys_arg_value("kfdpath", args)

    if path is None:
        path = "../ExportKFD"
        pass
    pass

    kfdTable = KFDTable()

    #加载内嵌的KFD
    kfdTable.load_kfd_dir(abspath("./embed"), False, True)
    #加载导出FKD
    kfdTable.load_kfd_dir(abspath(path),False)

    kfdTable.make_typedef()


    pyKFDTable.kfdTB = kfdTable

    view = KFDataViewer()
    view.show()
