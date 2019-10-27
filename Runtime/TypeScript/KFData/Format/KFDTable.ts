import {KFDataType} from "./KFD";

export class KFDTable
{
    public static kfdTB:KFDTable = new KFDTable();

    public static find_prop_info(kfddata
                                 ,pid:number):any
    {
        if(kfddata == null)return null;
        let __ids__ = kfddata["__ids__"];
        if(__ids__ == null)
        {
            __ids__ = {};
            kfddata["__ids__"] = __ids__;
            let propertys = kfddata["propertys"];
            let proplen = propertys.length;
            let i = 0;
            while (i < proplen)
            {
                let prop = propertys[i];
                __ids__[prop["id"]] = prop;
                i += 1;
            }
        }
        return __ids__[pid];
    }


    public static find_extend_kfddata(kfddata):any
    {
        if(kfddata == null)return null;
        let __extend__ = kfddata["__extend__"];
        if( __extend__ && kfddata["extend"])
        {
            __extend__ = KFDTable.kfdTB.get_kfddata(kfddata["extend"]);
            if(__extend__)
                kfddata["__extend__"] = __extend__;
        }
        return __extend__;
    }


    private kfddata_maps:{[key:string]:any} = {};

    public get_kfddata(clsname:string):any
    {
        return this.kfddata_maps[clsname];
    }

    public has_cls(clsname:string):boolean
    {
        return this.kfddata_maps[clsname] != null;
    }

    public add_kfd(kfd):void
    {
        if(kfd instanceof Array)
        {
            for(let kfddata of kfd)
            {
                let clsname = kfddata["class"]
                this.kfddata_maps[clsname] = kfddata;
            }

        }else
        {
            let clsname = kfd["class"]
            if(clsname)
                this.kfddata_maps[clsname] = kfd;
        }
    }

    public has_otype(otype):boolean
    {
        if(otype == "")return true;
        if(KFDataType.Is_BaseTypeStr(otype))
            return true;
        return this.has_cls(otype);
    }
}