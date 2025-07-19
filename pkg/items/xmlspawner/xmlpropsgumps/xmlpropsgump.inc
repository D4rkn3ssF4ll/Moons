
include "xmlsetgump";

//const m_BoolNames := array{ "True", "False" };
//const m_BoolValues := array{1, 0};//new object[]{ true, false };

//const m_PoisonNames := array{ "None", "Lesser", "Regular", "Greater", "Deadly", "Lethal" };
//const m_PoisonValues := array{ null, Poison.Lesser, Poison.Regular, Poison.Greater, Poison.Deadly, Poison.Lethal };

const OldStyle := 0;
const GumpOffsetX := 30;
const GumpOffsetY := 30;
const TextHue := 0;
const TextOffsetX := 2;
const OffsetGumpID := 0x0a40;//pure black
const HeaderGumpID := 0x0E14;//OldStyle ? 0x0BBC : 0x0E14; // Light offwhite or Dark navy blue, textured
const EntryGumpID := 0x0BBC; // Light offwhite, textured
const BackGumpID := 0x13BE; // Gray slate/stoney
const SetGumpID := 0x0E14; // Empty or Dark navy blue, textured
const SetWidth := 20;
const SetOffsetX := 2;
const SetOffsetY := 2;
const SetButtonID1 := 0x15E1; // Arrow pointing right
const SetButtonID2 := 0x15E5; // " pressed
const PrevWidth := 20;
const PrevOffsetX := 2;
const PrevOffsetY := 2;
const PrevButtonID1 := 0x15E3; // Arrow pointing left
const PrevButtonID2 := 0x15E7; // " pressed
const NextWidth := 20;
const NextOffsetX := 2;
const NextOffsetY := 2;
const NextButtonID1 := 0x15E1; // Arrow pointing right
const NextButtonID2 := 0x15E5; // " pressed
const OffsetSize := 1;
const EntryHeight := 20;
const BorderSize := 10;
const PrevLabel := 0;//oldstyle
const NextLabel := 0;
const PrevLabelOffsetY := 0;
const NextLabelOffsetX := -29;
const NextLabelOffsetY := 0;
const NameWidth := 103;
const ValueWidth := 82;
//const EntryWidth := 180;
const EntryCount := 66;
const ColumnEntryCount := 22;
const TypeWidth := NameWidth + OffsetSize + ValueWidth;
const TotalWidth := OffsetSize + NameWidth + OffsetSize + ValueWidth + OffsetSize + SetWidth + OffsetSize;
const TotalHeight := OffsetSize + ((EntryHeight + OffsetSize) * (EntryCount + 1));
const BackWidth := BorderSize + TotalWidth + BorderSize;
const BackHeight := BorderSize + TotalHeight + BorderSize;
//set constant gump

const EntryWidth := 212;
const setTotalWidth := 3*OffsetSize + EntryWidth + SetWidth;
const setTotalHeight := OffsetSize + (2 * (EntryHeight + OffsetSize));
const setBackWidth := BorderSize + setTotalWidth + BorderSize;
const setBackHeight := BorderSize + setTotalHeight + BorderSize;

//XmlPropertiesGump(who, spawner_item, X, Y, m_ShowGump, xoffset, page);
function XmlPropertiesGump( mobile, m_Object, m_Stack:= 0, m_List := 0, parent:=0, m_Page := 0 )
/*m_Mobile = mobile;
			m_Object = o;
			m_Stack = stack;
			m_List = BuildList();

			if ( parent != null )
			{
				if ( m_Stack == null )
#if(NEWTIMERS)
					m_Stack = new Stack<StackEntry>();
#else
					m_Stack = new Stack();
#endif

				m_Stack.Push( parent );
			}
*/
	//	Initialize( page );


	//m_Page = page;
	if (m_List == 0)
		m_List := BuildList( m_Object );
	endif

	var count := cint(len(m_List) - (page * EntryCount));

	if ( count < 1 )
		count := 1;//parto da 1
	elseif ( count > EntryCount )
		count := EntryCount;
	endif

	var lastIndex := cint((page * EntryCount) + count );//tolto il -1 e parto da 1

	if ( lastIndex > 0 && lastIndex < len(m_List) && !m_List[lastIndex] )
		count -=1;
	endif

	var totalHeight := cint(OffsetSize + ((EntryHeight + OffsetSize) * (ColumnEntryCount + 1)));

	var propgump := GFCreateGump(0, 0, TotalWidth*3 + BorderSize*2, BorderSize + totalHeight + BorderSize);
	AddPage(propgump, 0 );

	AddBackground(propgump, 0, 0, TotalWidth*3 + BorderSize*2, BorderSize + totalHeight + BorderSize, BackGumpID );
	if ( OldStyle == 1 )
		AddImageTiled(propgump, BorderSize, BorderSize + EntryHeight, TotalWidth - (SetWidth + OffsetSize)*3, totalHeight-EntryHeight, OffsetGumpID );
	else
		AddImageTiled(propgump, BorderSize, BorderSize + EntryHeight, (TotalWidth)*3, totalHeight-EntryHeight, OffsetGumpID );
	endif

	var x := cint(BorderSize + OffsetSize);
	var y := cint(BorderSize) /*+ OffsetSize*/;

	var emptyWidth := cint(TotalWidth - PrevWidth - NextWidth - (OffsetSize * 4));
	if ( OldStyle == 1 )
		emptyWidth -= SetWidth + OffsetSize;
	endif

	if(m_Object.isA(POLCLASS_ITEM))
		AddLabelCropped(propgump, x + TextOffsetX, y, TypeWidth - TextOffsetX, EntryHeight, TextHue, m_Object.name );
	endif

	var propcount := 0;

	var i;
	var index := page * EntryCount;
	for ( i := 0; i < count && index <= len(m_List); i+=1 )
		// do the multi column display
		var column := cint(propcount/ColumnEntryCount);

		if(propcount%ColumnEntryCount == 0)
			y := BorderSize;
		endif

		x := BorderSize + OffsetSize + column*(ValueWidth + NameWidth +OffsetSize*2 + SetOffsetX + SetWidth);
		y += EntryHeight + OffsetSize;

		var o := m_List[index+1];//aggiungo 1 prima?

		//TODO FIX!
		if ( !o )
			AddImageTiled(propgump, x - OffsetSize, y, TotalWidth, EntryHeight, BackGumpID + 4 );
			propcount+=1;
		/*elseif ( o is Type )
			Type type = (Type)o;

			AddImageTiled( x, y, TypeWidth, EntryHeight, EntryGumpID );
			AddLabelCropped( x + TextOffsetX, y, TypeWidth - TextOffsetX, EntryHeight, TextHue, type.Name );
			x += TypeWidth + OffsetSize;

			if ( SetGumpID != 0 )
				AddImageTiled( x, y, SetWidth, EntryHeight, SetGumpID );
				}
				else
                */
		//TODO FIX PROPERTYINFO
                else //if ( o is PropertyInfo )
			propcount+=1;

			//PropertyInfo prop = (PropertyInfo)o;

			// look for the default value of the equivalent property in the XmlSpawnerDefaults.DefaultEntry class

			var huemodifier := cint(TextHue);
			var finfo := array;//FieldInfo finfo = null;
			var de;//Server.Mobiles.XmlSpawnerDefaults.DefaultEntry de = new Server.Mobiles.XmlSpawnerDefaults.DefaultEntry();
			//Type ftype = de.GetType();
			
			//if(ftype != null)
				/*finfo = ftype.GetField(prop.Name);
				// is there an equivalent default field?
				if(finfo != null)
					    // see if the value is different from the default
                        		if(ValueToString(finfo.GetValue(de)) != ValueToString(prop))
    					   huemodifier = 68;
					endif
				endif*/

				AddImageTiled( propgump, x, y, NameWidth, EntryHeight, EntryGumpID );
				AddLabelCropped( propgump, x + TextOffsetX, y, NameWidth - TextOffsetX, EntryHeight, huemodifier, o );//o sarebbe il testo della prop
				x += NameWidth + OffsetSize;
				AddImageTiled( propgump, x, y, ValueWidth, EntryHeight, EntryGumpID );
				var label := cstr(GetObjProperty( m_Object, o ));
				if (len(label) > 10)
					label := label[1,10];
					label += "...";
				endif
				AddLabelCropped( propgump, x + TextOffsetX, y, ValueWidth - TextOffsetX, EntryHeight, huemodifier, label );
				x += ValueWidth + OffsetSize;

				if ( SetGumpID != 0 )
					AddImageTiled( propgump, x, y, SetWidth, EntryHeight, SetGumpID );
				endif

				//CPA cpa = GetCPA( prop );

				//if ( prop.CanWrite && cpa != null && m_Mobile.AccessLevel >= cpa.WriteLevel )
					AddButton( propgump, x + SetOffsetX, y + SetOffsetY, SetButtonID1, SetButtonID2, i + 3, GF_CLOSE_BTN);//GumpButtonType.Reply, 0 );
				//endif
			//endif
		endif
		index+=1;
	endfor

	var keyret := GFSendGump(mobile, propgump);

	var buttonID := cint(keyret[0]);
	SendSysMessage( mobile, "BUTTONID:= "+cstr(buttonID)  );

	if (buttonID == 0)
		if ( m_Stack && len(m_Stack) > 0 )
			//#if (NEWTIMERS)

			//StackEntry entry = m_Stack.Pop();//pop per le liste.

			//XmlPropertiesGump( from, entry.m_Object, m_Stack, null );

			//#else
			//object obj = m_Stack.Pop();
			//
			//from.SendGump( new XmlPropertiesGump( from, obj, m_Stack, null ) );
			//#endif
		endif
	elseif (buttonID == 1)//Previous
		if ( m_Page > 0 )
			XmlPropertiesGump( mobile, m_Object, m_Stack, m_List, m_Page - 1 );
		endif
	elseif (buttonID == 2) // Next
		if ( (m_Page + 1) * EntryCount < len(m_List) )
			XmlPropertiesGump( mobile, m_Object, m_Stack, m_List, m_Page + 1 );
		endif
	else
		SendSysMessage( mobile, "BTNID:" + buttonID);

		var index := cint(m_Page * EntryCount) + (buttonID - 3) + 1;

		if ( index >= 1 && index <= len(m_List) )
			var prop := m_List[index];// as PropertyInfo;

			if ( !prop )
				return 0;
			endif

			//CPA attr = GetCPA( prop );

			//if ( !prop.CanWrite || attr == null || from.AccessLevel < attr.WriteLevel )
			//	return;

			XmlSetGump( prop, mobile, m_Object, m_Stack, m_Page, m_List );

			/*
			Type type = prop.PropertyType;

			if ( IsType( type, typeofMobile ) || IsType( type, typeofItem ) )
				from.SendGump( new XmlSetObjectGump( prop, from, m_Object, m_Stack, type, m_Page, m_List ) );
			else if ( IsType( type, typeofType ) )
				from.Target = new XmlSetObjectTarget( prop, from, m_Object, m_Stack, type, m_Page, m_List );
			else if ( IsType( type, typeofPoint3D ) )
				from.SendGump( new XmlSetPoint3DGump( prop, from, m_Object, m_Stack, m_Page, m_List ) );
			else if ( IsType( type, typeofPoint2D ) )
				from.SendGump( new XmlSetPoint2DGump( prop, from, m_Object, m_Stack, m_Page, m_List ) );
			else if ( IsType( type, typeofTimeSpan ) )
				from.SendGump( new XmlSetTimeSpanGump( prop, from, m_Object, m_Stack, m_Page, m_List ) );
			else if ( IsCustomEnum( type ) )
				from.SendGump( new XmlSetCustomEnumGump( prop, from, m_Object, m_Stack, m_Page, m_List, GetCustomEnumNames( type ) ) );
			else if ( IsType( type, typeofEnum ) )
				from.SendGump( new XmlSetListOptionGump( prop, from, m_Object, m_Stack, m_Page, m_List, Enum.GetNames( type ), GetObjects( Enum.GetValues( type ) ) ) );
			else if ( IsType( type, typeofBool ) )
				from.SendGump( new XmlSetListOptionGump( prop, from, m_Object, m_Stack, m_Page, m_List, m_BoolNames, m_BoolValues ) );
			else if ( IsType( type, typeofString ) || IsType( type, typeofReal ) || IsType( type, typeofNumeric ) )
				from.SendGump( new XmlSetGump( prop, from, m_Object, m_Stack, m_Page, m_List ) );
			else if ( IsType( type, typeofPoison ) )
				from.SendGump( new XmlSetListOptionGump( prop, from, m_Object, m_Stack, m_Page, m_List, m_PoisonNames, m_PoisonValues ) );
			else if ( IsType( type, typeofMap ) )
				from.SendGump( new XmlSetListOptionGump( prop, from, m_Object, m_Stack, m_Page, m_List, Map.GetMapNames(), Map.GetMapValues() ) );
			else if ( IsType( type, typeofSkills ) && m_Object is Mobile )
			{
				from.SendGump( new XmlPropertiesGump( from, m_Object, m_Stack, m_List, m_Page ) );
				from.SendGump( new SkillsGump( from, (Mobile)m_Object ) );
			}
			else if( HasAttribute( type, typeofPropertyObject, true ) )
			{
				//#if (NEWTIMERS)
				object obj = prop.GetValue( m_Object, null );

				if ( obj != null )
					from.SendGump( new XmlPropertiesGump( from, obj, m_Stack, new StackEntry( m_Object, prop ) ) );
				else
					from.SendGump( new XmlPropertiesGump( from, m_Object, m_Stack, m_List, m_Page ) );
				//#else
				//from.SendGump( new XmlPropertiesGump( from, prop.GetValue( m_Object, null ), m_Stack, m_Object ) );
				//#endif
			*/
		endif
	endif


endfunction

function BuildList( item )//arraylist
	var list := GetObjPropertyNames( item );
	return list;
/*
			Type type = m_Object.GetType();

			PropertyInfo[] props = type.GetProperties( BindingFlags.Static | BindingFlags.Instance | BindingFlags.Public );

			ArrayList groups = GetGroups( type, props );
			ArrayList list = new ArrayList();

			for ( int i = 0; i < groups.Count; ++i )
			{
				DictionaryEntry de = (DictionaryEntry)groups[i];
				ArrayList groupList = (ArrayList)de.Value;

				if ( !HasAttribute( (Type)de.Key, typeofNoSort, false ) )
					groupList.Sort( PropertySorter.Instance );

				if ( i != 0 )
					list.Add( null );

				list.Add( de.Key );
				list.AddRange( groupList );
			}

			return list;
*/
endfunction
