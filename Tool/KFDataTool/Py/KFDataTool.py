
from SourceCodeReader import *
from KFDTable import *
from SourceCodeWriter import *
from KFDataToolSetting import *


KFDATA_TOOL_VER = "1.0.0 beta2"

def main(args):

    init_logging(logging.DEBUG, True)

    if len(args) < 2:
        logging.error("parameters not enough")
        return
    global KFDATA_TOOL_VER

    logging.info("\n=====>\n=====>KFDataTool(%s)\n=====>\n", KFDATA_TOOL_VER)

    #初始化数据类型
    KFDataType.GetTypeID(0)
    #读取配置文件
    configpath = abspath(args[1])
    setting = None

    if not os.path.exists(configpath):
        logging.error("%s does not exist",configpath)
        pass
    else:
        workdir = os.path.dirname(configpath)
        ##切换工作目录吧
        os.chdir(workdir)
        logging.info("==>os.chdir(%s)", workdir)

        setting = KFDataToolSetting(configpath)

        if  setting.initSucc:
            #读取原有的KFD文件
            kfdtabel = KFDTable()
            kfdtabel.load(setting)
            #从源代码中读取KFD文件
            reader = SourceCodeReader(kfdtabel)
            reader.load(setting)
            #导出方法KFD
            kfdtabel.make_methoddef()
            #建立依赖关系
            kfdtabel.make_typedef()
            #导出所有文件
            writer = SourceCodeWriter(kfdtabel)
            writer.export(setting)
        else:
            logging.error("setting initSucc=False msg=%s",setting.errorMsg)
            pass
        pass
    pass

    logging.info("\n=====>\n=====>KFDataTool(%s)\n=====>\n", KFDATA_TOOL_VER)

    pass

if __name__ == '__main__':
    main(sys.argv)