
from pyKFD.pyKFData import *

##重定义float32的显示
class pyFloat32():

    def __init__(self,valobject):
        self.valThis = valobject
        pass

    def get_name(self):
        if hasattr(self.valThis, "propinfo"):
            return self.valThis.propinfo["name"]
        return ""
        pass

    def get_valueType(self):
        return pyKFDataType.OT_FLOAT
        pass

    def set_val(self,val):
        prop = self.valThis.get_propval("rawValue")
        if prop != None:
            prop.val = round(val * 1024)
            #logging.info("==>%d",prop.val)
            pass
        pass

    def get_val(self):
        prop = self.valThis.get_propval("rawValue")
        if prop != None:
            val = round(prop.val / 1024.0000 * 1000) / 1000.0;
            return (val)
            pass
        return 0
        pass
    pass

Style_ViewObject["float32"] = pyFloat32