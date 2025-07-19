const MAG_ELECTRIC:="Electric"; // elettrico
const MAG_EXPLOSION:="Explosion"; // esplosivo
const MAG_SPECTRE:="Spectre"; // abbassa mana, recupera hp
const MAG_SONIC:="Sonic"; // abbassa stamina e mana
const MAG_VENOM:="Posion"; // poison
const MAG_PARALYZE:="Paralyze"; // paralisi
const MAG_PERFORATION:="Perforation"; // distruggi armature
const MAG_VAMPIRE:="Vampire"; // curatore
const MAG_FIRE:="Fire"; // fuoco
const MAG_MENTAL:="Mental"; // mentale
const MAG_IMPACT:="Impact"; // magia da impatto
const MAG_GENERAL:="General"; // magia generale


const RES_ELECTRIC:="Resist_Electricity";
const RES_VENOM:="Resist_Poison";
const RES_FIRE:="Resist_Fire";
const RES_MENTAL:="Resist_Mental"; // mentale
const RES_IMPACT:="Resist_Impact"; // magia da impatto
const RES_GENERAL:="Resist_Magic"; // magia generale

var ALL_RESISTS:={RES_ELECTRIC,RES_VENOM,RES_FIRE,RES_MENTAL,RES_IMPACT,RES_GENERAL};

//include "../../pkg/opt/magicalitemssys/magicalsystem";

//Type is a MAG_ type of attack
function CustomSpellResist(byref who, type)
	var retvalue:=0;
	var resist_type;
	
	case (type)
		MAG_ELECTRIC:
			resist_type:=RES_ELECTRIC;
		MAG_EXPLOSION:
		MAG_FIRE:
			resist_type:=RES_FIRE;
		MAG_VENOM:
			resist_type:=RES_VENOM;
		MAG_SPECTRE:
		MAG_SONIC:
		MAG_MENTAL:
			resist_type:=RES_MENTAL;
		MAG_IMPACT:
			resist_type:=RES_IMPACT;
		MAG_PARALYZE:
		MAG_PERFORATION:
		MAG_VAMPIRE:
		MAG_GENERAL:
			resist_type:=RES_GENERAL;
	endcase
	
	retvalue:=cint(GetObjProperty(who,"#" + resist_type));
	return retvalue;
endfunction

// res_type is a RES_... resistance
function CustomResistof(byref who, resist_type)
	var retvalue:=0;
	retvalue:=cint(GetObjProperty(who,"#" + resist_type));
	return retvalue;
endfunction

function ResetCustomResist(byref who,item,isremoving:=0)
	//check if this item has a custom resist
	if (item)
		if (!HasaCustomResist(item))
			return;
		endif
	endif

	ForceResetAllCustomResists(who,item,isremoving);
	
	ExplainResistsbyItems(who,item);
endfunction

function ExplainItemResists(byref who,item)
	//var res;
	var itemres;
	var resname;
	var resword;
	foreach resist in ALL_RESISTS
		itemres:=Cint(GetObjProperty(item, resist));
		if (itemres!=0)
			resname:=resist;
			resname[7]:=" ";
			resword:=splitwords(resname);
			sendsysmessage(who,"...Resist " + itemres + "% against " + resword[2] + " attack");
		endif
	endforeach

endfunction

function ExplainResistsbyItems(byref who,item)
	var res;
	var itemres;
	var resname;
	var resword;
	foreach resist in ALL_RESISTS
		res:=Cint(GetObjProperty(who,"#" + resist));
		itemres:=Cint(GetObjProperty(item, resist));
		if (itemres!=0)
			resname:=resist;
			resname[7]:=" ";
			resword:=splitwords(resname);
			sendsysmessage(who,"Your Resist in " + resword[2] + " is now " + res + "%",0x03,68);
		endif
	endforeach

endfunction

function ForceResetAllCustomResists(byref who,equippingitem,isremoving:=0)
	var equipped:=ListEquippedItems(who);
	var allresists := array{};
	var count:=1;
	//TODO fix better?
	foreach resist in ALL_RESISTS
		allresists[count]:=0;
		count:=count+1;
	endforeach
	var tempres:=0;
	//var isresmodified:=0;
	count:=1;
	foreach resist in ALL_RESISTS
		if (equippingitem && !isremoving)
			tempres:=Cint(GetObjProperty(equippingitem,resist));
			if (tempres!=0)
				allresists[count]:=allresists[count]+tempres;
			endif
		endif		
		foreach item in equipped
			if (!(isremoving && item.serial==equippingitem.serial))
				tempres:=Cint(GetObjProperty(item,resist));
				if (tempres!=0)
					allresists[count]:=allresists[count]+tempres;
				endif
			endif
		endforeach	
		if (allresists[count]!=0)
			SetObjProperty(who,"#" + resist,allresists[count]);
		else
			EraseObjProperty(who,"#" + resist);
		endif
		count:=count+1;
	endforeach
	
endfunction

function HasaCustomResist(item)
	foreach resist in ALL_RESISTS
		if (GetObjProperty(item,resist))
			return 1;
		endif		
	endforeach
	return 0;
endfunction
/*function CustomProtectedGet(byref who)
	var reslevel:=0;

	reslevel := reslevel + Cint(GetObjProperty(who, Cstr("Res_" + RES_ELECTRIC))) +
	Cint(GetObjProperty(who, Cstr("Res_" + RES_EXPLOSION))) +
	Cint(GetObjProperty(who, Cstr("Res_" + RES_SPECTRE))) +
	Cint(GetObjProperty(who, Cstr("Res_" + RES_SONIC))) +
	Cint(GetObjProperty(who, Cstr("Res_" + RES_VENOM))) +
	Cint(GetObjProperty(who, Cstr("Res_" + RES_PARALYZE))) +
	Cint(GetObjProperty(who, Cstr("Res_" + RES_PERFORATION))) +
	Cint(GetObjProperty(who, Cstr("Res_" + RES_VAMPIRE))) +
	Cint(GetObjProperty(who, Cstr("Res_" + RES_FIRE))) +
	Cint(GetObjProperty(who, Cstr("Res_" + RES_MENTAL))) +
	Cint(GetObjProperty(who, Cstr("Res_" + RES_IMPACT))) +
	Cint(GetObjProperty(who, Cstr("Res_" + RES_GENERAL)));

	return reslevel;
endfunction*/


/*function setCustomProtection(byref who, item, msg := error)
	var level := Cint(GetObjProperty(item,"magical_level"));
	if (level<1)
		level:=1;
	elseif (level>5)
		level:=5;
	endif
	var magic_type := GetObjProperty(item,"magical_effect");

	var desc:="";

	case (magic_type)
		MAG_ELECTRIC:
		SetObjProperty(who, Cstr("Res_" + RES_ELECTRIC), level);
		desc := "Electric";
		break;
		MAG_EXPLOSION:
		SetObjProperty(who, Cstr("Res_" + RES_EXPLOSION), level);
		desc := "Explosion";
		break;
		MAG_SPECTRE:
		SetObjProperty(who, Cstr("Res_" + RES_SPECTRE), level);
		desc := "Spectre";
		break;
		MAG_SONIC:
		SetObjProperty(who, Cstr("Res_" + RES_SONIC), level);
		desc := "Sonic";
		break;
		MAG_VENOM:
		SetObjProperty(who, Cstr("Res_" + RES_VENOM), level);
		desc := "Venom";
		break;
		MAG_PARALYZE:
		SetObjProperty(who, Cstr("Res_" + RES_PARALYZE), level);
		desc := "Paralyze";
		break;
		MAG_PERFORATION:
		SetObjProperty(who, Cstr("Res_" + RES_PERFORATION), level);
		desc := "Perforation";
		break;
		MAG_VAMPIRE:
		SetObjProperty(who, Cstr("Res_" + RES_VAMPIRE), level);
		desc := "Vampire";
		break;
		MAG_FIRE:
		SetObjProperty(who, Cstr("Res_" + RES_FIRE), level);
		desc := "Fire";
		break;
		MAG_MENTAL:
		SetObjProperty(who, Cstr("Res_" + RES_MENTAL), level);
		desc := "Mental";
		break;
		MAG_IMPACT:
		SetObjProperty(who, Cstr("Res_" + RES_IMPACT), level);
		desc := "Impact";
		break;
		MAG_GENERAL:
		SetObjProperty(who, Cstr("Res_" + RES_GENERAL), level);
		desc := "General";
		break;
	endcase

	if (msg)
		SendSysMessage(who, "Your defense to magical " + magic_type + " attack has increased of " + (level * 4) + "%",0x03,96);
	endif
endfunction*/

/*function removeCustomProtection(byref who, item, msg := error)
	var level := Cint(GetObjProperty(item,"magical_level"));
	if (level<1)
		level:=1;
	elseif (level>5)
		level:=5;
	endif
	var magic_type := GetObjProperty(item,"magical_effect");

	var desc:="";

	case (magic_type)
		MAG_ELECTRIC:
		EraseObjProperty(who, Cstr("Res_" + RES_ELECTRIC));
		desc := "Electric";
		break;
		MAG_EXPLOSION:
		EraseObjProperty(who, Cstr("Res_" + RES_EXPLOSION));
		desc := "Explosion";
		break;
		MAG_SPECTRE:
		EraseObjProperty(who, Cstr("Res_" + RES_SPECTRE));
		desc := "Spectre";
		break;
		MAG_SONIC:
		EraseObjProperty(who, Cstr("Res_" + RES_SONIC));
		desc := "Sonic";
		break;
		MAG_VENOM:
		EraseObjProperty(who, Cstr("Res_" + RES_VENOM));
		desc := "Venom";
		break;
		MAG_PARALYZE:
		EraseObjProperty(who, Cstr("Res_" + RES_PARALYZE));
		desc := "Paralyze";
		break;
		MAG_PERFORATION:
		EraseObjProperty(who, Cstr("Res_" + RES_PERFORATION));
		desc := "Perforation";
		break;
		MAG_VAMPIRE:
		EraseObjProperty(who, Cstr("Res_" + RES_VAMPIRE));
		desc := "Vampire";
		break;
		MAG_FIRE:
		EraseObjProperty(who, Cstr("Res_" + RES_FIRE));
		desc := "Fire";
		break;
		MAG_MENTAL:
		EraseObjProperty(who, Cstr("Res_" + RES_MENTAL));
		desc := "Mental";
		break;
		MAG_IMPACT:
		EraseObjProperty(who, Cstr("Res_" + RES_IMPACT));
		desc := "Impact";
		break;
		MAG_GENERAL:
		EraseObjProperty(who, Cstr("Res_" + RES_GENERAL));
		desc := "General";
		break;
	endcase

	if (msg)
		SendSysMessage(who, "Your defense to magical " + magic_type + " attack has decreased of " + (level * 4) + "%",0x03,96);
	endif
endfunction*/
