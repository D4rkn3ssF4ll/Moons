use attributes;
use util;
use os;
use math;

include "include/skillpoints";//default raw points to get
include "include/stringarrays";//array2string e viceversa

include "include/karmafame"; // KarmaFame
include "include/client";

include "include/attributes";//new getskill

//region costants
/*
// costanti prese da attributes.inc
//
*/
const CFGFILESPEC_SKILLSDEF        := ":hooks:skillsdef";
const CORE_SETTINGS_MIN_ATTRIBUTE  := 0;
const CORE_SETTINGS_MAX_ATTRIBUTE  := 1049600;

const ATTRIBUTEID_ALCHEMY		:= "alchemy";
const ATTRIBUTEID_ANATOMY		:= "anatomy";
const ATTRIBUTEID_ANIMALLORE		:= "animallore";
const ATTRIBUTEID_ARCHERY		:= "archery";
const ATTRIBUTEID_ARMSLORE		:= "armslore";
const ATTRIBUTEID_BEGGING		:= "begging";
const ATTRIBUTEID_BLACKSMITHY		:= "blacksmithy";
const ATTRIBUTEID_BOWCRAFT		:= "bowcraft";
const ATTRIBUTEID_CAMPING		:= "camping";
const ATTRIBUTEID_CARPENTRY		:= "carpentry";
const ATTRIBUTEID_CARTOGRAPHY		:= "cartography";
const ATTRIBUTEID_COOKING		:= "cooking";
const ATTRIBUTEID_DETECTINGHIDDEN	:= "detectinghidden";
const ATTRIBUTEID_DETECTINGHIDDENALIAS	:= "detecthidden";
const ATTRIBUTEID_DISCORDANCE		:= "discordance";
const ATTRIBUTEID_EVALINT		:= "evaluatingintelligence";
const ATTRIBUTEID_EVALINTALIAS1		:= "evaluatingint";
const ATTRIBUTEID_EVALINTALIAS2		:= "evalint";
const ATTRIBUTEID_FENCING		:= "fencing";
const ATTRIBUTEID_FISHING		:= "fishing";
const ATTRIBUTEID_FORENSICS		:= "forensicevaluation";
const ATTRIBUTEID_FORENSICSALIAS1	:= "forensiceval";
const ATTRIBUTEID_FORENSICSALIAS2	:= "forensics";
const ATTRIBUTEID_HEALING		:= "healing";
const ATTRIBUTEID_HERDING		:= "herding";
const ATTRIBUTEID_HIDING		:= "hiding";
const ATTRIBUTEID_INSCRIPTION		:= "inscription";
const ATTRIBUTEID_ITEMID		:= "itemid";
const ATTRIBUTEID_LOCKPICKING		:= "lockpicking";
const ATTRIBUTEID_LUMBERJACKING		:= "lumberjacking";
const ATTRIBUTEID_MACEFIGHTING		:= "macefighting";
const ATTRIBUTEID_MACEFIGHTINGALIAS	:= "mace";
const ATTRIBUTEID_MAGERY		:= "magery";
const ATTRIBUTEID_MAGICRESISTANCE	:= "magicresistance";
const ATTRIBUTEID_MAGICRESISTANCEALIAS1	:= "magicresist";
const ATTRIBUTEID_MAGICRESISTANCEALIAS2 := "resistingspells";
const ATTRIBUTEID_MAGICRESISTANCEALIAS3 := "resist";
const ATTRIBUTEID_MEDITATION		:= "meditation";
const ATTRIBUTEID_MINING		:= "mining";
const ATTRIBUTEID_MUSICIANSHIP		:= "musicianship";
const ATTRIBUTEID_PARRY			:= "parry";
const ATTRIBUTEID_PEACEMAKING		:= "peacemaking";
const ATTRIBUTEID_POISONING		:= "poisoning";
const ATTRIBUTEID_PROVOCATION		:= "provocation";
const ATTRIBUTEID_REMOVETRAP		:= "removetrap";
const ATTRIBUTEID_SNOOPING		:= "snooping";
const ATTRIBUTEID_SPIRITSPEAK		:= "spiritspeak";
const ATTRIBUTEID_STEALING		:= "stealing";
const ATTRIBUTEID_STEALTH		:= "stealth";
const ATTRIBUTEID_SWORDSMANSHIP		:= "swordsmanship";
const ATTRIBUTEID_SWORDSMANSHIPALIAS	:= "swords";
const ATTRIBUTEID_TACTICS		:= "tactics";
const ATTRIBUTEID_TAILORING		:= "tailoring";
const ATTRIBUTEID_TAMING		:= "animaltaming";
const ATTRIBUTEID_TASTEID		:= "tasteidentification";
const ATTRIBUTEID_TINKERING		:= "tinkering";
const ATTRIBUTEID_TRACKING		:= "tracking";
const ATTRIBUTEID_VETERINARY		:= "veterinary";
const ATTRIBUTEID_WRESTLING		:= "wrestling";

const ATTRIBUTEID_FAITH			:= "faith";
const ATTRIBUTEID_CHIVARLY		:= "chivarly";
const ATTRIBUTEID_NECROMANCY		:= "necromancy";
const ATTRIBUTEID_THROWING		:= "throwing";
//end region

var skills_cfg:=ReadConfigFile("::skills");
//var skills_new_cfg:=ReadConfigFile("::skills_new");

// 52 valori, uno per ogni skill

//newskills //magiusche
const stdLNK:="0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0";
const stdMOD:="0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0";

// I lucchetti sono gestiti da una stringa di 53 valori
// 0 - frecca su
// 1 - freccia giu
// 2 - lucchetto

var StatCap :=225;//was 0
var SkillCap :=7000;//was 0
var MinCappedSkill :=250;//was 0
var subcap:=0;//was global
var AllSubCaps:={};//was global
var UncappedSkills:={};//was global

//SetUpGlobalStatSkillCapVars();

function SetUpGlobalStatSkillCapVars()
//	syslog("Setupping Global CAP VARS....");
	var CapVars:=GetGlobalProperty("SkillCap");
	//{capvalue,xminskill,defadv,combadv}
	StatCap := GetGlobalProperty("StatCap");
	SkillCap := cint(CapVars[1]); // value * 10
	MinCappedSkill := cint(CapVars[2]); // value * 10
	subcap:=1500; // Value *10
	AllSubCaps:=GetGlobalProperty("SubCaps");
	UncappedSkills:=GetGlobalProperty("OutOfCapSkills");
//	print("Global Skill caP vars: " + CapVars);
//	print("SkillCap= " + SkillCap);
//	print("StatCap= " + StatCap);
//	print("SubCaps= " + SubCap);
//	print("MinCappedSkill = " + MinCappedSkill);
endfunction

function GetSubCapbySkill(skillid)
	//restituisce un'array di subcaps dove esiste questa skill
	var lastsub:={};
	//var elem;
	foreach elem in GetGlobalProperty("SubCaps") //AllSubCaps
		if (skillid in elem)
			lastsub.append(elem);
		endif
	endforeach
	return lastsub;
endfunction

function ControlSkillcapValue(byref who)
	//var CapVars:=GetGlobalProperty("SkillCap");
	//{capvalue,xminskill,defadv,combadv}
	if (StatCap!=225)
		SendSysMessage(who,"DEBUG: An error occur while taking STATCAP [" +StatCap + "]");
		return 0;
	endif
	if (SkillCap!=7000)
		SendSysMessage(who,"DEBUG: An error occur while taking SKILLCAP: "+skillcap);
		return 0;
	endif
	if (MinCappedSkill!=250)
		SendSysMessage(who,"DEBUG: An error occur while taking MinCappedSkill");
		return 0;
	endif	
	return 1;
endfunction

function CheckCap (byref who,skillid,AdvOnStat:=error)//byref SkillLNK tolto

	// AdvOnStat needed for TrainSkill
	if (StatCap==0)
	        SetUpGlobalStatSkillCapVars();
	endif
	if (!ControlSkillcapValue(who))
		SendSysMessage(who,"BEWARE: No stat or skill advance during ERROR....contact a GM");
		return;
	endif
//	print("SkillCap= " + SkillCap);
//	print("StatCap= " + StatCap);
//	print("SubCaps= " + SubCap);
//	print("MinCappedSkill = " + MinCappedSkill);
//	syslog(retSkillName(skillid));

	//Controllo dei SubCap
	var s1,s2,s3,sk;//,incremento,res:={};
	s1:=0;
	s2:=0;
	s3:=0;
	sk:=0;
	var subskills:=2; // le skill interessate da questo cap
	var selectedsubcaps:=GetSubCapbySkill(skillid);
	//var thesubcap:={};
	foreach thesubcap in selectedsubcaps
		if (thesubcap.size()>0)
			s1:=GetBaseSkill(who,thesubcap[1]);//NewRawSkillToBaseSkill(cint(GetRawSkill(who,thesubcap[1])));
			s2:=GetBaseSkill(who,thesubcap[2]);//NewRawSkillToBaseSkill(cint(GetRawSkill(who,thesubcap[2])));
			if (thesubcap[3]>=0)
				s3:=GetBaseSkill(who,thesubcap[3]);//NewRawSkillToBaseSkill(cint(GetRawSkill(who,thesubcap[3])));
				subskills:=3; // esiste anche la 3 skills :-)
			endif
			sk:=s1+s2+s3;
			//TODO: controllare se subcap 4 è in tenths
			if (sk>thesubcap[4])
				var decremento:=sk-thesubcap[4];
				//le 3 o 2 skill sommate superano il sub cap stabilito, calcolo
				// i punti da sottrarre ad una delle 3 skill a caso, ovviamente esclusa
				// quella che e' salita per ultima
				var skn:=randomint(subskills-1)+1;
				var scount:=0;
				if (thesubcap[skn]==skillid)
					scount:=subskills+1;
				endif

				//faccio un ciclo per vedere se la skill selezionata puo'
				//scendere
				//syslog("cap :" + thesubcap);
				
				//syslog("Excedded skill: " + retskillname(thesubcap[skn]));
				while (scount>0)
					// controllo che non abbia il lucchetto settato
					skn:=skn+1;
					if (skn>subskills)
						skn:=1;
					endif
					//syslog("Excedded + 1 skill: " + retskillname(thesubcap[skn]));
					if (thesubcap[skn]!=skillid)					
						//syslog("Link on this skill: " + SkillLNK[thesubcap[skn]+1]);
						//if (SkillLNK[thesubcap[skn]+1]!=1)
						if (GetSkillLock(who, thesubcap[skn]) != 1)
							//ha un lucchetto o la freccia verso l'alto
							// provo a vedere un altra skill
						else
							//trovata :-)
							scount:=0;
						endif
					endif
					scount:=scount-1;
				endwhile

				var newsk:=GetBaseSkill(who,thesubcap[skn]);//NewRawSkillToBaseSkill(cint(GetRawSkill(who,thesubcap[skn])));

				if (newsk-decremento<0)
					decremento:=decremento-newsk;
					newsk:=0;
				else
					newsk:=newsk-decremento;
				endif
				//check se funzionante.
				SetRawSkill(who,thesubcap[skn],NewBaseSkillToRawSkill(newsk));

				SendSysMessage(who,"Your Skill in " + GetSkillbyNumber(thesubcap[skn]) + " is decreased of " + (decremento/10) + "% because sub cap limit exceeded.",0x03,40);
				// skill diminuita
			endif
		endif
	endforeach

	//Controllo dello SkillCap
	var totale:=retTotaleSkill(who);//700 non contando alcune skills

	/*print("Totale Skill= " + totale);
	print("Skill Cap= " + SkillCap);
	print("TOT SKILL Cap= " + GetGlobalProperty("SkillCap"));*/

	if (totale>SkillCap)
		//SendSysMessage(who,"DEBUG: HAI OLTREPASSATO IL CAP DI 700, CONTROLLO LA SKILL DA FAR SCENDERE.",0x03,68);

		var incremento:=totale-SkillCap;
		//SendSysMessage(who,"DEBUG: SEI OLTRE IL CAP DI " + incremento + " PUNTI (VALORE*10)",0x03,68);
		//print("cap raggiunto.  over di " + incremento);

		//var t;

		//SendSysMessage(who,"DEBUG: CERCO SKILL CON FRECCE DOWN.",0x03,68);

		//print("Cerco frecce DOWN. Incremento= " + incremento);
		var skl:=0;
		
		var SkDown;
		
		var locktype:=1; // partiamo dalle skill con il lucchetto in basso
		
		//var memlinckskill:=SkillLNK;
		var txtdecr:=0;
		//syslog("SKILL CAP SUPERATO INIZIO SCANSIONE");
		while (locktype<3 && incremento>0)

			skDown:=GetRandomicLockSkill(who,locktype);
			//syslog("Lock Type:=" + locktype + " Incremento:=" + incremento + " SkDown["+ skdown + "]:=" + retSkillName(skdown));

			while (incremento>0 && skDown>=0)
				//esiste una skill con il lucchetto in basso, quindi abbasso quella
				//SkillLNK[skDown+1]:=3;	// (modifico lo skilllock di questa skill per evitare che
				SetSkillLock(who,skDown,1);	// venga ripescata dal RandomicLockSkill, tanto poi lo risetto
						      		// come era prima :-)
				//siccome parte da 1, setto 1; comunque il randomiclock restituisce
				//a caso e mai se sono sotto un tot. Impossibile restituisca skill uguali(?)
				skl:=GetBaseSkill(who,skDown);//cint(NewRawSkillToBaseSkill(cint(GetRawSkill(who,skDown))));
				
				var decr:=cint(skl-incremento);
				if (decr<=MinCappedSkill)
					incremento:=incremento-skl;
					decr:=MinCappedSkill;
				else
					incremento:=0;
				endif
				if (decr<0)
					decr:=0;
				endif
				
				txtdecr:=skl-decr;
				//syslog("Decremento : " + txtdecr);
				SendSysMessage(who,"Your Skill in " + GetSkillbyNumber(skDown) + " is decreased of " + (txtdecr/10.0) + "% couse a skill cap limit excedded.",0x03,40);	
				
				//print("1) abbasso " + i + "   " + retSkillName(i) + "    di " + cdbl((skl-decr)/10.0));
				SetRawSkill(who,skDown,NewBaseSkillToRawSkill(decr));
				//SendSysMessage(who,"DEBUG: ABBASSO " + retSkillName(i) + " DI " + decr + " (VALORE*10)",0x03,68);		

				//continuo il ciclo finche' incremento >0
				if (incremento>0)
					//inutile ripetere l'operazione se l'incremento e' minore o uguale a 0
					skDown:=GetRandomicLockSkill(who,locktype);
				endif
				//syslog("INTERNO - Lock Type:=" + locktype + " Incremento:=" + incremento + " SkDown:=" + skdown);
			endwhile
			if (locktype==1)
				locktype:=0; //Lock Up
			elseif (locktype==0)
				locktype:=2; //Blocked
			elseif (locktype==2)
				locktype:=3; //esci
			endif
			
			
		endwhile
		//syslog("SKILL CAP SUPERATO FINE SCANSIONE");
		
		//SkillLNK:=memlinckskill;
	endif

	//print("Avanza con le STATS");
	//SendSysMessage(who,"DEBUG: AVANZO CON LA SALITA DELLE STATS",0x03,68);
	if (!AdvOnStat)
		AdvanceStats(who,skillid);
	endif
endfunction

/*
//	
//	returns skillid a caso di una skill con un certo LockType
*/
function GetRandomicLockSkill(byref who,LockType)//byref SkillLNK tolto
	//creo l'array di skills con il lucchetto in basso
	
	var t:=0;
	var retskl:={};
	var skl:=0;

	/*syslog("Array di skill da poter abbassare");
	syslog("Mincapped: " +MinCappedSkill);
	print("locket skill: " +skilllnk);
	syslog("Locktype: " + locktype);*/

	for (t:=0; t<SKILLID__HIGHEST;t+=1)
		if (t in UncappedSkills)
			//Queste Skills non sono nel cap
		else
			skl:=GetBaseSkill(who,t);//cint(NewRawSkillToBaseSkill(cint(GetRawSkill(who,t))));
			//if (cint(SkillLNK[t+1])==cint(LockType) && skl>MinCappedSkill)
			if (GetSkillLock(who, t)==cint(LockType) && skl>MinCappedSkill)
				//print(retskillname(t));
				retskl.append(t);
			endif
		endif
	endfor
	
	if (retskl.size()<=0)
		//print(" nessuna skill da abbassare");
		return -1; // nessuna skill con il lucchetto in basso
	else
		var rnd:=RandomInt(retskl.size())+1;
		//print("skill abbassata: " + retskillname(retskl[rnd]));
		return retskl[rnd]; // ritorno una skill random tra quelle scelte
	endif
endfunction

/*	da deprecare
//
//
*/
function loadSkillLnk(byref who)
	var lnk:=GetObjProperty(who,"SkillLNK");
	if (!lnk)
		lnk:=stdLNK;
		SetObjProperty(who,"SkillLNK",stdLNK);
	endif
	return (SplitWords(lnk));
endfunction

/*	da deprecare
//
//
*/
function loadSkillMOD(byref who)
	var modx:=GetObjProperty(who,"SkillMOD");
	if (!modx)
		modx:=stdMOD;
		SetObjProperty(who,"SkillMOD",stdMOD);
	endif
	return (SplitWords(modx));
endfunction

/*	da deprecare
//
//
*/
function saveSkillLNK(byref who, byref SkillLNK)
	var i;
	var word:=SkillLNK[1];
	for (i:=2; i<=SKILLID__HIGHEST+1; i+=1)
		if (cint(SkillLNK[i]))
			word += " " + cstr(SkillLNK[i]);
		else
			word += " 0";
		endif
	endfor
	SetObjProperty(who,"SkillLNK",word);
endfunction

/*	da deprecare
//
//
*/
function saveSkillMOD(byref who, byref SkillMOD)
	var i;
	var word:=SkillMOD[1];
	for (i:=2; i<=SKILLID__HIGHEST+1; i+=1)
		if (cint(SkillMOD[i]))
			word:=word + " " + cstr(SkillMOD[i]);
		else
			word:=word + " 0";
		endif
	endfor
	SetObjProperty(who,"SkillMOD",word);
endfunction

/*	da deprecare
//
//
*/
function addSkillMOD(byref who, skillid, value)
	var SkillMOD:=loadSkillMOD(who);
	SkillMOD[skillid+1]:=cint(value*10);
	// SkillMOD[skillid+1]:=cint(cint(SkillMOD[skillid+1])+cint(value*10));
	saveSkillMOD(who, SkillMOD);
endfunction

/*	da deprecare
//
//
*/
function removeSkillMOD(byref who, skillid, value)
	var SkillMOD:=loadSkillMOD(who);
	SkillMOD[skillid+1]:=0;
	// SkillMOD[skillid+1]:=cint(cint(SkillMOD[skillid+1])-cint(value*10));
	saveSkillMOD(who, SkillMOD);
endfunction

Function retStatName(id)
	var StatN:={"Intelligence","Dexterity","Strength"};
	return StatN[id];
endfunction

/*
//
//	return somma delle skill che non sono cappate in double (?)
*/
function retTotaleSkill(byref who)
	var totale:=0;
	var parziale;
	var i;

	for (i:=0; i<=SKILLID__HIGHEST; i+=1)
		if (i in UncappedSkills)
			//la skill non viene considerata nel cap
		else
			parziale:=GetBaseSkill(who,i);//NewRawSkillToBaseSkill(cint(GetRawSkill(who,i)));
			if (parziale>MinCappedSkill)
				totale:=totale+parziale;
			endif
		endif
	endfor

	return cdbl(totale);
endfunction

//TODO: controllare
function restoreSubCap(c1,c2,c3,c4,s1,s2,s3,incremento,byref SkillLNK)
	case (cint(SkillLNK[c2+1]))
		0:
		if (c3>0)
			case (cint(SkillLNK[c3+1]))
				0:
				s2:=s2-incremento;
				if (s2<0)
					s3:=s3+s2; // s2 è NEGATIVO !!
					s2:=0;
				endif
				1:
				s3:=s3-incremento;
				if (s3<0)
					s2:=s2+s3; // s3 è NEGATIVO !!
					s3:=0;
				endif
				2:
				s2:=s2-incremento;
				if (s2<0)
					s3:=s3+s2; // s2 è NEGATIVO !!
					s2:=0;
				endif
			endcase
		else
			s2:=s2-incremento;
			if (s2<0)
				s2:=0;
				s1:=c4;
			endif
		endif
		1:
		s2:=s2-incremento;
		if (s2<0)
			if (c3>0)
				s3:=s3+s2; // s2 è NEGATIVO !!
				s2:=0;
			else
				s2:=0;
				s1:=c4;
			endif
		endif
		2:
		if (c3>0)
			case (cint(SkillLNK[c3+1]))
				0:
				s3:=s3-incremento;
				if (s3<0)
					s2:=s2+s3; // s3 è NEGATIVO !!
					s3:=0;
				endif
				1:
				s3:=s3-incremento;
				if (s3<0)
					s2:=s2+s3; // s3 è NEGATIVO !!
					s3:=0;
				endif
				2:
				s2:=s2-incremento;
				if (s2<0)
					s3:=s3+s2; // s2 è NEGATIVO !!
					s2:=0;
				endif
			endcase
		else
			s2:=s2-incremento;
			if (s2<0)
				s2:=0;
				s1:=c4;
			endif
		endif
	endcase
	var arr:={};
	arr.append(s1);
	arr.append(s2);
	arr.append(s3);
	return arr;
endfunction

/*
//
//	Controlla l'avanzamento delle stat. Utilizzato se le skill salgono
*/
function AdvanceStats(byref who,skillid)
	//TODO: npc dovrebbero aumentare le stats!
	if (!who.acct)
		return;
	endif

	if (StatCap==0)
		SetUpGlobalStatSkillCapVars();
	endif
	
	if (!ControlSkillcapValue(who))
		SendSysMessage(who,"BEWARE: No stat or skill advance during ERROR....contact a GM");
		return;
	endif
	//print("Avanza con le STATS");
	//var tmp_skills_cfg:=skills_cfg;//GetSkillCFGbySkill(skillid);

	//print("CAPTURE STAT");
	var OldStr:=GetStrength(who)-GetStrengthMod(who);//Cint(GetObjProperty(who,"PGStrength"));
	var OldInt:=GetIntelligence(who)-GetIntelligenceMod(who);//Cint(GetObjProperty(who,"PGIntelligence"));
	var OldDex:=GetDexterity(who)-GetDexterityMod(who);//Cint(GetObjProperty(who,"PGDexterity"));
	var advanced:= 0;

	/*if (OldStr==0)
		OldStr:=NewBaseSkillToRawSkill(cint((who.strength-who.strength_mod)*10));
	endif
	if (OldInt==0)
		OldInt:=NewBaseSkillToRawSkill(cint((who.intelligence-who.intelligence_mod)*10));
	endif
	if (OldDex==0)
		OldDex:=NewBaseSkillToRawSkill(cint((who.dexterity-who.dexterity_mod)*10));
	endif*/
	//SendSysMessage(who,"Str: " + oldstr);
	//print("ADVANCE STAT");
	
	//controllo validita' campi STR INT e DEX

	var StrAdv:=splitwords(skills_cfg[cint(skillid)].StrAdv);
	var IntAdv:=splitwords(skills_cfg[cint(skillid)].IntAdv);
	var DexAdv:=splitwords(skills_cfg[cint(skillid)].DexAdv);

	//SendSysMessage(who,"Str ADV: " + StrAdv[2]);

	StrAdv[1]:=Cint(StrAdv[1]);
	IntAdv[1]:=Cint(IntAdv[1]);
	DexAdv[1]:=Cint(DexAdv[1]);

	//print("OLDSTR = "+ OldStr);
	//print("OLDINT = "+ OldInt);
	//print("OLDDEX = "+ OldDex);

	var NewStr:=OldStr;
	var NewInt:=OldInt;
	var NewDex:=OldDex;

	//print("StrAdv = " + StrAdv[2] + " con " + StrAdv[1] + "%");
	//print("IntAdv = " + IntAdv[2] + " con " + IntAdv[1] + "%");
	//print("DexAdv = " + DexAdv[2] + " con " + DexAdv[1] + "%");

	var rnd;

	if (!GetObjProperty(who,"LastStrGain"))
		SetObjProperty(who,"LastStrGain", readgameclock());
	endif

	if (!GetObjProperty(who,"LastIntGain"))
		SetObjProperty(who,"LastIntGain", readgameclock());
	endif

	if (!GetObjProperty(who,"LastDexGain"))
		SetObjProperty(who,"LastDexGain", readgameclock());
	endif
	var lastskill := 1;

	rnd:=randomint(100)+1;
	if ((rnd<=StrAdv[1]) && (OldStr<100))//1048600
		//print("inc STR");
		if (GetObjProperty(who,"LastStrGain") + GetStatGainDelay( OldStr ) < readgameclock() )
			NewStr:= OldStr+1;//cint(OldStr +  RandomDiceRoll(cstr(StrAdv[2])));
		else
			if ( who.cmdlevel > 0 )
				var secs := GetObjProperty(who,"LastStrGain") + GetStatGainDelay( OldStr ) - readgameclock();
				SendSysMessage(who,"Time delay str (secondi): " + secs);
			endif
		endif
		lastskill := 1;
	endif
	//SendSysMessage(who,"Str new: " + NewStr);

	rnd:=randomint(100)+1;
	if ((rnd<=IntAdv[1]) && (OldInt<100))
		//print("inc INT");
		if (GetObjProperty(who,"LastIntGain") + GetStatGainDelay( OldInt ) < readgameclock() )
			NewInt:=OldInt+1;//cint(OldInt +  RandomDiceRoll(cstr(IntAdv[2])));
		else
			if ( who.cmdlevel > 0 )
				var secs := GetObjProperty(who,"LastIntGain") + GetStatGainDelay( OldInt ) - readgameclock();
				SendSysMessage(who,"Time delay int (secondi): " + secs);
			endif
		endif
		lastskill := 2;
	endif
	
	rnd:=randomint(100)+1;//anche con i basevalue sorta di raw?
	if ((rnd<=DexAdv[1]) && (OldDex<100))
		//print("inc DEX");
		if (GetObjProperty(who,"LastDexGain") + GetStatGainDelay( OldDex ) < readgameclock() )
			NewDex:=OldDex+1; //cint(OldDex +  RandomDiceRoll(cstr(DexAdv[2])));
		else
			if ( who.cmdlevel > 0 )
				var secs := GetObjProperty(who,"LastDexGain") + GetStatGainDelay( OldDex ) - readgameclock();
				SendSysMessage(who,"Time delay dex (secondi): " + secs);
			endif
		endif
		lastskill := 3;
	endif

	//var lastskill;
	if ( OldStr + OldInt + OldDex >= 255 )//statcap
		advanced := 1;
	endif
	//print("NewSTR = "+ NewStr + " = " + (NewRawSkillToBaseSkill(cint(NewStr))/10));
	//print("NewINT = "+ NewInt + " = " + (NewRawSkillToBaseSkill(cint(NewInt))/10));
	//print("NewDEX = "+ NewDex + " = " + (NewRawSkillToBaseSkill(cint(NewDex))/10));

	//SendSysMessage(who,"DEBUG: INCREMENTO DI STR= " + cint(NewStr-OldStr),0x03,68);
	//SendSysMessage(who,"DEBUG: INCREMENTO DI INT= " + cint(NewInt-OldInt),0x03,68);
	//SendSysMessage(who,"DEBUG: INCREMENTO DI DEX= " + cint(NewDex-OldDex),0x03,68);

	//SetObjProperty(who,"PGStrength",cint(NewStr));
	//SetObjProperty(who,"PGIntelligence",cint(NewInt));
	//SetObjProperty(who,"PGDexterity",cint(NewDex));

	//var before;
	//var after;

	//before:=cint(NewRawSkillToBaseSkill(cint(OldStr))/10);
	//after:=cint(NewRawSkillToBaseSkill(cint(NewStr))/10);
	//if ((after-before)>0)
	//	SendSysMessage(who,"Your Strength is increased by " + (after-before) + ". Now is " + after);
	//endif
	//who.Setstr(cint(after));

	if ((NewStr-OldStr)>0 && GetStrengthLock(who) == 0 && advanced == 0)
		SetBaseStrength(who,cint(NewStr));
		SetObjProperty(who,"LastStrGain", readgameclock());
		SendSysMessage(who,"Your Strength is increased by 1. Now is " + NewStr);
		advanced := 1;
		lastskill := 1;
	endif

	//before:=cint(NewRawSkillToBaseSkill(cint(OldInt))/10);
	//after:=cint(NewRawSkillToBaseSkill(cint(NewInt))/10);
	//if ((after-before)>0)
	//	SendSysMessage(who,"Your Intelligence is increased by " + (after-before) + ". Now is " + after);
	//endif
	//who.Setint(cint(after));

	if ((NewInt-OldInt)>0 && GetIntelligenceLock(who) == 0 && advanced == 0)
		SetBaseIntelligence(who,cint(NewInt));
		SetObjProperty(who,"LastIntGain", readgameclock());
		SendSysMessage(who,"Your Intelligence is increased by 1. Now is " + NewInt);
		advanced := 1;
		lastskill := 2;
	endif

	//before:=cint(NewRawSkillToBaseSkill(cint(OldDex))/10);
	//after:=cint(NewRawSkillToBaseSkill(cint(NewDex))/10);
	//if ((after-before)>0)
	//	SendSysMessage(who,"Your Dexterity is increased by " + (after-before) + ". Now is " + after);
	//endif
	//who.Setdex(cint(after));

	if ((NewDex-OldDex)>0 && GetDexterityLock(who) == 0 && advanced == 0 )
		SetBaseDexterity(who,cint(NewDex));
		SetObjProperty(who,"LastDexGain", readgameclock());
		SendSysMessage(who,"Your Dexterity is increased by 1. Now is " + NewDex);
		advanced := 1;
		lastskill := 3;
	endif

	if (!who.cmdlevel)
		CheckStatCap(who, lastskill);
	endif
endfunction

function GetStatGainDelay( val:=0 )
	if( val <= 50.0 )
		return 900;
	elseif( val <= 70.0 )
		return 1800;
	elseif( val <= 90.0 )
		return 2400;
	else
		return 3000;
	endif
endfunction

/*	da deprecare
//
//
*/
function resetStats(byref who)
	if (!who.acct)
		return;
	endif

	var OldStr:=Cint(GetObjProperty(who,"PGStrength"));
	var OldInt:=Cint(GetObjProperty(who,"PGIntelligence"));
	var OldDex:=Cint(GetObjProperty(who,"PGDexterity"));

	if (OldStr==0)
		OldStr:=cint(GetStrength(who)-GetStrengthMod(who));
	endif
	if (OldInt==0)
		OldInt:=cint(GetIntelligence(who)-GetIntelligenceMod(who));
	endif
	if (OldDex==0)
		OldDex:=cint(GetDexterity(who)-GetDexterityMod(who));
	endif
	SetBaseStrength(who, cint(OldStr));
	SetBaseIntelligence(who, cint(OldInt));
	SetBaseDexterity(who, cint(OldDex));
	//who.Setstr(cint(cint(NewRawSkillToBaseSkill(cint(OldStr))/10)));
	//who.Setint(cint(cint(NewRawSkillToBaseSkill(cint(OldInt))/10)));
	//who.Setdex(cint(cint(NewRawSkillToBaseSkill(cint(OldDex))/10)));

endfunction

/*
//
//	setta le stat base del pg//This function Set Base stats into PG cprop and set it into paperdroll
*/
function SetStats(byref who,st:=error,it:=error,dx:=error)
	if (st!=error)
		//var oldstr:=NewBaseSkillToRawSkill(cint(st*10));
		//setObjProperty(who,"PGStrength",oldstr);
		//who.Setstr(st);
		SetBaseStrength(who,cint(st));
	endif

	if (it!=error)
		//var oldint:=NewBaseSkillToRawSkill(cint(it*10));
		//SetObjProperty(who,"PGIntelligence",oldint);
		//who.Setint(it);
		SetBaseIntelligence(who,cint(it));
	endif

	if (dx!=error)
		//var olddex:=NewBaseSkillToRawSkill(cint(dx*10));
		//SetObjProperty(who,"PGDexterity",olddex);
		//who.Setdex(dx);
		SetBaseDexterity(who,cint(dx));
	endif
endfunction

/*
//	CanUseSkill(who)
//	returns 0 if cannot use skill right now.
*/
function CanUseSkill(who)
	if (who.isA(POLCLASS_NPC))
		return 1;
	elseif ( who.dead )
		PrintTextAbovePrivate( who, "I am dead and cannot do that.", who );
		return 0;
	elseif ( /*who.active_skill ||*/ who.casting_spell )
		PrintTextAbovePrivate( who, "I am already performing another action.", who );
		return 0;
	elseif ( who.DisableSkillsFor(-1)==1 )
		SendSysmessage(who,"You must wait to perform another action.");
		return 0;
	elseif ( who.frozen )
		PrintTextAbovePrivate( who, "I am frozen and cannot do that.", who );
		return 0;
	elseif ( who.paralyzed )
		PrintTextAbovePrivate( who, "I am paralyzed and cannot do that.", who );
		return 0;
	endif

	return 1;
endfunction

/*
//
//	check stats per i players
*/
function CheckStatCap(byref who, lastskill:=1)
	if (who.cmdlevel)
		return;
	endif
	if (StatCap==0)
	        SetUpGlobalStatSkillCapVars();
	endif
	if (!ControlSkillcapValue(who))
		SendSysMessage(who,"BEWARE: No stat or skill advance during ERROR....contact a GM");
		return;
	endif
	//SendSysMessage(who,"DEBUG: CONTROLLO LO STATCAP",0x03,68);
	var dstr:=GetStrength(who)-GetStrengthMod(who);//cint(who.strength-who.strength_mod);
	var dint:=GetIntelligence(who)-GetIntelligenceMod(who);//cint(who.intelligence-who.intelligence_mod);
	var ddex:=GetDexterity(who)-GetDexterityMod(who);//cint(who.dexterity-who.dexterity_mod);

	var diff:=dstr+dint+ddex-StatCap;
	if (diff<=0)
		return 0;
	endif
	//SendSysMessage(who,"DEBUG: STATCAP OLTREPASSATO",0x03,68);

	var name:={"Strength","Intelligence","Dexterity"};
	//var act:=randomint(3)+1;//cint(getobjproperty(who,"DownStat"));
	var skdown:= {};
	if (GetDexterityLock(who) != 0)
		skdown.append(3);
	endif
	if (GetIntelligenceLock(who) != 0)
		skdown.append(2);
	endif
	if (GetStrengthLock(who) != 0)
		skdown.append(1);
	endif

	var act:=skdown[randomint(len(skdown))+1];
	if (!act || act.errortext || len(skdown) < 1)
		act := lastskill;
		if (act != 1 && act != 2 && act != 3)
			act := RandomInt(3)+1;
		endif
		Print( "[CheckStatCap] on"+who.name+"; error finding down skill, using"+ act );
		SendSysmessage(who, "Controlla i lucchetti delle skill!",0x03,68);
	endif

	//if (act==0)
	//	act:=randomint(3)+1;
	//	setobjproperty(who,"DownStat",act);
	//	//  syslog("ERROR: No DownStat CProp on player "+who.name+" attepting to assign "+act);
	//endif
	case (act)
		1: dstr -= diff; Setstats(who,dstr); break;
		//SendSysMessage(who,"DEBUG: ABBASSO STR",0x03,68);
		2: dint -= diff; Setstats(who,error,dint); break;
		//SendSysMessage(who,"DEBUG: ABBASSO INT",0x03,68);
		3: ddex -= diff; Setstats(who,error,error,ddex); break;
		//SendSysMessage(who,"DEBUG: ABBASSO DEX",0x03,68);
	endcase
	SendSysmessage(who,"La tua " + name[act] + " e' scesa di "+diff+" a causa dello statcap raggiunto!");
endfunction

//region skill checks
function UnModCheckSkill(byref who,attributeid,diff)
	var skill:=GetSkillFromString(who,attributeid);
	if (diff==-1)
		return percheck(skill);
	endif
	var mini:=diff-20;
	var maxi:=diff+20;
	var per:=10;
	if (skill>=maxi)
		per:=990;
	else
		per:=25*(skill-mini);
	endif
	if (randomint(1000)+1<=per)
		return 1;
	else
		return 0;
	endif
endfunction

function PerCheck(num)
	if (num>=100)
		num:=99;
	endif
	if (num<=0)
		num:=0;
	endif
	if (randomint(100)+1<=num)
		return 1;
	endif
	return 0;
endfunction

function UnModCheckDiff(skill,diff)
	if (diff=-1)
		return percheck(skill);
	endif
	var mini:=diff-20;
	var maxi:=diff+20;
	var per:=10;
	if (skill>=maxi)
		per:=990;
	else
		per:=25*(skill-mini);
	endif
	if (randomint(1000)+1<=per)
		return 1;
	else
		return 0;
	endif
endfunction

/*
//	NewCheckSkill(byref who, int skillid,difficulty,points)
//	checkskill con skillid numerico
*/
function NewCheckSkill(byref who,skillid,difficulty,points)
	NewCheckSkillFromString(who, GetAttributeIdBySkillId(skillid), difficulty, points, skillid);
endfunction

function NewCheckSkillFromString(byref who,attributeid,difficulty,points, skillid)

	if ( who.isA(POLCLASS_NPC) || who.paralyzed) //who.npctemplate || who.stamina<=0 || who.paralyzed)
		return UnModCheckSkill(who,attributeid,difficulty);
	endif
	//TODO FIX! usare i lucchetti interni
	//var SkillLNK:=loadSkillLnk(who);//lucchetti delle skill
	//syslog ("CheckSkill on " + GetSkillbyNumber(skillid) );

	var ret:=UnModCheckSkill(who,attributeid,difficulty);

	//if (cint(SkillLnk[skillid+1])==0)//freccia su
	// non va SendSysMessage(who,"Prova Skilllock:" +GetSkillLock(who,skillid-1));
	// OK SendSysMessage(who,"Prova Skilllock2:" +GetSkillLockFromString(who, attributeid));

	if(GetSkillLockFromString(who, attributeid)==0)//freccia su
		var gs:=cint(GetSkillFromString(who,attributeid));//skill senza decimi
		//print("Difficulty = " + difficulty);
		//print("Skill Val. = " + gs);
		//print("Skill RetUNMODCHECK. = " + ret);
		//syslog("rimuovere RET=1 assolutamente");
		//ret:=1;
		if (who.cmdlevel > 0 && difficulty != -1)
			if ( ret == 0 )
				SendSysMessage(who,"Staff use: check fallito -> buono",0x03,68);
				ret := 1;
			endif
		endif

		if (gs<10 || (ret && difficulty==-1) || (ret && difficulty<=(gs+20) && difficulty>=(gs-25)))
			//print("SDentro ");
			if (gs<100)
				//ritornare una percentuale dei punti se difficulty != -1
				var points_percent := 0;

				if (difficulty==-1)
					points_percent := 100;
				else
					if ( (difficulty + 20) < gs)
						points_percent := 0;
					elseif (difficulty > (gs + 20))
						points_percent := 100;
					else
						points_percent := (50 + ( (difficulty - gs) * 2.5) );
					endif
				endif

				//print("Points max = " + points);
				points:=cint((points * points_percent) / 100);

				//print("% P. Gain  = " + points_percent);
				//print("P. Gained  = " + points);

				if (points>0)
					if (points>200) //&& CInt(skillid) != CInt(SKILLID_ITEMID))
						points:=200;
					endif
				
					AwardRawPointsFromString( who, attributeid, points);//, skillid);
					//print("HOOK CHECK STAT");
					CheckCap (who,skillid);//,SkillLNK); // Controlla Cap ed avanza con le STATS.
				endif
			else
				//print("HOOK ADV STAT");
				AdvanceStats(who,skillid);
			endif
		//endif
		else
			//award some points for failure
			//CheckCap ->advanceonstats ->checkstatcap di base
			//CheckStatCap(who);
			AwardRawPointsFromString( who, attributeid, 20);//, skillid);
			//print("HOOK CHECK STAT");
			CheckCap (who,skillid);//,SkillLNK);
		endif
	endif

	return ret;
endfunction
//end region
//modificata posizione
//region basetoraw and viceversa
/*	OK
//	NewBaseSkillToRawSkill( basevalue )
//	FROM tenths to RAW
*/
function NewBaseSkillToRawSkill( basevalue )
	if ( basevalue <= 200 )
		var rawvalue := Cdbl(basevalue * 20.48);
		if( rawvalue > Cint(rawvalue) )
			return Cint(rawvalue + 1);
		else
			return Cint(rawvalue);
		endif
	else
		var scale := Cdbl(basevalue) / 100.0;
		var remain  := scale - Cint(scale);
		return Cint( Pow( 2, Cint(scale) ) * (1024 * (1.0 + remain) + 1) );
	endif
endfunction

/*	OK
//	NewRawSkillToBaseSkill( rawvalue )
//	return Base skill in tenths
//	FROM RAWSKILL stored in server TO skill in tenths
*/
function NewRawSkillToBaseSkill( rawvalue )
	//return RawSkillToBaseSkill(rawvalue);
	var scale := Cint(Log10(rawvalue)/Log10(2));
	if(scale <= 11)
		return Cint(rawvalue / 20.48);
	else
		return Cint((rawvalue / Pow(2,scale)+ scale - 11)* 100);
	endif
endfunction

/*	OK
//	GetNeededRawPointsToIncrease(int basevalue)
//	return raw necessari per il prossimo incremento di 0.1
//	ricordare che basevalue adesso sono già i raw, utilizzare value e moltiplicare per 10
*/
function GetNeededRawPointsToIncrease(basevalue)
	if(basevalue < 200)
		return 20.48;
	elseif(basevalue >= 2800)
		return Cdbl(0x7fffffff);
	else
		return Cdbl(Pow(2.0, Cint(basevalue/100))* 10.24);
	endif
endfunction
//end region

//region get/set skills
/*
//	SetSkill(byref who, int skillid, points)
//	sets skill value from int, points in tenths
*/
function SetSkill(byref who, skillid, points )
	SetSkillFromString(who, GetAttributeIdBySkillId(skillid), points );
endfunction

/*
//	SetSkillFromString(byref who, string attributeid)
//	sets skill value from string
*/
function SetSkillFromString(byref who, attributeid, points )
	var tenths := Cint(points*10);
	var basetoraw := NewBaseSkillToRawSkill(tenths);

	SetRawSkillFromString(who, attributeid, basetoraw );
endfunction

/*	OK
//	SetRawSkill(byref who, int skillid, nrawskill )
//	inserisce nella skill (baseserver) i rawpoints (invece dei tenths)
*/
function SetRawSkill(byref who, skillid, nrawskill )
	SetRawSkillFromString(who, GetAttributeIdBySkillId(skillid), nrawskill); 
endfunction

/*	OK
//	SetRawSkillFromString(byref who, string attributeid, nrawskill )
//	inserisce nella skill (baseserver) i rawpoints (invece dei tenths)
*/
function SetRawSkillFromString(byref who, attributeid, nrawskill )
	if(nrawskill < CORE_SETTINGS_MIN_ATTRIBUTE)
		nrawskill := CORE_SETTINGS_MIN_ATTRIBUTE;
	elseif(nrawskill > CORE_SETTINGS_MAX_ATTRIBUTE)
		nrawskill := CORE_SETTINGS_MAX_ATTRIBUTE;
	endif
	
	SetAttributeBaseValue(who, attributeid, cint(nrawskill));
endfunction

/*
//	SetSkillLock(byref who, int skillid, lockstate)
//	sets skill lock value from int; lockstate 0..1..2
*/
function SetSkillLock(byref who, skillid, lockstate )
	SetSkillLockFromString( who, GetAttributeIdBySkillId(skillid), lockstate );
endfunction

/*
//	SetSkillLockFromString(byref who, attributeid, lockstate)
//	sets skill lock value from int; lockstate 0..1..2
*/
function SetSkillLockFromString(byref who, attributeid, lockstate )
	SetAttributeLock( who, attributeid, lockstate );
endfunction

/*
//	SetSkillMod(byref who, int skillid, points)
//	sets skill mod value from int; points are in tenths
*/
function SetSkillMod(byref who, skillid, points )
	SetSkillModFromString( who, GetAttributeIdBySkillId(skillid), points );
endfunction

/*
//	SetSkillModFromString(byref who, string attributeid)
//	sets skill mod value from string, points are in tenths
*/
function SetSkillModFromString(byref who, attributeid, points )
	var tenths := Cint(points);
	if (tenths > 30000)
		tenths := 30000;
	elseif (tenths < -30000)
		tenths := -30000;
	endif

	SetAttributeTemporaryMod(who, attributeid, tenths );
	//SetRawSkillFromString(who, attributeid, NewBaseSkillToRawSkill(points*10) );//Cint(NewRawSkillToBaseSkill( GetRawSkillFromString(who, attributeid ) )/10);
endfunction

/*	deprecato
//	SetAttributeRawSkill(byref who, skillid, nrawskill )
//
*/
/*function SetAttributeRawSkill(byref who,skillid,rawskill)
	var newskills:=GetAttributeSkill(who,skillid);
	var oldone:=newskills;
	newskills:=rawskill;
	var newbase:=0;
	if (oldone<rawskill)
		newbase:=cdbl(NewRawSkillToBaseSkill(rawskill)-NewRawSkillToBaseSkill(oldone))/10;
		if (newbase<>0)
			SendSysMessage(who,"Your Skill in " + GetSkillbyNumber(skillid) + " is increased of " + newbase +"%. Now is " + cdbl(NewRawSkillToBaseSkill(rawskill))/10,0x03,68);	
		endif
	endif
	if (oldone>rawskill)
		newbase:=cdbl(NewRawSkillToBaseSkill(oldone)-NewRawSkillToBaseSkill(rawskill))/10;
		if (newbase<>0)
			SendSysMessage(who,"Your Skill in " + GetSkillbyNumber(skillid) + " is decreased of " + newbase +"%. Now is " + cdbl(NewRawSkillToBaseSkill(rawskill))/10,0x03,68);	
		endif
	endif
	SetObjProperty(who,GetSkillByNumber(skillid),newskills);
endfunction
*/

/*	deprecato
//	GetAttributeSkill(byref who,skillid)
//	return raw skill value nelle props del pg
*/
/*function GetAttributeSkill(byref who,skillid)
	var newskills:=GetObjProperty(who,GetSkillByNumber(skillid));
	if (cint(newskills)<=0)
		return 0;
	else
		return newskills;
	endif
endfunction
*/

/*	OK
//	GetSkill(who, int skillid)
//	return skill value from skill number
*/
function GetSkill(byref who,skillid)
	return GetSkillFromString(who, GetAttributeIdBySkillId(skillid));
endfunction

/*	OK
//	GetSkillFromString(byref who, string attributeid)
//	return (double) skill value from string
*/
function GetSkillFromString(byref who, attributeid)
	return Cdbl(NewRawSkillToBaseSkill( GetRawSkillFromString(who, attributeid ) )/10.0);
endfunction

/*	OK
//	GetBaseSkill(byref who, int skillid)
//	return baseskill (in decimi) value from int
*/
function GetBaseSkill(byref who, skillid)
	return GetBaseSkillFromString(who, GetAttributeIdBySkillId(skillid));
endfunction

/*	TODO: check bug: endPoison.ecl,plyrdeath.ecl, Call to function Log10 Parameter 0: Expected Integer or Real, got datatype Error
//	GetBaseSkillFromString(byref who, string attributeid)
//	return baseskill (in decimi) value from string
*/
function GetBaseSkillFromString(byref who, attributeid)
	return Cint(NewRawSkillToBaseSkill( GetRawSkillFromString(who, attributeid ) ));
endfunction

/*
//	GetSkillMod(byref who, int skillid)
//	return skill value from int
*/
function GetSkillMod(byref who, skillid)
	return GetSkillModFromString( who, GetAttributeIdBySkillId(skillid) );
endfunction

/*	TODO: return skill MOD
//	GetSkillModFromString(byref who, string attributeid)
//	return skill value from string
*/
function GetSkillModFromString(byref who, attributeid)
	return GetAttributeTemporaryMod(who, attributeid);
	//return 0;
endfunction

/*	OK
//	GetRawSkill(byref who,skillid)
//	return raw skill value stored in BASE from skill number
*/	
function GetRawSkill(byref who,skillid)
	//var ret := GetAttributeIdBySkillId(skillid);
	//if (ret.errortext)
	//	SendSysmessage(who,"GetRawSkill: errore nello skillid "+skillid);
	//endif
	return GetRawSkillFromString(who,GetAttributeIdBySkillId(skillid));
endfunction

/*	OK
//	GetRawSkillFromString(byref who,attributeid)
//	return raw skill value stored in BASE from string
*/
function GetRawSkillFromString(byref who,attributeid)
	//if (attributeid.errortext)
	//	SendSysmessage(who,"GetRawSkillFromString: "+attributeid.errortext );
	//endif
	return GetAttributeBaseValue(who, attributeid);
endfunction

/*
//	GetSkillLock(byref who, skillid)
//	return skill lock from int
*/
function GetSkillLock(byref who,skillid)
	return GetSkillLockFromString(who,GetAttributeIdBySkillId(skillid));
endfunction

/*
//	GetSkillLockFromString(byref who,attributeid)
//	return skill lock from string
*/
function GetSkillLockFromString(byref who,attributeid)
	return GetAttributeLock(who, attributeid);
endfunction

//end region


//region crescita skills
function LockAwardRawPointsFromString(who,attributeid,points,SkillLock:=error)
	
	if (!SkillLock)
		SkillLock:=GetSkillLockFromString(who, attributeid);
	endif
	
	if (SkillLock==0)
		var gs:=cint(GetSkillFromString(who,attributeid));
		if (gs<100)
			if (points>0)
				AwardRawPointsFromString(who, attributeid, points);//NewAwardRawPoints( who, skillid, points);
				CheckCap (who,GetSkillIdByAttributeId(attributeid));//,SkillLNK);
			endif
		else
			AdvanceStats(who,GetSkillIdByAttributeId(attributeid));
		endif
		return 1;
	else
		CheckStatCap(who);
		return 0;
	endif
endfunction

function LockAwardRawPoints(who,skillid,points,SkillLNK:=error)
	
	if (!SkillLNK)
		SkillLNK:=loadSkillLnk(who);
	endif
	
	if (cint(SkillLnk[skillid+1])==0)
	
		var gs:=cint(GetSkill(who,skillid));
		if (gs<100)
			if (points>0)
				NewAwardRawPoints( who, skillid, points);
				CheckCap (who,skillid);//,SkillLNK);
			endif
		else
			AdvanceStats(who,skillid);
		endif
		return 1;
	else
		CheckStatCap(who);
		return 0;
	endif
endfunction

function NewAwardRawPoints(byref who, skillid, points)
	// se si vuole controllare anche i lucchetti durante l'avanzamento dei rawpoints si DEVE usare
	// la funzione LockAwardRawPoints...
	// in generale cmq e' sempre MEGLIO NON USARE questa funzione al di fuori dello skillsinc.inc :-) 
	// Nota di Magius(CHE).
	//if (skillid>=SKILLID__NEWEST)
	//	//to do
	//	var new:=GetRawSkill(who,skillid)+points;
	//	SetRawSkill(who,skillid,new);
	//else
		AwardRawPoints(who, skillid, points);
	//endif	
endfunction

/*
//	AwardRawPoints(who, int skillid, rawpoints)
//	aumenta la skill (int) di tot rawpoints
*/
function AwardRawPoints(who, skillid, rawpoints)
  return AwardRawPointsFromString(who, GetAttributeIdBySkillId(skillid), rawpoints);//, skillid);
endfunction

/*
//	AwardRawPointsFromString(who, string attributeid, rawpoints)//, skillid)
//	aumenta la skill (string) di tot rawpoints
*/
function AwardRawPointsFromString(who, attributeid, rawpoints)//, skillid)

	if(!who.isA(POLCLASS_MOBILE))
		var ret := error;
		ret.+errortext := "Invalid parameter type: " + who + " isn't a mobileref.";
		return ret;
	endif

	rawpoints := Cdbl(rawpoints);

	if(rawpoints == error)
		return rawpoints;
	elseif(rawpoints == 0.0)
		return 1;
	endif

	set_critical(1);
	/*
	var initial := GetRawSkillFromString(who, attributeid);
	var current := initial;

	if(rawpoints > 0)
		var to_increase := GetNeededRawPointsToIncrease(current);
		while(rawpoints >= to_increase)
			rawpoints  := rawpoints - to_increase;
			current    := current + 1;
			to_increase := GetNeededRawPointsToIncrease(current);
		endwhile

		if(RandomInt(Cint(to_increase)) < rawpoints)
			current := current + 1;
		endif
	elseif(rawpoints < 0)
		var to_decrease  := GetNeededRawPointsToIncrease(current-1);
		while(rawpoints >= to_decrease && current > 0)
			rawpoints  := rawpoints - to_decrease;
			current    := current - 1;
			to_decrease  := GetNeededRawPointsToIncrease(current-1);
		endwhile

		if(RandomInt(Cint(to_decrease))< rawpoints)
			current := current - 1;
		endif
	endif

	if(current != initial)
		if(who.acctname)
			if(current > CORE_SETTINGS_MAX_ATTRIBUTE)
				current := CORE_SETTINGS_MAX_ATTRIBUTE;
			endif

			var pid := getprocess(GetObjProperty(who, "cap_pid"));
			var k := struct;
			if((attributeid == ATTRIBUTEID_STRENGTH) or (attributeid == ATTRIBUTEID_DEXTERITY) or (attributeid == ATTRIBUTEID_INTELLIGENCE))
				k.+skid   := attributeid;
				k.+skil   := skillid;
				k.+type   := EVID_STATCAP;
				k.+base   := current;
				pid.sendevent(k);
			else
				k.+skid   := attributeid;
				k.+type   := EVID_SKILLCAP;
				k.+base   := current;
				pid.sendevent(k);
			endif
		else
			SetAttributeBaseValue(who, attributeid, current);
		endif

		DoRecalcVitals(who);
	endif
	*/

	var initial := GetRawSkillFromString(who, attributeid);
	var current := initial + rawpoints;

	if(current > CORE_SETTINGS_MAX_ATTRIBUTE)//1024800? ora 1049600
		current := CORE_SETTINGS_MAX_ATTRIBUTE;
	endif
	if (who.acctname)
		var noraw := GetObjProperty( who, "AzerothProps" );//la seconda opzione è raw
		if (noraw == error || noraw[2] == 1)
			SendSysMessage(who,"Your Skill in " + GetSkillbyNumber(Cint(GetSkillIdByAttributeId(attributeid))) + " gained " + rawpoints + " rawpoints.",0x03,68);
		endif
	endif

	SetRawSkillFromString(who, attributeid, current);

	set_critical(0);
	return 1;
endfunction
//end region

function CreateSkillGroup()
	return {"Miscellaneous","Lore & Knowledge","Actions","Combat","Mystical"};
endfunction

/*	deprecato
//	GetSkillCFGbySkill(int skillid)
//	return file di config.
*/
/*function GetSkillCFGbySkill(skillid)
	if (skillid>=SKILLID__NEWEST)
		return skills_new_cfg;
	else
		return skills_cfg;
	endif	
endfunction
*/

//region skill names to skill id and viceversa
//deprecato retSkillName(skillid) -> Getskillbynumber(skillid)

/*
//	GetSkillbyName(string skillname)
//	return nome della skill dal suo numero
*/
function GetSkillbyName(skillname)
	var i;

	for (i:=0; i<=SKILLID__HIGHEST;i+=1)
		//if (i>=SKILLID__NEWEST)
		//	if (upper(skills_new_cfg[i].name)=upper(skillname))
		//		return i;
		//	endif
		//else
			if (upper(skills_cfg[i].name)==upper(skillname))
				return i;
			endif
		//endif
		
	endfor	
	return -1;
endfunction

/*
//	GetSkillByNumber(int skillid)
//	return stringa maiuscola col nome della skill
*/
function GetSkillByNumber(skillid)
	return skills_cfg[skillid].name;
endfunction

/*
//	GetAttributeIdBySkillId(int skillid)
//	return stringa col nome della skill senza spazi
*/
function GetAttributeIdBySkillId(skillid)
	case(skillid)
		SKILLID_ALCHEMY:	return ATTRIBUTEID_ALCHEMY;
		SKILLID_ANATOMY:	return ATTRIBUTEID_ANATOMY;
		SKILLID_ANIMALLORE:	return ATTRIBUTEID_ANIMALLORE;
		SKILLID_ITEMID:		return ATTRIBUTEID_ITEMID;
		SKILLID_ARMSLORE:	return ATTRIBUTEID_ARMSLORE;
		SKILLID_PARRY:		return ATTRIBUTEID_PARRY;
		SKILLID_BEGGING:	return ATTRIBUTEID_BEGGING;
		SKILLID_BLACKSMITHY:	return ATTRIBUTEID_BLACKSMITHY;
		SKILLID_BOWCRAFT:	return ATTRIBUTEID_BOWCRAFT;
		SKILLID_PEACEMAKING:	return ATTRIBUTEID_PEACEMAKING;
		SKILLID_CAMPING:	return ATTRIBUTEID_CAMPING;
		SKILLID_CARPENTRY:	return ATTRIBUTEID_CARPENTRY;
		SKILLID_CARTOGRAPHY:	return ATTRIBUTEID_CARTOGRAPHY;
		SKILLID_COOKING:	return ATTRIBUTEID_COOKING;
		SKILLID_DETECTINGHIDDEN:	return ATTRIBUTEID_DETECTINGHIDDEN;
		SKILLID_DISCORDANCE:	return ATTRIBUTEID_DISCORDANCE;
		SKILLID_EVALINT:	return ATTRIBUTEID_EVALINT;
		SKILLID_HEALING:	return ATTRIBUTEID_HEALING;
		SKILLID_FISHING:	return ATTRIBUTEID_FISHING;
		SKILLID_FORENSICS:	return ATTRIBUTEID_FORENSICS;
		SKILLID_HERDING:	return ATTRIBUTEID_HERDING;
		SKILLID_HIDING:		return ATTRIBUTEID_HIDING;
		SKILLID_PROVOCATION:	return ATTRIBUTEID_PROVOCATION;
		SKILLID_INSCRIPTION:	return ATTRIBUTEID_INSCRIPTION;
		SKILLID_LOCKPICKING:	return ATTRIBUTEID_LOCKPICKING;
		SKILLID_MAGERY:		return ATTRIBUTEID_MAGERY;
		SKILLID_MAGICRESISTANCE:	return ATTRIBUTEID_MAGICRESISTANCE;
		SKILLID_TACTICS:	return ATTRIBUTEID_TACTICS;
		SKILLID_SNOOPING:	return ATTRIBUTEID_SNOOPING;
		SKILLID_MUSICIANSHIP:	return ATTRIBUTEID_MUSICIANSHIP;
		SKILLID_POISONING:	return ATTRIBUTEID_POISONING;
		SKILLID_ARCHERY:	return ATTRIBUTEID_ARCHERY;
		SKILLID_SPIRITSPEAK:	return ATTRIBUTEID_SPIRITSPEAK;
		SKILLID_STEALING:	return ATTRIBUTEID_STEALING;
		SKILLID_TAILORING:	return ATTRIBUTEID_TAILORING;
		SKILLID_TAMING:		return ATTRIBUTEID_TAMING;
		SKILLID_TASTEID:	return ATTRIBUTEID_TASTEID;
		SKILLID_TINKERING:	return ATTRIBUTEID_TINKERING;
		SKILLID_TRACKING:	return ATTRIBUTEID_TRACKING;
		SKILLID_VETERINARY:	return ATTRIBUTEID_VETERINARY;
		SKILLID_SWORDSMANSHIP:	return ATTRIBUTEID_SWORDSMANSHIP;
		SKILLID_MACEFIGHTING:	return ATTRIBUTEID_MACEFIGHTING;
		SKILLID_FENCING:	return ATTRIBUTEID_FENCING;
		SKILLID_WRESTLING:	return ATTRIBUTEID_WRESTLING;
		SKILLID_LUMBERJACKING:	return ATTRIBUTEID_LUMBERJACKING;
		SKILLID_MINING:		return ATTRIBUTEID_MINING;
		SKILLID_MEDITATION:	return ATTRIBUTEID_MEDITATION;
		SKILLID_STEALTH:	return ATTRIBUTEID_STEALTH;
		SKILLID_REMOVETRAP:	return ATTRIBUTEID_REMOVETRAP;
		SKILLID_FAITH:		return ATTRIBUTEID_FAITH;
		SKILLID_CHIVARLY:	return ATTRIBUTEID_CHIVARLY;
		SKILLID_NECROMANCY:	return ATTRIBUTEID_NECROMANCY;
		SKILLID_THROWING:	return ATTRIBUTEID_THROWING;
	endcase

	var ret        := error;
	ret.+errortext := "No equivalent attributeid found for skillid " + skillid;

	return ret;
endfunction

function GetSkillIdByAttributeId(attributeid)
  case(Lower(attributeid))
    ATTRIBUTEID_ALCHEMY:    return SKILLID_ALCHEMY;
    ATTRIBUTEID_ANATOMY:    return SKILLID_ANATOMY;
    ATTRIBUTEID_ANIMALLORE:    return SKILLID_ANIMALLORE;
    ATTRIBUTEID_ITEMID:    return SKILLID_ITEMID;
    ATTRIBUTEID_ARMSLORE:    return SKILLID_ARMSLORE;
    ATTRIBUTEID_PARRY:    return SKILLID_PARRY;
    ATTRIBUTEID_BEGGING:    return SKILLID_BEGGING;
    ATTRIBUTEID_BLACKSMITHY:  return SKILLID_BLACKSMITHY;
    ATTRIBUTEID_BOWCRAFT:    return SKILLID_BOWCRAFT;
    ATTRIBUTEID_PEACEMAKING:  return SKILLID_PEACEMAKING;
    ATTRIBUTEID_CAMPING:    return SKILLID_CAMPING;
    ATTRIBUTEID_CARPENTRY:    return SKILLID_CARPENTRY;
    ATTRIBUTEID_CARTOGRAPHY:  return SKILLID_CARTOGRAPHY;
    ATTRIBUTEID_COOKING:    return SKILLID_COOKING;
    ATTRIBUTEID_DETECTINGHIDDEN:
    ATTRIBUTEID_DETECTINGHIDDENALIAS:
      return SKILLID_DETECTINGHIDDEN;
    ATTRIBUTEID_DISCORDANCE:    return SKILLID_DISCORDANCE;
    ATTRIBUTEID_EVALINT:
    ATTRIBUTEID_EVALINTALIAS1:
    ATTRIBUTEID_EVALINTALIAS2:
      return SKILLID_EVALINT;
    ATTRIBUTEID_HEALING:    return SKILLID_HEALING;
    ATTRIBUTEID_FISHING:    return SKILLID_FISHING;
    ATTRIBUTEID_FORENSICS:
    ATTRIBUTEID_FORENSICSALIAS1:
    ATTRIBUTEID_FORENSICSALIAS2:
      return SKILLID_FORENSICS;
    ATTRIBUTEID_HERDING:    return SKILLID_HERDING;
    ATTRIBUTEID_HIDING:    return SKILLID_HIDING;
    ATTRIBUTEID_PROVOCATION:  return SKILLID_PROVOCATION;
    ATTRIBUTEID_INSCRIPTION:  return SKILLID_INSCRIPTION;
    ATTRIBUTEID_LOCKPICKING:  return SKILLID_LOCKPICKING;
    ATTRIBUTEID_MAGERY:    return SKILLID_MAGERY;
    ATTRIBUTEID_MAGICRESISTANCE:
    ATTRIBUTEID_MAGICRESISTANCEALIAS1:
    ATTRIBUTEID_MAGICRESISTANCEALIAS2:
    ATTRIBUTEID_MAGICRESISTANCEALIAS3:
      return SKILLID_MAGICRESISTANCE;
    ATTRIBUTEID_TACTICS:    return SKILLID_TACTICS;
    ATTRIBUTEID_SNOOPING:    return SKILLID_SNOOPING;
    ATTRIBUTEID_MUSICIANSHIP:  return SKILLID_MUSICIANSHIP;
    ATTRIBUTEID_POISONING:    return SKILLID_POISONING;
    ATTRIBUTEID_ARCHERY:    return SKILLID_ARCHERY;
    ATTRIBUTEID_SPIRITSPEAK:  return SKILLID_SPIRITSPEAK;
    ATTRIBUTEID_STEALING:    return SKILLID_STEALING;
    ATTRIBUTEID_TAILORING:    return SKILLID_TAILORING;
    ATTRIBUTEID_TAMING:    return SKILLID_TAMING;
    ATTRIBUTEID_TASTEID:    return SKILLID_TASTEID;
    ATTRIBUTEID_TINKERING:    return SKILLID_TINKERING;
    ATTRIBUTEID_TRACKING:    return SKILLID_TRACKING;
    ATTRIBUTEID_VETERINARY:    return SKILLID_VETERINARY;
    ATTRIBUTEID_SWORDSMANSHIP:
    ATTRIBUTEID_SWORDSMANSHIPALIAS:
      return SKILLID_SWORDSMANSHIP;
    ATTRIBUTEID_MACEFIGHTING:
    ATTRIBUTEID_MACEFIGHTINGALIAS:
      return SKILLID_MACEFIGHTING;
    ATTRIBUTEID_FENCING:    return SKILLID_FENCING;
    ATTRIBUTEID_WRESTLING:    return SKILLID_WRESTLING;
    ATTRIBUTEID_LUMBERJACKING:  return SKILLID_LUMBERJACKING;
    ATTRIBUTEID_MINING:    return SKILLID_MINING;
    ATTRIBUTEID_MEDITATION:    return SKILLID_MEDITATION;
    ATTRIBUTEID_STEALTH:    return SKILLID_STEALTH;
    ATTRIBUTEID_REMOVETRAP:    return SKILLID_REMOVETRAP;
		ATTRIBUTEID_FAITH:	return SKILLID_FAITH;
		ATTRIBUTEID_CHIVARLY:	return SKILLID_CHIVARLY;
		ATTRIBUTEID_NECROMANCY:	return SKILLID_NECROMANCY;
		ATTRIBUTEID_THROWING:	return SKILLID_THROWING;
  endcase
  var ret        := error;
  ret.+errortext := "No equivalent skillid found for attributeid " + attributeid;
  return ret;
endfunction

function GetAttributeIds()
	return { ATTRIBUTEID_ALCHEMY,
		ATTRIBUTEID_ANATOMY,
		ATTRIBUTEID_ANIMALLORE,
		ATTRIBUTEID_ARCHERY,
		ATTRIBUTEID_ARMSLORE,
		ATTRIBUTEID_BEGGING,
		ATTRIBUTEID_BLACKSMITHY,
		ATTRIBUTEID_BOWCRAFT,
		ATTRIBUTEID_CAMPING,
		ATTRIBUTEID_CARPENTRY,
		ATTRIBUTEID_CARTOGRAPHY,
		ATTRIBUTEID_COOKING,
		ATTRIBUTEID_DETECTINGHIDDEN,
		ATTRIBUTEID_DEXTERITY,
		ATTRIBUTEID_DISCORDANCE,
		ATTRIBUTEID_EVALINT,
		ATTRIBUTEID_FENCING,
		ATTRIBUTEID_FISHING,
		ATTRIBUTEID_FORENSICS,
		ATTRIBUTEID_HEALING,
		ATTRIBUTEID_HERDING,
		ATTRIBUTEID_HIDING,
		ATTRIBUTEID_INSCRIPTION,
		ATTRIBUTEID_INTELLIGENCE,
		ATTRIBUTEID_ITEMID,
		ATTRIBUTEID_LOCKPICKING,
		ATTRIBUTEID_LUMBERJACKING,
		ATTRIBUTEID_MACEFIGHTING,
		ATTRIBUTEID_MAGERY,
		ATTRIBUTEID_MAGICRESISTANCE,
		ATTRIBUTEID_MEDITATION,
		ATTRIBUTEID_MINING,
		ATTRIBUTEID_MUSICIANSHIP,
		ATTRIBUTEID_PARRY,
		ATTRIBUTEID_PEACEMAKING,
		ATTRIBUTEID_POISONING,
		ATTRIBUTEID_PROVOCATION,
		ATTRIBUTEID_REMOVETRAP,
		ATTRIBUTEID_SNOOPING,
		ATTRIBUTEID_SPIRITSPEAK,
		ATTRIBUTEID_STEALING,
		ATTRIBUTEID_STEALTH,
		ATTRIBUTEID_STRENGTH,
		ATTRIBUTEID_SWORDSMANSHIP,
		ATTRIBUTEID_TACTICS,
		ATTRIBUTEID_TAILORING,
		ATTRIBUTEID_TAMING,
		ATTRIBUTEID_TASTEID,
		ATTRIBUTEID_TINKERING,
		ATTRIBUTEID_TRACKING,
		ATTRIBUTEID_VETERINARY,
		ATTRIBUTEID_WRESTLING };
endfunction

function GetAttributeIdsWithoutStatIds()
  return { ATTRIBUTEID_ALCHEMY,
     ATTRIBUTEID_ANATOMY,
     ATTRIBUTEID_ANIMALLORE,
     ATTRIBUTEID_ARCHERY,
     ATTRIBUTEID_ARMSLORE,
     ATTRIBUTEID_BEGGING,
     ATTRIBUTEID_BLACKSMITHY,
     ATTRIBUTEID_BOWCRAFT,
     ATTRIBUTEID_CAMPING,
     ATTRIBUTEID_CARPENTRY,
     ATTRIBUTEID_CARTOGRAPHY,
     ATTRIBUTEID_COOKING,
     ATTRIBUTEID_DETECTINGHIDDEN,
     ATTRIBUTEID_DISCORDANCE,
     ATTRIBUTEID_EVALINT,
     ATTRIBUTEID_FENCING,
     ATTRIBUTEID_FISHING,
     ATTRIBUTEID_FORENSICS,
     ATTRIBUTEID_HEALING,
     ATTRIBUTEID_HERDING,
     ATTRIBUTEID_HIDING,
     ATTRIBUTEID_INSCRIPTION,
     ATTRIBUTEID_ITEMID,
     ATTRIBUTEID_LOCKPICKING,
     ATTRIBUTEID_LUMBERJACKING,
     ATTRIBUTEID_MACEFIGHTING,
     ATTRIBUTEID_MAGERY,
     ATTRIBUTEID_MAGICRESISTANCE,
     ATTRIBUTEID_MEDITATION,
     ATTRIBUTEID_MINING,
     ATTRIBUTEID_MUSICIANSHIP,
     ATTRIBUTEID_PARRY,
     ATTRIBUTEID_PEACEMAKING,
     ATTRIBUTEID_POISONING,
     ATTRIBUTEID_PROVOCATION,
     ATTRIBUTEID_REMOVETRAP,
     ATTRIBUTEID_SNOOPING,
     ATTRIBUTEID_SPIRITSPEAK,
     ATTRIBUTEID_STEALING,
     ATTRIBUTEID_STEALTH,
     ATTRIBUTEID_SWORDSMANSHIP,
     ATTRIBUTEID_TACTICS,
     ATTRIBUTEID_TAILORING,
     ATTRIBUTEID_TAMING,
     ATTRIBUTEID_TASTEID,
     ATTRIBUTEID_TINKERING,
     ATTRIBUTEID_TRACKING,
     ATTRIBUTEID_VETERINARY,
     ATTRIBUTEID_WRESTLING };
endfunction

function GetSkillIds()
  return { SKILLID_ALCHEMY,
     SKILLID_ANATOMY,
     SKILLID_ANIMALLORE,
     SKILLID_ITEMID,
     SKILLID_ARMSLORE,
     SKILLID_PARRY,
     SKILLID_BEGGING,
     SKILLID_BLACKSMITHY,
     SKILLID_BOWCRAFT,
     SKILLID_PEACEMAKING,
     SKILLID_CAMPING,
     SKILLID_CARPENTRY,
     SKILLID_CARTOGRAPHY,
     SKILLID_COOKING,
     SKILLID_DETECTINGHIDDEN,
     SKILLID_DISCORDANCE,
     SKILLID_EVALINT,
     SKILLID_HEALING,
     SKILLID_FISHING,
     SKILLID_FORENSICS,
     SKILLID_HERDING,
     SKILLID_HIDING,
     SKILLID_PROVOCATION,
     SKILLID_INSCRIPTION,
     SKILLID_LOCKPICKING,
     SKILLID_MAGERY,
     SKILLID_MAGICRESISTANCE,
     SKILLID_TACTICS,
     SKILLID_SNOOPING,
     SKILLID_MUSICIANSHIP,
     SKILLID_POISONING,
     SKILLID_ARCHERY,
     SKILLID_SPIRITSPEAK,
     SKILLID_STEALING,
     SKILLID_TAILORING,
     SKILLID_TAMING,
     SKILLID_TASTEID,
     SKILLID_TINKERING,
     SKILLID_TRACKING,
     SKILLID_VETERINARY,
     SKILLID_SWORDSMANSHIP,
     SKILLID_MACEFIGHTING,
     SKILLID_FENCING,
     SKILLID_WRESTLING,
     SKILLID_LUMBERJACKING,
     SKILLID_MINING,
     SKILLID_MEDITATION,
     SKILLID_STEALTH,
     SKILLID_REMOVETRAP };
endfunction
//end region


function CreateSkillArray()
	var arr:={};
	
	//Miscellaneous
	arr.append(22);
	arr.append(ATTRIBUTEID_ALCHEMY);
	arr.append(ATTRIBUTEID_BLACKSMITHY);
	arr.append(ATTRIBUTEID_BOWCRAFT);
	arr.append(ATTRIBUTEID_CARPENTRY);
	arr.append(ATTRIBUTEID_COOKING);
	arr.append(ATTRIBUTEID_FISHING);
	arr.append(ATTRIBUTEID_HEALING);
	arr.append(ATTRIBUTEID_HERDING);
	arr.append(ATTRIBUTEID_LOCKPICKING);
	arr.append(ATTRIBUTEID_LUMBERJACKING);
	arr.append(ATTRIBUTEID_MAGERY);
	arr.append(ATTRIBUTEID_MEDITATION);
	arr.append(ATTRIBUTEID_MINING);
	arr.append(ATTRIBUTEID_MUSICIANSHIP);
	arr.append(ATTRIBUTEID_REMOVETRAP);
	arr.append(ATTRIBUTEID_MAGICRESISTANCE);
	arr.append(ATTRIBUTEID_SNOOPING);
	arr.append(ATTRIBUTEID_STEALING);
	arr.append(ATTRIBUTEID_STEALTH);
	arr.append(ATTRIBUTEID_TAILORING);
	arr.append(ATTRIBUTEID_TINKERING);
	arr.append(ATTRIBUTEID_VETERINARY);
	
	//Lore
	arr.append(7);
	arr.append(ATTRIBUTEID_ANATOMY);
	arr.append(ATTRIBUTEID_ANIMALLORE);
	arr.append(ATTRIBUTEID_ARMSLORE);
	arr.append(ATTRIBUTEID_EVALINT);
	arr.append(ATTRIBUTEID_FORENSICS);
	arr.append(ATTRIBUTEID_ITEMID);
	arr.append(ATTRIBUTEID_TASTEID);

	//Actions
	arr.append(12);
	arr.append(ATTRIBUTEID_TAMING);
	arr.append(ATTRIBUTEID_BEGGING);
	arr.append(ATTRIBUTEID_CAMPING);
	arr.append(ATTRIBUTEID_CARTOGRAPHY);
	arr.append(ATTRIBUTEID_DETECTINGHIDDEN);
	arr.append(ATTRIBUTEID_DISCORDANCE);
	arr.append(ATTRIBUTEID_HIDING);
	arr.append(ATTRIBUTEID_INSCRIPTION);
	arr.append(ATTRIBUTEID_PEACEMAKING);
	arr.append(ATTRIBUTEID_POISONING);
	arr.append(ATTRIBUTEID_PROVOCATION);
	arr.append(ATTRIBUTEID_TRACKING);
	//arr.append(ATTRIBUTEID_CHIVARLY);

	//Combat
	arr.append(7);
	arr.append(ATTRIBUTEID_ARCHERY);
	arr.append(ATTRIBUTEID_FENCING);
	arr.append(ATTRIBUTEID_MACEFIGHTING);
	arr.append(ATTRIBUTEID_PARRY);
	arr.append(ATTRIBUTEID_SWORDSMANSHIP);
	arr.append(ATTRIBUTEID_TACTICS);
	arr.append(ATTRIBUTEID_WRESTLING);
	//arr.append(ATTRIBUTEID_THROWING);
	//Combat
	arr.append(1);
	arr.append(ATTRIBUTEID_SPIRITSPEAK);
	//arr.append(ATTRIBUTEID_FAITH);
	//arr.append(ATTRIBUTEID_NECROMANCY);
	return arr;
endfunction