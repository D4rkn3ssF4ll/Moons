include "include/client";

function SetManaRateBaseAr(byref who, itemar:= error)

	if (who.isA(POLCLASS_NPC))//(!who.acct)
		//who.mana_regen_rate:=who.intelligence*2;
		SetManaRegenRate(who, cint(GetIntelligence(who)*24));
		return;
	endif

	var ar:=cint(GetObjProperty(who,"azar"));//who.ar-who.ar_mod;
	if (itemar)
		ar:=ar+itemar;
	endif

	var int:=GetIntelligence(who)-GetIntelligenceMod(who);//who.intelligence-who.intelligence_mod;
	var med:=GetSkillFromString(who,ATTRIBUTEID_MEDITATION);
	var regenMI:= cint(cdbl(int*1.8)+cdbl(med*1.8));
	var regenAR:= 360;

	if (ar>15)
		regenAR:=regenAR - ar*9;
		if (regenAR<0)
			regenAR:=0;
		endif
	endif
	var regen:=regenMI;

	if (regenAR<regen)
		regen:=regenAR;
	endif

	//who.mana_regen_rate:=140+regen;
	SetManaRegenRate(who, (140+regen)*12);
endfunction

function SetStaminaRate(byref who)
	if (who.isA(POLCLASS_NPC))//!who.acct
		//who.stamina_regen_rate:=who.dexterity*2;
		SetStaminaRegenRate(who, cint(GetDexterity(who)*24) );
		return;
	endif
	var hungry:=Cint(GetObjProperty(who, "hungrylevel"));
	if (!hungry)
		hungry :=600;
		SetObjProperty(who, "hungrylevel", hungry);
	endif
	if (hungry<1)
		hungry:=1;
	endif
	if (who.cmdlevel && who.cmdlevel!="test")
		hungry:=660;
	endif
	var id:=hungry/3;
	if (id<1)
		id:=1;
	endif
	//who.stamina_regen_rate:=id;
	SetStaminaRegenRate(who, id*12);
endfunction

function SetHpRate(byref who)
	if (who.isA(POLCLASS_NPC))//(!who.acct)
		//who.hp_regen_rate:=who.strength*2;
		SetHpRegenRate(who, cint(GetStrength(who)*24) );
		return;
	endif
	var hungry:=Cint(GetObjProperty(who, "hungrylevel"));
	if (hungry<1)
		hungry:=1;
	endif
	if (who.cmdlevel && who.cmdlevel!="test")
		hungry:=660;
	endif
	var id:=cint((hungry/6)/1.5);
	id:=id+cint(GetSkillFromString(who,ATTRIBUTEID_HEALING)/2);
	//who.hp_regen_rate:=id;

	if (id<1)
		id:=1;
	endif
	SetHpRegenRate(who, cint(id*12) );
endfunction