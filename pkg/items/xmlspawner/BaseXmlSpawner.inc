//BaseXmlSpawner.ParseObjectType(str);
//BaseXmlSpawner.IsTypeOrItemKeyword(typestr)

include "include/skillsinc";//va in conflitto con skillsinc!
include "include/string";
include ":brainAI:npcUtil";
include ":loot:lootParser";

const slashdelim := "/";
const commadelim := ",";
const spacedelim := " ";
const semicolondelim := ";";
const literalend := "§";

var typeKeyword := array{ "SET", "SETVAR", "SETONTRIGMOB", "SETONCARRIED",
				"SETONSPAWN", "SETONSPAWNENTRY", "SETONNEARBY", "SETONPETS",
				"SETONMOB", "SETONTHIS", "SETONPARENT", "SETACCOUNTTAG", "FOREACH",
				"GIVE", "TAKEGIVE", "TAKE", "GUMP", "TAKEBYTYPE", "BROWSER", "SENDMSG",
				"SENDASCIIMSG", "MUSIC", "RESURRECT", "POISON", "DAMAGE", "EFFECT", "MEFFECT",
				"SOUND", "WAITUNTIL", "WHILE", "IF", "GOTO", "CAST", "BCAST",
				"BSOUND", "COMMAND", "SPAWN", "DESPAWN" };
var itemKeyword := array{ "ARMOR", "WEAPON", "JARMOR", "JWEAPON", "SHIELD", "SARMOR",
				"LOOT", "JEWELRY", "POTION", "SCROLL", "NECROSCROLL", "LOOTPACK",
				"TAKEN", "GIVEN", "ITEM", "MULTIADDON" };

function AddSpawnItem(byref invoker, byref theSpawn, item, location, map, trigmob, requiresurface, spawnpositioning, propertyString, smartspawn, byref status_str)
	status_str := "";
	if (!item || !theSpawn)
		return 0;
	endif

	// add the item to the spawned list
	//theSpawn.SpawnedObjects.Add(item);
	//theSpawn.spawnedobjects.append(item.serial);

	//item.Spawner = spawner;
	//SetObjProperty(item, "Spawner", spawner_item.serial);//metto il seriale
	var amount := 1;
	// set the item amount
	if (cint(GetObjProperty(invoker, "StackAmount")) > 1 && item.stackable)
		amount := cint(GetObjProperty(invoker, "StackAmount"));
	endif
	var o;

	if (invoker)
		// if this is in any container such as a pack then add to the container.
		if (invoker.container)
			var loc := Point3d(invoker.x,invoker.y,invoker.z,map);

			//if (!smartspawn)
			//	item.OnBeforeSpawn(loc, map);
			//endif

			o := CreateItemAtLocation( loc.x, loc.y, loc.z, item, amount, map );
			theSpawn.spawnedobjects.append(o.serial);
			SetObjProperty(o, "Spawner", invoker.serial);
			MoveItemToContainer(o, invoker.container );

			// check to see whether we drop or add the item based on the spawnrange
			// this will distribute multiple items around the spawn point, and allow precise
			// placement of single spawns at the spawn point
			//TODO FIX
			//if (spawner.SpawnRange > 0)
			//	c.DropItem(item);
			//else
			//	c.AddItem(item);
			//endif

		else
			// if the spawn entry is in a subgroup and has a packrange, then get the packcoord

			var packcoord := Point3d(0,0,0,map);
			//TODO CHECK
			if (theSpawn.packrange >= 0 && theSpawn.subgroup > 0)
				packcoord := GetPackCoord(invoker, theSpawn.subgroup);
			endif
			var loc := GetSpawnPosition(invoker, requiresurface, theSpawn.packrange, packcoord, spawnpositioning);

			//TODO FIX
			//if (!smartspawn)
			//	item.OnBeforeSpawn(loc, map);
			//endif

			// standard placement for all items in the world
			o := CreateItemAtLocation( loc.x, loc.y, loc.z, item, amount, map );
			theSpawn.spawnedobjects.append(o.serial);

			SetObjProperty(o, "Spawner", invoker.serial);
		endif
	else
		//if (!smartspawn)
		//	item.OnBeforeSpawn(location, map);
		//endif

		// use the location and map info passed in
		// this allows AddSpawnItem to be called by objects other than spawners as long as they pass in a valid SpawnObject
		o := CreateItemAtLocation( location.x, location.y, location.z, item, amount, map );
		theSpawn.spawnedobjects.append(o.serial);
		SetObjProperty(o, "Spawner", invoker.serial);
		//item.MoveToWorld(location, map);
	endif
	//TODO SALVO GLI SPAWNEDOBJ. fuori?

	// clear the taken flag on all newly spawned items
	//ItemFlags.SetTaken(item, false);

	//if (!smartspawn)
	//	item.OnAfterSpawn();
	//endif

	// apply the parsed arguments from the typestring using setcommand
	// be sure to do this after setting map and location so that errors dont place the mob on the internal map
	ApplyObjectStringProperties(propertyString, o, trigmob, invoker, status_str);

	// if the object has an OnAfterSpawnAndModify method, then invoke it
	//InvokeOnAfterSpawnAndModify(item);
endfunction

// added in arg parsing to handle object property setting
function ApplyObjectStringProperties(str, byref o, trigmob, byref refobject, byref status_str)
	//broadcast("objproperty1");
//(XmlSpawner spawner, string str, object o, Mobile trigmob, object refobject, out string status_str)
	status_str := "";

	if( len(str) == 0 || !o )
		return 0;
	endif

	// object strings will be of the form "object/modifier" where the modifier string is of the form "propname/value/propname/value/..."
	// some keywords do not have value arguments so the modifier could take the form "propname/propname/value/..."
	// this is handled by parsing into both forms

	// make sure the string is properly terminated to assure proper parsing of any final keywords
	var terminated := 0;
	str := TrimStr(str);

	if (str[len(str)] != "/")
		str += "/";
		terminated := 1;
	endif
	//broadcast("objproperty1 str:"+str);
	var arglist :=array;//string[]

	arglist := ParseSlashArgs(str, 2);

	var remainder := "";

	// place the modifier section of the string in remainder
	if (len(arglist) > 1)
		remainder := arglist[2];//era 1
	endif
	//broadcast("objproperty1 arglist1:"+arglist[1]+"; 2:"+arglist[2]);// cougar x
	var no_error := 1;

	// process the modifier string if there is anything
	while (len(arglist) > 1)
		// place into arglist the parsed modifier up to this point
		// arglist[1] will contain the propname
		// arglist[2] will contain the value
		// arglist[3] will contain the reset of the modifier
		arglist := ParseSlashArgs(remainder, 3);

		// singlearglist will contain the propname and the remainder
		// for those keywords that do not have value args
		var singlearglist := ParseSlashArgs(remainder, 2);//string[]

		if (len(arglist) > 1)
			//broadcast("objproperty1 1.1");
			// handle value keywords that may take comma args

			// itemarglist[1] will contain arg2/arg3/arg4>/arg5
			// additemstr should have the full list of args <arg2/arg3/arg4>/arg5 if they are there.  In the case of /arg1/ADD/arg2
			// it will just have arg2
			var groupedarglist := ParseString(arglist[2], 2, "[");//string[]

			// take that argument list that should like like arg2/ag3/arg4>/arg5
			// need to find the matching ">"

			var groupargs := array;
			var groupargstring := "";
			if (len(groupedarglist) > 1)
				//TODO CHECK!
				groupargs := ParseToMatchingParen(groupedarglist[2], "[", "]");

				// and get the first part of the string without the >  so itemargs[0] should be arg2/ag3/arg4
				groupargstring := cstr(groupargs[1]);
			endif

			//string[] value_keywordargs = ParseString(groupedarglist[0],10,",");
			var value_keywordargs := SplitWords(TrimStr(groupedarglist[1]), ",");//string[]
			if( len( groupargstring )>1 )
				if (value_keywordargs && len(value_keywordargs) > 0)
					value_keywordargs[len(value_keywordargs)] := groupargstring;
				endif
			endif

			// handle propname keywords that may take comma args
			//string[] keywordargs = ParseString(arglist[0],10,",");
			var keywordargs := SplitWords(TrimStr(arglist[1]),",");

			// this quick optimization can determine whether this is a regular prop/value assignment
			// since most prop modification strings will use regular propnames and not keywords, it makes sense to check for that first
			if (len(value_keywordargs[1]) > 0 && !IsUpper(value_keywordargs[1][1]) &&
						len(arglist[1]) > 0 && !IsUpper(arglist[1][1]))
				//broadcast("objproperty1 1.2");
				// all of this code is also included in the keyword candidate tests
				// this is because regular props can also be entered with uppercase so the lowercase test is not definitive

				// restricted properties
				//if(arglist[0].ToLower() == "accesslevel")
				//{
				//	status_str = "accesslevel is a protected property";
				//	if(arglist.Length < 3) break;
				//	remainder = arglist[2];
				//} 
				//else
				// check for the literal char
				if (singlearglist[2] && len(singlearglist[2]) > 0 && singlearglist[2][1] == "@")//1 1 1 0 cambiati
					//broadcast("objproperty1 1.3");
					//support for literal terminator
					singlearglist := ParseLiteralTerminator(singlearglist[2],2);// 1 2
					var lstr := cstr(singlearglist[1]);
					if (terminated && lstr[len(lstr)] == "/")
						lstr[len(lstr)] := "";//lstr.Remove(lstr.Length - 1, 1);
					endif

					var result := cstr(SetPropertyValue(o, arglist[1], lstr[1,len(lstr)]/*.Remove(0, 1)*/));

					// see if it was successful
					if (result != "Property has been set.")
						status_str := arglist[1] + " : " + result;
						no_error := 0;
					endif
					if(len(singlearglist)>1 && singlearglist[2])
						remainder := singlearglist[2];
					else
						remainder :="";
						break;
					endif
				else
					//broadcast("objproperty1 1.4 arglist1:"+arglist[1]+"; 2:"+arglist[2]);// cougar x
					var result := cstr(SetPropertyValue(o, arglist[1], arglist[2]));

					// see if it was successful
					if (result != "Property has been set.")
						status_str := arglist[1] + " : " + result;
						no_error := 0;
					endif
					if (len(arglist) < 3)
						break;
					endif

					//broadcast("objproperty1 1.5 arglist3:"+arglist[3]);// cougar x
					remainder := arglist[3];
				endif
			else
				//todo fix!
				//if (IsValuemodKeyword(value_keywordargs[1]))
				//elseif (IsTypemodKeyword(keywordargs[1]))
				//else
				if (keywordargs[1] == "ADD")
					var add_array := GetObjProperty(o, "addarray" );
					if (!add_array || add_array.errortext)
						add_array := array;
					endif

					if (len(keywordargs) < 1)
						status_str := "Invalid ADD format";
					else
						SetObjProperty(o, "addarray" , SplitWords(arglist[2],","));
						Loot_Generate( o, 1 );
						status_str := o.name +" add set";
					endif

					if (len(arglist) < 3)
						break;
					endif
					sleepMS(2);

					remainder := arglist[3];
				elseif (keywordargs[1] == "LOOT")
					var loot_array := GetObjProperty(o, "lootarray" );
					if (!loot_array || loot_array.errortext)
						loot_array := array;
					endif

					if (len(keywordargs) < 1)
						status_str := "Invalid LOOT format";
					else//if (len(keywordargs) == 2)//LOOT,stringaloot1,stringa2,...
						//LOOT,Stack 2d4+12 GoldCoin
						//LOOT,Random 1 Gems
						//LOOT,ProtectStaffMage 10
//Stack {n=1} [ObjName] {color=0} {chancetospawn=100} Stacks the amount. DO NOT use on
//            non-stackable items such as arms and armor! Use "Item" for those.
//      Item {n=1} [ObjName] {color=0} {chancetospawn=100}  include 1 of ObjName
//      Random [n] [GroupName] {chancetospawn=100} pick n Items from GroupName randomly
//      Group [GroupName]       include GroupName too
//      Unique [ObjName]         don't give more than one
						//var stringaloot;
						//var vari_loot := SplitWords(arglist[2],",");
						//var i;
						//for (i:=1; i<=len(vari_loot); i +=1)
							//stringaloot := cstr(keywordargs[2]);
						//	loot_array.append(cstr(vari_loot[i]));
						//endfor
						SetObjProperty(o, "lootarray" , SplitWords(arglist[2],","));
						status_str := o.name +" loot set";
					/*elseif (len(keywordargs) == 3)//LOOT,stringaloot,chance
						//var stringaloot := cstr(keywordargs[2]);
						var chance_str := cstr(keywordargs[3]);
						var chance := 1;

						if (chance_str[len(chance_str)-1] == ",")
							chance_str[len(chance_str)-1] := ".";
						endif
						if (chance_str[len(chance_str)-1] == ".")//anche i decimi
							chance_str[len(chance_str)-1] := "";
							chance := cdbl(CInt( chance_str )*0.1);
						else
							chance := cdbl(CInt( chance_str )*0.01);//senza decimi
						endif
						if (RandomFloat(1.0) < chance )
							var stringaloot := cstr(keywordargs[2]);
							loot_array.append(stringaloot);

							SetObjProperty(o, "lootarray" , loot_array);
							status_str := "loot set";
						endif
					*/
					endif

					if (len(arglist) < 3)
						break;
					endif
					sleepMS(2);
					//TODO: still to test?
					//broadcast("objproperty1 1.7 arglist3:"+arglist[3]);// cougar x
					remainder := arglist[3];
				else
					// check for the literal char
					if (singlearglist[2] && len(singlearglist[2]) > 0 && singlearglist[2][1] == "@")
						//support for literal terminator
						singlearglist := ParseLiteralTerminator(singlearglist[2],2);
						var lstr := cstr(singlearglist[1]);
						if (terminated && lstr[len(lstr)] == "/")
							lstr[len(lstr)] := "";//lstr.Remove(lstr.Length - 1, 1);
						endif

						var result := cstr(SetPropertyValue(o, arglist[1], lstr[1,len(lstr)]/*.Remove(0, 1)*/));

						// see if it was successful
						if (result != "Property has been set.")
							status_str := arglist[1] + " : " + result;
							no_error := 0;
						endif
						if(len(singlearglist)>1 && singlearglist[2])
							remainder := singlearglist[2];
						else
							remainder :="";
							break;
						endif
					else
						//broadcast("objproperty1 1.6 arglist1:"+arglist[1]+"; 2:"+arglist[2]);// cougar x
						//arglist[1]:=CPROP,stay arglist[2]:= 1
						var result := cstr(SetPropertyValue(o, arglist[1], arglist[2]));

						// see if it was successful
						if (result != "Property has been set.")
							status_str := arglist[1] + " : " + result;
							no_error := 0;
						endif
						if (len(arglist) < 3)
							break;
						endif
						sleepMS(2);
						//TODO: still to test?
						//broadcast("objproperty1 1.7 arglist3:"+arglist[3]);// cougar x
						remainder := arglist[3];
					endif
				endif
			endif
		endif
	endwhile

	return (no_error);
endfunction

//TODO FIX TRIGMOB
function ApplySubstitution(byref o, trigmob:=0, typeName)//return string
//(XmlSpawner spawner, object o, Mobile trigmob, string typeName)
	var sb := "";//new System.Text.StringBuilder();

	// go through the string looking for instances of {keyword}
	var remaining := cstr(typeName);

	while (remaining && len(remaining) > 0)
		var startindex := ReturnIndex(remaining, "{");
		if (startindex <= 0 || startindex >= len(remaining))
			// if there are no more delimiters then append the remainder and finish
			sb += remaining;
			return cstr(sb);
			break;
		endif

		// might be a substitution, check for keywords
		var endindex := ReturnIndex( remaining[startindex,len(remaining)-startindex], "}");

		// if the ending delimiter cannot be found then just append and finish
		if (endindex <= 0)
			sb += remaining;
			return cstr(sb);
			break;
		endif

		// get the string up to the delimiter
		var firstpart := cstr(remaining[1, startindex]);
		sb += firstpart;

		var keypart := cstr(remaining[startindex, endindex]);

		// try to evaluate and then substitute the arg
		var ptype;

		var value := ParseForKeywords(o, TrimStr(keypart), trigmob, 1, ptype);//out ptype

		// trim off the " from strings
		if (value)
			if (value[len(value)] == CChr(34) )//"
				value[len(value)] := "";
			endif
			if (value[1] == CChr(34) )
				value[1] := "";
			endif
		endif

		// replace the parsed value for the keyword
		sb +=value;

		// continue processing the rest of the string
		if (endindex + startindex + 2 >= len(remaining))
			break;
		endif

		remaining := remaining[endindex + startindex + 2, len(remaining) - endindex - startindex - 2];
	endwhile

	return cstr(sb);
endfunction

function GetType(item)
//ricerca nei cfg, se trova il nome restituisce il tipo, altrimenti null
//var item := str;
var isitem := 1;

	if( GetObjTypeByName( item ))
		item := GetObjTypeByName( item );
	elseif (item.isA(POLCLASS_ITEM))
		item := CInt( item );
	else
		isitem := 0;
	endif

	if (isitem == 0 && item[1] != ":")
		var parsed := GetFullTemplate(item);
		item := ":"+parsed.package+":"+parsed.template;
		//SendSysMessage(who, "Package non completo (:..:...) trovato -> "+parsed.package);
	endif

	return item;

	/*var npc := NPC_GetNPCConfig(str);

	if (npc)
		return npc;
	endif

	return;
	*/
	/*
	if(Cint(str))
		objtype := Cint(str);
	else
		objtype := Cint(GetObjtypeByName(str));
	endif
	*/
	//return GetObjTypeByName(str);
endfunction

//TODO FIX
function IsSpecialItemKeyword(typeName)//bool
	if (!typeName || len(typeName) == 0 )//|| !IsUpper(typeName[0]))
		return 0;
	endif

	return 1;
/*
			if (string.IsNullOrEmpty(typeName) || !char.IsUpper(typeName[0])) return false;

			//if (itemKeywordHash == null) InitializeHash();

			return itemKeywordHash.ContainsKey(typeName);
*/
endfunction

//TODO: FIX
function IsTypeKeyword(typeName)//bool
	if (!typeName || len(typeName) == 0 )//|| !IsUpper(typeName[0]))
		return 0;
	endif
	if ( typeName in typeKeyword )
		return 1;
	endif
	return 0;
/*
			if (string.IsNullOrEmpty(typeName) || !char.IsUpper(typeName[0])) return false;

			//if (typeKeywordHash == null) InitializeHash();

			return (typeKeywordHash.ContainsKey(typeName));
*/
endfunction

//TODO: FIX
function IsTypeOrItemKeyword(typeName)//bool

	if ( typeName in typeKeyword || typeName in itemKeyword )
		return 1;
	endif

	return 0;
/*
	if (!typeName || len(typeName) == 0 )//|| !IsUpper(typeName[0]))
		return 0;
	endif

	if (typeName[1] == ":")//dovrebbe essere un npc
		return 1;
	elseif (typeName.isA(POLCLASS_ITEM))
		return 1;
	endif
*/
	return 0;//ritorno 0 finché non inserisco le keyword! tipo SET / ARMOR ecc.
/*
			if (string.IsNullOrEmpty(typeName) || !Char.IsUpper(typeName[0])) return false;

			//if (typeKeywordHash == null || itemKeywordHash == null) InitializeHash();

			return (typeKeywordHash.ContainsKey(typeName) || itemKeywordHash.ContainsKey(typeName));
*/
endfunction

function ParseCommaArgs(str, nitems)
	if (!str)
		return "";
	endif

	var args;
	str := TrimStr(str);//Trim(str);

	args := Split(str, commadelim, nitems);

	return args;
endfunction


function ParseForKeywords(byref o, valstr, trigmob, literal, ptype)
//(XmlSpawner spawner, object o, string valstr, Mobile trigmob, bool literal, out Type ptype)
	return "";
endfunction

function ParseLiteralTerminator(str, nitems)//string[]
	if (!str || len(str)==0)
		return "";
	endif

	var args := array;

	str := TrimStr(str);

	args := Split(str,literalend,nitems);

	return args;
endfunction

function ParseObjectType(str)//string remove from!
	var arglist := ParseSlashArgs(cstr(str), 2); //string[]
	if (len(arglist) > 0)//sempre se la str non è nulla?
		//SendSysMessage( from, "TEST LEN ARGLIST:"+cstr(len(arglist)) ); //10????
		//SendSysMessage( from, "TEST LEN ARGLIST:"+cstr(arglist) );	//    katana ??

		// parse out any arguments of the form typename,arg,arg,..
		var typeargs := ParseCommaArgs(cstr(arglist[1]), 2);//trasf in 1
		if (len(typeargs) > 1)
			//SendSysMessage( from, "TEST LEN TYPEARGS > 1" );
			return (cstr(typeargs[1]));//trasf in 1
		endif
		return cstr(arglist[1]);//trasf in 1
	else
		//SendSysMessage( from, "TEST LEN ARGLIST == 0" );
		return "";
	endif
endfunction

function ParseSemicolonArgs(str, nitems)//return string[]
	if (len(str) < 1 )
		return "";
	endif
	var args := array;

	str := TrimStr(str);

	args := Split(str, semicolondelim, nitems);

	return args;
endfunction

function ParseSlashArgs(str, nitems)
	if (!str)
		return "";
	endif
	var args := array;
	str := TrimStr(str);//Trim(str);

	//SendSysMessage( from, "TEST RETURNINDEX STRING:"+str );

	//SendSysMessage( from, "TEST RETURNINDEX </ :"+cstr(ReturnIndex(str, "</")) );
	//SendSysMessage( from, "TEST RETURNINDEX /> :"+cstr(ReturnIndex(str, "/>")) );

	// this supports strings that may have special html formatting in them that use the /
	if (ReturnIndex(str, "</") > 0 || ReturnIndex(str, "/>") > 0)//nel POL primo indice è 1
		// or use indexof to do it with more context control
		var tmparray := array;//new List<string>();
		// find the next slash char
		var index := 0;
		var preindex := 0;
		var searchindex := 0;
		var length := len(str);
		while (index >= 0 && searchindex < length && len(tmparray) < nitems - 1)
			index := ReturnIndex(str, "/", searchindex);

			if (index >= 0)
				// check the char before it and after it to ignore </ and />
				if ((index > 0 && str[index - 1] == "<") || (index < length - 1 && str[index + 1] == ">"))
					// skip it
					searchindex := index + 1;
				else
					// split it
					tmparray.Append(str[preindex, index - preindex]);

					preindex := index + 1;
					searchindex := preindex;
				endif
			endif
		endwhile

		// is there still room for more args?
		if (len(tmparray) <= nitems - 1 && preindex < length)
			// searched past the end and didnt find anything
			tmparray.Append(str[preindex, length - preindex]);
		endif

		// turn tmparray into a string[]
		//args = new string[tmparray.Count];
		//tmparray.CopyTo(args);
		args := tmparray;
	else
		// just use split to do it with no context control
		args := Split(str, slashdelim, nitems);

		//SendSysMessage( from, "TEST RETURNEDSPLIT len:"+len(args));
		//SendSysMessage( from, "TEST RETURNINDEX SPLIT:"+args[1] );
		//SendSysMessage( from, "TEST RETURNINDEX SPLIT:"+args[2] );
		//SendSysMessage( from, "TEST RETURNINDEX SPLIT:"+args[len(args)] );
	endif

	return args;
endfunction

function ParseString(str, nitems, delimstr)//string[]
	if (len(str) < 1 || len(delimstr) < 1)
		return "";
	endif
	//char[] delims = delimstr.ToCharArray();
	var args := array;//null;
	str := TrimStr(str);
	args := Split(str, delimstr, nitems);

	return args;
endfunction

// take a string of the form str-opendelim-str-closedelim-str-closedelimstr
function ParseToMatchingParen(str, opendelim, closedelim)//string[]
	var nopen := 1;
	var nclose := 0;
	var splitpoint := len(str);
	var i;
	for (i := 1; i <= len(str); i+=1)//str.Length
		// walk through the string until a matching close delimstr is found
		if (str[i] == opendelim)
			nopen+=1;
		endif
		if (str[i] == closedelim)
			nclose+=1;
		endif

		if (nopen == nclose)
			splitpoint := i;
			break;
		endif
	endfor

	var args := array;//new string[2];

	// allow missing closing delimiters at the end of the line, basically just treat eol as a closing delim

	args[1] := str[1, splitpoint];
	args[2] := "";
	if (splitpoint + 1 < len(str))
		args[2] := str[splitpoint + 1, len(str) - splitpoint - 1];//TODO controllare il +1
	endif

	return args;
endfunction

// set property values with support for nested attributes
function SetPropertyValue(byref o, name, value)//return string
	//broadcast("setpropvalue "+name+"="+value);
	if (!o)
		//broadcast("setpropvalue to null");
		return "Null object";
	endif

	//var ptype;// = null;
	//var po;// = null;
	//var type := o.GetType();
	//PropertyInfo[] props = type.GetProperties(BindingFlags.Static | BindingFlags.Instance | BindingFlags.Public);

	// parse the strings of the form property.attribute into two parts
	// first get the property
	var arglist := ParseString(name, 2, ".");//string[]
	var propname := cstr(arglist[1]);
	var keywordargs := ParseString(propname, 4, ",");//string[]
	//preso da SET:
	var old_val := o.Get_Member( propname );
	var old_type := Lower(TypeOf( old_val ));
	if ( old_type == "integer" )
		value := CInt( value );
	elseif ( old_type == "string" )
		value := CStr( value );
	elseif ( old_type == "double" )
		value := CDbl( value );
	endif

	// check for special keywords
	if (keywordargs[1] == "CPROP")
		if (len(keywordargs) < 2)
			return "Invalid CPROP format";
		endif
		// syntax is CPROP,name
		var name := cstr(keywordargs[2]);
		SetObjProperty(o, name, value);
		return "Property has been set.";
	elseif (keywordargs[1] == "SKILL")
		if (len(keywordargs) < 2)
			return "Invalid SKILL format";
		endif
		var attributeid := Lower( keywordargs[2] );
		var check := GetSkillIdByAttributeId(attributeid);
		if (check.errortext)
			return "Invalid skill name";
		endif

		if (value[len(value)-1] == ",")
			value[len(value)-1] := ".";
		endif
		if (value[len(value)-1] == ".")//anche i decimi
			value[len(value)-1] := "";
			value := CInt( value );
			SetRawSkillFromString(o, attributeid, NewBaseSkillToRawSkill(value) );
			return "Property has been set.";
		else
			value := CInt( value );//senza decimi
			value := value*10;
			SetRawSkillFromString(o, attributeid, NewBaseSkillToRawSkill(value) );
			return "Property has been set.";
		endif
	endif

	/*if (keywordargs[1] == "ATTACHMENT")
	//FIX
	elseif (keywordargs[1] == "SKILL")
		if (len(keywordargs) < 2)
			return "Invalid SKILL format";
		endif
		if (o.isA(POLCLASS_MOBILE))
			var newval := cdbl(value);
			if (newval)
				SetSkillFromString(o, cstr(keywordargs[1]), newval );
				return "Property has been set.";
			else
				return "Invalid double number";
			endif
		else
			return "Object is not mobile";
		endif
	elseif (keywordargs[1] == "STEALABLE")
	endif
	*/
	// do a bit of parsing to handle array references
	var arraystring := SplitWords(propname,"[");//strin[]
	var index := 0;
	if (len(arraystring) > 1)
		// parse the property name from the indexing
		propname := arraystring[1];

		// then parse to get the index value
		var arrayvalue := SplitWords(arraystring[1],"]");//string[]

		if (len(arrayvalue) > 0)
			//int.TryParse(arraystring[0], out index);
			index := cint(arraystring[1]);
		endif
	endif

	if (len(arglist) == 2)
		/*PropertyInfo plookup = LookupPropertyInfo(spawner, type, propname);

				if (plookup != null)
				{

					//if ( !plookup.CanWrite )
					//return "Property is read only.";

					if (BaseXmlSpawner.IsProtected(type, propname))
						return "Property is protected.";

					ptype = plookup.PropertyType;
					po = plookup.GetValue(o, null);

					// now set the nested attribute using the new property list
					return (SetPropertyValue(spawner, po, arglist[1], value));
				}
				else
				{
					// is a nested property with attributes so first get the property
					foreach (PropertyInfo p in props)
					{
						if (Insensitive.Equals(p.Name, propname))
						{
							//CPA pattr = Properties.GetCPA( p );

							//if ( pattr == null )
							//return "Property not found.";

							//if ( !p.CanWrite )
							//return "Property is read only.";

							if (BaseXmlSpawner.IsProtected(type, propname))
								return "Property is protected.";

							ptype = p.PropertyType;

							po = p.GetValue(o, null);

							// now set the nested attribute using the new property list
							return (SetPropertyValue(spawner, po, arglist[1], value));

						}
					}
				}
		*/
	else
		// its just a simple single property
		//broadcast("setproperty test 1");
		if (name == "x")
			//broadcast("setproperty test x");
			if (o.isA(POLCLASS_MOBILE) || o.isA(POLCLASS_ITEM))
				//broadcast("setproperty test x ->"+value+"<-");
				//o.x := cint(value);
				MoveObjectToLocation( o, value, o.y, o.z, o.realm, MOVEOBJECT_FORCELOCATION );
				return "Property has been set.";
			else
				//broadcast("setproperty test x fail");
				return "Cannot set X because of class.";
			endif
		elseif (name == "y")
			if (o.isA(POLCLASS_MOBILE) || o.isA(POLCLASS_ITEM))
				//broadcast("setproperty test y ->"+value+"<-");
				//o.y := cint(value);
				MoveObjectToLocation( o, o.x, value, o.z, o.realm, MOVEOBJECT_FORCELOCATION );
				return "Property has been set.";
			else
				return "Cannot set Y because of class.";
			endif
		elseif (name == "z")
			if (o.isA(POLCLASS_MOBILE) || o.isA(POLCLASS_ITEM))
				//o.z := cint(value);
				MoveObjectToLocation( o, o.x, o.y, value, o.realm, MOVEOBJECT_FORCELOCATION );
				return "Property has been set.";
			else
				return "Cannot set Z because of class.";
			endif
		elseif (name == "realm" || name == "map")
			if (o.isA(POLCLASS_MOBILE) || o.isA(POLCLASS_ITEM))
				//o.realm := cint(value);
				MoveObjectToLocation( o, o.x, o.y, o.z, value, MOVEOBJECT_FORCELOCATION );
				return "Property has been set.";
			else
				return "Cannot set X because of class.";
			endif
		elseif (name == "location" || name == "loc" )
			//broadcast("setlocation ="+value);
			if (value [1] == "(")
				value[1] := "";
			endif
			if (value[len(value)] == ")" )
				value[len(value)] := "";
			endif
			var loc := SplitWords(value, ",");
			if (len(loc) < 3)
				return "Error on loc value, too short";
			endif

			if (o.isA(POLCLASS_MOBILE) || o.isA(POLCLASS_ITEM))
				//o.realm := cint(value);
				MoveObjectToLocation( o, cint(loc[1]), cint(loc[2]), cint(loc[3]), o.realm, MOVEOBJECT_FORCELOCATION );
				return "Property has been set.";
			else
				return "Cannot set location because of class.";
			endif
		elseif (name == "frozen")
			if (o.isA(POLCLASS_MOBILE))
				o.frozen := cint(value);
				return "Property has been set.";
			else
				return "Cannot set frozen because of class.";
			endif
		elseif (name == "paralyzed")
			if (o.isA(POLCLASS_MOBILE))
				o.paralyzed := cint(value);
				return "Property has been set.";
			else
				return "Cannot set paralized because of class.";
			endif
		elseif (name == "invul")
			if (o.isA(POLCLASS_MOBILE))
				if (value == "1" || value == "true")
					GrantPrivilege(o, "invul");
					o.enable("invul");
				else
					o.disable("invul");
					RevokePrivilege(o, "invul");
				endif
				return "Property has been set.";
			else
				return "Cannot set invulnerable because of class.";
			endif
		elseif (name == "dexmod")
			if (o.isA(POLCLASS_MOBILE))
				SetDexterityMod(o, cint(value));
				return "Property has been set.";
			else
				return "Cannot set dexmod because of class.";
			endif
		elseif (name == "intmod")
			if (o.isA(POLCLASS_MOBILE))
				SetIntelligenceMod(o, cint(value));
				return "Property has been set.";
			else
				return "Cannot set intmod because of class.";
			endif
		elseif (name == "strmod")
			if (o.isA(POLCLASS_MOBILE))
				SetStrengthMod(o, cint(value));
				return "Property has been set.";
			else
				return "Cannot set intmod because of class.";
			endif
		elseif (name == "dex" || name == "dexterity")
			if (o.isA(POLCLASS_MOBILE))
				SetBaseDexterity(o, cint(value));
				return "Property has been set.";
			else
				return "Cannot set dex because of class.";
			endif
		elseif (name == "int" || name == "intelligence")
			if (o.isA(POLCLASS_MOBILE))
				SetBaseIntelligence(o, cint(value));
				return "Property has been set.";
			else
				return "Cannot set int because of class.";
			endif
		elseif (name == "str" || name == "strength")
			if (o.isA(POLCLASS_MOBILE))
				SetBaseStrength(o, cint(value));
				return "Property has been set.";
			else
				return "Cannot set str because of class.";
			endif
		elseif (name == "locked" || name == "lock")
			if (o.isA(POLCLASS_CONTAINER))
				if (cint(value) > 0)
					o.locked := 1;
					if (cint(value) > 1)
						Setobjproperty(o, "LockDifficulty", cint(value));
					endif
					return "Property has been set.";
				else
					o.locked := 0;
					return "Property has been set.";
				endif
			else
				return "Cannot set locked because of class.";
			endif
		else
			var setted := o.Set_Member( name, value );
			if (setted.errortext)
				return setted.errortext;
			else
				return "Property has been set.";
			endif
		endif
		/*
				PropertyInfo plookup = LookupPropertyInfo(spawner, type, propname);

				if (plookup != null)
				{
					if (!plookup.CanWrite)
						return "Property is read only.";

					if (BaseXmlSpawner.IsProtected(type, propname))
						return "Property is protected.";

					string returnvalue = InternalSetValue(null, o, plookup, value, false, index);

					return returnvalue;

				}
				else
				{
					// note, looping through all of the props turns out to be a significant performance bottleneck
					// good place for optimization

					foreach (PropertyInfo p in props)
					{
						if (Insensitive.Equals(p.Name, propname))
						{

							if (!p.CanWrite)
								return "Property is read only.";

							if (BaseXmlSpawner.IsProtected(type, propname))
								return "Property is protected.";

							string returnvalue = InternalSetValue(null, o, p, value, false, index);

							return returnvalue;

						}
					}
				}
		*/
	endif
	return "Property not found.";
endfunction

//TODO FIX!
function SpawnTypeKeyword(spawner, TheSpawn, typeName, substitutedtypeName, requiresurface, spawnpositioning, triggermob, location, gumpcallback, byref status_str, loops)
	status_str := "";

	if (len(typeName) == 0 || !TheSpawn || len(substitutedtypeName) == 0)
		return 0;
	endif
	//invoker == spawner
	// check for any special keywords that might appear in the type such as SET, GIVE, or TAKE
/*	if (IsTypeKeyword(typeName))
		var kw := typeKeywordHash[typeName];//typeKeyword kw = typeKeywordHash[typeName];
		case (kw)
			typeKeyword.SET:
			break;
		endcase
	else
		if (IsSpecialItemKeyword(typeName))
			// these are special keyword item drops
			var arglist := ParseSlashArgs(cstr(substitutedtypeName), 2); //string[]
			var itemtypestr := cstr(arglist[1]);
			var baseitemtype := typeName;

			// itemtypestr will have the form keyword[,x[,y]]
			var itemkeywordargs := ParseString(itemtypestr, 3, ",");

			// deal with the special keywords
			//itemKeyword kw = (itemKeyword)itemKeywordHash[baseitemtype];
			if (typeName == "LOOT")
				if (len(itemkeywordargs) < 2)
					status_str := "Invalid LOOT format";
					return 0;
				elseif (len(itemkeywordargs) == 2)//LOOT,stringaloot
					var stringaloot := cstr(itemkeywordargs[2]);
				elseif (len(itemkeywordargs) == 3)//LOOT,stringaloot,chance
					var stringaloot := cstr(itemkeywordargs[2]);
					var chance_str := cstr(itemkeywordargs[3]);
					var chance := 1;

					if (chance_str[len(chance_str)-1] == ",")
						chance_str[len(chance_str)-1] := ".";
					endif
					if (chance_str[len(chance_str)-1] == ".")//anche i decimi
						chance_str[len(chance_str)-1] := "";
						chance := cdbl(CInt( chance_str )*0.1);
					else
						chance := cdbl(CInt( chance_str )*0.01);//senza decimi
					endif
				endif
			endif
		else
			// should never get here
			status_str := "unrecognized keyword";
			return 0;
		endif

	endif
*/

return 1;		
	













endfunction