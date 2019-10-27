def _func(ls):
    for i in ls:
        print(i)

def hook_dropfiles(tkwindow_or_winfoid,func=_func):
    """
    # this func to deal drag icon & drop to load in windows

    *args:
        hwnd
    **kw:
        func = _func
        # default func just use path list print each one.
        # default each full_path_file_name type: bytes; in py3
        # default each full_path_file_name type: str  ; in py2
        #===================
        def _func(ls):
            for i in ls:
                print(i)
        #===================
    test evironment:
        py2, py3
        work on win7 32bit & 64bit.
    
    if you use tk, you can hook like this:
    =================================================
    > import windnd
    >
    > def my_func(ls):
          for idx,i in enumerate(ls):
              print(idx,i)
    >
    > import tk
    > tk = tk.Tk()
    > hwnd = tk.winfo_id()
    >
    > # you don't have to write "hwnd = tk.winfo_id()" in tkinter
    > # because you can put "tk" in this function like:
    > # "windnd.hook_dropfiles(tk,func = my_func)"
    > # the reason for this is to expand interface
    >
    > windnd.hook_dropfiles(hwnd,func = my_func)
    >
    > tk.mainloop()
    =================================================
    """
    
    # this place just for expand interface
    # because may anther window tools need use hwnd to hook
    hwnd = tkwindow_or_winfoid
    if hasattr(tkwindow_or_winfoid,"winfo_id"):
        hwnd = tkwindow_or_winfoid.winfo_id()

    import platform
    import ctypes

    from ctypes.wintypes import DWORD,HWND,UINT,WPARAM,LPARAM,LPCVOID
    prototype = ctypes.WINFUNCTYPE(DWORD,HWND,UINT,WPARAM,LPARAM)

    if platform.architecture()[0] == "32bit":
        GetWindowLong = ctypes.windll.user32.GetWindowLongW
        SetWindowLong = ctypes.windll.user32.SetWindowLongW

    elif platform.architecture()[0] == "64bit":
        GetWindowLong = ctypes.windll.user32.GetWindowLongPtrW
        SetWindowLong = ctypes.windll.user32.SetWindowLongPtrW

    CallWindowProcW = ctypes.windll.user32.CallWindowProcW

    #CallWindowProcW.argtypes = [ctypes.c_uint64,HWND,UINT,WPARAM,LPARAM]
    SetWindowLong.restype = LPCVOID
    GetWindowLong.restype = LPCVOID

    WM_DROPFILES = 0x233
    GWL_WNDPROC = -4

    def py_drop_func(hwnd,msg,wp,lp):
        global files, SetWindowLong
        if msg == WM_DROPFILES:
            pwint = WPARAM(wp)
            count = ctypes.windll.shell32.DragQueryFile(pwint,-1,None,None)
            szFile = ctypes.c_buffer(1024)
            files = []
            for i in range(count):
                ctypes.windll.shell32.DragQueryFile(pwint,i,szFile,ctypes.sizeof(szFile))
                dropname = szFile.value
                files.append(dropname)
                #print(dropname)
            func(files)
            ctypes.windll.shell32.DragFinish(pwint)
        oldfunc = globals()[old]

        return CallWindowProcW(LPCVOID(oldfunc),hwnd,msg,WPARAM(wp),LPARAM(lp))

    # for limit hook number, protect computer.
    limit_num = 200
    for i in range(limit_num):
        if i+1 == limit_num:
            raise "over hook limit number 200, for protect computer."
        if "old_wndproc_%d" % i not in globals():
            old, new = "old_wndproc_%d"%i, "new_wndproc_%d"%i
            break

    globals()[old] = None
    globals()[new] = prototype(py_drop_func)
    ctypes.windll.shell32.DragAcceptFiles(hwnd,True)

    globals()[old] = SetWindowLong(hwnd,GWL_WNDPROC,globals()[new])
