
/*
*
 *给所有的字符串分配一个ID吧
  * */

export class KFDNameStrings
{
    public  constructor()
    {
        this._ID2Strings.push("");
    }

    public GetNameID(namestr:string):number
    {
        if(namestr == "")
            return 0;
        let nameid = this._Strings2ID[namestr];
        if(nameid)
        {
            return nameid
        }
        return this.__NewNameID(namestr);
    }

    public GetNameStr(nameid:number):string
    {
        if(nameid <= 0)
            return "";
        return this._ID2Strings[nameid];
    }

    private __NewNameID(namestr:string):number
    {
        this._ID2Strings.push(namestr);
        ///ID=数组的长度-1
        let nameid:number = this._ID2Strings.length - 1;
        this._Strings2ID[namestr] = nameid;
        return nameid;
    }

    ///找字符串的索引
    private _Strings2ID:{[key: string]: number;} = {};

    ///所有字符串的集合
    private _ID2Strings:Array<string> = new Array<string>();
}

export class KFDName
{
    public constructor(namestr:string = "")
    {
        this.value = KFDName._Strs.GetNameID(namestr);
    }

    public setString(namestr:string)
    {
        this.value = KFDName._Strs.GetNameID(namestr);
    }

    public toString():string
    {
        return KFDName._Strs.GetNameStr(this.value);
    }

    public value:number = 0;
    public static _Strs:KFDNameStrings = new KFDNameStrings();
}