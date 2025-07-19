include "xmlpropsgump";

//XmlSetGump( prop, from, m_Object, m_Stack, m_Page, m_List );

function XmlSetGump( m_Property, mobile, byref m_Object, m_Stack:= 0, m_Page := 0, m_List := 0 )

	var canNull := 0;// = !prop.PropertyType.IsValueType;
	var canDye := 0;// = prop.IsDefined( typeof( HueAttribute ), false );
	var isBody := 0;// = prop.IsDefined( typeof( BodyAttribute ), false );
	var xextend := 0;
	//if(prop.PropertyType == typeof(string))
	//	xextend := 300;
	//endif
	var val := GetObjProperty( m_Object, m_Property );
	var initialText := "";
	if ( val )
		initialText := cstr(val);
		if (len(initialText) > 30)
			initialText := initialText[1,30];
			initialText += "...";
		endif
	endif

	var altHeight := 0;
	//var altWidth := 0;
	if ( canNull )
		altHeight += EntryHeight + OffsetSize;
	endif
	if ( canDye )
		altHeight += EntryHeight + OffsetSize;
	endif
	if ( isBody )
		altHeight += EntryHeight + OffsetSize;
	endif

	var setgump := GFCreateGump(0, 0);//, BackWidth+xextend, BackHeight + altHeight);//, BackGumpID);
	AddBackground(setgump, 0, 0, setBackWidth+xextend, setBackHeight + altHeight, BackGumpID );
	AddImageTiled(setgump, BorderSize, BorderSize, setTotalWidth+xextend, setTotalHeight + altHeight, OffsetGumpID );

	var x := BorderSize + OffsetSize;
	var y := BorderSize + OffsetSize;

	AddImageTiled(setgump, x, y, EntryWidth+xextend, EntryHeight, EntryGumpID );
	AddLabelCropped(setgump, x + TextOffsetX, y, EntryWidth+xextend - TextOffsetX, EntryHeight, TextHue, m_Property );
	x += EntryWidth+xextend + OffsetSize;

	if ( SetGumpID != 0 )
		AddImageTiled(setgump, x, y, SetWidth, EntryHeight, SetGumpID );
	endif

	x := BorderSize + OffsetSize;
	y += EntryHeight + OffsetSize;

	AddImageTiled(setgump, x, y, EntryWidth+xextend, EntryHeight, EntryGumpID );
	AddTextEntry(setgump, x + TextOffsetX, y, EntryWidth+xextend - TextOffsetX, EntryHeight, TextHue, 1, initialText );
	x += EntryWidth+xextend + OffsetSize;

	if ( SetGumpID != 0 )
		AddImageTiled(setgump, x, y, SetWidth, EntryHeight, SetGumpID );
	endif

	AddButton(setgump, x + SetOffsetX, y + SetOffsetY, SetButtonID1, SetButtonID2, 2, GF_CLOSE_BTN);//GumpButtonType.Reply, 0 );

	if ( canNull )
	/*
		x = BorderSize + OffsetSize;
		y += EntryHeight + OffsetSize;

		AddImageTiled( x, y, EntryWidth+xextend, EntryHeight, EntryGumpID );
		AddLabelCropped( x + TextOffsetX, y, EntryWidth+xextend - TextOffsetX, EntryHeight, TextHue, "Null" );
		x += EntryWidth+xextend + OffsetSize;

		if ( SetGumpID != 0 )
			AddImageTiled( x, y, SetWidth, EntryHeight, SetGumpID );

		AddButton( x + SetOffsetX, y + SetOffsetY, SetButtonID1, SetButtonID2, 2, GumpButtonType.Reply, 0 );
	*/
	endif
	if ( canDye )
	/*
				x = BorderSize + OffsetSize;
				y += EntryHeight + OffsetSize;

				AddImageTiled( x, y, EntryWidth+xextend, EntryHeight, EntryGumpID );
				AddLabelCropped( x + TextOffsetX, y, EntryWidth+xextend - TextOffsetX, EntryHeight, TextHue, "Hue Picker" );
				x += EntryWidth+xextend + OffsetSize;

				if ( SetGumpID != 0 )
					AddImageTiled( x, y, SetWidth, EntryHeight, SetGumpID );

				AddButton( x + SetOffsetX, y + SetOffsetY, SetButtonID1, SetButtonID2, 3, GumpButtonType.Reply, 0 );
	*/
	endif
	if ( isBody )
	/*
x = BorderSize + OffsetSize;
				y += EntryHeight + OffsetSize;

				AddImageTiled( x, y, EntryWidth+xextend, EntryHeight, EntryGumpID );
				AddLabelCropped( x + TextOffsetX, y, EntryWidth+xextend - TextOffsetX, EntryHeight, TextHue, "Body Picker" );
				x += EntryWidth+xextend + OffsetSize;

				if ( SetGumpID != 0 )
					AddImageTiled( x, y, SetWidth, EntryHeight, SetGumpID );

				AddButton( x + SetOffsetX, y + SetOffsetY, SetButtonID1, SetButtonID2, 4, GumpButtonType.Reply, 0 );
	*/
	endif

	var keyret := GFSendGump(mobile, setgump);

	var buttonID := cint(keyret[0]);
	//SendSysMessage( mobile, "BUTTONID:= "+cstr(buttonID)  );
	//object toSet;
	//bool shouldSet, shouldSend = true;
	//buttonid <=1 closed with right click!
	if ( buttonID == 2 )//set
		//TextRelay text = info.GetTextEntry( 0 );
		var value := GetTextEntry(keyret, 1);

		if( value[1] in {"+","-","0","1","2","3","4","5","6","7","8","9"} )
			// Make it an actual integer variable if it starts with +/-
			// This makes a difference to the setting of a CProp (ie. how it's packed)
			value := CInt( value );
		endif

	        var ret_val := SetObjProperty( m_Object, m_Property, value );
	        if( !ret_val )
		         SendSysMessage( mobile, "Error setting CProp ->"+ret_val );
		         return 0;
                endif
                
                SendSysMessage( mobile, "Successful (set as "+TypeOf(value)+")" );
		//SendSysMessage( mobile, "text := "+text  );
	endif
endfunction