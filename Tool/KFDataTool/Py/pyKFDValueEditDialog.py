
from tkinter import *
from tkinter.simpledialog import askstring, askinteger, askfloat
from tkinter import ttk
from tkinter import filedialog
from tkinter import messagebox

from KFDataDefine import *

import logging


class Dialog(Toplevel):

    def __init__(self, parent, title = None):

        Toplevel.__init__(self, parent)
        self.transient(parent)

        if title:
            self.title(title)

        self.parent = parent
        self.result = None

        self.winframe = Frame(self)
        self.winframe.pack(fill="both",expand="yes")

        body = Frame(self.winframe)
        self.initial_focus = self.body(body)
        body.pack(side=TOP,fill="both",expand="yes")

        self.buttonbox()

        self.grab_set()

        if not self.initial_focus:
            self.initial_focus = self

        self.protocol("WM_DELETE_WINDOW", self.cancel)

        self.geometry("+%d+%d" % (parent.winfo_rootx()+50,
                                  parent.winfo_rooty()+50))

        self.initial_focus.focus_set()
        #self.minsize(400,400)
        #self.resizable(0,0)

        self.wait_window(self)

    #
    # construction hooks

    def body(self, master):
        # create dialog body.  return widget that should have
        # initial focus.  this method should be overridden

        pass

    def buttonbox(self):
        # add standard button box. override if you don't want the
        # standard buttons

        box = Frame(self.winframe)

        okBtn = Button(box, text="OK", width=10, command=self.ok, default=ACTIVE)
        okBtn.pack(side=LEFT, padx=5, pady=5)
        cancelBtn = Button(box, text="Cancel", width=10, command=self.cancel)
        cancelBtn.pack(side=LEFT, padx=5, pady=5)

        self.bind("<Return>", self.ok)
        self.bind("<Escape>", self.cancel)

        box.pack(side=TOP)

    #
    # standard button semantics

    def ok(self, event=None):

        if not self.validate():
            self.initial_focus.focus_set() # put focus back
            return

        self.withdraw()
        self.update_idletasks()

        self.apply()

        self.cancel()

    def cancel(self, event=None):

        # put focus back to the parent window
        self.parent.focus_set()
        self.destroy()

    #
    # command hooks

    def validate(self):

        return 1 # override

    def apply(self):

        pass # override

class StringListDialog(Dialog):
    def __init__(self,parent,propnames,retobj = None):
        self.propnames = propnames
        self.retobj = retobj
        Dialog.__init__(self,parent,"添加属性")
        pass
    def body(self, master):

        Lb1 = Listbox(master,width=50)
        i = 0
        selname = trystr(self.retobj,"selection")
        selitm = None
        while i < len(self.propnames):
            pname = self.propnames[i]
            itm = Lb1.insert(i,pname)
            if pname == selname:
                selitm = i
            i += 1

        Lb1.pack(side='left',fill="both",expand="yes")
        vsb = ttk.Scrollbar(master, orient="vertical", command=Lb1.yview)
        vsb.pack(side='left', fill='y')
        Lb1.configure(yscrollcommand=vsb.set)

        self.LB =  Lb1

        if selitm != None:
            self.LB.select_set(selitm)
        pass

    def apply(self):
        if self.retobj is not None:
            arr = self.LB.curselection()
            index = len(arr) - 1
            if index >= 0:
                self.retobj["return"] = self.propnames[arr[index]]
            pass
        pass
    pass


