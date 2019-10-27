\#include "${CLASS}TypeDef.h"

#if not $DEF_ENUM
#if $info.TYPE == 1
\#include "Script/KFScriptType.h"
#end if
#end if
namespace ${NS}
{

     kfMap<kfstr,int32> ${CLASS}TypeDef::KeyTypes = { {"",0}};

     ${CLASS}* ${CLASS}TypeDef::Create(uint32 keytype)
     {
        ${CLASS}* obj = NULL;
        return obj;
     }

     ${CLASS}* ${CLASS}TypeDef::Read(KFByteArray& buffarr,${CLASS}* obj/* = nullptr*/)
     {
            uint8 valueType = buffarr.ReadUByte(); /// type is object
            if(valueType == OT_NULL || valueType == 0)
            { 
                return nullptr;
            }
            else if(valueType != OT_MIXOBJECT)
            {
                buffarr.Skip(-1);
                KFDataFormat::SkipValue(buffarr);
                return nullptr;
            }
             
            if(buffarr.ReadVarUInt() == 1)
            {
                kfstr clsname = "";
                buffarr.ReadString(clsname);
                ${CLASS}* anyObj = (${CLASS}*)KFDataFormat::ReadAny(buffarr, clsname, obj);
                if(anyObj != nullptr)
                {
                  anyObj->${KEY_PROP_NAME} = clsname;
                }
                return anyObj;
            }
            else
            {
                KFDataFormat::SkipObject(buffarr);
            }

            return NULL;
     }

    void ${CLASS}TypeDef::Write(KFByteArray& buffarr,${CLASS}* obj)
    {
    	if(obj == NULL) 
        {
            buffarr.WriteByte(OT_NULL);
            return;
        }

        buffarr.WriteByte(OT_MIXOBJECT);///objecttype
        buffarr.WriteVarUInt(1);
        kfstr clsname = obj->${KEY_PROP_NAME}.ToString();
        buffarr.WriteString(clsname);
        KFDataFormat::WriteAny(buffarr, clsname, obj);
    }

}
