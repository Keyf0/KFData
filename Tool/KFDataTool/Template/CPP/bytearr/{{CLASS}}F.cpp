#for $includestr in $includes 
\#include "${includestr}"
#end for

namespace ${NS}
{
#for $data in $datas
     ${data.class}* ${data.class}F::Read(KFByteArray& buffarr,${data.class}* obj)
     {
        if(obj == NULL)
        {
            #if $data.newfunc != None
            obj = ${data.newfunc}(buffarr);
            #else
            obj = new ${data.class}();
            #end if
        }

        int32 cpos = buffarr.GetPosition();
        uint32 id = buffarr.ReadVarUInt();
        ///Deal with problems if there is no inheritance before
        if(id != OBJ_PROP_ID_BEGIN)
        {
            buffarr.SetPosition(cpos);
            return obj;
        }

    #if $data.extend 
        ${data.extend}F::Read(buffarr,obj);
    #end if

        cpos = buffarr.GetPosition();
        id = buffarr.ReadVarUInt();

        while(id != OBJ_PROP_ID_END)
        {
            switch(id)
            {

#for $prop in $data.propertys
                case ${prop.id}:
        #if $prop.read != None
                {
                    uint8 valueType = buffarr.ReadUByte();
                    if(valueType == OT_BYTES)
                    {
                        int32 len = buffarr.ReadVarUInt();
                        int32 origin_position = buffarr.GetPosition();
                        obj->${prop.read}(buffarr, len);
                        buffarr.SetPosition(origin_position + len);
                    }
                    else
                    {
                        buffarr.Skip(-1);
                        KFDataFormat::SkipValue(buffarr); 
                    }
                }
        #else if $prop.type == "arr" 
                {
                    //buffarr.Skip(1); /// type is array or object
                    uint8 valueType = buffarr.ReadUByte();
                    if(valueType == OT_ARRAY)
                    {
                        uint32 arrsize = buffarr.ReadVarUInt();
                        buffarr.Skip(1); /// array object type

                        #if "clear" in $prop 
                        obj->${prop.clear}();
                        #else
                        
                        #if "arrclear" in $prop
                        obj->${prop.name}.${prop.arrclear}();
                        #else
                        obj->${prop.name}.clear();/// clear arr
                        #end if

                        #end if

                        #if $baseids.get($prop.otype) == None 
                        for(uint32 i = 0 ;i < arrsize; i ++)
                        {
                            #if "arrpush" in $prop
                            obj->${prop.name}.${prop.arrpush}(${prop.otype}());
                            #else
                            obj->${prop.name}.push_back(${prop.otype}());
                            #end if


                            #if "arrback" in $prop
                            auto& itm = obj->${prop.name}.${prop.arrback}();
                            #else
                            auto& itm = obj->${prop.name}.back();
                            #end if


                            ${prop.otype}F::Read(buffarr,&itm);
                            ///如果有CALL需要调用下元素的CALL
                            #if "call" in $prop 
                                obj->${prop.call}(itm);
                            #end if
                        }
                        #else
                        for(uint32 i = 0 ;i < arrsize; i ++)
                        {
                            ${prop.otype} itm;
                            ${prop.otype}Read(buffarr,itm);

                            #if "arrpush" in $prop
                            obj->${prop.name}.${prop.arrpush}(itm);
                            #else
                            obj->${prop.name}.push_back(itm);
                            #end if
                        }
                        #end if
                    }
                    else if(valueType == OT_OBJECT)
                    {
                       ///a virtual object
                       auto& arrval = obj->${prop.name};
                       
                       #if "arrsize" in $prop
                       auto arrsize = kf_2_uint32(arrval.${prop.arrsize}());
                       #else
                       auto arrsize = arrval.size();
                       #end if

                       auto arrindex = buffarr.ReadVarUInt(); /// OBJ_PROP_ID_BEGIN

                       while(arrindex != OBJ_PROP_ID_END)
                       {
                            if(arrindex != OBJ_PROP_ID_BEGIN)
                            {
                                arrindex = arrindex - (arrindex > OBJ_PROP_ID_BEGIN ? 2 : 1);

                                if(arrindex < arrsize)
                                {
                                    auto& itm = obj->${prop.name}[arrindex];
                                    #if $baseids.get($prop.otype) == None 
                                    ${prop.otype}F::Read(buffarr,&itm);
                                    #else
                                    ${prop.otype}Read(buffarr,itm);
                                    #end if
                                }
                                else
                                {
                                    #if $baseids.get($prop.otype) == None 

                                    #if "arrpush" in $prop
                                    obj->${prop.name}.${prop.arrpush}(${prop.otype}());
                                    #else
                                    obj->${prop.name}.push_back(${prop.otype}());
                                    #end if

                                    #if "arrback" in $prop
                                    auto& itm = obj->${prop.name}.${prop.arrback}();
                                    #else
                                    auto& itm = obj->${prop.name}.back();
                                    #end if

                                    ${prop.otype}F::Read(buffarr,&itm);
                                    #else
                                    ${prop.otype} itm;
                                    ${prop.otype}Read(buffarr,itm);
                                    

                                    #if "arrpush" in $prop
                                    obj->${prop.name}.${prop.arrpush}(itm);
                                    #else
                                    obj->${prop.name}.push_back(itm);
                                    #end if
                                    

                                    #end if
                                }
                            }
                            arrindex = buffarr.ReadVarUInt();
                       }
                    }else
                    {
                        buffarr.Skip(-1);
                        KFDataFormat::SkipValue(buffarr);
                    }
                }
    #else if $prop.type == "mixarr" 
                {
                    //buffarr.Skip(1); /// type is mixarray<${prop.otype}>
                    uint8 valueType = buffarr.ReadUByte();
                    if(valueType == OT_MIXARRAY)
                    {
                        uint32 arrsize = buffarr.ReadVarUInt();

                        #if "clear" in $prop 
                        obj->${prop.clear}();
                        #else

                        #if $prop.arrclear != None
                        obj->${prop.name}.${prop.arrclear}();
                        #else
                        obj->${prop.name}.clear();/// clear arr Memory Leak
                        #end if

                        #end if

                        #if $baseids.get($prop.otype) == None 
                        for(uint32 i = 0 ;i < arrsize; i ++)
                        {
                           ${prop.otype}* itm = ${prop.otype}F::Read${prop.otype}(buffarr);
                           if(itm)
                           {
                                #if "call" in $prop 
                                obj->${prop.call}(itm);
                                #end if

                                #if $prop.arrpush != None
                                obj->${prop.name}.${prop.arrpush}(${prop.otype}());
                                #else
                                obj->${prop.name}.push_back(${prop.otype}());
                                #end if
                           }
                           else
                           {
                                LOG_WARNING("${prop.otype} read failed");
                           }
                           
                        }
                        #else
                        #silent $sys.stderr.write("mixarr Only supports the definition of the Object type\n")
                        #end if
                    }
                    else if(valueType == OT_OBJECT)
                    {
                        ///a virtual object
                       auto& arrval = obj->${prop.name};
                       #if "arrsize" in $prop
                       auto arrsize = kf_2_uint32(arrval.${prop.arrsize}());
                       #else
                       auto arrsize = arrval.size();
                       #end if

                       auto arrindex = buffarr.ReadVarUInt(); /// OBJ_PROP_ID_BEGIN

                       while(arrindex != OBJ_PROP_ID_END)
                       {
                            if(arrindex != OBJ_PROP_ID_BEGIN)
                            {
                                arrindex = arrindex - (arrindex > OBJ_PROP_ID_BEGIN ? 2 : 1);

                                if(arrindex < arrsize)
                                {
                                    auto itm = obj->${prop.name}[arrindex];
                                    #if $baseids.get($prop.otype) == None 

                                    ${prop.otype}* newitm = ${prop.otype}F::Read${prop.otype}(buffarr,itm);
                                    if(newitm != itm)
                                    {
                                        /// clear old value
                                        #if "clear" in $prop
                                        obj->${prop.clear}(itm);
                                        #else
                                        kfDel(itm);
                                        #end if
                                        /// set new value index
                                        itm = newitm;
                                        obj->${prop.name}[arrindex] = itm;
                                    }

                                    #if "call" in $prop 
                                    /// call itm update...
                                    obj->${prop.call}(itm);
                                    #end if

                                    #else
                                    #silent $sys.stderr.write("mixarr Only supports the definition of the Object type\n")
                                    #end if
                                }
                                else
                                {
                                    #if $baseids.get($prop.otype) == None 
                                    ${prop.otype}* itm = ${prop.otype}F::Read${prop.otype}(buffarr);
                                    #if "call" in $prop 
                                    obj->${prop.call}(itm);
                                    #end if

                                    #if "arrpush" in $prop
                                    obj->${prop.name}.${prop.arrpush}(itm);
                                    #else
                                    obj->${prop.name}.push_back(itm);
                                    #end if

                                    #else
                                    #silent $sys.stderr.write("mixarr Only supports the definition of the Object type\n")
                                    #end if
                                }
                            }
                            arrindex = buffarr.ReadVarUInt();
                       }
                    }
                    else
                    {
                        buffarr.Skip(-1);
                        KFDataFormat::SkipValue(buffarr);
                    }
                }
    #else if $prop.type == "object" 
                
                if(buffarr.ReadUByte() == OT_OBJECT)
                {
                    ${prop.otype}F::Read(buffarr,&obj->${prop.name});
                }
                else
                {
                    buffarr.Skip(-1);
                    KFDataFormat::SkipValue(buffarr);
                }

    #else if $prop.type == "mixobject" 

                {
                    ///mix object self checking 
                    auto oldval = obj->${prop.name};
                    auto newval = ${prop.otype}F::Read${prop.otype}(buffarr,oldval);

                    if(oldval != newval)
                    {
                        #if "clear" in $prop
                        obj->${prop.clear}();
                        #else
                        kfDel(obj->${prop.name});
                        #end if
                        obj->${prop.name} = newval;
                    }
                }
    #else
               if(buffarr.ReadUByte() == ${baseids.get($prop.type)}) 
              {
                //buffarr.Skip(1);/// type is base value
                ${prop.type}Read(buffarr,obj->${prop.name});
              }
              else
              {
                 buffarr.Skip(-1);
                 KFDataFormat::SkipValue(buffarr);
              }
    #end if
                    break;
#end for
                case OBJ_PROP_ID_END:
                    break;
                case OBJ_PROP_ID_BEGIN:
                    ///Processing if there is a class inheritance before
                    ///Inheritance has disappeared
                    buffarr.Skip(-1);
                    KFDataFormat::SkipObject(buffarr);
                    break;
                default:
                    KFDataFormat::SkipValue(buffarr);
                    break;
            }

            id = buffarr.ReadVarUInt();
        }

        return obj;
     }

     void ${data.class}F::Write(KFByteArray& buffarr,${data.class}* obj)
     {
        if(obj == NULL) return;
        buffarr.WriteVarUInt(OBJ_PROP_ID_BEGIN);
 #if $data.extend 
        ${data.extend}F::Write(buffarr,obj);
 #end if


#for $prop in $data.propertys
        buffarr.WriteVarUInt(${prop.id});///property id
    #if $prop.write != None
        {
            buffarr.WriteByte(OT_BYTES);
            KFDataFormat::ClearUseBuff.Clear();
            obj->${prop.write}(KFDataFormat::ClearUseBuff);
            buffarr.WritekfBytes(KFDataFormat::ClearUseBuff);
        }
    #else if $prop.type == "arr" 
        {
            buffarr.WriteByte(${typeids.get($prop.type)});///write arr type
            
            auto& arritms = obj->${prop.name};
            
            #if "arrsize" in $prop
            uint32 arrsize = kf_2_uint32(arritms.${prop.arrsize}());
            #else
            uint32 arrsize = kf_2_uint32(arritms.size());
            #end if

            buffarr.WriteVarUInt(arrsize);

            #if $baseids.get($prop.otype) == None 
            buffarr.WriteByte(OT_OBJECT);///write object type
            for(uint32 i = 0 ;i < arrsize; i ++)
            {
                ${prop.otype}F::Write(buffarr,&arritms[i]);
            }
            #else
            buffarr.WriteByte(${baseids.get($prop.otype)});///write base value type
            for(uint32 i = 0 ;i < arrsize; i ++)
            {
                ${prop.otype}Write(buffarr,arritms[i]);
            }
            #end if
        }
    #else if $prop.type == "mixarr" 
        {
            buffarr.WriteByte(${typeids.get($prop.type)});///write mixarr type
            auto& arritms = obj->${prop.name};
            
            #if "arrsize" in $prop
            uint32 arrsize = kf_2_uint32(arritms.${prop.arrsize}());
            #else
            uint32 arrsize = kf_2_uint32(arritms.size());
            #end if


            buffarr.WriteVarUInt(arrsize);
            for(uint32 i = 0 ;i < arrsize; i ++)
            {
                ${prop.otype}F::Write${prop.otype}(buffarr,arritms[i]);
            }
        }
    #else if $prop.type == "object" 
        buffarr.WriteByte(OT_OBJECT);///write object type
        ${prop.otype}F::Write(buffarr,&obj->${prop.name});
    #else if $prop.type == "mixobject" 
        ${prop.otype}F::Write${prop.otype}(buffarr,obj->${prop.name});
    #else
        ${prop.type}WriteVal(buffarr,obj->${prop.name});
    #end if

#end for

        buffarr.WriteVarUInt(OBJ_PROP_ID_END);
     }

#if $data.typedef == 1 
        ${data.class}* ${data.class}F::Read${data.class}(KFByteArray& buffarr,${data.class}* obj /*= nullptr*/)
        {
            return ${data.class}TypeDef::Read(buffarr,obj);
        }

        void ${data.class}F::Write${data.class}(KFByteArray& buffarr,${data.class}* obj)
        {
            ${data.class}TypeDef::Write(buffarr,obj);
        }

#else

        ${data.class}* ${data.class}F::Read${data.class}(KFByteArray& buffarr,${data.class}* obj /*= nullptr*/)
        {
            uint8 valueType = buffarr.ReadUByte(); /// type is object
            if(valueType == OT_NULL || valueType == 0)
            {
                return nullptr;
            }
            else if(valueType == OT_OBJECT)
            {
                return ${data.class}F::Read(buffarr,obj);
            }
            else if(valueType == OT_MIXOBJECT)
            {
                uint32 clsid = buffarr.ReadVarUInt();///clsid
                if(clsid == 1)
                {
                    kfstr clsname = "";
                    buffarr.ReadString(clsname);
                    kfAny anyObj = KFDataFormat::ReadAny(buffarr, clsname, obj);
                    return (${data.class}*)anyObj;
                }
                else
                {
                    return ${data.class}F::Read(buffarr,obj);
                }
                
            }
            else
            {
                buffarr.Skip(-1);
                KFDataFormat::SkipValue(buffarr);
            }

            return nullptr;
        }

        void ${data.class}F::Write${data.class}(KFByteArray& buffarr,${data.class}* obj)
        {
           if(obj == NULL)
            {
                buffarr.WriteByte(OT_NULL);
                return;
            }

            buffarr.WriteByte(OT_MIXOBJECT);///objecttype
            buffarr.WriteVarUInt(${data.clsid});
            ${data.class}F::Write(buffarr,obj);
        }
#end if
#end for
}
