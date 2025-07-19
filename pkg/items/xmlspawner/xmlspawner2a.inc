use util;
include "include/npcdeath";
include "include/util_extend";
include "BaseXmlSpawner";

include ":timedscripts:timedScripts";
include ":areas:isValidLoc";

include "include/npccreate";

//TODO: FIX
function AddSpawnObject(byref spawner_item, SpawnObjectName)
	Defrag(spawner_item, 0);
	var spawnobjects := GetObjProperty(spawner_item, "SpawnObjects");
	//TODO: se non trovo inizializzo!
	if (len(spawnobjects) < 1)
		return 0;
	endif

	var maxc := cint(GetObjProperty(spawner_item, "Count"));

	// Find the spawn object and increment its count by one
	foreach so in (spawnobjects)
		if (Upper(cstr(so.typename)) == Upper(cstr(SpawnObjectName)))
			// Add one to the total count
			maxc+=1;//m_Count

			// Increment the max count for the current creature
			so.maxcount+=1;//TODO entrambi salvano in m_MaxCount! tolgo actualmaxcount
			//so.actualmaxcount+=1;

			//only spawn them immediately if the spawner is running
			if ( cint(GetObjProperty(spawner_item, "Running")) == 1)
				//Spawn(SpawnObjectName, false, 0);
				//Spawn(string SpawnObjectTypeName, bool smartspawn, byte loops)
				//Spawn(SpawnObjectTypeName, smartspawn, -1, Point3D.Zero, loops);
				//Spawn(string SpawnObjectTypeName, bool smartspawn, int packrange, Point3D packcoord, byte loops)
				Spawn4(spawner_item, SpawnObjectName, 0, -1, Point3d(0,0,0,spawner_item.realm), 0);
			endif
		endif
	endforeach

	//salvo count & spawnedobj
	SetObjProperty(spawner_item, "Count", maxc );
	SetObjProperty(spawner_item, "SpawnObjects", spawnobjects);

/*
	InvalidateProperties();
*/
endfunction

function BringToHome(byref spawner_item)
	var spawnobjects := GetObjProperty(spawner_item, "SpawnObjects");
	if (len(spawnobjects) < 1)
		return 0;
	endif

	Defrag(spawner_item, 0);
	var i;
	foreach so in (spawnobjects)
		for (i := 1; i <= len(so.spawnedobjects); i+=1)
			var o := SystemFindObjectBySerial(so.spawnedobjects[i]);

			if (o.isA(POLCLASS_MOBILE) || o.isA(POLCLASS_ITEM))
				MoveObjectToLocation( o, spawner_item.x, spawner_item.y, spawner_item.z, spawner_item.realm );
			endif
		endfor
	endforeach
endfunction

//TODO FIX
function CanFreeSpawn(byref spawner_item)//return bool
	// allow free spawning if proximity sensing is off and if all of the potential free-spawning triggers are disabled
	if (cint(GetObjProperty(spawner_item, "Running")))
		/*	TODO FIX!
			 && m_ProximityRange == -1 &&
					(m_ObjectPropertyName == null || m_ObjectPropertyName.Length == 0) &&
					(m_MobPropertyName == null || m_MobPropertyName.Length == 0 ||
					m_MobTriggerName == null || m_MobTriggerName.Length == 0) &&
					!m_ExternalTriggering)
		*/
		return 1;
	else
		return 0;
	endif
endfunction

function ClearSpawnedThisTick(byref spawner_item, value)//return bool
	var spawnobjects := GetObjProperty(spawner_item, "SpawnObjects");
	if (len(spawnobjects) < 1 || value == 0)
		return 0;
	endif

	var i;//usare foreach?
	for (i := 1; i <= len(spawnobjects); i+=1)
		//var sobj := spawnobjects[i];
		//if (sobj)
			spawnobjects[i].spawnedthistick := 0;
		//endif
	endfor
	//TODO SAVE!
	SetObjProperty(spawner_item, "SpawnObjects", spawnobjects);
endfunction

//TODO FIX!
function Defrag(byref spawner_item, killtest)//bool
	var spawnobjects := GetObjProperty(spawner_item, "SpawnObjects");

	if (len(spawnobjects) < 1)
		return 0;
	endif

	var removed := 0;
	var total_removed := 0;

	var deleteilist := array;
	var deletemlist := array;
	var x;
	var sequentialsp := cint(GetObjProperty(spawner_item, "SequentialSpawning"));
	var desptime := cint(GetObjProperty(spawner_item, "DespawnTime"));
	//PrintTextAbove( spawner_item, "defrag test");
	//TODO FIX! usare FOR e cancellare l'indice!
	foreach so in (spawnobjects)
		for (x := 1; x <= len(so.spawnedobjects); x+=1)
			//PrintTextAbove( spawner_item, "defrag: "+so.spawnedobjects[x]);
			var o := SystemFindObjectBySerial(so.spawnedobjects[x]);//ora dovrebbe essere il seriale.
			if (o.isA(POLCLASS_ITEM))
				//Broadcast("defrag item, serial:"+o.serial+"/"+so.spawnedobjects[x]+"; item:"+o);
				var despawned := 0;//bool
				// check to see if the despawn time has elapsed.  If so, then delete it if it hasnt been picked up or stolen.
				if (desptime > 0 && o /*&& (spawner_item.container == o.container )*/ && ( !(o.getgottenby).isA(POLCLASS_MOBILE)))
					deleteilist.append(so.spawnedobjects[x]);//append serial
					despawned := 1;
				endif
				// Check if the items has been deleted or
				// if something else now owns the item (picked it up for example)
				// also check the stolen/placed in container flag.  If any of those are true then the spawner doesnt own it any more so take it off the list.
				// the stolen/container flag prevents spawns from being left on the list when players take them and lock them back down on the ground.
				// If you have made the changes to stealing.cs and container.cs described in xmlspawner2.txt then just uncomment the line below to
				// enable this check
				if (!o || despawned || (spawner_item.container != o.container ) || (o.getgottenby).isA(POLCLASS_MOBILE))   // taken and in the world, or a different container
					so.spawnedobjects.erase(x);//was o
					x-=1;
					removed := 1;
					// if sequential spawning is active and the RestrictKillsToSubgroup flag is set, then check to see if
					// the object is in the current subgroup before adding to the total
					if (sequentialsp >= 1 && so.restrictkillstosubgroup == 1)
						if (so.subgroup == sequentialsp)
							total_removed+=1;
						endif
					else
						// just add it
						total_removed+=1;
					endif
				endif
			elseif (o.isA(POLCLASS_MOBILE))
				// check to see if the spawn has been idle for a long time
				// if it has then reposition it because it might be in an inaccessible location
				// note, vendors and special cases of positioning from the Spawn method will not get relocated
				// i'm not saying I like those special cases, but they should be treated consistently
				// doesnt work properly under RunUO 2.0 and also doesnt properly take spawn control keywords into consideration
				// when repositioning, so disable this for now
				/*
				if (SpawnIdleTime > 0 && !m.Deleted && !(m is BaseVendor) && (m.CreationTime < DateTime.Now - TimeSpan.FromHours(SpawnIdleTime))
					&& m.Map != null && m.Map != Map.Internal && !m.Map.GetSector(m.Location).Active)

					// determine whether the requiresurface flag is set
					m.Location = GetSpawnPosition(so.RequireSurface, m);

					// and reset the creation time (simulates respawning the identical mob at a new location)
					//m.CreationTime = DateTime.Now;
				endif
				*/
				var despawned := 0;
				// check to see if the despawn time has elapsed.  If so, and the sector is not active then delete it.
				if (desptime > 0 && o )
					deletemlist.append(so.spawnedobjects[x]);//serial
					despawned := 1;
				endif

				if (!o || despawned == 1)
					// Remove the delete mobile from the list
					so.spawnedobjects.erase(x);//was o
					x-=1;
					removed := 1;
					// if sequential spawning is active and the RestrictKillsToSubgroup flag is set, then check to see if
					// the object is in the current subgroup before adding to the total
					if (sequentialsp >= 1 && so.restrictkillstosubgroup == 1)
						if (so.subgroup == sequentialsp)
							total_removed+=1;
						else
							// just add it
							total_removed+=1;
						endif
					endif
				elseif ( o.master || o.script == "tamed" || GetObjProperty( o, "master"))
					// Check if the creature has been tamed or previously tamed and released
					// and if it is, remove it from the list of spawns
					so.spawnedobjects.erase(x);
					x-=1;
					removed := 1;
					// if sequential spawning is active and the RestrictKillsToSubgroup flag is set, then check to see if
					// the object is in the current subgroup before adding to the total
					if (sequentialsp >= 1 && so.restrictkillstosubgroup == 1)
						if (so.subgroup == sequentialsp)
							total_removed+=1;
						else
							// just add it
							total_removed+=1;
						endif
					endif
				endif
			else //qui vanno gli oggetti deletati ad esempio
				//error errortext = "Character not found"
				//Console.WriteLine("removing unknown {0} from spawnlist", so);
				if (o.errortext == "Character not found")//TODO: inserire il check dei deletati di prima qui
					//PrintTextAbove(spawner_item, "removing deleted item/npc");
				else
					//PrintTextAbove(spawner_item, "removing unknown "+o+" from spawnlist" );
					//PrintTextAbove(spawner_item, "removed "+so.typename+" from spawnlist" );
					print("[spawner] error on spawner (" +spawner_item.x + " "+spawner_item.y +") on: "+so.typename );
				endif
				//print("System Message: removing unknown "+so+" from spawnlist");

				so.spawnedobjects.erase(x);//was o
				x-=1;
				removed := 1;

				/*TODO FIX KEYWORD
				if (o is BaseXmlSpawner.KeywordTag)
					BaseXmlSpawner.KeywordTag tag = (BaseXmlSpawner.KeywordTag)o;
					if (tag.Deleted)
								so.SpawnedObjects.Remove(o);
								x--;
								removed = true;
					endif
				else
					// Don't know what this is, so remove it
					Console.WriteLine("removing unknown {0} from spawnlist", so);
					so.SpawnedObjects.Remove(o);
					x--;
					removed = true;
				endif
				*/
			endif
		endfor
	endforeach

	DeleteFromList2(deleteilist, deletemlist);

	// Check if anything has been removed
	//if (removed == 1)
	//	InvalidateProperties();
	//endif

	// increment the killcount based upon the number of items that were removed from the spawnlist (i.e. were spawned but now are gone, presumed killed)
	if (killtest == 1)
		//spawner.killcount += cint(total_removed);
		var killc := cint(GetObjProperty(spawner_item, "KillCount"));
		SetObjProperty(spawner_item, "KillCount", killc + total_removed);
	endif

	//salvo killcount & spawnedobj
	//SetObjProperty(spawner_item, "KillCount", cint(spawner.killcount));
	//salvo solo se ne ho tolto qualcuno
	if (removed == 1)
		SetObjProperty(spawner_item, "SpawnObjects", spawnobjects);
	endif
endfunction

//TODO FIX? rimuovo spawner.
function DeleteFromList(list)//liste di seriali.
	if (!list)
		return 0;
	endif

	foreach o in (list)
		o := SystemFindObjectBySerial(o);
		if (o.isA(POLCLASS_ITEM))
			DestroyItem(o);
		elseif (o.isA(POLCLASS_MOBILE))
			NPCDeath(o);//TODO FIX NPCDEATH
		endif
		sleepms(2);
	endforeach
endfunction

function DeleteFromList2(listitem, listmobile)//liste di seriali.
	foreach o in (listitem)
		o := SystemFindObjectBySerial(o);
		if (o.isA(POLCLASS_ITEM))//aggiunte 3/05/2016
			DestroyItem(o);
		endif
		sleepms(2);
	endforeach
	foreach m in (listmobile)
		m := SystemFindObjectBySerial(m);
		if (m.isA(POLCLASS_MOBILE))//aggiunte 3/05/2016
			NPCDeath(m);
		endif
		sleepms(2);
	endforeach
endfunction

//TODO: FIX. ora sembra funzionare
function DeleteSpawnObject(from, byref spawner_item, SpawnObjectName)
	var WasRunning := cint(GetObjProperty(spawner_item, "Running"));//spawner.running;

	// Stop spawning for a moment
	Stop(spawner_item);

	// Clean up any spawns marked as deleted
	Defrag(spawner_item, 0);

	// Keep a reference to the spawn object
	/*var TheSpawn; sembra che la referenza non si salvi. cambio con un for

	// Find the spawn object and increment its count by one
	foreach so in (spawner.spawnobjects)
		if (Upper(so.typename) == Upper(SpawnObjectName))//TODO FIX TYPENAME?
			// Set the spawn
			TheSpawn := so;
			break;
		endif
	endforeach
	SendSysMessage( from,  "DeleteSpawnObject: TheSpawn:"+ TheSpawn.typename+", maxcount: "+TheSpawn.maxcount );
	if (TheSpawn)
		var delete_this_entry := 0;

		// Decrement the max count for the current creature
		TheSpawn.maxcount-=1;//TODO entrambi salvano in m_MaxCount! tolgo actualmaxcount
		//TheSpawn.actualmaxcount-=1;

		// Make sure the spawn count does not go negative
		if (TheSpawn.maxcount < 0)//se disabled maxcount == 0
			TheSpawn.maxcount := 0;
			delete_this_entry := 1;
		endif

		if (delete_this_entry==0)
			// Subtract one to the total count
			spawner.maxcount-=1;
		endif

		// Make sure the count does not go negative
		if (spawner.maxcount < 0)
			spawner.maxcount := 0;
		endif
		var deletelist := array;

		// Remove any spawns over the count
		while (TheSpawn.spawnedobjects && len(TheSpawn.spawnedobjects) > 0 && len(TheSpawn.spawnedobjects) > TheSpawn.maxcount)
			var o := SystemFindObjectBySerial(TheSpawn.spawnedobjects[1]);

			// Delete the object
			if (o.isA(POLCLASS_MOBILE) || o.isA(POLCLASS_ITEM))
				deletelist.append(TheSpawn.spawnedobjects[1]);
			endif

			TheSpawn.spawnedobjects.erase(o);
		endwhile
		DeleteFromList(deletelist);

		// Check if the spawn object should be removed
		//if( TheSpawn.MaxCount < 1 )
		if (delete_this_entry == 1)
			spawner.spawnobjects.erase(TheSpawn);
			if (from)
				SendSysMessage( from,  "removed from XmlSpawner "+ spawner.name+" "+SpawnObjectName );
			//	CommandLogging.WriteLine(from, "{0} {1} removed from XmlSpawner {2} '{3}' [{4}, {5}] ({6}) : {7}", from.AccessLevel, CommandLogging.Format(from), Serial, Name, GetWorldLocation().X, GetWorldLocation().Y, Map, SpawnObjectName);
			endif
		endif
	endif
	*/
	//
	//foreach so in (spawner.spawnobjects)
	var x;
	var spawnobjects := GetObjProperty(spawner_item, "SpawnObjects");
	var maxcount := cint(GetObjProperty(spawner_item, "Count"));

	for (x := 1; x <= len(spawnobjects); x+=1)
		//var so := spawnobjects[x];
		if (Upper(spawnobjects[x].typename) == Upper(SpawnObjectName))//TODO FIX TYPENAME?
			var delete_this_entry := 0;

			// Decrement the max count for the current creature
			spawnobjects[x].maxcount-=1;

			// Make sure the spawn count does not go negative
			if (spawnobjects[x].maxcount < 0)//se disabled maxcount == 0
				spawnobjects[x].maxcount := 0;
				delete_this_entry := 1;
			endif

			if (delete_this_entry==0)
				// Subtract one to the total count
				maxcount-=1;
			endif

			// Make sure the count does not go negative
			if (maxcount < 0)
				maxcount := 0;
			endif
			var deletelist := array;

			// Remove any spawns over the count
			while (len(spawnobjects[x].spawnedobjects) > 0 && len(spawnobjects[x].spawnedobjects) > spawnobjects[x].maxcount)
				var o := SystemFindObjectBySerial((spawnobjects[x]).spawnedobjects[1]);

				// Delete the object
				if (o.isA(POLCLASS_MOBILE) || o.isA(POLCLASS_ITEM))
					deletelist.append((spawnobjects[x]).spawnedobjects[1]);
				//else
				//	print("[spawner] error on: "+o);
				endif

				spawnobjects[x].spawnedobjects.erase(1);//was o
				sleepms(2);
			endwhile
			DeleteFromList(deletelist);

			// Check if the spawn object should be removed
			//if( so.MaxCount < 1 )
			if (delete_this_entry == 1)
				spawnobjects.erase(x);
				if (from)
					SendSysMessage( from,  "removed from XmlSpawner "+ spawner_item.name+" "+SpawnObjectName );
				//	CommandLogging.WriteLine(from, "{0} {1} removed from XmlSpawner {2} '{3}' [{4}, {5}] ({6}) : {7}", from.AccessLevel, CommandLogging.Format(from), Serial, Name, GetWorldLocation().X, GetWorldLocation().Y, Map, SpawnObjectName);
				endif
			endif
			break;
		endif
		sleepms(2);
	endfor//each
	//
	//salvo count & spawnedobj
	SetObjProperty(spawner_item, "Count", maxcount);
	SetObjProperty(spawner_item, "SpawnObjects", spawnobjects);

	//InvalidateProperties();

	if (WasRunning == 1)
		StartSpawn(spawner_item);
	endif
endfunction

function DoRespawn(byref spawner_item, value := 0)
	if (value == 0)
		return 0;
	else//value 1
		if (!GetObjProperty(spawner_item, "InRespawn"))
			Respawn();
		endif
	endif
endfunction

//TODO FIX, stoppare il timer, settare l'end e passare la struct
function DoTimer(byref spawner_item, delay:=0)
	if (cint(GetObjProperty(spawner_item, "Running")) == 0)
		return;
	endif

	if (delay <= 0)
		delay := RandomIntRange(cint(GetObjProperty(spawner_item, "MinDelay")), cint(GetObjProperty(spawner_item, "MaxDelay")));
		if (delay <= 1 || delay.errortext )
			delay := 1;
		endif
		//if (delay < cint(GetObjProperty(spawner_item, "MinDelay")))
		//	delay := cint(GetObjProperty(spawner_item, "MinDelay"));
		//endif
	endif

	SetObjProperty(spawner_item, "End", readgameclock() + delay );

	//controllo se il timer è già attivo, se sì lo spengo.
	var dur := cint(TS_GetTimerDuration( spawner_item, "SpawnerTimer" ));
	if (dur > 0 || dur < 0)//la duration è anche negativa al riavvio mi sa!!!
		//Broadcast( "Dotimer timer attivo, stop." );
		//PrintTextAbove(spawner_item, "stopped timer");
		TS_StopTimer( spawner_item, "SpawnerTimer" );
		SleepMS(200);
	endif
	//PrintTextAbove(spawner_item, "[duration:"+dur+"]");
	//PrintTextAbove(spawner_item, "[next tick:"+delay+"]");
	TS_StartTimer( spawner_item, "SpawnerTimer", delay, 0, 0 );//last should be part of a struct
endfunction

function DoTimer2(byref spawner_item, delay:=0)
/*
		if (spawner.duration > 0 || spawner.duractivated == 1)
			TS_StartTimer( spawner_item, "SpawnerInternalTimer", delay, 0, spawner );
			spawner.duractivated := 1;
			SetObjProperty(spawner_item, "DurActivated", 1);//bool
		endif
*/
/*
		[CommandProperty(AccessLevel.GameMaster)]
		public TimeSpan DurationOver
		{
			get
			{
				if (m_durActivated)
					return m_DurEnd - DateTime.Now;
				else
					return TimeSpan.FromSeconds(0);
			}
			set
			{
				DoTimer2(value);
			}
		}
			m_DurEnd = DateTime.Now + delay;
			if (m_Duration > TimeSpan.FromMinutes(0) || (m_durActivated == true))
			{
				if (m_DurTimer != null)
					m_DurTimer.Stop();
				m_DurTimer = new InternalTimer(this, delay);
				m_DurTimer.Start();
				m_durActivated = true;
			}
*/
endfunction

// returns the spawn index of a spawn entry in the current sequential subgroup
function GetCurrentSequentialSpawnIndex(byref spawner_item, sgroup)//return int
	if (sgroup < 0)
		return (-1);
	endif

	var spawnobjects := GetObjProperty(spawner_item, "SpawnObjects");
	if (len(spawnobjects) < 1)
		return (-1);
	endif

	if (sgroup == 0)
		return (RandomSpawnIndex(spawner_item, 0));
	endif

	//return the first instance of a spawn object that is an available member of the requested subgroup
	var j;
	for (j := 1; j <= len(spawnobjects); j+=1)
		if (spawnobjects[j].subgroup == sgroup)
			return (j);
		endif
	endfor

	// failed to find any spawn entry of the requested subgroup
	return (-1);
endfunction

function GetPackCoord(spawner_item, sgroup)//ritorna point3d
	//var packcoord;// = Point3D.Zero;
	var j;
	var i;
	var spawnobjects := GetObjProperty(spawner_item, "SpawnObjects");
	for (j := 1; j <= len(spawnobjects); j+=1)
		var so := spawnobjects[j];
		if (so && so.subgroup == sgroup && len(so.spawnedobjects) > 0 && so.packrange >= 0)
			// if pack spawning is enabled for this subgroup, then get the
			// the origin for pack spawning using the first existing pack spawn
			// in the subgroup

			for (i := 1; i <= len(so.spawnedobjects); i+=1)
				var o := SystemFindObjectBySerial(so.spawnedobjects[i]);
				if (o.isA(POLCLASS_ITEM) || o.isA(POLCLASS_MOBILE))
					return Point3d(o.x,o.y,o.z,o.realm);
				endif
			endfor
		endif
	endfor

	return Point3d(0,0,0,spawner_item.realm);
endfunction

//TODO FIX COORD e resto
function GetSpawnPosition (byref spawner_item, requiresurface, packrange := -1, packcoord,spawnpositioning := 0, mob:= 0 )//return struct with coordinates
//(bool requiresurface, int packrange, Point3D packcoord, List<SpawnPositionInfo> spawnpositioning, Mobile mob)
	//Broadcast ("GetSpawnPosition");
	var map := spawner_item.realm;

	if (!map)
		return Point3d(spawner_item.x,spawner_item.y,spawner_item.z);
	endif

	var positioning := 1;//random
	var trigmob;
	var positionargs;//string[]
	var includetilelist := array;//int
	var excludetilelist := array;//int
	var checkitems := 0;//bool

	// restrictions on tile flags
	//TODO: FIX TILEFLAGS
	//TileFlag tileflag = TileFlag.None;
	var locations := array;//List<Point3D> locations = null;
	var fillinc := 1;//int
	var positionrange := 0;//int
	var prefix := "";//string
	var WayList := array;//List<Item> WayList = null;
	var xinc := 0;
	var yinc := 0;
	var zinc := 0;
	var width := cint(GetObjProperty(spawner_item, "Width"));
	var height := cint(GetObjProperty(spawner_item, "Height"));
	var mostrecentspawnposition := GetObjProperty(spawner_item, "MostRecentSpawnPosition");
	var spawnerx := cint(GetObjProperty(spawner_item, "X"));
	var spawnery := cint(GetObjProperty(spawner_item, "Y"));

	if (spawnpositioning)
		foreach s in (spawnpositioning)
			if (!s)
				continue;
			endif
			trigmob := s.trigmob;
			positionargs := s.positionargs;
			case (s.positiontype)
//				    1	,   2	 ,    3	  ,     4    ,    5  ,    6    ,   7  ,       8      ,     9   ,  10,   11 ,    12  ,   13  ,    14
//public enum SpawnPositionType { Random, RowFill, ColFill, Perimeter, Player, Waypoint, RelXY, DeltaLocation, Location, Wet, Tiles, NoTiles, ItemID, NoItemID }
				10:
				// syntax Wet
				// find all of the wet tiles
				//tileflag |= TileFlag.Wet;
				requiresurface := 0;
				break;
				13://ItemID
				checkitems := 1;
				//goto case SpawnPositionType.Tiles;
				requiresurface := 0;
				var start := -1;
				var end := -1;
				if (positionargs && len(positionargs) > 1)
					start := cint(positionargs[2]);//era 1
				endif
				if (positionargs && len(positionargs) > 2)
					end := cint(positionargs[3]);
				endif
				if (!includetilelist)
					includetilelist := array;//new List<int>();
				endif
				// add the tiles to the list
				if (start > -1 && end < 0)
					includetilelist.append(start);
				else
					if (start > -1 && end > -1)
						var j;
						for ( j := start; j <= end; j+=1)
							includetilelist.append(j);
						endfor
					endif
				endif
				break;
				14://NOITEMID
				checkitems := 1;
				//goto case SpawnPositionType.NoTiles;
				requiresurface := 0;
				var start := -1;
				var end := -1;
				if (positionargs && len(positionargs) > 1)
					start := cint(positionargs[2]);
				endif
				if (positionargs && len(positionargs) > 2)
					end := cint(positionargs[3]);
				endif
				if (!excludetilelist)
					excludetilelist := array;//new List<int>();
				endif
				// add the tiles to the list
				if (start > -1 && end < 0)
					excludetilelist.append(start);
				else
					if (start > -1 && end > -1)
						var j;
						for (j := start; j <= end; j+=1)
							excludetilelist.append(j);
						endfor
					endif
				endif
				break;
				11:
				// syntax Tiles,start[,end]
				// get the tiles in the range
				requiresurface := 0;
				var start := -1;
				var end := -1;
				if (positionargs && len(positionargs) > 1)
					start := cint(positionargs[2]);//era 1
				endif
				if (positionargs && len(positionargs) > 2)
					end := cint(positionargs[3]);
				endif
				if (!includetilelist)
					includetilelist := array;//new List<int>();
				endif
				// add the tiles to the list
				if (start > -1 && end < 0)
					includetilelist.append(start);
				else
					if (start > -1 && end > -1)
						var j;
						for ( j := start; j <= end; j+=1)
							includetilelist.append(j);
						endfor
					endif
				endif
				break;
				
				12://NOTILES
				// syntax Tiles,start[,end]
				// get the tiles in the range
				requiresurface := 0;
				var start := -1;
				var end := -1;
				if (positionargs && len(positionargs) > 1)
					start := cint(positionargs[2]);
				endif
				if (positionargs && len(positionargs) > 2)
					end := cint(positionargs[3]);
				endif
				if (!excludetilelist)
					excludetilelist := array;//new List<int>();
				endif
				// add the tiles to the list
				if (start > -1 && end < 0)
					excludetilelist.append(start);
				else
					if (start > -1 && end > -1)
						var j;
						for (j := start; j <= end; j+=1)
							excludetilelist.append(j);
						endfor
					endif
				endif
				break;
				2:
				3:
				4:
				// syntax XFILL[,inc]
				// syntax YFILL[,inc]
				// syntax EDGE[,inc]
				positioning := s.positiontype;
				if (positionargs && len(positionargs) > 1)
					fillinc := cint(positionargs[2]);
				endif
				break;
				7:
				8:
				9:
				// syntax RELXY,xinc,yinc[,zinc]
				// syntax XY,x,y[,z]
				// syntax DXY,dx,dy[,dz]
				positioning := s.positiontype;
				if (positionargs && len(positionargs) > 2)
					xinc := cint(positionargs[2]);
					yinc := cint(positionargs[3]);
				endif
				if (positionargs && len(positionargs) > 3)
					zinc := cint(positionargs[4]);
				endif
				break;
				6:
				// syntax WAYPOINT,prefix[,range]
				positioning := s.positiontype;
				if (positionargs && len(positionargs) > 1)
					prefix := positionargs[2];
				endif
				if (positionargs && len(positionargs) > 2)
					positionrange := cint(positionargs[3]);
				endif
				// find a list of items that match the waypoint prefix
				if (len(prefix) > 0)
				/*TODO FIX
								// see if there is an existing hashtable for the waypoint lists
								if (spawnPositionWayTable == null)
								{
									spawnPositionWayTable = new Dictionary<string, List<Item>>();
								}

								// try to find the waypoint list in the local table
								//WayList = spawnPositionWayTable[prefix];

								// no existing list so create a new one
								if (!spawnPositionWayTable.TryGetValue(prefix, out WayList) || WayList == null)
								{
									WayList = new List<Item>();

									foreach (Item i in World.Items.Values)
									{
										if (i is WayPoint && !string.IsNullOrEmpty(i.Name) && i.Map == Map && i.Name == prefix)
										{
											// add it to the list of items
											WayList.Add(i);
										}
									}
									// add the new list to the local table
									spawnPositionWayTable[prefix] = WayList;
								}
				*/
				endif
				break;
				5:
				// syntax PLAYER[,range]
				positioning := s.positiontype;
				if (positionargs && len(positionargs) > 1)
					positionrange := (positionargs[2]);
				endif
				break;
			endcase
		endforeach
	endif

	// precalculate tile locations if they have been specified
	//TODO: TILEFLAG!
	/*if (len(includetilelist)>0 || len(excludetilelist)>0 )//|| tileflag != TileFlag.None)
		if (spawner.region != null && HasRegionPoints(spawner.region))
			FindRegionTileLocations(ref locations, spawner.region, includetilelist, excludetilelist, tileflag, checkitems, this.Z);
		elseif (positioning == 1)//random
					FindTileLocations(ref locations, this.Map, m_X, m_Y, spawner.width, m_Height, includetilelist, excludetilelist, tileflag, checkitems, this.Z);
		endif
	endif*/

	var reg := GetObjProperty(spawner_item, "Region");
	var i;
	for (i:= 0; i < 10; i+=1)
		var x := spawner_item.x;
		var y := spawner_item.y;
		var z := spawner_item.z;//solo Z! metto spawner_item.z TODO: verificare
		var realm := spawner_item.realm;
		var defaultZ := spawner_item.z;
		//Broadcast("GSP packrange:"+packrange+"; positioning:"+positioning+";");
		if (packrange >= 0 && packcoord.x != 0 && packcoord.y != 0 && packcoord.z != 0)
			defaultZ := packcoord.z;
			// find a random coord relative to the packcoord
			x := packcoord.x - packrange + RandomInt(packrange * 2 + 1);
			y := packcoord.y - packrange + RandomInt(packrange * 2 + 1);
		else
			if (reg && HasRegionPoints(reg))
				// if region spawning is selected then use that to find an x,y loc instead of the spawn box
				if (includetilelist || excludetilelist) // || tileflag != TileFlag.None)
					// use the precalculated tile locations
					if (locations && len(locations) > 0)
						var p := locations[RandomInt(len(locations))+1];
						x := p.x;
						y := p.y;
						defaultZ := p.z;
					endif
				else
					x:=spawner_item.x;
					y := spawner_item.y;
					/*TODO FIX!
					var p := GetRandomRegionPoint(spawner.region);//point2d
					x := p.x;
					y := p.y;
					*/
				endif
			else
				case (cint(positioning))
//				    1	,   2	 ,    3	  ,     4    ,    5  ,    6    ,   7  ,       8      ,     9   ,  10,   11 ,    12  ,   13  ,    14
//public enum SpawnPositionType { Random, RowFill, ColFill, Perimeter, Player, Waypoint, RelXY, DeltaLocation, Location, Wet, Tiles, NoTiles, ItemID, NoItemID }

				1://Random
				//Broadcast("GSP includetilelist:"+includetilelist+"; excludetilelist:"+excludetilelist+";");
				if (len(includetilelist) > 0 || len(excludetilelist)>0)//  || tileflag != TileFlag.None)
					if (locations && len(locations) > 0)
						var p := locations[RandomInt(len(locations))+1];
						x := p.x;
						y := p.y;
						defaultZ := p.z;
					endif
				else
					//Broadcast("GSP random, width:"+width+"; height:"+height+";");
					if (width > 0)
						x := spawnerx + RandomInt(width + 1);
					endif
					if (height > 0)
						y := spawnery + RandomInt(height + 1);
					endif
				endif
				break;
				7://RelXY
				x := mostrecentspawnposition.x + xinc;
				y := mostrecentspawnposition.y + yinc;
				defaultZ := mostrecentspawnposition.z + zinc;
				break;
				8: //DeltaLocation
				x := spawner_item.x + xinc;
				y := spawner_item.y + yinc;
				defaultZ := spawner_item.z + zinc;
				break;
				9://Location
				x := xinc;
				y := yinc;
				defaultZ := zinc;
				break;
				2:// RowFill
				x := mostrecentspawnposition.x + fillinc;
				y := mostrecentspawnposition.y;

				if (x < spawnerx)
					x := spawnerx;
				endif
				if (y < spawnery)
					y := spawnery;
				endif
				if (x > spawnerx + width)
					x := spawnerx + (x - spawnerx - width - 1);
					y+=1;
				endif
				if (y > spawnery + height)
					y := spawnery;
				endif
				break;
				3://ColFill
				x := mostrecentspawnposition.x;
				y := mostrecentspawnposition.y + fillinc;

				if (x < spawnerx)
					x := spawnerx;
				endif
				if (y < spawnery)
					y := spawnery;
				endif
				if (y > spawnery + height)
					y := spawnery + (y - spawnery - height - 1);
					x+=1;
				endif
				if (x > spawnerx + width)
					x := spawnerx;
				endif
				break;
				4://Perimeter
				x := mostrecentspawnposition.x;
				y := mostrecentspawnposition.y;

				// if the point is not on the perimeter, reset it to the corner
				if (x != spawnerx && x != spawnerx + width && y != spawnery && y != spawnery + height)
					x := spawnerx;
					y := spawnery;
				endif

				if (y == spawnery && x < spawnerx + width)
					x += fillinc;
				elseif (y == spawnery + height && x > spawnerx)
					x -= fillinc;
				elseif (x == spawnerx && y > spawnery)
					y -= fillinc;
				elseif (x == spawnerx + width && y < spawnery + height)
					y += fillinc;
				endif
				if (x > spawnerx + width)
					x := spawnerx + width;
				endif
				if (y > spawnery + height)
					y := spawnery + height;
				endif
				if (x < spawnerx)
					x := spawnerx;
				endif
				if (y < spawnery)
					y := spawnery;
				endif
				break;
				5://Player
				if (trigmob)
					x := trigmob.x;
					y := trigmob.y;
					if (positionrange > 0)
						x += RandomInt(positionrange * 2 + 1) - positionrange;
						y += RandomInt(positionrange * 2 + 1) - positionrange;
					endif
				endif
				break;
				6://Waypoint
				// pick an item randomly from the waylist
				/*TODO FIX
				if (WayList != null && WayList.Count > 0)
					int index = Utility.Random(WayList.Count);
					Item waypoint = WayList[index];
					if (waypoint != null)
						x = waypoint.Location.X;
						y = waypoint.Location.Y;
						defaultZ = waypoint.Location.Z;
						if (positionrange > 0)
							x += Utility.Random(positionrange * 2 + 1) - positionrange;
							y += Utility.Random(positionrange * 2 + 1) - positionrange;
						endif
					endif
				endif
				break;
				*/
				break;
				endcase
				//spawner.mostrecentspawnposition := Point3d(x, y, defaultZ,spawner_item.realm);
				SetObjProperty(spawner_item, "MostRecentSpawnPosition", Point3d(x, y, defaultZ,spawner_item.realm));//Point3d
			endif
		endif
		//Broadcast("GSP x:"+x+"; y:"+y+"; z:"+z+"; requiresurface:"+requiresurface);
		// skip invalid points
		if (x < 0 || y < 0 || (x == 0 && y == 0))
			continue;
		endif
		// try to find a valid spawn location using the z coord of the spawner
		// relax the normal surface requirement for mobiles if the flag is set
		var fit := 0;

		if (requiresurface)
			//fit := 1;
			fit := CanSpawnMobile(x, y, defaultZ, realm);//,mob for debug
		else
			//fit := 1;
			fit := CanFit(x, y, defaultZ, realm, 16, 1, 0, 0);
		endif

		// if that fails then try to find a valid z coord
		if (fit)
			return Point3d(x, y, defaultZ, realm);
		else
			z := GetMapInfo( x, y, realm ).z;//Map.GetAverageZ(x, y);
			if (requiresurface)
				fit := CanSpawnMobile(x, y, z, realm);//CanSpawnMobile(x, y, z, mob);
			else
				fit := CanFit(x, y, z, realm, 16, 1, 0, 0);//Map.CanFit(x, y, z, SpawnFitSize, true, false, false);
			endif
			if (fit)
				return Point3d(x, y, z, realm);
			endif
		endif
	endfor
	//Broadcast("GSP packrange:"+packrange+"; packcoordx:"+packcoord.x+"; y:"+packcoord.y+"; z:"+packcoord.z);
	if (packrange >= 0 && packcoord.x != 0 && packcoord.y != 0 && packcoord.z != 0)
		return packcoord;
	else
		return Point3d(spawner_item.x, spawner_item.y,spawner_item.z,spawner_item.realm);
	endif
endfunction

function HasIndividualSpawnTimes(byref spawner_item)
	var spawnobjects := GetObjProperty(spawner_item, "SpawnObjects");
	if (len(spawnobjects) > 0)
		var i;
		for (i := 1; i <= len(spawnobjects); i+=1)
			var so := spawnobjects[i];
			if (so.mindelay != -1 || so.maxdelay != -1)
				return 1;
			endif
		endfor
	endif

	return 0;
endfunction

function HasRegionPoints(r)//return bool
	if (r)// != null && r.Area.Length > 0) return true;
		return 1;
	else
		return 0;
	endif
endfunction

function HasSubGroups(byref spawner_item)
	var spawnobjects := GetObjProperty(spawner_item, "SpawnObjects");
	if (len(spawnobjects) < 1)// o non esistono
		return 0;
	endif
	var j;
	for (j := 1; j <= len(spawnobjects); j+=1)//spawnobjects.count
		if (spawnObjects[j].subgroup > 0)
			return 1;
		endif
	endfor

	return 0;
endfunction

//TODO CHECK COUNT?
function IsFull(byref spawner_item)//return bool
	var nobj := TotalSpawnedObjects(spawner_item);
	//Broadcast("Isfull, nobj:"+nobj+"; count:"+spawner.maxcount+"; totalsp:"+TotalSpawnObjectCount(spawner));
	//0	1		0		1
	if ((nobj >= cint(GetObjProperty(spawner_item, "Count")) ) || (nobj >= TotalSpawnObjectCount(spawner_item)))
		return 1;
	else
		return 0;
	endif
endfunction

function NextSpawn( byref spawner_item, value:=-1 )
	if (value < 0)//get
		if (cint(GetObjProperty(spawner_item, "Running")) == 1)
			return (cint(GetObjProperty(spawner_item, "End")) - readgameclock());
		else
			return 0;
		endif
	else//set
		StartSpawn(spawner_item);
		//nello startspawn c'e' già il dotimer, TODO: togliere? check se attivo? togliere il timer?
		DoTimer(spawner_item, value);
	endif
endfunction

function Point3d( X:= 0, Y:=0, Z:=0, Realm:=_DEFAULT_REALM)
	var point := struct;
	point.+x := X;
	point.+y := Y;
	point.+z := Z;
	point.+realm := realm;
	return point;
endfunction

// get spawn indices randomly from all available spawns of a group
function RandomAvailableSpawnIndex(byref spawner_item, sgroup)//return int
	var spawnobjects := GetObjProperty(spawner_item, "SpawnObjects");
	if (len(spawnobjects)<1)
		return -1;
	endif

	var maxrange := 0;
	var sgrouplist := array;
	var totalcount := 0;
	// make a pass to determine which subgroups are available for spawning
	// by finding any subgroups that do not have available spawns
	var i;
	for (i := 1; i <= len(spawnobjects); i+=1)
		var s := spawnobjects[i];
		if (s.subgroup > 0 && (s.ignore==1 || s.disabled==1))
			continue;
		endif

		totalcount += len(s.spawnedobjects);//count!
		if (s.subgroup > 0 && len(s.spawnedobjects) >= s.maxcount)
			// this subgroup is not available so add it to the list
			if (!sgrouplist)
				sgrouplist := array;//new List<int>();
			endif
			sgrouplist.append(s.subgroup);
		endif
	endfor

	for (i := 1; i <= len(spawnobjects); i+=1)
		var s := spawnobjects[i];

		if (s.subgroup > 0 && (s.ignore==1 || s.disabled==1))
			continue;
		endif
		//TODO: FIX CONTAINS
		if ((s.maxcount > len(s.spawnedobjects)) && (sgroup < 0 || sgroup == s.subgroup)
				/*&& (!sgrouplist || !sgrouplist.Contains(s.SubGroup))*/ && (s.subgroup <= 0 || SubGroupCount(spawner_item, s.subgroup) + totalcount <= cint(GetObjProperty(spawner_item, "Count")) ))
			// keep track of the number of spawn objects that are not at max (hence available for spawning)
			// this will be used to compute the probabilistic weighting function based on the relative
			// maxcounts of each entry
			maxrange += s.maxcount;
			spawnobjects[i].available := 1;//was s TODO: SAVE?
		else
			spawnobjects[i].available := 0;
		endif
	endfor
	//salvo? should be temporary
	//SetObjProperty(spawner_item, "SpawnObjects", spawner.spawnobjects);
	//broadcast("maxrange:"+maxrange);
	if (maxrange > 0)
		var randindex := RandomInt(cint(maxrange))+1;//+1?
		// and map it into the avail spawns
		var currentrange := 1;//era 0 metto 1.
		for (i := 1; i <= len(spawnobjects); i+=1)
			var s := spawnobjects[i];
			if (s.subgroup > 0 && (s.ignore==1 || s.disabled==1))
				continue;
			endif
			//broadcast("available:"+s.available+"; randind:"+randindex+"; currentrange:"+currentrange+"; maxc:"+s.maxcount);
			// keep track of the number of spawn objects that are not at max (hence available for spawning)
			if (s.available == 1)
				// check to see if the random value maps into the range of the current index
				//	1	>=	0		1	<	0	+	1
				if (randindex >= currentrange && randindex < currentrange + s.maxcount)//metto <= alla seconda?
					return (i);
				endif
				currentrange += s.maxcount;
			endif
		endfor
		// should never get here
		return (-1);
	else
		// no spawns are available
		return (-1);
	endif
endfunction

function RandomSpawnIndex(byref spawner_item, sgroup)//return int
	var spawnobjects := GetObjProperty(spawner_item, "SpawnObjects");
	if (len(spawnobjects) < 1)
		return (-1);
	endif

	var avail := 0;
	var maxrange := 0;
	var i;
	for (i:=1; i <= len(spawnobjects); i+=1)
		var s := spawnobjects[i];

		// keep track of the number of spawn objects that are not at max (hence available for spawning)
		if (sgroup < 0 || (sgroup == s.subgroup))
			avail+=1;
			maxrange += cint(s.maxcount);//if spawner disabled maxcount == 0
		endif
	endfor
	// now generate a random number over the available spawnobjects
	if (avail > 0 && maxrange > 0)
		var randindex := RandomInt(cint(maxrange));//Utility.Random(maxrange);
		// and map it into the avail spawns
		var currentrange := 0;

		for (i := 1; i <= len(spawnobjects); i+=1)
			var s := spawnobjects[i];

			// keep track of the number of spawn objects that are not at max (hence available for spawning)
			if (sgroup < 0 || (sgroup == s.subgroup))
				if (randindex >= currentrange && randindex < currentrange + cint(s.maxcount))//if spawner disabled maxcount == 0
					return (i);
				endif

				currentrange += cint(s.maxcount);//if spawner disabled maxcount == 0
			endif
		endfor
		// should never get here
		return (-1);
	else
		// no spawns are available
		return (-1);
	endif
endfunction

//TODO VERIFICARE IL NEXTSPAWN
function RefreshNextSpawnTime(byref spawner_item, byref so)
	if (!so)
		return 0;
	endif
	var mind := cint(so.mindelay);//mindelay lo lascio già in secondi. tolgo il *60
	var maxd := cint(so.maxdelay);
	if (mind < 0 || maxd < 0)
		so.nextspawn := readgameclock();//NextSpawn(spawner_item, spawner, readgameclock()); //so.nextspawn := readgameclock();//DateTime.Now;
	else
		var delay := RandomIntRange(mind, maxd);
		so.nextspawn := readgameclock() + delay;//NextSpawn(spawner_item, spawner, readgameclock() + delay);//so.nextspawn := readgameclock() + delay;
	endif

	//TODO: il nextspawn restarta lo spawn, inutile salvare? salvo spawnedobj, forse meglio fuori alla fine.
	//SetObjProperty(spawner_item, "SpawnObjects", spawner.spawnobjects);
endfunction

function RemoveSpawnObject(byref spawner_item)
	Defrag(spawner_item, 0);
	var spawnobjects := GetObjProperty(spawner_item, "SpawnObjects");
	if (len(spawnobjects) < 1)
		return 0;
	endif

	//TODO: FIX TAGS!
	//ClearTags(spawner, 1);
	//var deletelist := array;
	var deletelist := array;
	var i;
	foreach so in (spawnobjects)
		for (i := 1; i <= len(so.spawnedobjects); i+=1)
			var o := SystemFindObjectBySerial(so.spawnedobjects[i]);

			if (o.isA(POLCLASS_ITEM) || o.isA(POLCLASS_MOBILE))
				//broadcast("removespawnobject found; :"+so.spawnedobjects[i] );
				//PrintTextAbove( spawner_item, "removespawnobject found; :"+so.spawnedobjects[i] );
				deletelist.Append(so.spawnedobjects[i]);
			//else
			//	broadcast("removespawnobject problem" );
			endif
		endfor
	endforeach

	DeleteFromList(deletelist);

	// Defrag again
	Defrag(spawner_item, 0);
endfunction

function RemoveSpawnObjects(byref spawner_item, byref so)
	//PrintTextAbove( spawner_item, "test remove" );
	if (!so)
		return 0;
	endif

	Defrag(spawner_item, 0);

	var deletelist := array;//new List<object>();
	var i;

	for (i := 1; i <= len(so.spawnedobjects); i+=1)
		var o := SystemFindObjectBySerial(so.spawnedobjects[i]);

		if (o.isA(POLCLASS_ITEM) || o.isA(POLCLASS_MOBILE))
			deletelist.Append(so.spawnedobjects[i]);
		endif

	endfor

	DeleteFromList(deletelist);

	// Defrag again
	Defrag(spawner_item, 0);
endfunction

function Reset(byref spawner_item)
	Stop(spawner_item);
	// reset the protection against runaway looping
	ClearSpawnedThisTick(spawner_item, 1);
	RemoveSpawnObject(spawner_item);
	//should be in RemoveSpawnObject already!
	//ClearTags(true);
	ResetAllFlags(spawner_item);
	SetObjProperty(spawner_item, "status_str", "");
	//spawner.status_str := "";
	//già in resetallflags
	//m_killcount = 0;
	EraseObjProperty(spawner_item, "OnHold");
	//spawner.mostrecentspawnposition := Point3d();
	SetObjProperty(spawner_item, "MostRecentSpawnPosition", Point3d());
	//TODO: FIX
	//spawnPositionWayTable = null;
	//spawner.holdsequence := 1;
	SetObjProperty(spawner_item, "HoldSequence", 1 );
	//spawner.isinactivated := 0;
	SetObjProperty(spawner_item, "IsInactivated", 0 );
	ResetSequential(spawner_item);
endfunction

//TODO FIX
function ResetAllFlags(byref spawner_item)
	//spawner.proximityactivated := 0;
	SetObjProperty(spawner_item, "ProximityActivated", 0);
	//spawner.exttrigstate := 0;
	SetObjProperty(spawner_item, "ExternalTrigger", 0);//bool
	//spawner.duractivated := 0;
	SetObjProperty(spawner_item, "DurActivated", 0);//bool
	//spawner.refractactivated := 0;
	SetObjProperty(spawner_item, "RefractActivated", 0);//bool
	//check mob - int
	//spawner.triggermob := 0;
	SetObjProperty(spawner_item, "mob_who_triggered", 0);
	//spawner.triggerskill //m_skill_that_triggered = XmlSpawnerSkillCheck.RegisteredSkill.Invalid;
	//spawner.killcount := 0;
	SetObjProperty(spawner_item, "KillCount", 0 );
	//m_GumpState = null;
	//spawner.freerun := 0;
	SetObjProperty(spawner_item, "FreeRun", 0);
endfunction

function ResetNextSpawnTimes(byref spawner_item)
	var spawnobjects := GetObjProperty(spawner_item, "SpawnObjects");
	if (len(spawnobjects) > 0)
		var i;
		for (i := 1; i <= len(spawnobjects); i+=1)//todo: usare foreach? test.
			//var so := spawnobjects[i];

			spawnobjects[i].nextspawn := readgameclock();
		endfor
		SetObjProperty(spawner_item, "SpawnObjects", spawnobjects );
	endif
endfunction

//TODO CHECK FREERUN?
function ResetProximityActivated( byref spawner_item )
	// dont reset triggering if free run mode has been selected
	if (cint(GetObjProperty(spawner_item, "FreeRun")) == 0)
		//spawner.proximityactivated := 0;
		SetObjProperty(spawner_item, "ProximityActivated", 0);
	endif
endfunction

function ResetSequential(byref spawner_item)
	// go back to the lowest level
	//TODO FIX? default -1
	//if (spawner.sequentialspawn >= 0)
	//	spawner.sequentialspawn := NextSequentialIndex(-1);
	//	SetObjProperty(spawner_item, "SequentialSpawning", spawner.sequentialspawn);
	//endif

	// reset the nextspawn times
	ResetNextSpawnTimes(spawner_item);

	// and reset the kill count
	//spawner.killcount := 0;
	SetObjProperty(spawner_item, "KillCount", 0 );
endfunction

function Respawn(byref spawner_item )//return bool

	//spawner.inrespawn := 1;
	SetObjProperty(spawner_item, "InRespawn", 1);
	//spawner.isinactivated := 0;
	SetObjProperty(spawner_item, "IsInactivated", 0);//bool
	// reset the protection against runaway looping
	ClearSpawnedThisTick(spawner_item, 1);
	// Delete all currently spawned objects
	RemoveSpawnObject(spawner_item);
	// added the explicit start.  Previously it relied on the automatic start that occurred when the spawnobject list was updated.
	StartSpawn(spawner_item);

	ResetNextSpawnTimes(spawner_item);

	// Respawn all objects up to the spawners current maximum allowed
	// note that by default, for proximity sensing, the spawner will only trigger once, but for respawns allow them all
	var keepProximityActivated := cint(GetObjProperty(spawner_item, "ProximityActivated"));//bool
	var triedtospawn := 0;

	// attempt to spawn up to the MaxCount of the spawner
	var x;
	var maxcount := cint(GetObjProperty(spawner_item, "Count"));
	for (x:= 0; x < maxcount; x+=1)
		//CHECK?

		triedtospawn := Spawn(spawner_item,0,0);

		sleepMS(2);
		if (x < maxcount - 1 || GetObjProperty(spawner_item, "OnHold") )
			SetObjProperty(spawner_item, "ProximityActivated", keepProximityActivated);//spawner.proximityactivated := keepProximityActivated;
		endif
	endfor
	/* TODO FIX
	if (cint(GetObjProperty(spawner_item, "FreeRun")) == 0)
		m_mob_who_triggered = null;
		m_skill_that_triggered = XmlSpawnerSkillCheck.RegisteredSkill.Invalid;
	endif
	*/
	//TODO FIX TAGS
	//ClearTags(1);

	//spawner.inrespawn := 0;
	EraseObjProperty(spawner_item, "InRespawn");

	return cint(triedtospawn);
endfunction

// this can be used in loops over world objects since it will not defrag and potentially modify the world object lists
function SafeTotalSpawnedObjects(byref spawner_item)//return int
	var spawnobjects := GetObjProperty(spawner_item, "SpawnObjects");
	if (len(spawnobjects) < 1)
		return 0;
	endif

	var count := 0;

	foreach so in (spawnobjects)
		count += len(so.spawnedobjects);//so.spawnedobjects.count
	endforeach

	return cint(count);
endfunction

//TODO FIX
function ShowBounds(byref spawner_item, value:=-1)
	var showboundsitem := GetObjProperty(spawner_item, "ShowBoundsItems"));
	if (value < 0)
		return (showboundsitem && len(showboundsitem) > 0)
	endif
/*
	if ((value == 1) && !(spawner.showboundsitem && len(spawner.showboundsitem) > 0))
					if (m_ShowBoundsItems == null) m_ShowBoundsItems = new List<Static>();

					// Boundary lines
					int ValidX1 = m_X;
					int ValidX2 = m_X + m_Width;
					int ValidY1 = m_Y;
					int ValidY2 = m_Y + m_Height;

					for (int x = 0; x <= m_Width; x++)
					{
						int NewX = m_X + x;
						for (int y = 0; y <= m_Height; y++)
						{
							int NewY = m_Y + y;

							if (NewX == ValidX1 || NewX == ValidX2 || NewX == ValidY1 || NewX == ValidY2 || NewY == ValidX1 || NewY == ValidX2 || NewY == ValidY1 || NewY == ValidY2)
							{
								// Add an object to show the spawn area
								Static s = new Static(ShowBoundsItemId);
								s.Visible = false;
								s.MoveToWorld(new Point3D(NewX, NewY, Z), this.Map);
								m_ShowBoundsItems.Add(s);
							}
						}
					}
	endif

	if (value == 0 && spawner.showboundsitems)
					// Remove all of the items from the array
					foreach (Static s in m_ShowBoundsItems)
						s.Delete();

					m_ShowBoundsItems.Clear();
	endif
*/
endfunction

// select and spawn something
// return false if it cannot spawn, e.g. there is nothing to spawn or it is a triggerable spawner and has not been triggered
function Spawn(byref spawner_item, smartspawn, loops)//return bool
	//TODO: FIX TODInRange
	var spawnobjects := GetObjProperty(spawner_item, "SpawnObjects");
	//Broadcast ("spawn; objectcount:"+len(spawnobjects)+"; proximity:" +GetObjProperty(spawner_item, "ProximityActivated")+" canfree:"+CanFreeSpawn(spawner_item) );

	if (len(spawnobjects) > 0 && (cint(GetObjProperty(spawner_item, "ProximityActivated")) == 1 || CanFreeSpawn(spawner_item)) /* && TODInRange(spawner)*/)
		//TODO: CHECK!
		//spawner.holdsequence := 0;
		SetObjProperty(spawner_item, "HoldSequence", 0);
		//Broadcast ("spawn 1, isfull:"+IsFull(spawner_item));
		// if the spawner is full then dont bother
		if (IsFull(spawner_item) == 1)
			ResetProximityActivated(spawner_item);
			return 1;
		endif

		// Pick a spawn object to spawn
		var SpawnIndex := 1;//era 0

		// see if sequential spawning has been selected
		if (cint(GetObjProperty(spawner_item, "SequentialSpawning")) >= 0)//off by default
			// if so then use its value to get the index of the first available spawn entry in the next subgroup to be spawned
			// note, if the current sequence finds a zero group then the spawn will be picked at random from it
			//TODO FIX
			//SpawnIndex := GetCurrentAvailableSequentialSpawnIndex(m_SequentialSpawning);
		else
			// if sequential spawning is not set then select the next spawn at random
			SpawnIndex := RandomAvailableSpawnIndex(spawner_item, -1);
		endif
		//Broadcast ("spawn 1.5, spawnindex:"+SpawnIndex);

		// no spawns are available so no point in continuing
		if (SpawnIndex < 1)
			ResetProximityActivated(spawner_item);
			return 1;
		endif

		//var sobj := spawner.spawnobjects[SpawnIndex];
		var sgroup := cint(spawnobjects[SpawnIndex].subgroup);

		// if this is part of a non-zero group, then spawn all of the group members as well
		if (sgroup != 0)
			SpawnSubGroup(spawner_item, sgroup, smartspawn, 0, loops);
		else
			// Found a valid spawn object so spawn it and see if it successful
			//TODO FIX!
			//Spawn(SpawnIndex, smartspawn, sobj.SpawnsPerTick, loops)
			//Spawn(int index, bool smartspawn, int count, byte loops)
			//Spawn(index, smartspawn, count, false, loops);
			//Spawn(int index, bool smartspawn, int count, bool ignoreloopprotection, byte loops)
			//Spawn(index, smartspawn, count, -1, Point3D.Zero, ignoreloopprotection, loops);
			//Spawn(int index, bool smartspawn, int count, int packrange, Point3D packcoord, bool ignoreloopprotection, byte loops)
			if (Spawn2(spawner_item, SpawnIndex, smartspawn, spawnobjects[SpawnIndex].spawnspertick, -1, Point3d(0,0,0,spawner_item.realm), 0, loops) == 1)
				if (smartspawn==0)
					//25/05/2016 ricarico spawnobjects che si è modificato durante lo spawn: TODO FIX?
					spawnobjects := GetObjProperty(spawner_item, "SpawnObjects");
					RefreshNextSpawnTime(spawner_item, spawnobjects[SpawnIndex]);//refresh devo salvare dopo gli spawnobj.
					SetObjProperty(spawner_item, "SpawnObjects", spawnobjects);
				endif
			endif
		endif

		ResetProximityActivated(spawner_item);
		return 1;
	endif

	ResetProximityActivated(spawner_item);
	return 0;
endfunction

//TODO: FIX!
// spawn an individual entry by index up to count times
function Spawn2(byref spawner_item, index, smartspawn, count, packrange, packcoord, ignoreloopprotection, loops)
//(int index, bool smartspawn, int count, int packrange, Point3D packcoord, bool ignoreloopprotection, byte loops)
	//Broadcast("spawn2");
	Defrag(spawner_item, 0);
	var spawnobjects := GetObjProperty(spawner_item, "SpawnObjects");
	//Broadcast ("spawn2 lenspobj:"+len(spawnobjects)+"; index:"+index);
	if (len(spawnobjects) < 1 || index > len(spawnobjects))
		return 0;
	endif
	var didspawn := 0;
	var so := spawnobjects[index];
	//Broadcast ("spawn2 len(so):"+len(so));
	if (!so)
		return 0;
	endif

	// make sure you dont go over the individual entry maxcount
	var somax := so.maxcount;
	var socnt := len(so.spawnedobjects);//count
	var nspawn := so.spawnspertick;
	var scnt := SafeTotalSpawnedObjects(spawner_item); //spawner.safecurrentcount;//TODO CHECK!
	var maxcount := cint(GetObjProperty(spawner_item, "Count"));//TODO: check sullo script

	var k;
	//Broadcast ("spawn2 nspawn:"+nspawn+"; socnt:"+socnt+";somax:"+somax+";scnt:"+scnt+";maxcount:"+maxcount);
			//k < 1 &&	k + 0 < 1 &&		k+0 < 1
	for (k := 0; (k < nspawn) && (k + socnt < somax) && (k + scnt < maxcount ); k+=1)
		if (packrange >= 0 && so.subgroup > 0 && packcoord.x == 0 && packcoord.y == 0 && packcoord.z == 0)//TODO FIX POINT ZERO
			packcoord := GetPackCoord(spawner_item, so.subgroup);
		endif
		if (Spawn3(spawner_item, index, smartspawn, packrange, packcoord, ignoreloopprotection, loops))
			// if any of the attempts were successful then flag it as having spawned
			//TotalSpawnedObjects(spawner_item);//ok fin qui
			didspawn := 1;
		endif
	endfor

	return didspawn;
endfunction

// spawn an individual entry by index
function Spawn3(byref spawner_item, index, smartspawn, packrange, packcoord, ignoreloopprotection, loops)
//(int index, bool smartspawn, int packrange, Point3D packcoord, bool ignoreloopprotection, byte loops)
	//Defraggo prima di prenderli Remove any spawns that don't belong to the spawner any more.
	//broadcast("spawn3");
	Defrag(spawner_item, 0);

	var spawnobjects := GetObjProperty(spawner_item, "SpawnObjects");
	var map := spawner_item.realm;

	// Make sure everything is ok to spawn an object
	if (!map || !spawnobjects || len(spawnobjects) == 0 || index <= 0 || index > len(spawnobjects))
		return 0;
	endif

	// Get the spawn object at the required index
	var TheSpawn := spawnobjects[index];
	var trigmob := GetObjProperty(spawner_item, "mob_who_triggered");
	//broadcast("spawn3 1");
	if (TheSpawn)

		// dont allow an entry to be spawned more than once per tick
		// this protects against runaway recursive looping
		if (TheSpawn.spawnedthistick && !ignoreloopprotection)
			return 0;
		endif
		//TODO: CHECK NEXTSPAWN!
		// check the nextspawn time to see if it is available
		if (TheSpawn.nextspawn > readgameclock())
			return 0;
		endif
		//Broadcast ("spawn3 1.3 totalsp:"+TotalSpawnedObjects(spawner_item, spawner)+"; maxc:"+spawner.maxcount);
		var CurrentCreatureMax := cint(TheSpawn.maxcount);
		var CurrentCreatureCount := len(TheSpawn.spawnedobjects);//count

		// Check that the current object to be spawned has not reached its maximum allowed
		// and make sure that the maximum spawner count has not been exceeded as well
		if ((CurrentCreatureCount >= CurrentCreatureMax) ||
			(TotalSpawnedObjects(spawner_item) >= cint(GetObjProperty(spawner_item, "Count")) ))
				return 0;
		endif
		//Broadcast ("spawn3 1.4");
		// check for string substitions
		//TODO FIX:!
		var substitutedtypeName := cstr(ApplySubstitution(spawner_item, trigmob, TheSpawn.typename));

		// random positioning is the default
		var spawnpositioning := array;

		// require valid surfaces by default
		var requiresurface := 1;
		var triggermob := GetObjProperty(spawner_item, "mob_who_triggered");

		while (substitutedtypeName[1] == "#")
			var args := ParseSemicolonArgs(substitutedtypeName, 2);
			if (len(args)>0)
				if (!spawnpositioning)
					spawnpositioning := array;
				endif
				// parse any comma args
				var keyvalueargs := ParseCommaArgs(args[1], 10);
				if (len(keyvalueargs)>0)
					
					//				    1	,   2	 ,    3	  ,     4    ,    5  ,    6    ,   7  ,       8      ,     9   ,  10,   11 ,    12  ,   13  ,    14
					//public enum SpawnPositionType { Random, RowFill, ColFill, Perimeter, Player, Waypoint, RelXY, DeltaLocation, Location, Wet, Tiles, NoTiles, ItemID, NoItemID }
					case (keyvalueargs[1])
						"#NOITEMID":
						spawnpositioning.append( SpawnPositionInfo(14, triggermob, keyvalueargs));
						break;
						"#ITEMID":
						spawnpositioning.append( SpawnPositionInfo(13, triggermob, keyvalueargs));
						break;
						"#NOTILES":
						spawnpositioning.append( SpawnPositionInfo(12, triggermob, keyvalueargs));
						break;
						"#TILES":
						spawnpositioning.append( SpawnPositionInfo(11, triggermob, keyvalueargs));
						break;
						"#WET":
						spawnpositioning.append( SpawnPositionInfo(10, triggermob, keyvalueargs));
						break;
						"#XFILL":
						spawnpositioning.append( SpawnPositionInfo(2, triggermob, keyvalueargs));
						break;
						"#YFILL":
						spawnpositioning.append( SpawnPositionInfo(3, triggermob, keyvalueargs));
						break;
						"#EDGE":
						spawnpositioning.append( SpawnPositionInfo(4, triggermob, keyvalueargs));
						break;
						"#PLAYER":
						spawnpositioning.append( SpawnPositionInfo(5, triggermob, keyvalueargs));
						break;
						"#WAYPOINT":
						spawnpositioning.append( SpawnPositionInfo(6, triggermob, keyvalueargs));
						break;
						"#RELXY":
						spawnpositioning.append( SpawnPositionInfo(7, triggermob, keyvalueargs));
						break;
						"#DXY":
						spawnpositioning.append( SpawnPositionInfo(8, triggermob, keyvalueargs));
						break;
						"#XY":
						spawnpositioning.append( SpawnPositionInfo(9, triggermob, keyvalueargs));
						break;
						"#CONDITION":
						// test the specified condition string
						// syntax is #CONDITION,proptest
						// reparse with only one arg after the comma, this allows property tests that use commas as well
						var ckeyvalueargs := ParseCommaArgs(args[1], 2);
						if (len(ckeyvalueargs) > 1)
							// dont spawn if it fails the test ckeyvalueargs era 1 metto 2
							//TODO FIX!
							//if (CheckPropertyString(spawner, spawner_item, ckeyvalueargs[2], spawner.triggermob, spawner.status_str)==0)
							//	return 0;
							//endif
						else
							SetObjProperty(spawner_item, "status_str", "invalid #CONDITION specification: " + args[1]);
						endif
						break;
						default:
						SetObjProperty(spawner_item, "status_str", "invalid # specification: " + args[1]);
						break;
					endcase
					
				endif
			endif

			if (len(args)>1)
				substitutedtypeName := TrimStr(args[2]);//dovrebbe tornare la seconda, metto 2
			else
				substitutedtypeName := "";
			endif
		endwhile

		if (substitutedtypeName[1] == "*")
			requiresurface := 0;
			substitutedtypeName[1] := "";
		endif

		spawnobjects[index].requiresurface := requiresurface;
		SetObjProperty(spawner_item, "SpawnObjects", spawnobjects );
		//TODO CHECK: SAVE
		var typeName := ParseObjectType(substitutedtypeName);

		if ( IsTypeOrItemKeyword(typeName) )
			var status_str := "";
			//TODO: FIX!
			return 0;

			//TODO FIX XMLGUMPCALLBACK
			var completedtypespawn := SpawnTypeKeyword(spawner_item, TheSpawn, typeName, substitutedtypeName, requiresurface, spawnpositioning,
						trigmob, Point3d(spawner_item.x, spawner_item.y, spawner_item.z, spawner_item.realm), /*XmlGumpCallback(SpawnerGumpCallback)*/ 0, status_str, loops);

			//passato dal ref!
			//if (status_str != null)
			//	this.status_str = status_str;

			if (completedtypespawn)
				// successfully spawned the keyword
				// note that returning true means that Spawn will assume that it worked and will not try to respawn something else
				// added the duration timer that begins on spawning
				//TODO FIX TIMER2
				DoTimer2(spawner_item, cint(GetObjProperty(spawner_item, "Duration")) );

				//InvalidateProperties();

				return 1;
			else
				return 0;
			endif
		else
			// its a regular type descriptor so find out what it is
			var type := GetType(typeName);//TODO: forse conviene salvare il tipo e non richiederlo sempre
			// dont try to spawn invalid types, or Mobile type spawns in containers
			if (type && !(spawner_item.container && NPC_GetNPCConfig( type ) ) )
				//Broadcast ("spawn3 1.8");
				var arglist := ParseString(substitutedtypeName, 3, "/");

				//TODO CHECK!
				//object o = CreateObject(type, arglist[0]);
				if (!type)//(o == null)
					SetObjProperty(spawner_item, "status_str", "invalid type specification: " + arglist[1]);
					return 1;
				endif

				if (NPC_GetNPCConfig( type ))//todo controllare errortext? //type.isA(POLCLASS_MOBILE))
					//Broadcast ("spawn3 1.9");
					if (spawner_item.container)
						SetObjProperty(spawner_item, "status_str", "container problem.");
						return 1;
					endif
					//Broadcast ("spawn3 2.0");
					var loc := GetSpawnPosition(spawner_item, requiresurface, packrange, packcoord, spawnpositioning, type);
					var m:= NewCreateNpcFromTemplate( type, loc.x, loc.y, loc.z, /*override_properties :=*/ 0, loc.realm);
					if (m.errortext == "Not a valid location for an NPC!")
						//print("not a valid spawn location");
						var prove := 0;
						while (prove < 5)//provo 5 locations
							loc := GetSpawnPosition(spawner_item, requiresurface, packrange, packcoord, spawnpositioning, type);
							m:= NewCreateNpcFromTemplate( type, loc.x, loc.y, loc.z, /*override_properties :=*/ 0, loc.realm);
							if ( m.errortext )
								prove +=1;
							else
								prove := 100;
								//PrintTextAbove(spawner_item, "creating "+m.name+" at "+loc.x+","+loc.y+","+loc.z);
								break;
							endif
							sleepms(2);
						endwhile
						if (prove < 100)
							m := NewCreateNpcFromTemplate( type, spawner_item.x, spawner_item.y, spawner_item.z, /*override_properties :=*/ 0, spawner_item.realm);
							//PrintTextAbove(spawner_item, "creating "+m.name+" here. Consider fix spawn location.");
						endif
					elseif (m.errortext)
						m := NewCreateNpcFromTemplate( type, spawner_item.x, spawner_item.y, spawner_item.z, /*override_properties :=*/ 0, spawner_item.realm);
						//broadcast("error creating: " + m.errortext);
						//PrintTextAbove(spawner_item, "creating "+m.name+" here. Location problem.");
					else
						//PrintTextAbove(spawner_item, "creating "+m.name+" at "+loc.x+","+loc.y+","+loc.z);
					endif
					// add the mobile to the spawned list
					//FIX!
					(spawnobjects[index]).spawnedobjects.append(m.serial);//col thespawn non conta bene?; salvo il seriale e non il mob
					SetObjProperty(spawner_item, "SpawnObjects", spawnobjects );
					SetObjProperty(m, "Spawner", spawner_item.serial);//metto il seriale
					//SleepMS(2);
					//Broadcast("total now:"+SafeTotalSpawnedObjects(spawner_item));

					//if (smartspawn == 0)
					//	OnBeforeSpawn(o);
					//endif
					//m.MoveToWorld(loc, map);

					/*
						c.RangeHome = m_HomeRange;
						c.CurrentWayPoint = m_WayPoint;
						if (m_Team > 0)
							c.Team = m_Team;

						// Check if this spawner uses absolute (from spawnER location)
						// or relative (from spawnED location) as the mobiles home point
						if (m_HomeRangeIsRelative == true)
							c.Home = m.Location; // Mobiles spawned location is the home point
						else
							c.Home = this.Location; // Spawners location is the home point
					*/
					//NPC_SetAnchorInfos
					SetObjProperty(m, "HomeRange", cint(GetObjProperty(spawner_item, "HomeRange")) );

					// Check if this spawner uses absolute (from spawnER location)
					// or relative (from spawnED location) as the mobiles home point
					if ( cint(GetObjProperty(spawner_item, "HomeRangeIsRelative")) == 1 )
						var location := struct;
							location.+x := m.x;
							location.+y := m.y;
						SetObjProperty(m, "StartingPoint", location );
					else
						var location := struct;
							location.+x := spawner_item.x;
							location.+y := spawner_item.y;
						SetObjProperty(m, "StartingPoint", location );
					endif

					sleepms(2);
					//if (smartspawn == 0)
					//	OnAfterSpawn(o);
					//endif

					// apply the parsed arguments from the typestring using setcommand
					// be sure to do this after setting map and location so that errors dont place the mob on the internal map
					var status_str;

					ApplyObjectStringProperties(substitutedtypeName, m, trigmob, spawner_item, status_str);
					/*TODO FIX
					// if the object has an OnAfterSpawnAndModify method, then invoke it
					//BaseXmlSpawner.InvokeOnAfterSpawnAndModify(o);

								#region mod by Dies Irae
								if( m is BaseCreature && m_HomeRangeIsRelative )
									( (BaseCreature)m ).Home = m.Location;
								#endregion

								if (status_str != null)
								{
									this.status_str = status_str;
								}

								InvalidateProperties();

								m.InvalidateProperties(); // mod by Dies Irae

								// added the duration timer that begins on spawning
								DoTimer2(m_Duration);

								return true;
						*/
				else //if (type.isA(POLCLASS_ITEM) || GetObjTypeByName(type))
					//Broadcast ("spawn3 2.1");
					//Item item = (Item)o;

					var stat_str;

					//TODO FIX
					//BaseXmlSpawner.AddSpawnItem(this, TheSpawn, item, this.Location, map, m_mob_who_triggered, requiresurface, spawnpositioning, substitutedtypeName, smartspawn, out stat_str);
		//public static void AddSpawnItem(XmlSpawner spawner, XmlSpawner.SpawnObject theSpawn, Item item, Point3D location, Map map, Mobile trigmob, bool requiresurface,
		//	List<XmlSpawner.SpawnPositionInfo> spawnpositioning, string propertyString, bool smartspawn, out string status_str)
		//	AddSpawnItem(spawner, spawner, theSpawn, item, location, map, trigmob, requiresurface, spawnpositioning, propertyString, smartspawn, out status_str);

					//(spawnobjects[index]).spawnedobjects.append(m.serial);
					//PrintTextAbove( spawner_item, "te:"+cstr(spawnobjects[index].spawnedobjects));
					AddSpawnItem(spawner_item , spawnobjects[index], type, Point3d(spawner_item.x,spawner_item.y,spawner_item.z,spawner_item.realm), map, trigmob, requiresurface, spawnpositioning, substitutedtypeName, smartspawn, stat_str);
					//PrintTextAbove( spawner_item, "te:"+cstr(spawnobjects[index].spawnedobjects));
					SetObjProperty(spawner_item, "SpawnObjects", spawnobjects );
					SleepMS(2);
	//TotalSpawnedObjects(spawner_item); //mi da defrag count 1 e tso 1
	/*conta uno ma se defraggo conta 0?
	var count := 0;
	foreach so in (spawnobjects)
		//broadcast("TRY LEN:"+len(so.spawnedobjects)+"; OBJ SER:"+so.spawnedobjects[1]);
		count += len(so.spawnedobjects);//so.spawnedobjects.count
	endforeach
	PrintTextAbove( spawner_item, "count:"+count);
	*/
					if (len(stat_str)>0)
						SetObjProperty(spawner_item, "status_str", stat_str);//todo save?
					endif

					//InvalidateProperties();
					//item.InvalidateProperties(); // mod by Dies Irae

					// added the duration timer that begins on spawning
					//DoTimer2(m_Duration);

					return 1;
				endif
			else
				//Broadcast ("spawn3 notype");
				SetObjProperty(spawner_item, "status_str", "invalid type specification: " + typeName);
				return 1;
			endif
		endif
	endif
	return 0;

endfunction
/*
// Spawn4(spawner_item, string SpawnObjectTypeName, bool smartspawn, int packrange, Point3D packcoord, byte loops)
// spawn an individual entry by spawn object
*/
function Spawn4(byref spawner_item, SpawnObjectTypeName, smartspawn, packrange:=-1, packcoord, loops)
	var spawnobjects := GetObjProperty(spawner_item, "SpawnObjects");
	if (len(spawnobjects) < 1)
		return;
	endif
	var i;
	for (i := 1; i <= len(spawnobjects); i+=1)
		if (Upper((spawnobjects[i]).typename) == Upper(SpawnObjectTypeName))
			//Spawn(i, smartspawn, packrange, packcoord, loops)
			//Spawn(int index, bool smartspawn, int packrange, Point3D packcoord, byte loops)
			//Spawn(index, smartspawn, packrange, packcoord, false, loops);
			if(Spawn3(spawner_item, i, smartspawn, packrange, packcoord, 0, loops))
				RefreshNextSpawnTime(spawner_item, spawnobjects[i]);//refresh devo salvare dopo gli spawnobj.
				SetObjProperty(spawner_item, "SpawnObjects", spawnobjects);
			endif
			break;
		endif
	endfor
endfunction

//TODO FIX SECTOR LIST & SHOWBOUNDS
function SpawnRange(byref spawner_item, value:=-1)
	if (value < 0)//-1 returns value?
		return cint(GetObjProperty(spawner_item, "SpawnRange"));
	endif

	// reset the sector list
	//ResetSectorList();

	//spawner.spawnrange := value;
	//spawner.width := value * 2;
	//spawner.height := value * 2;
	SetObjProperty(spawner_item, "SpawnRange", value);
	SetObjProperty(spawner_item, "Width", value * 2 );
	SetObjProperty(spawner_item, "Height", value * 2);

	// dont set the bounding box locations if the initial location is 0,0 since this occurs when the item is just being made
	// because m_X and m_Y are restored on loading, it creates problems with OnLocationChange which has to avoid applying translational
	// adjustments to newly placed spawners (because the actual m_X and m_Y is associated with the original location, not the 0,0 location)
	// basically, before placement, dont set m_X or m_Y to anything that needs to be adjusted later on

	if (spawner_item.x == 0 && spawner_item.y == 0)
		return 0;
	endif

	//spawner.x := spawner_item.x - value;
	//spawner.y := spawner_item.y - value;
	SetObjProperty(spawner_item, "X", spawner_item.x - value);
	SetObjProperty(spawner_item, "Y", spawner_item.y - value);

	// Check if the spawner is showing its bounds
	/*if (ShowBounds(spawner_item, spawner))
		ShowBounds(spawner_item, spawner, 0);
		ShowBounds(spawner_item, spawner, 1);
	endif*/
endfunction

//TODO FIX!
function SpawnerTimerOnTick( byref spawner_item )
	//print("spawntick item "+spawner_item.name );
	//PrintTextAbove(spawner_item, "[SpawnTick]");
	//Broadcast("spawner tick spawner mindelay:"+GetObjProperty(spawner_item, "MinDelay")+"; maxdelay:"+GetObjProperty(spawner_item, "MaxDelay"));//manda uno spawner sbagliato poi.

	// start up the timer again for the next Ontick
	DoTimer( spawner_item );

	// reset the protection against runaway looping
	ClearSpawnedThisTick( spawner_item, 1);

	// if regional spawning is enabled, update the region in case new regions were added after the initialization pass
	//TODO FIX
	//CheckRegionAssignment( , 1);

	// reset the killcount whenever a spawntick goes by in which it could have spawned, ie the spawner is full, or proximity triggered
	// spawns were not activated.  Note that killcount gets incremented within Defrag whenever a spawn is that had been generated is removed from the active list.
	// Check the count before and then after the spawn passes.
	// if the spawner is still refractory then dont do a reset of the killcount.
	//int startcount = this.m_killcount;
	//TODO FIX
	//var startcount := killcount_held;
	//if (!m_skipped)
	//	killcount_held = m_killcount;
	//endif

	// killcount will be updated in Defrag
	Defrag(spawner_item, 1);

	// remove any keyword tags that were made
	// note, tags only last a single ontick except for WAIT type
	//TODO FIX
	//ClearTags(false);
	//TODO FIX
	//if (!m_DisableGlobalAutoReset && startcount == m_killcount && !m_refractActivated && !m_skipped)
	//	m_spawncheck--;
	//endif
	//m_skipped = false;

	// allow for some slack in the killcount reset by resetting after a certain number of spawn ticks without kills pass
	//if (m_spawncheck <= 0)
	//	m_killcount = 0;
	//	m_spawncheck = m_KillReset;     // wait for 1 spawn ticks to pass before resetting.  This can be set to anything you like
	//endif

	//TODO FIX:
	// check for smart spawning
	//if (SmartSpawning && IsFull && !HasActiveSectors && !HasDamagedOrDistantSpawns /*&& !HasHoldSmartSpawning */ )
	//	IsInactivated = true;
	//	// for multiple sector spawning ranges use the sector timer, otherwise just rely on OnSectorActivate to detect sector activation
	//	//if(!UseSectorActivate)
	//	//DoSectorTimer(TimeSpan.FromSeconds(1));

	//	SmartRemoveSpawnObjects();
	//endif

	// dont process spawn ticks while inactivated if smart spawning is enabled
	//if (SmartSpawning && IsInactivated)
	//	return;
	//endif

	//spawner.isinactivated := 0;
	SetObjProperty(spawner_item, "IsInactivated", 0);

	// check to see if spawning is on hold due to a WAIT keyword
	if ( !GetObjProperty(spawner_item, "OnHold") )
		//Broadcast("spawner tick 1");
		// look for  triggers that are not player activated.
		//TODO FIX
		//if (m_ProximityRange == -1 && CanSpawn)
		//	CheckTriggers(null, null, false);
		//endif

		// check for proximity triggers without movement activation
		//TODO FIX
		//
		//if (m_ProximityRange >= 0 && CanSpawn)
		//	// check all nearby players
		//	foreach (Mobile p in GetMobilesInRange(m_ProximityRange))
		//		if (ValidPlayerTrig(p))
		//			CheckTriggers(p, null, true);
		//	endforeach
		//endif

		if ( cint(GetObjProperty(spawner_item, "Group")) == 1 )
			Respawn(spawner_item);
			/*TODO FIX
			// check the seq reset time on the current subgroup
			// if the reset time is greater than zero then check the timer
			// if it has expired then reset the sequential subgroup
			// only do this if it can actually spawn
			if (CheckForSequentialReset())
				// it has expired so reset the sequential spawn level
				SeqResetTo(m_SequentialSpawning);

				bool triedtospawn = Respawn();

				if (triedtospawn) ClearGOTOTags();

				// dont advance if the spawn isnt triggered after resetting
				if (!triedtospawn) HoldSequence = true;
			else
				if (TotalSpawnedObjects <= 0)
					// advance the sequential spawn index if it is enabled
					AdvanceSequential();

					//bool hadhold = HoldSequence;

					//HoldSequence = false;

					bool triedtospawn = Respawn();

					if (triedtospawn) ClearGOTOTags();

					//if(!triedtospawn) HoldSequence = hadhold;
				endif
			endif
			*/
		else
			//Broadcast("spawner tick 2");
			Spawn(spawner_item,0,0);//Spawn(spawner, 0, 0);
			/*TODO FIX
			if (CheckForSequentialReset())
				// it has expired so reset the sequential spawn level
				SeqResetTo(m_SequentialSpawning);

				// dont advance if the spawn isnt triggered after resetting
				HoldSequence = true;
			else
				// advance the sequence before spawning
				AdvanceSequential();
			endif

			// keep track of the hold flag before trying to spawn in case no spawn attempt is made
			//bool hadhold = HoldSequence;

			// clear the hold flag to see if any of the spawned entries try to set it
			//HoldSequence = false;

			// try to spawn.  If spawning conditions such as triggering or TOD are not met, then it returns false
			bool triedtospawn = Spawn(false, 0);

			if (triedtospawn) ClearGOTOTags();
			// this will maintain any sequential holds if spawning was suppressed due to triggering
			// if nothing was spawned or triggered, then restore the hold status to previous state
			//if(!triedtospawn) HoldSequence = hadhold;

			if (!FreeRun)
				m_mob_who_triggered = null;
				m_skill_that_triggered = XmlSpawnerSkillCheck.RegisteredSkill.Invalid;
			endif
			*/
		endif
		// remove any keyword tags that were made except for WAIT type
		/*TODO FIX
		ClearTags(false);

		// and clear triggering flags
		if (!OnHold && !FreeRun)
			m_proximityActivated = false;
		endif
		*/
	endif

	/*TODO FIX
	if (FreeRun && SpawnOnTrigger && m_proximityActivated)
		// if it is in free run and was triggered, then just keep spawning as though it was triggered immediately
		NextSpawn = TimeSpan.Zero;
		ResetNextSpawnTimes();
	endif

	//this.m_ExternalTrigger = false;
	// if it is out of the TOD range then delete the spawns
	if (!TODInRange)
		RemoveSpawnObjects();
		ResetAllFlags();
	endif
	*/
endfunction

function SpawnerStruct( byref spawner_item )
	var spawnobjects  := GetObjProperty(spawner_item, "SpawnObjects");
	if (!spawnobjects)
		spawnobjects := array;
		SetObjProperty(spawner_item, "SpawnObjects", spawnobjects);
	endif
	var status_str := GetObjProperty(spawner_item, "status_str");
	if (!status_str)
		SetObjProperty(spawner_item, "status_str", "-stringa stato-");
	endif
	SetObjProperty(spawner_item, "SequentialSpawning", -1);
	var maxcount := GetObjProperty(spawner_item, "Count");
	if (!maxcount)
		SetObjProperty(spawner_item, "Count", 0);
	endif
	//TODO check freerun!

	SpawnRange(spawner_item, cint(GetObjProperty(spawner_item, "SpawnRange")));
/*	var pt := struct;

	//pt.+spawnobjects := array;
	//pt.+sequentialspawn := -1;
	pt.+name := cstr(spawner.name);

	//spawner.uniqueid
	pt.+playercreated := cint(GetObjProperty(spawner, "PlayerCreated"));//di base bool := 0
	pt.+homerangeisrelative := cint(GetObjProperty(spawner, "HomeRangeIsRelative"));//di base bool := 0
	pt.+team:= cint(GetObjProperty(spawner, "Team"));
	pt.+homerange := cint(GetObjProperty(spawner, "HomeRange"));
	pt.+stackamount := cint(GetObjProperty(spawner, "StackAmount"));
	pt.+spawnrange := cint(GetObjProperty(spawner, "SpawnRange"));
	pt.+maxcount := cint(GetObjProperty(spawner, "Count"));
	pt.+mindelay := cint(GetObjProperty(spawner, "MinDelay"));
	pt.+maxdelay := cint(GetObjProperty(spawner, "MaxDelay"));
	pt.+duration := cint(GetObjProperty(spawner, "Duration"));
	pt.+spawnobjects := GetObjProperty(spawner, "SpawnObjects");//spawnobjects array
	if (!pt.spawnobjects)
		pt.spawnobjects := array;
	endif
	pt.+end := cint(GetObjProperty(spawner, "End"));
	pt.+refractend := cint(GetObjProperty(spawner, "RefractEnd"));
	pt.+durend := cint(GetObjProperty(spawner, "DurEnd"));
	//pt.+timer
	//pt.+durtimer
	//pt.+refractorytimer
	pt.+running := cint(GetObjProperty(spawner, "Running")); //bool
	pt.+group := cint(GetObjProperty(spawner, "Group"));//bool
	pt.+x := cint(GetObjProperty(spawner, "X"));
	pt.+y := cint(GetObjProperty(spawner, "Y"));
	pt.+width := cint(GetObjProperty(spawner, "Width"));
	pt.+height :=  cint(GetObjProperty(spawner, "Height"));
	//pt.+waypoint
	//ShowContainerStatic
	pt.+proximityactivated := cint(GetObjProperty(spawner, "ProximityActivated"));//bool
	pt.+refractactivated := cint(GetObjProperty(spawner, "RefractActivated"));//bool
	pt.+duractivated := cint(GetObjProperty(spawner, "DurActivated"));//bool
	//pt.+todstart
	//pt.+todend
	pt.+refractmin := cint(GetObjProperty(spawner, "MinRefractory"));
	pt.+refractmax := cint(GetObjProperty(spawner, "MaxRefractory"));
	pt.+triggeroncarried := cstr(GetObjProperty(spawner, "ItemTriggerName"));
	pt.+notriggeroncarried := cstr(GetObjProperty(spawner, "NoItemTriggerName"));
	pt.+triggerobject := GetObjProperty(spawner, "ObjectPropertyItem");//item (serial?)
	pt.+triggerobjectprop := cstr(GetObjProperty(spawner, "ObjectPropertyName"));
	pt.+status_str := "-stringa stato-";
	pt.+killcount := cint(GetObjProperty(spawner, "KillCount"));
	//pt.+proximityrange
	//pt.+proximitytriggersound
	//pt.+proximitytriggermessage
	//pt.+speechtrigger
	//pt.+speechtriggeractivated
	//pt.+mobpropertyname
	//pt.+mobtriggername
	//pt.+playerpropertyname
	//pt.+triggerprobability
	pt.+triggermob := GetObjProperty(spawner, "mob_who_triggered");
	//pt.+setpropertyitem

	//private bool m_skipped = false;
	//private int m_KillReset = defKillReset;	  // number of spawn ticks that pass without kills before killcount gets reset to zero
	//private int m_spawncheck = 0;
	//private TODModeType m_TODMode = TODModeType.Realtime;
	//private string m_GumpState;
	pt.+externaltriggering := cint(GetObjProperty(spawner, "ExternalTriggering"));//bool
	pt.+exttrigstate := cint(GetObjProperty(spawner, "ExternalTrigger"));//bool
	//private int m_SequentialSpawning = -1;	  // off by default
	//private DateTime m_SeqEnd;
	//-1 default!
	//pt.+sequentialspawn := cint(GetObjProperty(spawner, "SequentialSpawning"));
	pt.+sequentialspawn := -1;
	pt.+spawnregion := GetObjProperty(spawner, "Region");
	pt.+regionname := cstr(GetObjProperty(spawner, "RegionName"));

	pt.+textentrybook;//List<XmlTextEntryBook> m_TextEntryBook;
	pt.+spawnergump;//SpawnerGump

	//private bool m_AllowGhostTriggering = false;
	//private bool m_AllowNPCTriggering = false;
	//private string m_ConfigFile;
	//private bool m_OnHold = false;
	//private bool m_HoldSequence = false;
	//private bool m_SpawnOnTrigger = false;

	pt.+firstmodifiedby := GetObjProperty(spawner, "FirstModifiedBy");
	pt.+lastmodifiedby := GetObjProperty(spawner, "LastModifiedBy");

	//public List<XmlSpawner> RecentSpawnerSearchList = null;
	//public List<Item> RecentItemSearchList = null;
	//public List<Mobile> RecentMobileSearchList = null;
	//private TimeSpan m_DespawnTime;

	//private string m_SkillTrigger;
	//private bool m_skillTriggerActivated;
	//private SkillName m_skill_that_triggered; pt.+triggerskill
	//private bool m_FreeRun = false;	 pt.+freerun// override for all other triggering modes
	pt.+freerun := cint(GetObjProperty(spawner, "FreeRun"));//bool
	//private SkillName m_SkillTriggerName;
	//private double m_SkillTriggerMin;
	//private double m_SkillTriggerMax;
	//private int m_SkillTriggerSuccess;
	//private Map currentmap;

	pt.+isinactivated := cint(GetObjProperty(spawner, "IsInactivated"));//bool
	pt.+smartspawning := cint(GetObjProperty(spawner, "SmartSpawning"));//bool
	//SectorTimer m_SectorTimer

	//pt.+showboundsitems := GetObjProperty(spawner, "ShowBoundsItems")); //private List<Static> m_ShowBoundsItems = new List<Static>();
	pt.+inrespawn := 0;
	pt.+disableglobalautoreset := cint(GetObjProperty(spawner, "DisableGlobalAutoReset"));//bool
	pt.+mostrecentspawnposition := GetObjProperty(spawner, "MostRecentSpawnPosition");//Point3d

	//TODO: forse meglio metterlo alla fine
	pt.+currentcount := cint(TotalSpawnedObjects(spawner));
	SpawnRange(spawner, pt.spawnrange);//per settare tutto il range.

	pt.+holdsequence := GetObjProperty(spawner, "HoldSequence");
	//pt.+freerun := GetObjProperty(spawner, "FreeRun");
	return pt;
*/
endfunction

function SpawnObject( from, spawner_item, name, maxamount, subgroup := 0, sequentialresettime := 0.0, sequentialresetto := 0, killsneeded := 0,
                           restrictkills := 0, clearadvance:= 1, mindelay:= -1, maxdelay:=-1, spawnsper:=1, packrange:=-1 )

	if (from && spawner_item)
		var found := 0;
		var spawnobjects := GetObjProperty(spawner_item, "SpawnObjects");
		// go through the current spawner objects and see if this is a new entry
		if (spawnobjects)
			var i;
			for (i := 1; i <= len(spawnobjects); i+=1)
				var s := spawnobjects[i];
				if (s && s.typename == name)
					found := 1;
					break;
				endif
			endfor

			if (found==0)
				Print("added to Xmlspawner");
				//CommandLogging.WriteLine(from, "{0} {1} added to XmlSpawner {2} '{3}' [{4}, {5}] ({6}) : {7}", from.AccessLevel, CommandLogging.Format(from), spawner.Serial, spawner.Name, spawner.GetWorldLocation().X, spawner.GetWorldLocation().Y, spawner.Map, name);
			endif
		endif
	endif

	var so := struct;
	so.+typename := cstr(name);
	so.+maxcount := cint(maxamount);
	so.+subgroup := subgroup;
	so.+sequentialresettime := sequentialresettime;
	so.+sequentialresetto := sequentialresetto;
	so.+killsneeded := killsneeded;
	so.+restrictkillstosubgroup := restrictkills;
	so.+clearonadvance := clearadvance;
	so.+mindelay := mindelay;
	so.+maxdelay := maxdelay;
	so.+spawnspertick := spawnsper;
	so.+packrange := packrange;
	//m_ignore
	so.+available;// := 0;
	so.+spawnedobjects := array;

	//so.+actualmaxcount := cint(maxamount); uso solo maxcount
	so.+disabled := 0;
	so.+ignore := 0;
	so.+nextspawn;
	so.+spawnedthistick;

	return(so);
endfunction

//public enum SpawnPositionType { Random, RowFill, ColFill, Perimeter, Player, Waypoint, RelXY, DeltaLocation, Location, Wet, Tiles, NoTiles, ItemID, NoItemID }
function SpawnPositionInfo(postype, mob, args)
	var posinfo := struct;
	posinfo.+positiontype := postype;
	posinfo.+trigmob := mob;
	posinfo.+positionargs := args;
	return posinfo;
endfunction

//TODO FIX
function SpawnSubGroup(byref spawner_item, sgroup, smartspawn:= 0, ignoreloopprotection := 0, loops)//return bool
	var spawnobjects := GetObjProperty(spawner_item, "SpawnObjects");
	if (len(spawnobjects) < 1)
		return 0;
	endif
	if (sgroup >= 0)
		var didspawn := 0;
		/*TODO FIX COORDINATES
		var packcoord := Point3d(0,0,0,spawner_item.realm);//Point3D.Zero;
		var j;
		for (j := 1; j <= len(spawner.spawnobjects); j+=1)
			var so := spawner.spawnobjects[j];

			if (so && so.subgroup == sgroup)
				// find the first subgroup spawn to determine the packspawning reference coordinates
				if (so.packrange >= 0 && packcoord.x == 0 && packcoord.y == 0 && packcoord.z == 0)
					packcoord := GetPackCoord(spawner_item,spawner,sgroup);
				endif

				// get the SpawnsPerTick count and spawn up to that number
				var success = Spawn2(j, smartspawn, so.spawnspertick, so.packrange, packcoord, ignoreloopprotection, loops);

				if (success)
					didspawn := 1;
				endif

				if (success && !smartspawn)
					RefreshNextSpawnTime(spawner_item, so);//refresh devo salvare dopo gli spawnobj.
					SetObjProperty(spawner_item, "SpawnObjects", spawnobjects);
				endif
			endif
		endfor
		*/
		// success if any of the subgroup spawned
		if (didspawn==1)
			return 1;
		endif
	endif
	return 0;
endfunction

//calling this Start will cause problems!
function StartSpawn(byref spawner_item)
	if (cint(GetObjProperty(spawner_item, "Running")) == 0)
		var spawnobjects := GetObjProperty(spawner_item, "SpawnObjects");

		if (len(spawnobjects) > 0)
			//spawner.running := 1;
			SetObjProperty(spawner_item, "Running",1);
			//Broadcast( "StartSpawn" );//TODO FIX
			DoTimer(spawner_item);
		endif
	endif

endfunction

//TODO FIX TIMERS! -> un check su ontick?
function Stop(byref spawner_item)

	if ( cint(GetObjProperty(spawner_item, "Running")) == 1)
		// turn off all timers
		/*if (m_Timer != null)
			m_Timer.Stop();
		if (m_DurTimer != null)
			m_DurTimer.Stop();
		if (m_RefractoryTimer != null)
			m_RefractoryTimer.Stop();*/

		//controllo se il timer è già attivo, se sì lo spengo.
		var dur := cint(TS_GetTimerDuration( spawner_item, "SpawnerTimer" ));
		if (dur > 0)
			//Broadcast( "[Stop] timer attivo, stop." );
			//PrintTextAbove(spawner_item, "[Stop] timer attivo, stop.");
			TS_StopTimer( spawner_item, "SpawnerTimer" );//TODO FIX: STOPPA REALMENTE?
			//SleepMS(200);
		endif

		//spawner.running := 0;
		SetObjProperty(spawner_item, "Running",0);
		//spawner.proximityactivated := 0;
		SetObjProperty(spawner_item, "ProximityActivated", 0);
		//spawner.externaltrigger := 0;
		//m_mob_who_triggered = null;
		//m_skill_that_triggered = XmlSpawnerSkillCheck.RegisteredSkill.Invalid;
	endif
endfunction

function SubGroupCount(byref spawner_item , sgroup)//return int
	var spawnobjects := GetObjProperty(spawner_item, "SpawnObjects");
	if (len(spawnobjects) < 1)
		return (0);
	endif

	var nsub := 0;
	var i;
	for ( i := 1; i <= len(spawnobjects); i+=1)
		var s := spawnobjects[i];

		if (s.subgroup == sgroup)
			nsub+=1;
		endif
	endfor

	return nsub;
endfunction

function TotalSpawnedObjects(byref spawner_item)//return int
	//defraggo prima! defrag so that accurately reflects currently active spawns
	Defrag(spawner_item, 1);

	var spawnobjects := GetObjProperty(spawner_item, "SpawnObjects");
	if (len(spawnobjects) < 1)
		return 0;
	endif

	var count := 0;
	foreach so in (spawnobjects)
		//broadcast("TRY LEN:"+len(so.spawnedobjects)+"; OBJ SER:"+so.spawnedobjects[1]);
		count += len(so.spawnedobjects);//so.spawnedobjects.count
	endforeach

	//PrintTextAbove( spawner_item, "tso :" +count);
	return cint(count);
endfunction

function TotalSpawnObjectCount(byref spawner_item)//return int
	var spawnobjects := GetObjProperty(spawner_item, "SpawnObjects");
	if (len(spawnobjects) < 1)
		return 0;
	endif
	var count := 0;
	foreach so in (spawnobjects)
		count += so.maxcount;
	endforeach

	return count;
endfunction