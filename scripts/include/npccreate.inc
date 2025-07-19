use cfgfile;

include ":brainAI:npcUtil";

include "include/skillsinc";

function NewCreateNPCFromTemplate(template, x, y, z, props := 0, realm := _DEFAULT_REALM)

	// Permette di modificare gli NPC creati in modo da fargli superare la soglia dei 210 nelle Stat.

	var result :=CreateNPCFromTemplate( template, cint(x), cint(y), cint(z), props, realm );
	if (!result)
		// la gestione dell'errore è rimandata al chiamante
	else
		//var npcdesc := ReadConfigFile( "::npcdesc" );
		var elem := NPC_GetNPCConfig( template );//FindConfigElem( npcdesc, template );
		var s := cint(GetConfigInt( elem , "Strength" ));
		var i := cint(GetConfigInt( elem , "Intelligence" ));
		var d := cint(GetConfigInt( elem , "Dexterity" ));
		var a := cint(GetConfigInt( elem , "AR" ));
		var obty := cint(GetConfigInt( elem , "ObjType" ));

		//print("CREATO = " + result.name);
		//print("VALORI: " + GetStrength(result) + " , " + GetIntelligence(result) + " , " + GetDexterity(result));
		//print("VALORI2: " + s + " , " + i + " , " + d + " , " + a);

		SetStrengthMod(result,s-GetStrength(result));
		SetIntelligenceMod(result,i-GetIntelligence(result));
		SetDexterityMod(result,d-GetDexterity(result));
		SetHp(result, GetStrength(result));
		SetMana(result, GetIntelligence(result));
		SetStamina(result, GetDexterity(result));

		if (a > result.ar)
			result.ar_mod := a - result.ar;
		endif

		//FIX delle skill!:
	/*
	ResistingSpells	80
	Tactics		100
	Archery
	Fencing		85
	Macefighting	
	Wrestling	130
	Swordsmanship	80
	Parry		55
	*/
		var t;
		var oldvalue;
		for (t:=0; t<=SKILLID__HIGHEST; t+=1)
			oldvalue:= GetRawSkill(result,t);

			if (oldvalue>0)
				var btrs:=NewBaseSkillToRawSkill(oldvalue);
				SetRawSkillFromString(result,GetAttributeIdBySkillId(t),btrs); 
			endif
			sleepms(2);
		endfor

		/*
		print("CREATO = " + result.name);
		print("RAW = " + NewBaseSkillToRawSkill( 8000 ) );
		print("VALORI: " + result.strength + " , " + result.intelligence + " , " + result.dexterity);
		NewSetRawSkill( result , 43 , NewBaseSkillToRawSkill( 8000 ) );
		result.hp:=500;
		print("NUOVI: " + result.strength + " , " + result.intelligence + " , " + result.dexterity);
		print("RAW= " + NewGetSkill( result, 43));
		*/

		//controllo backpack
		if ( obty == cint (0x191) || obty == cint(0x190) )
			SetupBackPack( result );
		endif
		//x lo shove
		GrantPrivilege( result, "freemove" );
		result.Enable( "freemove" );
	endif

	return result;
endfunction

function SetupBackPack( mobile )
	if( !mobile.backpack )
		var backpack := CreateItemAtLocation( mobile.x, mobile.y, mobile.z, "backpack", 1, mobile.realm );
		var result := EquipItem( mobile, backpack );
		if( !result )
			DestroyItem( backpack );
			return result;
		else
			return backpack;
		endif
	endif

	return mobile.backpack;
endfunction