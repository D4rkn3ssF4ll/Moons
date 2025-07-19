
const defProximityRange := 3;
const defResetRange := 16;
const defResetTime := 60;//secondi
const defSpeechPace := 10;

const NPCDataSetName := "XmlQuestNPC";
const NPCPointName := "NPC";
const SpeechPointName := "SpeechEntry";
const DefsDir := "XmlQuestNPC";

var m_CurrentEntryNumber := -1;
var m_CurrentEntry := 0;

function XmlDialog ( ConfigFile := "" )


	var dialog := struct;

	dialog.+SpeechEntries := array; //private ArrayList m_SpeechEntries = new ArrayList();
	dialog.+ActivePlayer; // keep track of the player that is currently engaged in conversation so that other players speech can be ignored.
	dialog.+AllowGhostTrig := 0;
	dialog.+AttachedTo;
	dialog.+ConfigFile := "";
	dialog.+HoldProcessing;
	dialog.+IsActive := 0;
	dialog.+LastInteraction := 0;
	dialog.+ProximityRange := defProximityRange;
	dialog.+ResetRange := defResetRange;
	dialog.+ResetTime := defResetTime;//timespan
	dialog.+ResponseString;
	dialog.+Running := 1;
	dialog.+SpeechPace := defSpeechPace;// used for automatic prepause delay calculation.  delayinsecs = speechlength/speechpace + 1
	//private InternalTimer m_Timer;
	dialog.+TextEntryBook;//lista libri per cancellarli
	dialog.+TriggerAccessLevel := 0;//0 player
	dialog.+TriggerOnCarried := "";//item name m_ItemTriggerName;
	dialog.+NoTriggerOnCarried := "";
	dialog.+EntryNumber := m_CurrentEntryNumber;

	if (len(ConfigFile) > 0)
		DoLoadNPC(0, ConfigFile);
	else
		//EntryNumber := 0;
                m_CurrentEntryNumber := 0;
                // get the entry corresponding to the number
                m_CurrentEntry := GetEntry(dialog, m_CurrentEntryNumber);

		if (m_CurrentEntry != 0 && !m_CurrentEntry.errortext)
			m_CurrentEntryNumber := m_CurrentEntry.EntryNumber;
		else
			m_CurrentEntryNumber := -1;
		endif
	endif

	dialog.+CurrentEntry := m_CurrentEntry;
	dialog.+EntryNumber := m_CurrentEntryNumber;

	return dialog;
endfunction

function DeleteTextEntryBook( byref dialog )
/*if (m_TextEntryBook != null)
            {
                foreach (Item s in m_TextEntryBook)
                    s.Delete();

                m_TextEntryBook = null;
            }
*/
endfunction

function DoLoadNPC(from, filename)
endfunction

function GetEntry(byref dialog, entryid)//SpeechEntry
	if (entryid < 0)
		return 0;
	endif

	if (len(dialog.SpeechEntries) < 1 || dialog.SpeechEntries.errortext)
		dialog.SpeechEntries := array;
	endif

	// find the speech entry that matches the current entry number
	foreach s in (dialog.SpeechEntries)
		if (s.EntryNumber == entryid)
			return s;
			break;
		endif
	endforeach

	// didnt find it so make a new entry
	var newentry := SpeechEntry();
	newentry.EntryNumber := entryid;
	if (newentry.EntryNumber != 0)
		newentry.ID := entryid;
	endif
	dialog.SpeechEntries.append(newentry);//Add
	return newentry;
endfunction

function Action(byref dialog, value := "")
	if (!dialog.CurrentEntry.errortext)
		if (len(value) > 0)
			dialog.CurrentEntry.Action := cstr(value);
		else
			return dialog.CurrentEntry.Action;
		endif
	else
		return "";
	endif
endfunction

function AllowNPCTrigger(byref dialog, value := -1)
	if (!dialog.CurrentEntry.errortext)
		if (value >= 0)
			dialog.CurrentEntry.AllowNPCTrigger := cint(value);
		else
			return dialog.CurrentEntry.AllowNPCTrigger;
		endif
	else
		return 0;
	endif
endfunction

function Condition(byref dialog, value := "")
	if (!dialog.CurrentEntry.errortext)
		if (len(value) > 0)
			dialog.CurrentEntry.Condition := cstr(value);
		else
			return dialog.CurrentEntry.Condition;
		endif
	else
		return "";
	endif
endfunction

function DependsOn(byref dialog, value := "")
	if (!dialog.CurrentEntry.errortext)
		if (len(value) > 0)
			dialog.CurrentEntry.DependsOn := cstr(value);
		else
			return dialog.CurrentEntry.DependsOn;
		endif
	else
		return "-1";
	endif
endfunction

function Gump(byref dialog, value := "")
	if (!dialog.CurrentEntry.errortext)
		if (len(value) > 0)
			dialog.CurrentEntry.Gump := cstr(value);
		else
			return dialog.CurrentEntry.Gump;
		endif
	else
		return "";
	endif
endfunction

function ID(byref dialog, value := -1)
	if (!dialog.CurrentEntry.errortext)
		if (value >= 0)
			dialog.CurrentEntry.ID := cint(value);
		else
			return dialog.CurrentEntry.ID;
		endif
	else
		return -1;
	endif
endfunction

function IgnoreCarried(byref dialog, value := -1)
	if (!dialog.CurrentEntry.errortext)
		if (value >= 0)//0;1
			dialog.CurrentEntry.IgnoreCarried := cint(value);
		else
			return dialog.CurrentEntry.IgnoreCarried;
		endif
	else
		return 1;
	endif
endfunction

function Keywords(byref dialog, value := "")
	if (!dialog.CurrentEntry.errortext)
		if (len(value) > 0)
			dialog.CurrentEntry.Keywords := cstr(value);
		else
			return dialog.CurrentEntry.Keywords;
		endif
	else
		return "";
	endif
endfunction

function LockConversation(byref dialog, value := -1)
	if (!dialog.CurrentEntry.errortext)
		if (value >= 0)//0;1
			dialog.CurrentEntry.LockConversation := cint(value);
		else
			return dialog.CurrentEntry.LockConversation;
		endif
	else
		return 1;
	endif
endfunction

function Pause(byref dialog, value := -1)
	if (!dialog.CurrentEntry.errortext)
		if (value >= 0)
			dialog.CurrentEntry.Pause := cint(value);
		else
			return dialog.CurrentEntry.Pause;
		endif
	else
		return -1;
	endif
endfunction

function PrePause(byref dialog, value := -1)
	if (!dialog.CurrentEntry.errortext)
		if (value >= 0)
			dialog.CurrentEntry.PrePause := cint(value);
		else
			return dialog.CurrentEntry.PrePause;
		endif
	else
		return -1;
	endif
endfunction

function SpeechHue(byref dialog, value := -1)
	if (!dialog.CurrentEntry.errortext)
		if (value >= 0)
			dialog.CurrentEntry.SpeechHue := cint(value);
		else
			return dialog.CurrentEntry.SpeechHue;
		endif
	else
		return -1;
	endif
endfunction

function SpeechStyle(byref dialog, value := "")
	if (!dialog.CurrentEntry.errortext)
		if (len(value) > 0)
			dialog.CurrentEntry.SpeechStyle := cstr(value);
		else
			return dialog.CurrentEntry.SpeechStyle;
		endif
	else
		return "Regular";
	endif
endfunction

function Text(byref dialog, value := "")
	if (!dialog.CurrentEntry.errortext)
		if (len(value) > 0)
			dialog.CurrentEntry.Text := cstr(value);
		else
			return dialog.CurrentEntry.Text;
		endif
	else
		return "";
	endif
endfunction

function SpeechEntry()
	var sentry := struct;
	sentry.+EntryNumber;
        sentry.+ID;
	sentry.+Text;     // text displayed when the entry is activated
	sentry.+Keywords; // comma separated list of keywords that can be matched to activate the entry.  If no keywords are present then it is automatically activated
	sentry.+Action;   // action string
	sentry.+Condition;    // condition test string
	sentry.+DependsOn;   // the previous entrynumber required to activate this entry
	sentry.+Pause := 1;       // pause in seconds before advancing to the next entry
	sentry.+PrePause := -1;    // pause in seconds before saying the speech for this entry.  -1 indicates the use of auto pause calculation based on triggering speech length.
	sentry.+LockConversation := 1;    // flag to determine if the conversation locks to one player
	sentry.+AllowNPCTrigger := 0; // flag to determine if npc speech can trigger it
	sentry.+SpeechStyle := "Regular";//MessageType
	sentry.+Gump;		// GUMP specification string
	sentry.+SpeechHue := -1;			// speech hue
	sentry.+IgnoreCarried := 0; // ignore the TriggerOnCarried/NoTriggerOnCarried settings for the dialog when activating this entry
	return sentry;
/*
            public int SpeechHue
            {
                get { return m_SpeechHue; }
                set
                {
                    // dont allow invalid hues
                    m_SpeechHue = value;
                    if (m_SpeechHue > 37852) m_SpeechHue = 37852;
                }
            }
            public int ID
            {
                get { return m_ID; }
                set
                {
                    // dont allow ID modification of entry 0
                    if (EntryNumber == 0) return;
                    m_ID = value;
                }
            }
	*/
endfunction

// see if the DependsOn property contains the specified id
function CheckDependsOn(s, id)
	if (s.errortext || s.DependsOn.errortext)
		return 0;
	endif

	// parse the DependsOn string
	var args := SplitWords(s.DependsOn, ","); //s.DependsOn.Split(',');
	var i;
	for (i := 1; i <= len(args); i++)
		if (cint(TrimStr(args[i])) == id)
			return 1;
			break;
		endif
	endfor
	return 0;
endfunction

function CheckForReset(dialog)
	// check to see if the interaction time has elapsed or player has gone out of range.  If so then reset to entry zero
	if (!dialog.HoldProcessing && 
                ((ReadGameClock() - dialog.ResetTime > dialog.LastInteraction) ||
                (/*AttachedTo is IEntity && m_ */ !dialog.ActivePlayer.errortext && !IsInRange(dialog.ActivePlayer, dialog.AttachedTo, dialog.ResetRange))))
                Reset( dialog );
	endif
endfunction

//TODO: FIX TIMER:
function Reset( dialog )
	dialog.EntryNumber := 0;
	dialog.IsActive := 0;
	dialog.ActivePlayer := error{"errortext":="null"};
	// turn off the timer
	//if (m_Timer != null)
	//	m_Timer.Stop();
	//endif
endfunction

//TODO: fix
function ProcessSpeech(byref dialog, m, speech)
	if (dialog.HoldProcessing)
		return 1;
	endif

	// check the speech against the entries that depend on the present entry
	var matchentry := FindMatchingKeyword(m, speech, ID(dialog));

	if (!matchentry || matchentry.errortext)
		return 0;
	endif

	// when attempting to process speech-triggered speech, check for oncarried dependencies
	// This will not apply to movement-triggered speech (banter with -1 dependson) which will continue to be activated
	// regardless of oncarried status
	// dependson of -2 will allow non-speech triggering but will still apply oncarried dependencies

	// if player-carried item triggering is set then test for the presence of an item on the player an in their pack
	if ((len(speech) > 0 || CheckDependsOn(matchentry, -2)) && !dialog.TriggerOnCarried.errortext && len(dialog.TriggerOnCarried) > 0)
		//TODO FIX
		/*var found := BaseXmlSpawner.CheckForCarried(m, TriggerOnCarried) || matchentry.IgnoreCarried;

                // is the player carrying the right item, if not then dont process
                if (!found) return false;
		*/
	endif

	// if player-carried noitem triggering is set then test for the presence of an item in the players pack that should block triggering
	if ((len(speech) > 0 || CheckDependsOn(matchentry, -2)) && !dialog.NoTriggerOnCarried.errortext && len(dialog.NoTriggerOnCarried) > 0)
		//TODO FIX
		/*
                bool notfound = BaseXmlSpawner.CheckForNotCarried(m, NoTriggerOnCarried) || matchentry.IgnoreCarried;

                // is the player carrying the right item, if so then dont process
                if (!notfound) return false;
		*/
	endif

	dialog.ResponseString := speech;

	// the player that successfully activates a conversation by speech becomes the exclusive conversationalist until the npc resets
	if ((len(speech) > 0 || dialog.LockConversation) && !m.errortext)
                dialog.ActivePlayer := m;
	endif
endfunction

function ValidMovementTrig(dialog, m)
	if (m.errortext)
		return 0;
	endif

	return (
                ((m.acctname && (m.cmdlevel <= dialog.TriggerAccessLevel))) &&
                ((!IsGhost(m.graphic) && !dialog.AllowGhostTrig) || (IsGhost(m.graphic) && dialog.AllowGhostTrig)));
endfunction

function ValidSpeechTrig(dialog, m)
	if (m.errortext)
		return 0;
	endif

	var allownpctrigger := 0;
	if (!dialog.CurrentEntry.errortext)
		allownpctrigger := dialog.CurrentEntry.AllowNPCTrigger;
	endif

            return (
                ((m.acctname && (m.cmdlevel <= dialog.TriggerAccessLevel)) || (allownpctrigger && !(m.acctname))) &&
                ((!IsGhost(m.graphic) && !dialog.AllowGhostTrig) || (IsGhost(m.graphic) && dialog.AllowGhostTrig)));
endfunction

function IsGhost(m_BodyID)
	return m_BodyID == 402 || m_BodyID == 403
		|| m_BodyID == 607 || m_BodyID == 608
		|| m_BodyID == 694 || m_BodyID == 695
		|| m_BodyID == 970;
endfunction