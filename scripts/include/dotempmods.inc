use uo;
use util;

//include "include/dismount";
include "include/regenerations";
include "include/client";
include "include/sounds";
include "include/customresist";
include "include/skillsinc";

//include ":mounts:mounts";
include ":timedScripts:timedScripts";

function CanMod(byref who, stat, persistedprop:=0)
	var modarray := "#mods";
	if ( persistedprop )
		modarray := "mods";
	endif

	var currentmods := GetObjProperty(who,modarray);
	if (!currentmods)
		return 1;
	endif
	
	foreach mmod in currentmods
		if ( mmod[1] == stat )
			return 0;
		endif
	endforeach
	
	return 1;
endfunction

//for nonpersistedmods
function DoTempMod(byref who, stat, amt, duration)
	var mmod := {};
	mmod[1] := stat;
	mmod[2] := amt;
	mmod[3] := ReadGameClock()+duration;

	set_critical(1);

	var allmods := GetObjProperty(who,"#mods");
	if (!allmods)
		allmods := {};
	endif

	// Controllo se il PG ha gia' quel modificatore.
	var found:=0;
	foreach oldmod in allmods
		if ( oldmod[1]==mmod[1] )
			// Modificatore gi  presente
			found:=1;
			break;
		endif
	endforeach

	if (!found)
		// Posso aggiungere il nuovo modificatore
		allmods.append(mmod);

		SetObjProperty(who,"#mods",allmods);
		
		case (stat)
			"statloss_s":
			SetStrengthMod(who, GetStrengthMod(who) + mmod[2]);
			break;
			"statloss_i":
			SetIntelligenceMod(who, GetIntelligenceMod(who) + mmod[2]);
			break;
			"statloss_d":
			SetDexterityMod(who, GetDexterityMod(who) + mmod[2]);
			break;
			"bstr" :
			SetStrengthMod(who, GetStrengthMod(who) + mmod[2]);
			break;
			"bdex" :
			SetDexterityMod(who, GetDexterityMod(who) + mmod[2]);
			break;
			"bint" :
			SetIntelligenceMod(who, GetIntelligenceMod(who) + mmod[2]);
			break;
			"mstr" :
			SetStrengthMod(who, GetStrengthMod(who) - mmod[2]);
			break;
			"mdex" :
			SetDexterityMod(who, GetDexterityMod(who) - mmod[2]);
			break;
			"mint" :
			SetIntelligenceMod(who, GetIntelligenceMod(who) - mmod[2]);
			break;
			"powerstr" :
			SetStrengthMod(who, GetStrengthMod(who) + mmod[2]);
			break;
			"powerdex" :
			SetDexterityMod(who, GetDexterityMod(who) + mmod[2]);
			break;
			"powerint" :
			SetIntelligenceMod(who, GetIntelligenceMod(who) + mmod[2]);
			break;
			"powerar" :
			who.ar_mod := who.ar_mod + mmod[2];
			"ball" :
			SetStrengthMod(who, GetStrengthMod(who) + mmod[2]);
			SetDexterityMod(who, GetDexterityMod(who) + mmod[2]);
			SetIntelligenceMod(who, GetIntelligenceMod(who) + mmod[2]);
			break;
			"mall" :
			SetStrengthMod(who, GetStrengthMod(who) - mmod[2]);
			SetDexterityMod(who, GetDexterityMod(who) - mmod[2]);
			SetIntelligenceMod(who, GetIntelligenceMod(who) - mmod[2]);
			break;
			"ar":
			who.ar_mod := who.ar_mod + mmod[2];
			break;
			"reactivearmor":
			SetObjProperty(who,"ReactiveArmor",mmod[2]);
			break;
			"p":
			who.paralyzed := 1;
			break;
			"poly":
			MP_Unmount( who );
			//dismount(who, 0);
			break;
		endcase

		if (who.npctemplate)
			var parms := {};
			parms[1] := who;
			parms[2] := mmod[1];
			parms[3] := mmod[2];
			parms[4] := duration;
			parms[5] := 0;//nonpersisted mods
			start_script("::/misc/unmodnpc", parms);
		endif

	endif

	set_critical(0);
	return 1;

endfunction

//for name, graphic and color changes!
function DoPersistedMod(byref who, prop, newvalue, oldvalue, duration)

	var mmod := {};
	mmod[1] := prop;
	mmod[2] := oldvalue;
	mmod[3] := ReadGameClock()+duration;

	set_critical(1);

	var allmods := GetObjProperty(who,"mods");
	if (!allmods)
		allmods := {};
	endif

	// Controllo se il PG ha gia' quel modificatore.
	var found:=0;
	foreach oldmod in allmods
		if ( oldmod[1]==mmod[1] )
			// Modificatore gi  presente
			found:=1;

			break;
		endif
	endforeach

	if (!found)
		// Posso aggiungere il nuovo modificatore
		allmods.append(mmod);

		SetObjProperty(who,"mods",allmods);

		case (prop)
			"disguise":
			SetObjProperty( who , "disguise" , newvalue );
			break;
			"title_guild":
			who.title_guild := newvalue;
			break;
			"title_prefix":
			who.title_prefix := newvalue;
			break;
			"title_suffix":
			who.title_suffix := newvalue;
			break;
			"name":
			who.name := newvalue;
			break;
			"graphic":
			MP_Unmount( who );
			//dismount(who, 0);
			who.graphic := newvalue;
			break;
			"color":
			who.color := newvalue;
			break;
			"gender":
			who.gender := newvalue;
			break;
			"spiritspeak":
			GrantPrivilege( who, "hearghosts" );
			who.enable( "hearghosts" );
			break;
			"hair":
			var h:=GetEquipmentByLayer(who,LAYER_HAIR);
			SetObjProperty(who,"HairColor",(h.color));
			DestroyItem(h);
			if (newvalue)
				var newhair := CreateItemAtLocation(5288 , 1176 , 0, newvalue, 1);
				newhair.color := RandomHairColor();
				EquipItem(who,newhair);
			endif
			break;
			"beard":
			var b:=GetEquipmentByLayer(who,LAYER_BEARD);
			SetObjProperty(who,"BeardColor",(b.color));
			DestroyItem(b);
			if (newvalue)
				var beard := CreateItemAtLocation(5288 , 1176 , 0, newvalue, 1);
				beard.color := RandomHairColor();
				EquipItem(who,beard);
			endif
			break;
		endcase

		if (who.npctemplate)
			var parms := {};
			parms[1] := who;
			parms[2] := mmod[1];
			parms[3] := mmod[2];
			parms[4] := duration;
			parms[5] := 1;//persisted mod
			start_script("::/misc/unmodnpc", parms);
		endif

	endif

	set_critical(0);
	return 1;

endfunction

function RemoveNPCMod(parms)

	var who := parms[1];
	var prop := parms[2];
	var amount := parms[3]; //for persisted mods, this is the oldvalue
	var duration := parms[4];
	var persistedmod := parms[5];

	sleep( duration + 1);

	if (who)
		CheckMods(who, persistedmod);
	endif

endfunction

function CheckMods(byref who, persisted)

	//For Reference:
	// mmod[1] := stat;
	// mmod[2] := amt;
	// mmod[3] := ReadGameClock()+duration;

	set_critical(1);
	var now := ReadGameClock();
	var modarray := "#mods";
	if (persisted)
		modarray := "mods";
		if ( CanMod(who,"poison",1) || (!who.poisoned) )
			// this person should not be poisoned
			SetHpRate(who);
			who.SetPoisoned(0);
			SendSysMessage( who, "Poison worn off." );
			//TS_StopTimer( who, "DefaultPoison" ); should be already stopped?
		else
			//   set_critical(1);
			var cmods := GetObjProperty(who,"mods");
			if (!cmods)
				cmods := { };
			endif
			var curplvl := 0;
			if (cmods)
				curplvl := GetCurrentPoisonLevel(cmods);
			endif
			curplvl-=1;
			var dur;
			var mmod := {};
			if (curplvl<1)
				dur := -1000;
				mmod[1] := "poison";
				mmod[2] := 0;
				mmod[3] := ReadGameClock()+dur;
				SetHpRate(who);
				who.SetPoisoned(0);
				SendSysMessage( who, "Poison worn off." );
				//TS_StopTimer( who, "DefaultPoison" ); //should be already stopped?
			else
				dur := 20+(10*curplvl);
				mmod[1] := "poison";
				mmod[2] := curplvl;
				mmod[3] := ReadGameClock()+dur;
				SetHpRate(who);
				SetHpRegenRate(who, GetHpRegRate(who) + cint(GetPoisonRegenMod(curplvl))*12 );//hundreth_per_minute
				who.SetPoisoned(1);
				TS_StartTimer( who, "DefaultPoison", dur, curplvl, who );//check finale
				//who.hp_regen_rate := who.hp_regen_rate + cint(GetPoisonRegenMod(curplvl));
			endif
			AddToPoisonMod(cmods, mmod);
			SetObjProperty(who,"mods",cmods);
			//   set_critical(0);
		endif
	endif

	var newmods := {};

	// set_critical(1);
	var currentmods := GetObjProperty(who,modarray);
	if (!currentmods)
		set_critical(0);
		return;
	endif
	foreach mmod in currentmods
		if ( now >= mmod[3] || who.dead )
			UndoTheMod( who, mmod[1], mmod[2] );
		else
			newmods.append(mmod);
		endif
	endforeach

	if (len(newmods))
		SetObjProperty(who,modarray,newmods);
	else
		EraseObjProperty(who,modarray);
	endif

	set_critical(0);

endfunction

function UndoTheMod( byref who, prop, amt )

	var msg, sound;

	var oldvalue := amt;
	case (prop)
		"statloss_s":
		SetStrengthMod(who, GetStrengthMod(who) - amt);
		break;
		"statloss_i":
		SetIntelligenceMod(who, GetIntelligenceMod(who) - amt);
		break;
		"statloss_d":
		SetDexterityMod(who, GetDexterityMod(who) - amt);
		break;
		"str" :
		"bstr" :
		SetStrengthMod(who, GetStrengthMod(who) - amt);
		break;
		"dex":
		"bdex":
		SetDexterityMod(who, GetDexterityMod(who) - amt);
		break;
		"int":
		"bint":
		SetIntelligenceMod(who, GetIntelligenceMod(who) - amt);
		break;
		"mstr":
		SetStrengthMod(who, GetStrengthMod(who) + amt);
		break;
		"mdex":
		SetDexterityMod(who, GetDexterityMod(who) + amt);
		break;
		"mint":
		SetIntelligenceMod(who, GetIntelligenceMod(who) + amt);
		break;
		"powerstr" :
		SetStrengthMod(who, GetStrengthMod(who) - amt);
		break;
		"powerdex" :
		SetDexterityMod(who, GetDexterityMod(who) - amt);
		break;
		"powerint":
		SetIntelligenceMod(who, GetIntelligenceMod(who) - amt);
		break;
		"powerar":
		who.ar_mod := who.ar_mod - amt;
		break;
		"all":
		"ball":
		SetStrengthMod(who, GetStrengthMod(who) - amt);
		SetDexterityMod(who, GetDexterityMod(who) - amt);
		SetIntelligenceMod(who, GetIntelligenceMod(who) - amt);
		break;
		"mall":
		SetStrengthMod(who, GetStrengthMod(who) + amt);
		SetDexterityMod(who, GetDexterityMod(who) + amt);
		SetIntelligenceMod(who, GetIntelligenceMod(who) + amt);
		break;
		"ar":
		who.ar_mod := who.ar_mod - amt;
		msg := "Armor is now " + who.ar;
		sound := SFX_1E3;
		break;
		"reactivearmor":
		msg := "Reactive Armor effect is now finished";
		sound := SFX_1E3;
		EraseObjProperty(who,"ReactiveArmor");
		break;
		"p":
		who.paralyzed := 0;
		break;
		"poly" :
		msg := "Polymorph effect is now finished";
		sound := SFX_1E3;
		var realgraph:=GetObjProperty(who,"origform");
		if (!realgraph)
			realgraph:=who.trueobjtype;
		else
			who.graphic:=realgraph;
		endif
		var realcolor:=GetObjProperty(who,"origcolor");
		if (!realcolor)
			who.color:=1109;
		else
			who.color:=realcolor;
		endif
		EraseObjProperty(who,"origform");
		EraseObjProperty(who,"origcolor");
		break;
		"name":
		who.name := oldvalue;
		msg := "Your name is now " + who.name;
		sound := SFX_1E3;
		break;
		"graphic":
		who.graphic := oldvalue;
		break;
		"color":
		who.color := oldvalue;
		break;
		"spiritspeak":
		if (!amt) RevokePrivilege( who, "hearghosts" ); endif
		break;
		"disguise":
		EraseObjProperty( who , "disguise");
		break;
		"title_guild":
		who.title_guild :=  oldvalue;
		break;
		"title_prefix":
		who.title_prefix := oldvalue;
		break;
		"title_suffix":
		who.title_suffix := oldvalue;
		break;
		"gender":
		who.gender := oldvalue;
		break;
		"hair":
		DestroyItem(GetEquipmentByLayer(who,LAYER_HAIR));
		if (oldvalue)
			var newhair := CreateItemAtLocation(5288 , 1176 , 0, oldvalue, 1);
			newhair.color := Cint(GetObjProperty(who,"HairColor"));
			EraseObjProperty(who,"HairColor");
			EquipItem(who,newhair);
		endif
		break;
		"beard":
		DestroyItem(GetEquipmentByLayer(who,LAYER_BEARD));
		if (oldvalue)
			var beard := CreateItemAtLocation(5288 , 1176 , 0, oldvalue, 1);
			beard.color := Cint(GetObjProperty(who,"BeardColor"));
			EraseObjProperty(who,"BeardColor");
			EquipItem(who,beard);
		endif
		break;
		"poison":
		SetHpRate(who);
		who.SetPoisoned(0);
		//SendSysMessage( who, "Poison worn off." );
		TS_StopTimer( who, "DefaultPoison" ); //should be already stopped?
		break;
	endcase

	if (sound) PlaySoundEffectPrivate(who, sound, who); endif
	if (msg) PrintTextAbovePrivate(who,msg,who); endif

endfunction

function SetPoison(byref who, plvl, msg)
	//:poisoning:poisondamage is called, which calls
	//setthepoison() below.  This is so we can tweak poisondamage
	//on-the-fly, and only have to recompile the one script

	var parms := {};
	parms[1] := who;
	parms[2] := plvl;
	parms[3] := msg;

	start_script(":poisoning:poisondamage", parms);

endfunction

function SetThePoison(byref who, plvl, msg)
	if (ProtectedFromPoison(who))
		return 0;
	endif

	plvl := cint(plvl);
	
	//####################### CUSTOM RESIST FOR POISON DAMAGE #################################
	//MAgic spell POISON no need to check custom resist...
	// In poison damage.. custom resist wil be calvulated
	var customres:=CustomResistof(who,RES_VENOM);
	if (customres>0)
		if (customres<RandomInt(100)) //percentuale di sottrarre livelli di veleno
			var leveltosubtract:=randomint(cint(cdbl(customres)/10));
			if (leveltosubtract < 1)
				leveltosubtract:=1;
			endif
			if (leveltosubtract>5)
				leveltosubtract:=5;
			endif
			if (!who.npctemplate)
				case (leveltosubtract)
				1: SendSysmessage(who,"Your venom resistance protect you from lesser poison damage."); break;
				2: SendSysmessage(who,"Your venom resistance protect you from normal poison damage."); break;
				3: SendSysmessage(who,"Your venom resistance protect you from grater poison damage."); break;
				4: SendSysmessage(who,"Your venom resistance protect you from semideadly poison damage."); break;
				5: SendSysmessage(who,"Your venom resistance protect you from deadly poison damage."); break;
				endcase
			endif
			plvl -= leveltosubtract;
		endif
	endif

	if (plvl < 1)
		return 0;
	endif

	if ( plvl > 5 )
		plvl := 5;
	endif

	//midrunuo
	//TS_StartTimer( who, "RuoPoison", poisondelay(plvl), poisoninterval(plvl), who );//check finale
	//if (who.cmdlevel)
		//if ( CInt( GetObjProperty( who, "#PoisonedLevel" )) <= plvl )
		SetObjProperty( who, "#PoisonedTicks", 10 );
		TS_StartTimer( who, "RuoPoison", cint( (10*poisondelay(plvl)+poisoninterval(plvl))/1000 ), plvl, who );//check finale
		//SendSysmessage(who,"test: veleno "+plvl + ";dur:"+cint( (10*poisondelay(plvl)+poisoninterval(plvl))/1000 ) );
		return 1;
	//endif

	//amount delay
	//level interval
	//attacker = who

	//pol
	/*
	var currentmods := GetObjProperty(who,"mods");
	if (!currentmods)
		currentmods := { };
	endif
	var current := 0;

	if (currentmods)
		current := GetCurrentPoisonLevel(currentmods);
	else
		currentmods := {};
	endif

	//SendSysmessage(who,"[test] current poison: "+current);

	if (plvl < current)
		return 0;
	endif //they are already more poisoned

	set_critical(1);

	// var currentpoisonregenmod := GetPoisonRegenMod(current);

	var newpoisonregenmod := GetPoisonRegenMod(plvl);
	//SendSysmessage(who,"[test] poisonregenmod: "+ newpoisonregenmod );

	SetHpRate(who);
	//resetta la rigenerazione e mette una prop con la rigenerazione base
	SetHpRegenRate(who, GetHpRegRate(who) + newpoisonregenmod*12 );//hundreth_per_minute
	who.SetPoisoned(1);
	//resetta la rigenerazione rimettendo la prop con la rigenerazione moddata

	//who.hp_regen_rate := who.hp_regen_rate + (newpoisonregenmod);
	//Regen rates are in "hundredths of points per minute."
	//A current regen rate of 100 (1 point per 5 seconds) would therefore
	//be 1200 (12 points per minute) under the new system.

	var duration := 20+(2*plvl);//era 10*plvl
	//SendSysmessage(who,"[test] duration: "+ duration );

	var mmod := {};
	mmod[1] := "poison";
	mmod[2] := plvl;
	mmod[3] := ReadGameClock()+duration;

	if (!current)
		//who.SetPoisoned(1);
		if ( msg )
			PrintTextAbovePrivate(who,msg,who);
		endif
		currentmods.append(mmod);
	else
		AddToPoisonMod(currentmods, mmod);
	endif

	SetObjProperty(who,"mods",currentmods);
	set_critical(0);
	TS_StartTimer( who, "DefaultPoison", duration, plvl, who );//check finale

	if (who.npctemplate)
		var parms := {};
		parms[1] := who;
		parms[2] := mmod[1];
		parms[3] := mmod[2];
		parms[4] := duration;
		parms[5] := 1;//persisted mods
		start_script("::/misc/unmodnpc", parms);
	endif
	*/
endfunction

function GetPoisonRegenMod(plvl)
	case (plvl)
		0: return 0;
		1: return -1100;
		2: return -1500;
		3: return -1900;
		4: return -2300;
		5: return -2700;
	endcase
endfunction

function poisondelay(plvl)
	return 3500;
	/*
	case (plvl)
		0: return 0;
		1: return -1100;
		2: return -1500;
		3: return -1900;
		4: return -2300;
		5: return -2700;
	endcase
	*/
endfunction

function poisoninterval(plvl)
	case (plvl)
		0: return 0;
		1: return 3000;//lesser
		2: return 3000;//regular
		3: return 3000;//greater
		4: return 3500;//semideadly
		5: return 4000;//deadly
		6: return 5000;//lethal
	endcase
endfunction
function GetCurrentPoisonLevel(byref currentmods)

	if (!len(currentmods)) return 0; endif
	foreach mmod in currentmods
		if ( mmod[1] == "poison" )
			return mmod[2];
		endif
	endforeach

	return 0;
endfunction

function AddToPoisonMod(byref currentmods, byref mmod)
	set_critical(1);
	foreach oldmod in currentmods
		if (oldmod[1] == "poison")
			oldmod[2] := mmod[2];
			oldmod[3] := mmod[3];
			return 0;
		endif
	endforeach

	currentmods.append(mmod);
	set_critical(0);

	return 1;
endfunction

function CurePoison(byref who, sound := 0, msg := 0, byref whocure, plvlcure:=0, tickscure:=0)
	var parms := {};
	parms[1] := who;//a chi viene curata
	parms[2] := sound;
	parms[3] := msg;
	parms[4] := whocure;//chi cura
	parms[5] := plvlcure;
	parms[6] := tickscure;

	start_script(":poisoning:poisoncure", parms);
endfunction

function CureThePoison(byref who, sound := 0, msg := 0, plvlcure:=0, cured_ticks := 0)
	//RUnUO:
	set_critical(1);
	var current_poison := CInt( GetObjProperty( who, "#PoisonedLevel" ));
	var current_ticks := CInt( GetObjProperty( who, "#PoisonedTicks" ));
	if (current_poison-cint(plvlcure) > 0 )
		SetObjProperty( who, "#PoisonedLevel", cint(current_poison-plvlcure) );
	else
		EraseObjProperty( who, "#PoisonedLevel" );
		EraseObjProperty( who, "#PoisonedTicks" );
		if (who.poisoned)
			who.SetPoisoned(0);
		endif
	endif
	if (current_ticks-cint(cured_ticks) > 0 )
		SetObjProperty( who, "#PoisonedTicks", cint(current_ticks-cured_ticks) );
	else
		EraseObjProperty( who, "#PoisonedLevel" );
		EraseObjProperty( who, "#PoisonedTicks" );
		if (who.poisoned)
			who.SetPoisoned(0);
		endif
	endif
	set_critical(0);
	if (sound) PlaySoundEffectPrivate(who, sound, who); endif
	if (msg) PrintTextAbovePrivate(who,msg,who); endif

	/*POL:
	set_critical(1);
	var currentmods := GetObjProperty(who,"mods");
	if (!currentmods) set_critical(0); return; endif
	var newmods := {};
	foreach mmod in currentmods
		if ( mmod[1] == "poison" )
			mmod[2]:=mmod[2]-plvlcure;
			if (mmod[2]<1)
				UndoTheMod( who, mmod[1], mmod[2] );
			else
				SetHpRate(who);
				SetHpRegenRate(who, GetHpRegRate(who) + cint(GetPoisonRegenMod(mmod[2]))*12 );//hundreth_per_minute
				//who.hp_regen_rate := who.hp_regen_rate + cint(GetPoisonRegenMod(mmod[2]));
				newmods.append(mmod);
			endif
		else
			newmods.append(mmod);
		endif
	endforeach

	if (len(newmods))
		SetObjProperty(who,"mods",newmods);
	else
		EraseObjProperty(who,"mods");
	endif

	set_critical(0);
	if (sound) PlaySoundEffectPrivate(who, sound, who); endif
	if (msg) PrintTextAbovePrivate(who,msg,who); endif
	*/
endfunction

function PoisonLevel(byref who)
	//poisonrunuo:
	return CInt( GetObjProperty( who, "#PoisonedLevel" ));

	var currentmods := GetObjProperty(who,"mods");
	if (!currentmods)
		return 0;
	else
		return GetCurrentPoisonLevel(currentmods);
	endif
endfunction

function ProtectedFromPoison(byref who)

	var protserial := GetObjProperty(who,"#protpoison");
	if (!protserial)
		return 0;
	endif

	var item;
	foreach thing in ListEquippedItems(who)
		if ( thing.serial == protserial )
			item := thing;
			break;
		endif
	endforeach

	if (!item)
		return 0;
	endif

	var charges := GetObjProperty(item,"protpoison");
	if (!charges)
		eraseobjproperty(who,"#protpoison");
		eraseobjproperty(item,"protpoison");
		PrintTextAbovePrivate(item,item.desc + " is out of charges!",who);
		return 0;
	else
		charges -= 1;
		setobjproperty(item,"protpoison",charges);
		EraseObjProperty(who, "poison_level");
		PrintTextAbovePrivate(who,item.desc + " protects you from the poison!",who);
		return 1;
	endif
endfunction

function skillreq(str)//GetResistDifficulty()
	case (str)
		0: return 15; break;
		1: return 30; break;
		2: return 45; break;
		3: return 63; break;
		4: return 80; break;
		5: return 95; break;
		default: return 95; break;
	endcase
endfunction

function RemovePolyMods(byref who)
	set_critical(1);
	var currentmods := GetObjProperty(who,"#mods");
	if (!currentmods)
		set_critical(0);
		return;
	endif
	var newmods := {};
	foreach mmod in currentmods
		if ( mmod[1] == "poly" || mmod[1] == "bstr" || mmod[1] == "bdex" || mmod[1] == "bint" ||
			mmod[1] == "ar" || mmod[1] == "title_guild" || mmod[1] == "title_prefix" ||
			mmod[1] == "title_suffix" || mmod[1] == "name" || mmod[1] == "disguise")

			UndoTheMod( who, mmod[1], mmod[2] );
		else
			newmods.append(mmod);
		endif
	endforeach

	if (len(newmods))
		SetObjProperty(who,"#mods",newmods);
	else
		EraseObjProperty(who,"#mods");
	endif

	set_critical(0);
endfunction

function RemovePersistedMods(byref who, stat)
	set_critical(1);
	var currentmods := GetObjProperty(who,"mods");
	if (!currentmods)
		set_critical(0);
		return;
	endif
	var newmods := {};
	foreach mmod in currentmods
		if ( mmod[1] == stat )
			UndoTheMod( who, mmod[1], mmod[2] );
		else
			newmods.append(mmod);
		endif
	endforeach

	if (len(newmods))
		SetObjProperty(who,"mods",newmods);
	else
		EraseObjProperty(who,"mods");
	endif

	set_critical(0);
endfunction

function RandomHairColor()
	return RandomInt(46)+1102;
endfunction