
//include "BaseXmlSpawner";
include "xmlspawner2a";

include "xmlpropsgumps/xmlpropsgump";

var nclicks := 0;
const MaxSpawnEntries := 60;
const MaxEntriesPerPage := 15;
var m_ShowGump := 0;
var AllowGumpUpdate := 1;//bool
var xoffset := 0;
var initial_maxcount;
var page;

function XmlSpawnerGump(byref who,byref spawner_item, X, Y, extension, textextension:=0, newpage, rentry:=struct{} )
	if (!spawner_item)
		return 0;
	endif

	xoffset := textextension;
	initial_maxcount := cint(GetObjProperty(spawner_item, "Count"));
	page := newpage;
	var sequentialspawn := cint(GetObjProperty(spawner_item, "SequentialSpawning"));
	//var nclicks := 0;
		/*private static int nclicks = 0;
		public XmlSpawner m_Spawner;

		public int m_ShowGump = 0;
		public bool AllowGumpUpdate = true;
		public int xoffset = 0;
		public int initial_maxcount;
		public int page;
		public ReplacementEntry Rentry;
		public class ReplacementEntry
		{
			public string Typename;
			public int Index;
			public int Color;

			public ReplacementEntry()
			{
			}
		}
			m_Spawner = spawner;
			spawner.SpawnerGump = this;
			Rentry = rentry;
		*/

	var spawnergump := GFCreateGump(0, 0, 670 + xoffset + 30, 474);
	AddPage(spawnergump, 0);

	// automatically change the gump depending on whether sequential spawning and/or subgroups are activated
	if (sequentialspawn >= 0 || HasSubGroups(spawner_item) || HasIndividualSpawnTimes(spawner_item))
		// show the fully extended gump with subgroups and reset timer info
		m_ShowGump := 2;
	endif

	if (extension > 0)
		m_ShowGump := extension;
	endif
	if (extension < 0)
		m_ShowGump := 0;
	endif

	if (m_ShowGump > 1)
		AddBackground(spawnergump, 0, 0, 670 + xoffset + 30, 474, 5054);
		AddAlphaRegion(spawnergump, 0, 0, 670 + xoffset + 30, 474);
	else
		if (m_ShowGump > 0)
			AddBackground(spawnergump, 0, 0, 335 + xoffset, 474, 5054);
			AddAlphaRegion(spawnergump, 0, 0, 335 + xoffset, 474);
		else
			AddBackground(spawnergump, 0, 0, 305 + xoffset, 474, 5054);
			AddAlphaRegion(spawnergump, 0, 0, 305 + xoffset, 474);
		endif
	endif

	// spawner name area
	AddImageTiled(spawnergump, 3, 5, 227, 23, 0x52);
	AddImageTiled(spawnergump, 4, 6, 225, 21, 0xBBC);
	AddTextEntry(spawnergump, 6, 5, 222, 21, 0, 999, spawner_item.name); // changed from color 50

	AddButton(spawnergump, 5, 450, 0xFAE, 0xFAF, 4, 1, 0);
	AddLabel(spawnergump, 38, 450, 0x384, "Goto");

	AddButton(spawnergump, 5, 428, 0xFAE, 0xFAF, 703, 1, 0);
	AddLabel(spawnergump, 38, 428, 0x384, "Help");

	AddButton(spawnergump, 71, 428, 0xFB4, 0xFB6, 2, 1, 0);
	AddLabel(spawnergump, 104, 428, 0x384, "Bring Home");

	AddButton(spawnergump, 71, 450, 0xFA8, 0xFAA, 3, 1, 0);
	AddLabel(spawnergump, 104, 450, 0x384, "Respawn");

	// Props button
	AddButton(spawnergump, 168, 428, 0xFAB, 0xFAD, 9999, 1, 0);
	AddLabel(spawnergump, 201, 428, 0x384, "Props");

	// Sort button
	AddButton(spawnergump, 168, 450, 0xFAB, 0xFAD, 702, 1, 0);
	AddLabel(spawnergump, 201, 450, 0x384, "Sort");

	// Reset button
	AddButton(spawnergump, 71, 406, 0xFA2, 0xFA3, 701, 1, 0);
	AddLabel(spawnergump, 104, 406, 0x384, "Reset");

	// Refresh button
	AddButton(spawnergump, 168, 406, 0xFBD, 0xFBE, 9998, 1, 0);
	AddLabel(spawnergump, 201, 406, 0x384, "Refresh");

	if (cint(GetObjProperty(spawner_item, "DisableGlobalAutoReset")) == 1)
		AddButton(spawnergump, 280, 395, 0xD3, 0xD2, 9997, 1, 0);
		AddLabel(spawnergump, 263, 410, 68, "Disable");
		AddLabel(spawnergump, 247, 424, 68, "TickReset");
	else
		AddButton(spawnergump, 280, 395, 0xD2, 0xD3, 9997, 1, 0);
		AddLabel(spawnergump, 263, 410, 33, "Disable");
		AddLabel(spawnergump, 247, 424, 33, "TickReset");
	endif

	if (cint(GetObjProperty(spawner_item, "Running")) == 1)
		AddButton(spawnergump, 5, 399, 0x2A4E, 0x2A3A, 700, 1, 0);
		AddLabel(spawnergump, 38, 406, 0x384, "On");
	else
		AddButton(spawnergump, 5, 399, 0x2A62, 0x2A3A, 700, 1, 0);
		AddLabel(spawnergump, 38, 406, 0x384, "Off");
	endif
	// Add sequential spawn state
	if (sequentialspawn >= 0)
		AddLabel(spawnergump, 15, 365, 33, cstr(sequentialspawn));
	endif

	AddLabel(spawnergump, 231 + xoffset, 9, 68, "Count");
	AddLabel(spawnergump, 270 + xoffset, 9, 33, "Max");

	if (m_ShowGump > 0)
		// Add subgroup label
		AddLabel(spawnergump, 334 + xoffset, 9, 68, "Sub");
	endif
	if (m_ShowGump > 1)
		// Add entry field labels
		AddLabel(spawnergump, 303 + xoffset, 9, 68, "Per");
		AddLabel(spawnergump, 329 + xoffset + 30, 9, 68, "Reset");
		AddLabel(spawnergump, 368 + xoffset + 30, 9, 68, "To");
		AddLabel(spawnergump, 392 + xoffset + 30, 9, 68, "Kills");
		AddLabel(spawnergump, 432 + xoffset + 30, 9, 68, "MinD");
		AddLabel(spawnergump, 472 + xoffset + 30, 9, 68, "MaxD");
		AddLabel(spawnergump, 515 + xoffset + 30, 9, 68, "Rng");
		AddLabel(spawnergump, 545 + xoffset + 30, 9, 68, "RK");
		AddLabel(spawnergump, 565 + xoffset + 30, 9, 68, "Clr");
		AddLabel(spawnergump, 590 + xoffset + 30, 9, 68, "NextSpawn");
	endif

	// add area for spawner max
	AddLabel(spawnergump, 180 + xoffset, 365, 50, "Spawner");
	AddImageTiled(spawnergump, 267 + xoffset, 365, 35, 23, 0x52);
	AddImageTiled(spawnergump, 268 + xoffset, 365, 32, 21, 0xBBC);
	AddTextEntry(spawnergump, 273 + xoffset, 365, 33, 33, 33, 300, Cstr( GetObjProperty(spawner_item, "Count" )));

	// add area for spawner count
	AddImageTiled(spawnergump, 231 + xoffset, 365, 35, 23, 0x52);
	AddImageTiled(spawnergump, 232 + xoffset, 365, 32, 21, 0xBBC);
	AddLabel(spawnergump, 233 + xoffset, 365, 68, Cstr(TotalSpawnedObjects(spawner_item)));

	// add the status string
	AddTextEntry(spawnergump, 38, 384, 235, 33, 33, 900, cstr(GetObjProperty(spawner_item, "status_str") ));

	// add the page buttons
	var i;
	for ( i := 0; i < cint(MaxSpawnEntries / MaxEntriesPerPage); i+=1)
		AddButton(spawnergump, 38 + i * 25, 365, 0x8B1 + i, 0x8B1 + i, 4000 + i, 1, 0);
	endfor

	// add gump extension button
	if (m_ShowGump > 1)
		AddButton(spawnergump, 645 + xoffset + 30, 450, 0x15E3, 0x15E7, 200, 1, 0);
	else
		if (m_ShowGump > 0)
			AddButton(spawnergump, 315 + xoffset, 450, 0x15E1, 0x15E5, 200, 1, 0);
		else
			AddButton(spawnergump, 285 + xoffset, 450, 0x15E1, 0x15E5, 200, 1, 0);
		endif
	endif

	// add the textentry extender button
	if (xoffset > 0)
		AddButton(spawnergump, 160, 365, 0x15E3, 0x15E7, 201, 1, 0);
	else
		AddButton(spawnergump, 160, 365, 0x15E1, 0x15E5, 201, 1, 0);
	endif

	//i sul pol parte da 1
	var spawnobjects := GetObjProperty(spawner_item, "SpawnObjects");

	//cerco eventuali libri e carico le modifiche, cancellando i libri?
	var bookarray  := GetObjProperty(spawner_item, "textentrybook");
	//var modifica := 0;
	for ( i:= 1; i <= len(bookarray); i+= 1)
		var libro := SystemFindObjectBySerial(bookarray[i]);
		if (!libro.errortext)
			var testo := libro.GetText();
			var args := GetObjProperty(libro, "args");
			var m_Index := cint(args[2]);
			//SendSysMessage( who, "found book " +m_Index+":"+testo );
			//if (m_Index <= len(spawnobjects))
			if (len(testo) < 1)//libro nuovo
				continue;
			endif

			if ( spawnobjects[m_Index].typename != testo )
				SendSysMessage( who, "entry " +m_Index+" changed." );
				ProcessSpawnerBookEntry( who, args, testo, spawnobjects );//aggiungo il byref spawnobjects
				var result := DestroyItem(libro);
				if (!result.errortext)
					bookarray.erase(i);
				else
					SendSysMessage( who, "errore nella cancellazione del libro" );
				endif
			else
				//SendSysMessage( who, "deleting unmodified entry book" );
				if (libro.decayat <= readgameclock() )
					var result := DestroyItem(libro);
					if (!result.errortext)
						bookarray.erase(i);
					else
						SendSysMessage( who, "errore nella cancellazione del libro" );
					endif
				else
					SendSysMessage( who, "entry " +m_Index+" unmodified: modificare il libro, chiudere il gump del libro e riaprire il gump dello spawner. (1000 sec rimasti)" );
					//libro.decayat := readgameclock() + 1000;
					//libro.color := 946;
				endif
			endif
			//endif
			//modifica := 1;
		else
			//SendSysMessage( who, "book not found, erase " +i );
			bookarray.erase(i);
		endif
		sleepms(3);
	endfor
	SetObjProperty(spawner_item, "textentrybook", bookarray );
	//

	for ( i := 1; i <= MaxSpawnEntries; i+=1)
		if (page != cint((i-1) / MaxEntriesPerPage))
			continue;
		endif

		var str := "";
		var texthue := 0;
		var background := 0xBBC;

		if ((i-1) % MaxEntriesPerPage == 0)
			//AddPage(page+1);
			// add highlighted page button
			AddImageTiled(spawnergump, 35 + page * 25, 363, 25, 25, 0xBBC);
			AddImage(spawnergump, 38 + page * 25, 365, 0x8B1 + page);
		endif

		if (i <= len(spawnobjects))
			// disable button

			if (spawnobjects[i].disabled == 1)
				// change the background for the spawn text entry if disabled
				background := 0x23F4;
				AddButton(spawnergump, 2, 22 * ((i-1) % MaxEntriesPerPage) + 34, 0x82C, 0x82C, 6000 + i, 1, 0);
			else
				AddButton(spawnergump, 2, 22 * ((i-1) % MaxEntriesPerPage) + 36, 0x837, 0x837, 6000 + i, 1, 0);
			endif
		endif

		var hasreplacement := 0;

		// check for replacement entries
		if (rentry && rentry.index == i)
			hasreplacement := 1;
			str := cstr(rentry.typename);
			background := rentry.color;
			// replacement is one time only.
			rentry.Erase(1);// := null;
		endif

		// increment/decrement buttons
		AddButton(spawnergump, 15, 22 * ((i-1) % MaxEntriesPerPage) + 34, 0x15E0, 0x15E4, 6 + ((i-1) * 2), 1, 0);
		AddButton(spawnergump, 30, 22 * ((i-1) % MaxEntriesPerPage) + 34, 0x15E2, 0x15E6, 7 + ((i-1) * 2), 1, 0);

		// categorization gump button
		AddButton(spawnergump, 171 + xoffset - 18, 22 * ((i-1) % MaxEntriesPerPage) + 34, 0x15E1, 0x15E5, 5000 + i, 1, 0);

		// goto spawn button
		AddButton(spawnergump, 171 + xoffset, 22 * ((i-1) % MaxEntriesPerPage) + 30, 0xFAE, 0xFAF, 1300 + i, 1, 0);

		// text entry gump button
		AddButton(spawnergump, 200 + xoffset, 22 * ((i-1) % MaxEntriesPerPage) + 30, 0xFAB, 0xFAD, 800 + i, 1, 0);

		// background for text entry area
		AddImageTiled(spawnergump, 48, 22 * ((i-1) % MaxEntriesPerPage) + 30, 133 + xoffset - 25, 23, 0x52);
		AddImageTiled(spawnergump, 49, 22 * ((i-1) % MaxEntriesPerPage) + 31, 131 + xoffset - 25, 21, background);

		// Add page number
		//AddLabel(spawnergump,  15, 365, 33, String.Format("{0}",(int)((i-1)/MaxEntriesPerPage + 1)) );
		//AddButton(spawnergump,  38+page*25, 365, 0x8B1+i, 0x8B1+i, 0, 0, 1+i );??

		if (i <= len(spawnobjects))
			if (hasreplacement==0)
				str := cstr(spawnobjects[i].typename);
			endif

			var count := cint(len(spawnobjects[i].spawnedobjects));//count
			var maxc := cint(spawnobjects[i].maxcount);//cambio da actualmaxcount a maxcount
			var subgrp := cint(spawnobjects[i].subgroup);
			var spawnsper := cint(spawnobjects[i].spawnspertick);

			texthue := subgrp * 11;
			if (texthue < 0)
				texthue := 0;
			endif

			// Add current count
			AddImageTiled(spawnergump, 231 + xoffset, 22 * ((i-1) % MaxEntriesPerPage) + 30, 35, 23, 0x52);
			AddImageTiled(spawnergump, 232 + xoffset, 22 * ((i-1) % MaxEntriesPerPage) + 31, 32, 21, 0xBBC);
			AddLabel(spawnergump, 233 + xoffset, 22 * ((i-1) % MaxEntriesPerPage) + 30, 68, cstr(count));

			// Add maximum count
			AddImageTiled(spawnergump, 267 + xoffset, 22 * ((i-1) % MaxEntriesPerPage) + 30, 35, 23, 0x52);
			AddImageTiled(spawnergump, 268 + xoffset, 22 * ((i-1) % MaxEntriesPerPage) + 31, 32, 21, 0xBBC);
			// AddTextEntry(x,y,w,ht,color,id,str)
			AddTextEntry(spawnergump, 270 + xoffset, 22 * ((i-1) % MaxEntriesPerPage) + 30, 30, 30, 33, 500 + i, cstr(maxc));

			if (m_ShowGump > 0)
				// Add subgroup
				AddImageTiled(spawnergump, 334 + xoffset, 22 * ((i-1) % MaxEntriesPerPage) + 30, 25, 23, 0x52);
				AddImageTiled(spawnergump, 335 + xoffset, 22 * ((i-1) % MaxEntriesPerPage) + 31, 22, 21, 0xBBC);
				AddTextEntry(spawnergump, 338 + xoffset, 22 * ((i-1) % MaxEntriesPerPage) + 30, 17, 33, texthue, 600 + i, cstr(subgrp));
			endif
			if (m_ShowGump > 1)
				// Add subgroup timer fields
				var strrst := "";
				var strto := "";
				var strkill := "";
				var strmind := "";
				var strmaxd := "";
				var strnext := "";
				var strpackrange := "";
				var strspawnsper := cstr(spawnsper);

				if (cint(spawnobjects[i].sequentialresettime) > 0 && cint(spawnobjects[i].subgroup) > 0)
					strrst := cstr(spawnobjects[i].sequentialresettime);
					strto := cstr(spawnobjects[i].sequentialresetto);
				endif
				if (cint(spawnobjects[i].killsneeded) > 0)
					strkill := cstr(spawnobjects[i].killsneeded);
				endif
				if (cint(spawnobjects[i].mindelay) >= 0)
					strmind := cstr(spawnobjects[i].mindelay);
				endif
				if (cint(spawnobjects[i].maxdelay) >= 0)
					strmaxd := cstr(spawnobjects[i].maxdelay);
				endif
				if (cint(spawnobjects[i].packrange) >= 0)
					strpackrange := cstr(spawnobjects[i].packrange);
				endif

				if (spawnobjects[i].nextspawn > ReadGameClock())
					// if the next spawn tick of the spawner will occur after the subgroup is available for spawning
					// then report the next spawn tick since that is the earliest that the subgroup can actually be spawned
					if ((ReadGameClock() + NextSpawn(spawner_item) ) > spawnobjects[i].nextspawn)
						strnext := cstr(NextSpawn(spawner_item));
					else
						// estimate the earliest the next spawn could occur as the first spawn tick after reaching the subgroup nextspawn 
						strnext := cstr(spawnobjects[i].nextspawn - ReadGameClock() + NextSpawn(spawner_item));
					endif
				else
					strnext := cstr(NextSpawn(spawner_item));
				endif

				var yoff := cint(22 * ((i-1) % MaxEntriesPerPage) + 30);

				// spawns per tick
				AddImageTiled(spawnergump, 303 + xoffset, yoff, 30, 23, 0x52);
				AddImageTiled(spawnergump, 304 + xoffset, yoff + 1, 27, 21, 0xBBC);
				AddTextEntry(spawnergump, 307 + xoffset, yoff, 22, 33, texthue, 1500 + i, strspawnsper);
				// reset time
				AddImageTiled(spawnergump, 329 + xoffset + 30, yoff, 35, 23, 0x52);
				AddImageTiled(spawnergump, 330 + xoffset + 30, yoff + 1, 32, 21, 0xBBC);
				AddTextEntry(spawnergump, 333 + xoffset + 30, yoff, 27, 33, texthue, 1000 + i, strrst);
				// reset to
				AddImageTiled(spawnergump, 365 + xoffset + 30, yoff, 26, 23, 0x52);
				AddImageTiled(spawnergump, 366 + xoffset + 30, yoff + 1, 23, 21, 0xBBC);
				AddTextEntry(spawnergump, 369 + xoffset + 30, yoff, 18, 33, texthue, 1100 + i, strto);
				// kills needed
				AddImageTiled(spawnergump, 392 + xoffset + 30, yoff, 35, 23, 0x52);
				AddImageTiled(spawnergump, 393 + xoffset + 30, yoff + 1, 32, 21, 0xBBC);
				AddTextEntry(spawnergump, 396 + xoffset + 30, yoff, 27, 33, texthue, 1200 + i, strkill);
				// mindelay
				AddImageTiled(spawnergump, 428 + xoffset + 30, yoff, 41, 23, 0x52);
				AddImageTiled(spawnergump, 429 + xoffset + 30, yoff + 1, 38, 21, 0xBBC);
				AddTextEntry(spawnergump, 432 + xoffset + 30, yoff, 33, 33, texthue, 1300 + i, strmind);
				// maxdelay
				AddImageTiled(spawnergump, 470 + xoffset + 30, yoff, 41, 23, 0x52);
				AddImageTiled(spawnergump, 471 + xoffset + 30, yoff + 1, 38, 21, 0xBBC);
				AddTextEntry(spawnergump, 474 + xoffset + 30, yoff, 33, 33, texthue, 1400 + i, strmaxd);
				// packrange
				AddImageTiled(spawnergump, 512 + xoffset + 30, yoff, 33, 23, 0x52);
				AddImageTiled(spawnergump, 513 + xoffset + 30, yoff + 1, 30, 21, 0xBBC);
				AddTextEntry(spawnergump, 516 + xoffset + 30, yoff, 25, 33, texthue, 1600 + i, strpackrange);

				if (sequentialspawn >= 0)
					// restrict kills button
					if (spawnobjects[i].restrictkillstosubgroup)
						AddButton(spawnergump, 545 + xoffset + 30, yoff, 0xD3, 0xD2, 300 + i, 1, 0);
					else
						AddButton(spawnergump, 545 + xoffset + 30, yoff, 0xD2, 0xD3, 300 + i, 1, 0);
					endif
					//clear on advance button for spawn entries in subgroups that require kills
					if (spawnobjects[i].clearonadvance)
						AddButton(spawnergump, 565 + xoffset + 30, yoff, 0xD3, 0xD2, 400 + i, 1, 0);
					else
						AddButton(spawnergump, 565 + xoffset + 30, yoff, 0xD2, 0xD3, 400 + i, 1, 0);
					endif
				endif

				// add the next spawn time
				AddLabelCropped(spawnergump, 590 + xoffset + 30, yoff, 70, 20, 55, strnext);

			endif
			//AddButton(spawnergump,  20, 22 * ((i-1)%MaxEntriesPerPage) + 34, 0x15E3, 0x15E7, 5 + ((i-1) * 2), GumpButtonType.Reply, 0 );
		endif
		// the spawn specification text
		//if(str != null)
		AddTextEntry(spawnergump, 52, 22 * ((i-1) % MaxEntriesPerPage) + 31, 119 + xoffset - 25, 21, texthue, i, str);
	endfor

	if (HasGump(who, "xmlspawner"))
		SendSysMessage(who, "Already Opened.");
		return 0;
	endif

	//OnResponse
	var keyret := GFSendGump(who, spawnergump, "xmlspawner", GetPid() );
	//the return value is a dictionary. return[0] contains the button ID
	// the user used to close the gump. 0 is returned if the gump was cancelled
	//. return.keys contains the IDs for the radio buttons, checkboxes, and textentries
	// set when the gump was returned. Note control characters are stripped from the text input lines.

	/*if ( keyret[0] <= 1 )
		SendSysMessage(who, "Cancelled.");
		return 0;
	elseif ( keyret[1] )
		SendSysMessage(who, "*** HELP button was pressed! ***");
	elseif ( keyret[2] )
		SendSysMessage(who, "*** OPEN? button was pressed! ***");
	endif
	SendSysMessage(who, "Gump buttons/objects found enabled.");
	*/
	/*
	foreach key in (keyret.keys)
		var value := keyret[key];
		if ( value[2, 2] ==": " )
			SendSysMessage(who, "Text Entry ["+key+"] ->"+GFExtractData(keyret, key));
		else
			SendSysMessage(who, "Button Input ["+key+"] -> "+value);
		endif
		SleepMS(2);
	endforeach
	*/
	/*
	Text Entry [1] ->error errortext = "Subscript out of range"

	Button Input [10] -> 10:
	Button Input [11] -> 11:
	Button Input [12] -> 12:
	Button Input [13] -> 13:
	Button Input [14] -> 14:
	Button Input [300] -> 300:<uninitialized object>
			900
			999
	//messo da 1 a 15: HELP clicked
	Button Input [0] -> 1
	Text Entry [1] ->2
	Text Entry [9] ->10
	//messo da 1 a 15; sort check
	Button Input [0] -> 702
	Text Entry [1] ->2
	createarray 0: 702
	createarray 1: 2
	*/
	//l'unico button input è lo 0, tutto il resto è textentry per me.

	// Get the current name GetTextEntry o ExtractText.
	var tr := cstr(GetTextEntry(keyret, 999));

	if (len(tr)>0)
		spawner_item.name := cstr(tr);
	endif
	//controllo se ci sono residui non salvo
	var residui := 0;
	foreach spwobj in spawnobjects//potrebbero essere diversi dagli attuali se il timer è attivo, ma non salvo se è attivo.
		residui += len(spwobj.spawnedobjects);
	endforeach

	// update typenames of the spawn objects based upon the current text entry strings
	UpdateTypeNames(who, spawner_item, keyret);
	// Update the creature list
	spawnobjects := CreateArray(who, keyret, spawner_item, page);

	/*
		if (m_Spawner.SpawnObjects == null)
			{
				m_Spawner.SpawnerGump = null;
				return;
			}
	*/
	AllowGumpUpdate := 1;

	for (i := 1; i <= len(spawnobjects); i+=1)//nel POL le array partono da 1?
		if (page != cint((i-1) / MaxEntriesPerPage))
			continue;
		endif

		// check the max count entry
		var temcnt := cstr(GetTextEntry(keyret,500+i));

		if (len(temcnt)>0)
			var maxval := cint(temcnt);
			if (maxval < 0)
				maxval := 0;
			endif

			spawnobjects[i].maxcount := cint(maxval);
		endif

		if (m_ShowGump > 0)
			// check the subgroup entry
			var tegrp := cstr(GetTextEntry(keyret, 600+i));
			if (len(tegrp)>0)
				var grpval := cint(tegrp);
				if (grpval < 0)
					grpval := 0;
				endif

				spawnobjects[i].subgroup := cint(grpval);
			endif
		endif

		if (m_ShowGump > 1)
			// note, while these values can be entered in any spawn entry, they will only be assigned to the subgroup leader
			var subgroupindex := cint(GetCurrentSequentialSpawnIndex(spawner_item, spawnobjects[i].subgroup));
			var tegrp;
			//TODO! SUBGROUPINDEX FARLO PARTIRE DA 1!
			if (subgroupindex >= 1 && subgroupindex <= len(spawnobjects))
				// check the *reset time* entry
				tegrp := cstr(GetTextEntry(keyret, 1000+i));
				if (len(tegrp)>0)//!= null && tegrp.Text != null && tegrp.Text.Length > 0)
					var grpval := cdbl(tegrp);
					if (grpval < 0)
						grpval := 0;
					endif

					spawnobjects[i].sequentialresettime := 0;

					spawnobjects[subgroupindex].sequentialresetTime := cdbl(grpval);
				endif
				// check the *reset to* entry
				tegrp := cstr(GetTextEntry(keyret, 1100+i));
				if (len(tegrp)>0)// != null && tegrp.Text != null && tegrp.Text.Length > 0)
					var grpval := cint(tegrp);
					if (grpval < 0)
						grpval := 0;
					endif

					spawnobjects[subgroupindex].sequentialresetto := cint(grpval);
				endif
				// check the kills entry
				tegrp := cstr(GetTextEntry(keyret, 1200+i));
				if (len(tegrp)>0)// != null && tegrp.Text != null && tegrp.Text.Length > 0)
					var grpval := cint(tegrp);
					if (grpval < 0)
						grpval := 0;
					endif

					spawnobjects[subgroupindex].killsneeded := grpval;
				endif
			endif

			// check the mindelay
			tegrp := cstr(GetTextEntry(keyret, 1300+i));
			if (len(tegrp)>0)//tegrp.Text != null && tegrp.Text.Length > 0)
				var grpval := cdbl(tegrp);
				if (grpval < 0)
					grpval := -1;
				endif

				// if this value has changed, then update the next spawn time
				if (grpval != cdbl(spawnobjects[i].mindelay))
					spawnobjects[i].mindelay := cdbl(grpval);
					RefreshNextSpawnTime(spawner_item, spawnobjects[i]);
				endif
			else
				spawnobjects[i].mindelay := -1;
				spawnobjects[i].maxdelay := -1;
				RefreshNextSpawnTime(spawner_item, spawnobjects[i]);
			endif

			// check the maxdelay
			tegrp := cstr(GetTextEntry(keyret, 1400+i));
			if (len(tegrp)>0)// tegrp.Text != null && tegrp.Text.Length > 0)
				var grpval := cdbl(tegrp);
				if (grpval < 0)
					grpval := -1;
				endif

				// if this value has changed, then update the next spawn time
				if (grpval != cdbl(spawnobjects[i].maxdelay))
					spawnobjects[i].maxdelay := cdbl(grpval);
					RefreshNextSpawnTime(spawner_item, spawnobjects[i]);
				endif
			else
				spawnobjects[i].mindelay := -1;
				spawnobjects[i].maxdelay := -1;
				RefreshNextSpawnTime(spawner_item, spawnobjects[i]);
			endif

			// check the spawns per tick
			tegrp := cstr(GetTextEntry(keyret, 1500+i));
			if (len(tegrp)>0)//tegrp.Text != null && tegrp.Text.Length > 0)
				var grpval := cint(tegrp);
				if (grpval < 0)
					grpval := 1;
				endif

				// if this value has changed, then update the next spawn time
				if (grpval != cint(spawnobjects[i].spawnspertick))
					spawnobjects[i].spawnspertick := cint(grpval);
				endif
			else
				spawnobjects[i].spawnspertick := 1;
			endif

			// check the packrange
			tegrp := cstr(GetTextEntry(keyret, 1600+i));
			if (len(tegrp)>0)//tegrp.Text != null && tegrp.Text.Length > 0)
				var grpval := cint(tegrp);
				if (grpval < 0)
					grpval := 1;
				endif

				// if this value has changed, then update 
				if (grpval != cint(spawnobjects[i].packrange) )
					spawnobjects[i].packrange := cint(grpval);
				endif
			else
				spawnobjects[i].packrange := -1;
			endif
		endif
	endfor

	//SALVO QUI SPAWNOBJECTS?
	var dur := cint(TS_GetTimerDuration( spawner_item, "SpawnerTimer" ));
	//SendSysMessage( who, "[test] residui:"+residui);

	if (dur > 0 || cint(GetObjProperty(spawner_item, "Running"))==1 || residui > 0)
		SendSysMessage( who, "Attenzione: nessun salvataggio poiche' il timer e' attivo o sono presenti spawn residui.");
	else// caso in cui timer spento ma sono comunque presenti spawn? non salvo?
		SetObjProperty(spawner_item, "SpawnObjects", spawnobjects);
	endif

	var temax := cstr(GetTextEntry(keyret, 300));//maxcount

	if (len(temax)>0)
		var maxval := cint(temax);
		if (maxval < 0)
			maxval := 0;
		endif
		// if the maxcount of the spawner has been altered external to the interface (e.g. via props, or by the running spawner itself
		// then that change will override the text entry
		if (cint(GetObjProperty( spawner_item, "Count")) == initial_maxcount)
			SetObjProperty( spawner_item, "Count", maxval );
		endif
	endif

	//var keys := keyret.keys;

	var buttonID := cint(keyret[0]);
	//SendSysMessage( who, "BUTTONID:= "+cstr(buttonID)  );

	if (buttonID <=1)//== 0)// Close
		//SendSysMessage( who, "closed with right click." );
		CloseGumpPid(who, "xmlspawner", 0);
		return 0;
		/*
		// clear any text entry books
		spawner.DeleteTextEntryBook();
		// and reset the gump status
		spawner.GumpReset = true;//cioè spawner.spawnergump = null
		return;
		*/
	//elseif (buttonID == 1)// Help-close
	//	SendSysMessage( who, "HELP BUTTON CLICKED" );
	//	HelpGump(who, spawner, X + 290, Y);//state.Mobile.SendGump(new HelpGump(m_Spawner, this, this.X + 290, this.Y));
	//	return 0;
	elseif (buttonID == 2)// Bring everything home
		BringToHome(spawner_item);
	elseif (buttonID == 3)// Complete respawn
		SendSysMessage( who, "Respawning; killcount reset to 0." );

		//var dur := cint(TS_GetTimerDuration( spawner_item, "SpawnerTimer" ));
		//if (dur > 0 || cint(GetObjProperty(spawner_item, "Running"))==1)
		//	SendSysMessage( who, "Attenzione: nessun respawn poiche' il timer e' attivo.");
		//else
			Respawn(spawner_item);//potrebbe essersi modificata la struct col timer durante.
		//endif

		//Respawn(spawner_item, spawner);
		SetObjProperty( spawner_item, "KillCount", 0 );//spawner.killcount := 0;
	elseif (buttonID == 4)// Goto
		MoveObjectToLocation( who, spawner_item.x, spawner_item.y, spawner_item.z, spawner_item.realm );
	elseif (buttonID == 703)// Help
		HelpGump(who, spawner_item, X + 290, Y);
	elseif (buttonID == 200)// gump extension
		if (m_ShowGump > 1)
			XmlSpawnerGump(who, spawner_item, X, Y, -1, xoffset, page);
		else
			XmlSpawnerGump(who, spawner_item, X, Y, m_ShowGump + 2, xoffset, page);
		endif
	elseif (buttonID == 201)// gump text extension
		//SendSysMessage( who, "GUMP TEXT EXTENSION BUTTON CLICKED" );
		if (xoffset > 0)
			XmlSpawnerGump(who, spawner_item, X, Y, m_ShowGump, 0, page);
		else
			XmlSpawnerGump(who, spawner_item, X, Y, m_ShowGump, 250, page);
		endif
	elseif (buttonID == 700)// Start/stop spawner
		//SendSysMessage( who, "START/STOP BUTTON CLICKED" );
		if (cint(GetObjProperty(spawner_item, "Running"))==1)//il timer potrebbe aver modificato la struct
			Stop( spawner_item );
		else
			//SendSysMessage( who, "Width:"+cint(GetObjProperty(spawner_item, "Width"))+"; Height:"+cint(GetObjProperty(spawner_item, "Height")) );
			if (cint(GetObjProperty(spawner_item, "Width")) >= 0 && cint(GetObjProperty(spawner_item, "Height")) >= 0)
				StartSpawn( spawner_item );
			endif
		endif
	elseif (buttonID == 701)// Complete reset
		//SendSysMessage( who, "COMPLETE RESET BUTTON CLICKED" );
		//var dur := cint(TS_GetTimerDuration( spawner_item, "SpawnerTimer" ));
		//if (dur > 0 || cint(GetObjProperty(spawner_item, "Running"))==1)
		//	SendSysMessage( who, "Attenzione: nessun reset poiche' il timer e' attivo.");
		//else
			Reset(spawner_item);//potrebbe essersi modificata la struct col timer durante.
		//endif
		//Reset(spawner_item, spawner);
	elseif (buttonID == 702)// Sort spawns
		SendSysMessage( who, "SORT SPAWNS BUTTON CLICKED" );
		//m_Spawner.SortSpawns();
	elseif (buttonID == 900)// empty the status string
		SendSysMessage( who, "EMPTY ST STRING BUTTON CLICKED" );
		SetObjProperty(spawner_item, "status_str", "");//spawner.status_str := "";
	elseif (buttonID == 9997)
		SendSysMessage( who, "disableglobalautor. BUTTON CLICKED" );
		if (cint(GetObjProperty(spawner_item, "DisableGlobalAutoReset")) == 1)
			//spawner.disableglobalautoreset := 0;
			SetObjProperty( spawner_item, "DisableGlobalAutoReset", 0 );
		else
			//spawner.disableglobalautoreset := 1;
			SetObjProperty( spawner_item, "DisableGlobalAutoReset", 1 );
		endif
	elseif (buttonID == 9998)
		SendSysMessage( who, "? BUTTON CLICKED" );
		XmlSpawnerGump(who, spawner_item, X, Y, m_ShowGump, xoffset, page);
	elseif (buttonID == 9999)
		SendSysMessage( who, "prop gump? BUTTON CLICKED" );
		XmlSpawnerGump(who, spawner_item, X, Y, m_ShowGump, xoffset, page);
		//TODO: ADD GUMP
		XmlPropertiesGump(who, spawner_item);//, X, Y, m_ShowGump, xoffset, page);
/*
		state.Mobile.SendGump(new XmlSpawnerGump(m_Spawner, this.X, this.Y, this.m_ShowGump, this.xoffset, this.page));
#if(NEWPROPSGUMP)
		state.Mobile.SendGump(new XmlPropertiesGump(state.Mobile, m_Spawner));
#else
		state.Mobile.SendGump( new PropertiesGump( state.Mobile, m_Spawner ) );
#endif
*/
	else
		// check the restrict kills flag
		if (buttonID >= 300 && ButtonID <= 300 + MaxSpawnEntries)
			//SendSysMessage( who, "restrictkills BUTTON CLICKED" );
			var index := cint(ButtonID - 300);
			spawnobjects := GetObjProperty(spawner_item, "SpawnObjects");//potrebbe essersi modificato quindi ricarico
			if (index <= len(spawnobjects))
				if (spawnobjects[index].restrictkillstosubgroup == 1)
					spawnobjects[index].restrictkillstosubgroup := 0;
				else
					spawnobjects[index].restrictkillstosubgroup := 1;
				endif
				//var dur := cint(TS_GetTimerDuration( spawner_item, "SpawnerTimer" ));
				//if (dur > 0 || cint(GetObjProperty(spawner_item, "Running"))==1)
				//	SendSysMessage( who, "Attenzione: nessun salvataggio poiché il timer è attivo.");
				//else
					SetObjProperty(spawner_item, "SpawnObjects", spawnobjects);
				//endif
			endif
		else
			// check the clear on advance flag
			if (ButtonID >= 400 && ButtonID <= 400 + MaxSpawnEntries)
				//SendSysMessage( who, "clearonadv BUTTON CLICKED" );
				var index := cint(ButtonID - 400);
				spawnobjects := GetObjProperty(spawner_item, "SpawnObjects");//potrebbe essersi modificato quindi ricarico
				if (index <= len(spawnobjects))
					if (spawnobjects[index].clearonadvance == 1)
						spawnobjects[index].clearonadvance := 0;
					else
						spawnobjects[index].clearonadvance := 1;
					endif
					//var dur := cint(TS_GetTimerDuration( spawner_item, "SpawnerTimer" ));
					//if (dur > 0 || cint(GetObjProperty(spawner_item, "Running"))==1)
					//	SendSysMessage( who, "Attenzione: nessun salvataggio poiché il timer è attivo.");
					//else
						SetObjProperty(spawner_item, "SpawnObjects", spawnobjects);
					//endif
				endif
			else
				// text entry gump scroll buttons
				if (ButtonID >= 800 && ButtonID <= 800 + MaxSpawnEntries)
					var index := cint(ButtonID - 800);
					// display a new gump
					//XmlSpawnerGump(who, spawner_item, X, Y, m_ShowGump, xoffset, page);

					var textentrybook  := GetObjProperty(spawner_item, "textentrybook");
					if (!textentrybook)
						textentrybook := array;
						SetObjProperty(spawner_item, "textentrybook", textentrybook);
					else//controllo doppioni:
						for ( i:= 1; i <= len(textentrybook); i+= 1)
							var libro := SystemFindObjectBySerial(textentrybook[i]);
							if (!libro.errortext)
								//var testo := libro.GetText();
								var args := GetObjProperty(libro, "args");
								var m_Index := cint(args[2]);

								if ( m_Index == index )
									SendSysMessage( who, "deleting found duplicate." );
									var result := DestroyItem(libro);
									if (!result.errortext)
										textentrybook.erase(i);
									else
										SendSysMessage( who, "errore nella cancellazione del libro" );
									endif
								endif
							endif
							sleepms(3);
						endfor
					endif
					var book := CreateItemAtLocation( who.x, who.y, who.z, 0xFF5, 1 , who.realm );//z-100
					book.SetTitle( "Entry " + cstr(index) );
					book.SetAuthor( "chiudere libro - riaprire gump" );//spawner_item.name
					book.ReadOnly(0);//writable
					// fill the contents of the book with the current text entry data
					var text := "";
					if (index <= len(spawnobjects))
						text := cstr(spawnobjects[index].typename);
					endif
					//TODO: quando apro il gump dello spawner controllo tutti i book e faccio partire
					//ProcessSpawnerBookEntry, poi li cancello.
					book.FillTextEntryBook(text);
					//TODO: fixare gli spazi nel method del book.

					// put the book at the location of the player so that it can be opened, but drop it below visible range
					//book.Visible := 0;
					//book.movable := 0;
					book.decayat := readgameclock() + 1000;
					book.color := 946;
					var args := array;
					args[1] := spawner_item.serial;
					args[2] := index;
					args[3] := X;
					args[4] := Y;
					args[5] := m_ShowGump;
					args[6] := page;
					//SendSysMessage( who, "Serial:"+spawner_item.serial );
					SetObjProperty(book, "args", args);

					textentrybook.append( book.serial );
					SetObjProperty(spawner_item, "textentrybook", textentrybook);

					// and open it
					//SendOpenBook( who, book );
					UseItem( book, who );

					// is there an existing book associated with the gump?
					/*
					if (!spawner.textentrybook)
						spawner.textentrybook := array;//new List<XmlTextEntryBook>();
					endif

					var args := array;
					args[0] := spawner;
					args[1] := index;
					args[2] := X;
					args[3] := Y;
					args[4] := m_ShowGump;
					args[5] := page;

					var book := XmlTextEntryBook(0, "", spawner.Name, 20, 1, XmlTextEntryBookCallback(ProcessSpawnerBookEntry), args);
					spawner.textentrybook.Append(book);
					book.Title := "Entry " + cstr(index);
					book.Author := spawner.name;
					// fill the contents of the book with the current text entry data
					var text = "";
					if (spawner.spawnobjects && index < len(Spawner.spawnobjects))
						text := cstr(spawner.spawnobjects[index].typename);
					endif

					book.FillTextEntryBook(text);

					// put the book at the location of the player so that it can be opened, but drop it below visible range
					book.Visible := 0;
					//book.decayat := readgameclock() + 1000;
					book.color := 946;
					book.Movable := 0;
					book.MoveToWorld(new Point3D(state.Mobile.Location.X, state.Mobile.Location.Y, state.Mobile.Location.Z - 100), state.Mobile.Map);

					// and open it
					book.OnDoubleClick(state.Mobile);

					//vecchio prima di booktextentry
					//state.Mobile.SendGump( new TextEntryGump(m_Spawner,this, index, this.X, this.Y));
					return;
					*/
				else
					if (ButtonID >= 1300 && ButtonID <= 1300 + MaxSpawnEntries)
						nclicks+=1;
						var index := cint(ButtonID - 1300);
						spawnobjects := GetObjProperty(spawner_item, "SpawnObjects");//quando lo clicco potrebbe essersi modificato, lo ricarico.
						if (index <= len(spawnobjects))
							var scount := len(spawnobjects[index].spawnedobjects);
							if (scount > 0)
								var so := spawnobjects[index].spawnedobjects[(nclicks % scount) + 1];//TODO: controllare nclicks forse +1
								so := SystemFindObjectBySerial(so);
								if (ValidGotoObject(who, so))
									MoveObjectToLocation( who, so.x, so.y, so.z, so.realm, MOVEOBJECT_FORCELOCATION);
								endif
							else
								SendSysMessage( who, "no spawned object found.");
							endif
						endif
					else
						//page buttons
						if (ButtonID >= 4000 && ButtonID < 4001 + cint(MaxSpawnEntries / MaxEntriesPerPage))
							page := cint(ButtonID - 4000);
							//SendSysMessage( who, "Page "+page);
						else
							// toggle the disabled state of the entry
							if (ButtonID >= 6000 && ButtonID <= 6000 + MaxSpawnEntries)
								var index := cint(ButtonID - 6000);
								spawnobjects := GetObjProperty(spawner_item, "SpawnObjects");//quando lo clicco potrebbe essersi modificato, lo ricarico.
								if (index <= len(spawnobjects))
									if (spawnobjects[index].disabled == 1)
										spawnobjects[index].disabled := 0;
									else
										spawnobjects[index].disabled := 1;
									endif

									//var dur := cint(TS_GetTimerDuration( spawner_item, "SpawnerTimer" ));
									//if (dur > 0 || cint(GetObjProperty(spawner_item, "Running"))==1)
									//	SendSysMessage( who, "Attenzione: nessun salvataggio poiché il timer è attivo.");
									//else
										SetObjProperty(spawner_item, "SpawnObjects", spawnobjects);
									//endif
									//SetObjProperty(spawner_item, "SpawnObjects", spawnobjects);

									// clear any current spawns on the disabled entry
									if (spawnobjects[index].disabled==1)
										RemoveSpawnObjects(spawner_item, spawnobjects[index]);
									endif
								endif
							else
								if (ButtonID >= 5000 && ButtonID <= 5000 + MaxSpawnEntries)
									var i := cint(ButtonID - 6000);
									var categorystring := "";
									var entrystring := "";
									var te := cstr(GetTextEntry(keyret, i));
									if (len(te)>1) //&& te.text)
										var cargs := SplitWords(te, ",");
										categorystring := cargs[1];//was 0
										entrystring := te;
									endif

									if (categorystring == "" || len(categorystring) <= 0)
										CloseGumpPid(who, "xmlspawner", 0);
										SleepMS(2);
										XmlSpawnerGump(who, spawner_item, X, Y, m_ShowGump, xoffset, page);
										/*XmlSpawnerGump newg = new XmlSpawnerGump(m_Spawner, this.X, this.Y, this.m_ShowGump, this.xoffset, this.page);
										state.Mobile.SendGump(newg);

										// if no string has been entered then just use the full categorized add gump
										state.Mobile.CloseGump(typeof(Server.Gumps.XmlCategorizedAddGump));
										state.Mobile.SendGump(new Server.Gumps.XmlCategorizedAddGump(state.Mobile, i, newg));
										*/
									else
										/*
										// use the XmlPartialCategorizedAddGump
										state.Mobile.CloseGump(typeof(Server.Gumps.XmlPartialCategorizedAddGump));

										//Type [] types = (Type[])XmlPartialCategorizedAddGump.Match( categorystring ).ToArray( typeof( Type ) );
										ArrayList types = XmlPartialCategorizedAddGump.Match(categorystring);


										XmlSpawnerGump.ReplacementEntry re = new XmlSpawnerGump.ReplacementEntry();
										re.Typename = entrystring;
										re.Index = i;
										re.Color = 0x1436;

										XmlSpawnerGump newg = new XmlSpawnerGump(m_Spawner, this.X, this.Y, this.m_ShowGump, this.xoffset, this.page, re);

										state.Mobile.SendGump(new XmlPartialCategorizedAddGump(state.Mobile, categorystring, 0, types, true, i, newg));

										state.Mobile.SendGump(newg);
										*/
									endif
									return 0;
								else
									// up and down arrows
									var butID := cint(ButtonID - 6);
									var index := cint(butID / 2) +1;//il bottone 6 dovrebbe dare index 1
									var type := cint(butID % 2);

									var entrystr := cstr(GetTextEntry(keyret, index));
									//SendSysMessage( who, "arrows: butID:"+butID+" index:"+index+" type:"+ type+" entrystr:"+entrystr );
									
									if (len(entrystr)>0)
										spawnobjects := GetObjProperty(spawner_item, "SpawnObjects");//quando lo clicco potrebbe essersi modificato, lo ricarico.
										if (index <= len(spawnobjects))
											var str := cstr(spawnobjects[index].typename);

											if (str && len(str) >= 230)
												entrystr := str;
											endif
										endif

										if (type == 0) // ^ Add creature
											AddSpawnObject(spawner_item, entrystr);
										else // \/ Remove creatures
											//SendSysMessage( who, "entry ["+entrystr+"]" );
											DeleteSpawnObject(who, spawner_item, entrystr);
										endif
									endif
									
								endif
							endif
						endif
					endif
				endif
			endif
		endif
	endif

	// Create a new gump
	//m_Spawner.OnDoubleClick( state.Mobile);
	SleepMS(2);
	CloseGumpPid(who, "xmlspawner", 0);
	XmlSpawnerGump(who, spawner_item, X, Y, m_ShowGump, xoffset, page, rentry);
endfunction

function IsNullOrError(str)
	if (str == "" || len(str) <=0)
		return 1;
	elseif (str[1] == "e" && str[2] == "r" && str[3] == "r" && str[4] == "o" && str[5] == "r")
		return 2;
	endif
	return 0;
endfunction
//toglie i primi numeri dalla stringa es stringa 1 toglie "1:" stringa 10 toglie "10:"
function GetTextEntry(input, data_id)// (string str, string input, INT data_id)
	var key_string := CStr(data_id)+": ";//"1: "
	var data_string := input[data_id];
	if ( len(key_string) >= len(data_string))
		return "";
	endif

	var ret_val := data_string[Len(key_string)+1,Len(data_string)];

	return cstr(ret_val);

endfunction

//funzioni della classe XmlSpawnerGump
//TODO: FIX CREATEARRAY!!!!
function CreateArray(from, byref keyret, byref spawner_item, page := 0)
	var SpawnObjects := array;
	var i;
	//se sono vuote le guarda comunque tutte e 60
	var spawnerspawnobjects := GetObjProperty(spawner_item, "SpawnObjects");
	for ( i := 1; i <= cint(MaxSpawnEntries); i+=1)
		var str := GetTextEntry(keyret, i);
		//TODO: controllo che str non sia la stessa di uno spawnobject già inserito.
		//
		if (page != cint((i-1) / MaxEntriesPerPage))//siamo sulla pagina sbagliata
			//il textentry sarà lo stesso che c'era prima.
			if ( i <= len(spawnerspawnobjects) )
				str := cstr(spawnerspawnobjects[i].typename);
			endif
		endif

		if (str && len(str) > 0)
			//SendSysMessage( from, "createarray " + cstr(i) +":= "+str  );
			str := TrimStr(str);
			if (i <= len(spawnerspawnobjects))//forse <= ci vuole! se len = 1? era <
				var currenttext := cstr(spawnerspawnobjects[i].typename);
				if (currenttext && len(currenttext) >= 230)//supera il buffer. non la tronco
					str := currenttext;
				endif
			endif

			//aggiunta!
			var s := spawnerspawnobjects[i];
			if (s && s.typename == str)//found in the same place
				//SendSysMessage( from, "already found entry " + i +" = "+str  );
				SpawnObjects[i] := spawnerspawnobjects[i];
			else//fine aggiunta
				var typestr := ParseObjectType(str);//BaseXmlSpawner.inc
				var type := GetType(typestr);//sostituire con cfg_elem
				//SendSysMessage( from, "createarray typestr(" + cstr(str) +"):= "+cstr(typestr)  );
				//SendSysMessage( from, "createarray type, " + cstr(i) +":= "+cstr(type)  );
				if (type)
					//SendSysMessage( from, "createarray found type for ["+str+"], " + cstr(i) +":= "+cstr(type)  );
					SpawnObjects.Append( SpawnObject(from, spawner_item, str, 0) );
				else
					// check for special keywords
					//SendSysMessage( from, "createarray cannot find type " + cstr(i) );
					//SendSysMessage( from, "Returnindex " + cstr(ReturnIndex(typestr,"{")) );
					//SendSysMessage( from, "typestr1 " + cstr(typestr[1]) );
					if (typestr && (IsTypeOrItemKeyword(typestr) || ReturnIndex(typestr,"{") != 0 || typestr[1] == "*" || typestr[1]=="#" ))
						SpawnObjects.Append( SpawnObject(from, spawner_item, str, 0) );
					else
						SetObjProperty(spawner_item, "status_str", str +" is not a valid type name.");
					endif
				endif
			endif
		//else
		//	SendSysMessage( from, "createarray null str := " + cstr(i)  );
		endif
	endfor
	return SpawnObjects;
endfunction


//TODO FIX! SAVE?
function UpdateTypeNames(from, byref spawner_item, keyret)//, RelayInfo info)
	var i;
	var spawnobjects := GetObjProperty(spawner_item, "SpawnObjects");
	for( i:=1; i <= MaxSpawnEntries; i+=1)
		var str := GetTextEntry(keyret, i);
		if (len(str)>0)
			str := TrimStr(str);
			//SendSysMessage( from, "UpdateTypeName trim " + cstr(i) +":= "+str  );//fin qui ok "katana"

			if (i <= len(spawnobjects))
				// check to see if the existing typename is longer than the max textentry buffer
				// if it is then dont update it since we will assume that the textentry has truncated the actual string
				// that could be longer than the buffer if booktextentry is used

				var currentstr := cstr(spawnobjects[i].typename);
				//SendSysMessage( from, "UpdateTypeName currentstr" + cstr(i) +":= "+str  );
				if (currentstr && len(currentstr) < 230)

					if (spawnobjects[i].typename != str)
						//print(from.name+"changed XmlSpawner "+spawner_item.serial+" "+spawner_item.name+" ["+spawner_item.x+","+spawner_item.y+"] ("+spawner.realm+") "+str);
						//CommandLogging.WriteLine(from, "{0} {1} changed XmlSpawner {2} '{3}' [{4}, {5}] ({6}) : {7} to {8}", from.AccessLevel, CommandLogging.Format(from), m_Spawner.Serial, m_Spawner.Name, m_Spawner.GetWorldLocation().X, m_Spawner.GetWorldLocation().Y, m_Spawner.Map, m_Spawner.SpawnObjects[i].TypeName, str);
					endif
					spawnobjects[i].typename := str;
				endif
			endif
		endif
	endfor
endfunction

//TODO FIX: SAVE?
function ProcessSpawnerBookEntry(from, args, entry, byref spawnobjects := 0)
	if (!from || !args || len(args) < 6)
		return 0;
	endif

	var m_Spawner_serial := args[1];
	if (m_Spawner_serial.errortext)
		return 0;
	endif
	var m_Spawner := SystemFindObjectBySerial(m_Spawner_serial);
	var m_Index := cint(args[2]);
	var m_X := cint(args[3]);
	var m_Y := cint(args[4]);
	var m_Extension := cint(args[5]);
	var m_page := cint(args[6]);
	//var spawnobjects := GetObjProperty(m_Spawner, "SpawnObjects");
	if ( spawnobjects == 0 )
		spawnobjects := GetObjProperty(m_Spawner, "SpawnObjects");
	endif

	if (!m_Spawner || !spawnobjects)
		return 0;
	endif

	// place the book text into the spawn entry
	if (m_Index <= len(spawnobjects))
		var so := spawnobjects[m_Index];

		if (so.typename != entry)
			print(from.name+"changed XmlSpawner "+m_Spawner.serial+" "+m_Spawner.name+" ["+m_Spawner.x+","+m_Spawner.y+"] ("+m_Spawner.realm+") "+entry);
			//CommandLogging.WriteLine(from, "{0} {1} changed XmlSpawner {2} '{3}' [{4}, {5}] ({6}) : {7} to {8}", from.AccessLevel, CommandLogging.Format(from), m_Spawner.Serial, m_Spawner.Name, m_Spawner.GetWorldLocation().X, m_Spawner.GetWorldLocation().Y, m_Spawner.Map, so.TypeName, entry);
		endif
		spawnobjects[m_Index].typename := entry;
	else
		// add a new spawn entry
		spawnobjects.Append( SpawnObject(from, m_Spawner, entry, 1));//SpawnObject(from, spawner_item, str, 0)
		m_Index := len(spawnobjects);//TODO VERIFY INDEX

		// and bump the maxcount of the spawner
		//m_Spawner.maxcount+=1;
		SetObjProperty( m_Spawner, "Count", GetObjProperty( m_Spawner, "Count") + 1 );
	endif

	SetObjProperty(m_Spawner, "SpawnObjects", spawnobjects);

	// refresh the spawner gumps			
	//TODO: FIX
	//RefreshSpawnerGumps(from);
	//CloseGumpPid(from, "xmlspawner", 0);
	//XmlSpawnerGump(from, m_Spawner, m_X, m_Y, m_Extension, 0, m_page);
endfunction

function ValidGotoObject(from, o)

	if (o.isA(POLCLASS_ITEM))
		if (o.realm)
			return 1;
		endif
		/*
		if (!i.Deleted && (i.Map != null) && (i.Map != Map.Internal))
			return 1;
		*/
		SendSysMessage( from, o.name +" is not available" );
	else
		if (o.isA(POLCLASS_MOBILE))
			if (o.realm)
				return 1;
			endif
			/*
			if (!m.Deleted && (m.Map != null) && (m.Map != Map.Internal))
				return 1;
			*/
			SendSysMessage( from, o.name +" is not available" );
		endif
	endif

	SendSysMessage( from, "not a valid object: "+ o);

	return 0;
endfunction

//fine funzioni classe XmlSpawnerGump