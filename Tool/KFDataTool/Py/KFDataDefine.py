import logging
import os
import traceback
import sys
import json

from pyKFD.pyKFData import *

from logging.handlers import TimedRotatingFileHandler
from logging.handlers import RotatingFileHandler

def init_exception_log():
    def excepthook(type, value, trace):
        errstr = ''.join(traceback.format_exception(type,value,trace))
        logging.error(errstr)
        sys.__excepthook__(type, value, trace)
    sys.excepthook = excepthook
    pass

def init_logging(level,screen = True,keepsize = 100):
    root = logging.getLogger()
    if len(root.handlers) == 0:

        filename = './logs/log'
        #create logdir
        logpath = os.path.abspath(filename)
        dirpath = os.path.dirname(logpath)
        if not os.path.exists(dirpath):
            os.mkdir(dirpath)
            print("create logdir:%s" % dirpath)

        format = '%(filename)s[%(levelname)s:%(lineno)d] %(message)s   %(asctime)s'
        datefmt = '%Y-%m-%d %H:%M:%S'
        fmt = logging.Formatter(format,datefmt)

        #all write to file
        hdlr = TimedRotatingFileHandler(filename, "H", 1,keepsize)
        hdlr.setFormatter(fmt)
        root.addHandler(hdlr)

        #error write to
        errorLG = RotatingFileHandler('./logs/err','a',1024*1024*10,10)
        errorLG.setFormatter(fmt)
        errorLG.setLevel(logging.ERROR)

        root.addHandler(errorLG)

        if screen:
            console = logging.StreamHandler()
            console.setFormatter(fmt)
            root.addHandler(console)

        root.setLevel(level)
    init_exception_log()
    pass

def LoadFileLines(path):
    lines = None
    if os.path.isfile(path):
        try:
            file_object = open(path, 'r', encoding='utf-8')
            lines = file_object.readlines()
        except:
            file_object = None
        finally:
            if file_object is not None:
                file_object.close()
        pass
    return lines
    pass

def LoadFile(path):

    #logging.info("LoadFile(%s)",path)
    file_object = None
    filetxt = ""

    try:
        file_object = open(path,'r',encoding='utf-8')
    except:
        file_object = None

    if file_object is None:
        logging.info("[LoadConfig] not found path = %s error", path)
        return filetxt

    try:
        filetxt = file_object.read()
    except Exception as err:
        logging.error(err)
        filetxt = ""
    finally:
        file_object.close()

    #EF BB BF #python2.x
    if len(filetxt) >= 3:
        if ord(filetxt[0]) == 0xEF and ord(filetxt[1]) == 0xBB and ord(filetxt[2]) == 0xBF:
            filetxt = filetxt[3:]
            logging.info("========UTF-8-Bom ======")
        elif ord(filetxt[0]) == 0xFEFF:
            filetxt = filetxt[1:]
            logging.info("========UTF-8-Bom-sig ======")
        pass
    return filetxt
    pass

def LoadConfigFromJson(path):
    logging.info("LoadConfigFromJson(%s)",path)
    json_config = None
    filetxt = LoadFile(path)

    if filetxt == "":
        return None

    try:
        json_config = json.loads(filetxt)
    except:
        json_config = None
    finally:
        pass

    if json_config == None:
        logging.info("[LoadConfig] decode error path = %s", path)
        return None

    return json_config
    pass

def SaveFile(path,datastr,UTF = False):
    datafile = None

    try:
        if not os.path.isfile(path):
            dirname = os.path.dirname(path)
            if not os.path.exists(dirname):
                os.mkdir(dirname)
                logging.info("create cachedir:%s", dirname)
            pass
        pass
        datafile = open(path, 'w+' ,encoding='utf-8')
        datafile.write(datastr)
    except Exception as err:
        logging.error(err)
        pass
    finally:
        if datafile is not None:
            datafile.close()
    pass

def SaveConfigFromObject(path, jsonobj):
    datastr = json.dumps(jsonobj,ensure_ascii=False, indent = 4)
    SaveFile(path,datastr,True)
    pass

def abspath(path):
    if  not os.path.isabs(path):
        return os.path.abspath(path)
    return path

def trynum(obj,name):
    if obj is None or name not in obj:
        return 0
    return obj[name]

def trystr(obj,name):
    if obj is None or name not in obj:
        return ""
    return obj[name]

def tryobj(obj,name):
    if obj is None or name not in obj:
        return None
    return obj[name]

def template_replace(tstr,keyvalues):
    count = len(keyvalues)
    i = 0
    while i < count:
        keyvalue = keyvalues[i]
        tstr = tstr.replace(keyvalue[0],keyvalue[1])
        i += 1
    return tstr
    pass






