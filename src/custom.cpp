//REVIEW: split into 3 separate files (ROM, actions, condition, other)

#include "stdafx.h"
#include "YICT.h"
#include "maindlg.h"
#include "custom.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


///////////////////////////////////////////////////////
// CART_DATA specific to one being edited

CART_DATA g_cart; // cartridge being edited

#define WHICH_AUDIO
#ifdef WHICH_AUDIO

struct MOT2AUD
{
    WORD mot; // index
    const char* sz; // string for which audio used
};

MOT2AUD g_mot2aud[] =
{
#include "mot2aud.c_"
};

static void AppendWhichAudio(CString& str, addr_t addr)
{
	str += " {";

	byte* pbAction = g_cart.Lookup(addr);
	int nMotion = *pbAction++;
	pbAction++;	// skip repeat
	for (int iMotion = 0; iMotion < nMotion; iMotion++)
	{
		// motions
		WORD mot = *(WORD*)pbAction;
        for (int iM = 0; iM < sizeof(g_mot2aud)/sizeof(MOT2AUD); iM++)
        {
            if (g_mot2aud[iM].mot == mot)
            {
				str += g_mot2aud[iM].sz;
                str += '+';
                break;
            }
        }
		pbAction += 6;
	}

	str += "}";
}
#endif

/////////////////////////////////////////////////////////



static bool SaveHalf(const char* szName, const char* szExt, int iSpan)
	// if failure, report error then return false
{
	char szT[_MAX_PATH];
	sprintf(szT, "%s%s", szName, szExt);
	FILE* pf = fopen(szT, "wb");
	if (pf == NULL)
	{
		CString str;
		str.Format("ERROR: Can't open file '%s' for writing", szT);
		AfxMessageBox(str);
		return false;
	}
	byte* loadImage = new byte[ROM_SIZE/2];
	for (int i = 0; i < ROM_SIZE/2; i++)
		loadImage[i] = g_cart.m_image[i*2+iSpan];
	if (fwrite(loadImage, ROM_SIZE/2, 1, pf) != 1)
	{
		CString str;
		str.Format("ERROR: Invalid image file '%s'", szT);
		AfxMessageBox(str);
		fclose(pf);
		delete [] loadImage;
		return false;
	}
	delete [] loadImage;
	fclose(pf);
	return true;
}

bool SaveRomImage()
{
    if (!SaveHalf(g_cart.GetBaseName(), "-l.bin", 0))
        return false;
    if (!SaveHalf(g_cart.GetBaseName(), "-h.bin", 1))
        return false;
	g_cart.SetDirty(false);
	return true;
}

////////////////////////////////////////////////////////////////////////////
// Moods/Conditions

enum COND_TYPE
{
    CT_PROBACTION,
    CT_ACTION,  // single action
};

struct CONDITION
{
	COND_TYPE ct;
    int index;
    const char* szName;
    const char* szNewCategory;

	addr_t* _GetImageAddress() const;	// sort-of const - the image data is being changed

	addr_t GetAddress() const { return *_GetImageAddress(); }
	void SetAddress(addr_t addrNew) const { *_GetImageAddress() = addrNew; }
};

addr_t* CONDITION::_GetImageAddress() const
{
	if (ct == CT_PROBACTION)
	{
		ASSERT(index >= 0 && index < 64);
		return g_cart.LookupAddr(g_cart.GetYD()->addrProbActionTable + index*4);
	}
	else
	{
		ASSERT(ct == CT_ACTION);
		ASSERT(index >= 0 && index < g_cart.GetYD()->numExtraActionTable);
		return g_cart.LookupAddr(g_cart.GetYD()->addrExtraActionTable + index*4);
	}
}


#include "conditions.c_"

#define NUM_COND    (sizeof(g_conditions)/sizeof(CONDITION))

const char* GetCondName(int iCond, const char* & szCatRet)
{
    if (iCond < NUM_COND)
	{
		szCatRet = g_conditions[iCond].szNewCategory;
		return g_conditions[iCond].szName;
	}
    return NULL;    // no more
}

bool IsMultiCond(int iCond)
{
	ASSERT(iCond >= 0 && iCond < NUM_COND);
    CONDITION const& cond = g_conditions[iCond];
	return (cond.ct == CT_PROBACTION);
}

////////////////////////////////////////////////////////////////////////////

#define UNAMES_DYNAMIC_ADDRESSES	// filled in by YICT_DATA info
#include "../unames.h"

static int FindActionByAddress(addr_t addrFind)
{
	// find the action from master list
	for (int iAction = 0; iAction < NUM_UNAME; iAction++)
		if (addrFind == g_uniNames[iAction].addrDynamic)
			return iAction;
	return -1;
}


static int FindActionByNumber(int iStore)
{
	for (int iAction = 0; iAction < NUM_UNAME; iAction++)
	{
		UNAME const& uname = g_uniNames[iAction];
		if (uname.iStore == iStore)
		{
#if 0
			// detect redundanct
			for (int i2 = iAction+1; i2 < NUM_UNAME; i2++)
				if (g_uniNames[i2].iStore == iStore)
					TRACE("BOGUS: %d and %d\n", iStore, i2);
#endif
			return iAction;
		}
	}
	return -1;
}

int GetCurrentAction(int iCond) // single
{
	ASSERT(iCond >= 0 && iCond < NUM_COND);
    CONDITION const& cond = g_conditions[iCond];
	ASSERT(cond.ct == CT_ACTION);
	return FindActionByAddress(cond.GetAddress());
}

int GetStorageNum(int iAct)
{
	ASSERT(iAct >= 0 && iAct < NUM_UNAME);
	UNAME const& uname = g_uniNames[iAct];
	return uname.iStore;
}

int FindStdName(const char* szStdName)
{
	for (int iAction = 0; iAction < NUM_UNAME; iAction++)
	{
		UNAME const& uname = g_uniNames[iAction];
		if (strcmp(uname.szStdAction, szStdName) == 0)
			return iAction;
	}
	return -1;
}

int FindSimilarName(int iActionBase, const char* szTag)
{
	ASSERT(iActionBase >= 0 && iActionBase < NUM_UNAME);
	UNAME const& uname = g_uniNames[iActionBase];
	if (uname.szStdAction == NULL)
		return -1;
	// see if there is a DUAL
	char szT[64];
	sprintf(szT, "%s%s", uname.szStdAction, szTag);
	return FindStdName(szT);
}


////////////////////////////////////////////////////////////////////////////

static CByteArray g_perfData[NUM_COND];
    // multiple of 5 bytes
    // in iCond order (only needed for CT_PROBACTION)

int FindSpan(addr_t addr)
{
    int n = 0;
    int percentLast = 0;
    while (percentLast != 100)
    {
		int percent = *g_cart.Lookup(addr++);
		if (percent < 1 || percent > 100)
            return false;
        if (percent <= percentLast)
            return false;

		addr_t addrAction = *g_cart.LookupAddr(addr);
		addr += 4;
	    if (addrAction < ADDR_BASE || addr >= ADDR_BASE + ROM_SIZE)
            return false;
        percentLast = percent;
        n++;
    }
    return n;
}

bool ExtractAllPerfData()
{
    for (int iCond = 0; iCond < NUM_COND; iCond++)
    {
        CONDITION const& cond = g_conditions[iCond];
        if (cond.ct != CT_PROBACTION)
            continue;

		addr_t addr = cond.GetAddress();
        CByteArray& data = g_perfData[iCond];
        ASSERT(data.GetSize() == 0);
        if (addr == NULL)
            continue; // empty

        int n = FindSpan(addr);
        if (n < 1)
            return false;
        data.SetSize(5*n);
        memcpy(&data[0], g_cart.Lookup(addr), 5*n);
    }

	// concatenate the three actions lists into one list,
	//  put actual addresses in g_uniNames array

	int nActionsFromYictData = 0;
	for (int iAC = 0; iAC < NUM_ACTION_CHUNK; iAC++)
	{
		int n = g_cart.GetYD()->availableActions[iAC].count;
		TRACE("%d ", n);
		nActionsFromYictData += n;
	}
		

	TRACE("; total %d / %d", nActionsFromYictData, NUM_UNAME);
	
	if (nActionsFromYictData != NUM_UNAME)
	{
		AfxMessageBox("YICT version mis-match");
		return false;
	}
	
	// actions must be in 'storage' order

	int iStorage = 0;
	for (/*int*/ iAC = 0; iAC < NUM_ACTION_CHUNK; iAC++)
	{
		YICT_ACTIONCHUNK const& aa = g_cart.GetYD()->availableActions[iAC];
		addr_t addr = aa.start;
		int count = aa.count;
		while (count--)
		{
			// use storage numbers instead
			ASSERT(iStorage < NUM_UNAME);
			int iAct = FindActionByNumber(iStorage);
			ASSERT(iAct != -1);
			ASSERT(g_uniNames[iAct].addrDynamic == NULL);
			g_uniNames[iAct].addrDynamic = addr;
			byte nChunks = *g_cart.Lookup(addr);
			addr += 1 + 1 + nChunks * 6;
			iStorage++;
		}
	}
	ASSERT(iStorage == NUM_UNAME);
    return true;
}

////////////////////////////////////////////////////////////////////////////


int StartPerfIterate(int iCond)
{
	ASSERT(iCond >= 0 && iCond < NUM_COND);
    CONDITION const& cond = g_conditions[iCond];
	if (cond.ct == CT_PROBACTION)
	{
		CByteArray& data = g_perfData[iCond];
		if (data.GetSize() == 0)
			return -1;
		return 0;	// start at the start
	}
	else if (cond.ct == CT_ACTION)
	{
		return 0;
	}
	ASSERT(false);
	return -1;
}

		
void ParsePerfData(int iCond, int& index, int& percentOut, CString& strStdOut, CString& strActionNameOut)
{
	ASSERT(iCond >= 0 && iCond < NUM_COND);
    CONDITION const& cond = g_conditions[iCond];
	addr_t addrAction;

	if (cond.ct == CT_PROBACTION)
	{
		CByteArray& data = g_perfData[iCond];
		percentOut = data[index];
		index++;
		ASSERT(percentOut >= 1 && percentOut <= 100);
		addrAction = *(addr_t*)&data[index];
		index += 4;
	}
	else if (cond.ct == CT_ACTION)
	{
		ASSERT(index == 0);
		percentOut = 999; // one and only
		addrAction = cond.GetAddress();
	}

	const UNAME* pu = FindUniName(addrAction);
	if (pu != NULL)
	{
		strStdOut = pu->szStdAction;
		strActionNameOut.Format("%s", pu->szDesc);
#ifdef WHICH_AUDIO
		AppendWhichAudio(strActionNameOut, pu->addrDynamic);
#endif
	}
	else
	{
		// unknown
		strStdOut.Empty();
		strActionNameOut.Format("unknown $%06X", addrAction);
	}
}

///////////////////////////////////////////////////////////////////////////


void DeletePerformance(int iCond, int iItem)
{
	ASSERT(iCond >= 0 && iCond < NUM_COND);
    CONDITION const& cond = g_conditions[iCond];
	ASSERT(cond.ct == CT_PROBACTION);
	CByteArray& data = g_perfData[iCond];
	ASSERT(iItem >= 0 && iItem * 5 < data.GetSize());
	data.RemoveAt(iItem*5, 1*5);
	g_cart.SetDirty(true);
}

void RebalanceProbabilities(int iCond)
{
	ASSERT(iCond >= 0 && iCond < NUM_COND);
    CONDITION const& cond = g_conditions[iCond];
	ASSERT(cond.ct == CT_PROBACTION);
	CByteArray& data = g_perfData[iCond];
	int nItems = data.GetSize()/5;
	if (nItems == 0)
		return;
	int percentEach = 100/nItems;
	int percentNow = 0;
	for (int i = 0; i < nItems-1; i++)
	{
		percentNow += percentEach;
		data[i*5] = percentNow;
	}
	data[(nItems-1)*5] = 100;
	g_cart.SetDirty(true);
}

int GetCurrentActions(int iCond, int selections[MAX_ACTIONS_PER_COND])
{
	ASSERT(iCond >= 0 && iCond < NUM_COND);
    CONDITION const& cond = g_conditions[iCond];
	ASSERT(cond.ct == CT_PROBACTION);
	CByteArray& data = g_perfData[iCond];
	int nItems = data.GetSize()/5;
	if (nItems > MAX_ACTIONS_PER_COND)
        return -1;
    for (int i = 0; i < nItems; i++)
    {
        int iAction = FindActionByAddress(*(addr_t*)&data[i*5+1]);
        if (iAction == -1)
            return -1;
		selections[i] = iAction;
    }
    return nItems;
}

void AddPerformance(int iCond, int iAction)
{
	ASSERT(iCond >= 0 && iCond < NUM_COND);
    CONDITION const& cond = g_conditions[iCond];
	ASSERT(cond.ct == CT_PROBACTION);
	CByteArray& data = g_perfData[iCond];
	int nOld = data.GetSize()/5;

	ASSERT(iAction >= 0 && iAction < NUM_UNAME);
	addr_t addrAction = g_uniNames[iAction].addrDynamic;
	ASSERT(addrAction != NULL);	// don't use holes

	data.SetSize((nOld+1)*5);
	data[nOld*5+0] = 0;
	*(addr_t*)&data[nOld*5+1] = addrAction;
	g_cart.SetDirty(true);
}

void ChangeSinglePerformance(int iCond, int iAction)
{
	ASSERT(iCond >= 0 && iCond < NUM_COND);
    CONDITION const& cond = g_conditions[iCond];
	ASSERT(cond.ct == CT_ACTION);

	ASSERT(iAction >= 0 && iAction < NUM_UNAME);
	addr_t addrAction = g_uniNames[iAction].addrDynamic;
	ASSERT(addrAction != NULL);	// don't use holes
	cond.SetAddress(addrAction);
	g_cart.SetDirty(true);
}

/////////////////////////////////////////////////////////////////////////////
// customdlg glue

#include "customdlg.h"


void FillListboxWithActions(CListBox& lbox)
{
	for (int iAction = 0; iAction < NUM_UNAME; iAction++)
	{
		UNAME const& uname = g_uniNames[iAction];
		ASSERT(uname.addrDynamic != NULL);
		ASSERT(uname.szSym != NULL);
		ASSERT(uname.szDesc != NULL);

		CString str;
		if (uname.szStdAction != NULL)
			str.Format("%s\t%s", uname.szStdAction, uname.szDesc);
		else
			str.Format("\t%s", uname.szDesc);

#ifdef WHICH_AUDIO
		AppendWhichAudio(str, uname.addrDynamic);
#endif

		int iItem = lbox.AddString(str);
        ASSERT(iItem == iAction);
		lbox.SetItemData(iItem, iAction);
	}
}


///////////////////////////////////////

int PromptAddItem(int iSelectOld)	// return 0 based index, or -1
{
	CPromptActionDlg dlg;
	dlg.m_lastSel = iSelectOld;
	if (dlg.DoModal() != IDOK)
		return -1;
	return dlg.m_lastSel;
}

int PromptChangeItems(int selections[MAX_ACTIONS_PER_COND], int nOld)
{
	CPromptMultiActionDlg dlg;
	dlg.m_pSelections = selections;
	dlg.m_nSelections = nOld;
	if (dlg.DoModal() != IDOK)
        return -1;
	return dlg.m_nSelections;
}


/////////////////////////////////////////////////////////
// Saving


bool SaveChanges(bool bKeepOrig)
{
	ASSERT(g_cart.IsDirty()); // otherwise don't bother

	const int iPool = NUM_AUDIO_POOLS-1;
	// reallocate PROBACTIONS - only one memory pool (at end of last audio pool)
	addr_t addrAvailableStart = g_cart.GetYD()->pools[2].start + g_cart.GetYD()->audio_sizes[2]; // after AUDIO
	int cbAvailable = g_cart.GetYD()->pools[2].max_size - g_cart.GetYD()->audio_sizes[2];
	memset(g_cart.Lookup(addrAvailableStart), 0xFF, cbAvailable);
	int cbUsed = 0;

	// muck directly with g_cart.m_image - since we don't rely on it anymore
    for (int iCond = 0; iCond < NUM_COND; iCond++)
    {
        CONDITION const& cond = g_conditions[iCond];
        if (cond.ct == CT_PROBACTION)
        {
	        CByteArray& data = g_perfData[iCond];
			addr_t addrNew = NULL; // NULL if empty
			ASSERT(data.GetSize() % 5 == 0);
			int cbNeeded = data.GetSize();
			if (cbNeeded > 0)
			{
				if (cbUsed + cbNeeded > cbAvailable)
				{
					AfxMessageBox("Out of cartridge memory to store changes\n"
						"Please reduce the complexity and try to save again");
					return false;
				}
				addrNew = addrAvailableStart + cbUsed;
				cbUsed += cbNeeded;
				memcpy(g_cart.Lookup(addrNew), &data[0], cbNeeded);
			}
			cond.SetAddress(addrNew);
		}
    }

	TRACE("YICT CUSTOM PROBACTIONS: was %d, now %d (%d max)\n", g_cart.GetYD()->pool3_yictsize, cbUsed, cbAvailable);
	g_cart.GetYD()->pool3_yictsize = cbUsed;

	if (bKeepOrig)
	{
		if (g_cart.GetYD()->generation != 1)
		{
			AfxMessageBox("ERROR: can't turn into original");
			return false;
		}
	}
	else
	{
		// bump it
		g_cart.GetYD()->generation++;
		if (g_cart.GetYD()->generation == 1)
		{
			// first generation master copy - must save as a different file name
			AfxMessageBox("ERROR: saving first generation copy????");
		}
	}

	if (!SaveRomImage())
		return false;

	if (!bKeepOrig)
	{
		CString str;
		str.Format("Saved to files - download to cartridge and try it!\n"
				" (%s-l.bin, %s-h.bin)", g_cart.GetBaseName(), g_cart.GetBaseName());
		AfxMessageBox(str);
	}

	return true;
}


////////////////////////////////////////////////////////////
// General properties

struct DISPROPENTRY
{
	const char* szName;	// name to fill drop-list (NULL to end)
	const char* pbMatch;	// bytes to match
};


DISPROPENTRY g_dpeIdleTime[] =
{
	{ "default 5-15 sec (*)",	"\x00" }, // 5->15 seconds
	{ "5 seconds",	"\x05" },
	{ "10 seconds",	"\x0A" },
	{ "20 seconds",	"\x14" },
	{ "30 seconds",	"\x1E" },
	{ "1 minute",	"\x3C" },
	{ "2 minutes",	"\x78" },
	{ "3 minutes",	"\xF0" },
	{ "1 second (expt'l)",	"\x01" },
	{ NULL }
};

DISPROPENTRY g_dpeStretch[] =
{
	{ "all the time",		"\x01" },
	{ "every second time",	"\x02" },
	{ "every third time (*)","\x03" },
	{ "every four times",	"\x04" },
	{ "every five times",	"\x05" },
	{ "every six times",	"\x06" },
	{ "every 7 times",	"\x07" },
	{ "every 10 times",	"\x0A" },
	{ "every 100 times",	"\x64" },
	{ NULL }
};

DISPROPENTRY g_dpeWagTail[] =
{
	{ "every 30 sec (*)", "\x00" },
	{ "every 1 min", "\x01" },
	{ "every 3 min", "\x05" },
	{ "every 10 min", "\x13" },
	{ "every 30 min", "\x3b" },
	{ "every 1 hr", "\x77" },
	{ "every 2 hr", "\xEF" }, // close to off
	{ NULL }
};

#define REP6of7(string) string string string string string "\x00\x00" string

DISPROPENTRY g_dpeSitStand[] =
{
	{ "Passive (*)", "\x37\x21\x3C\x14\x4D\x06\x4D\x06\x54\x01\x00\x00\x5E\x00" },
	{ "Active", REP6of7("\x19\x32") },
	{ "Balanced", REP6of7("\x22\x22") },
	{ "Always Sit", REP6of7("\xFF\x00") }, // x65 is enough
	{ "Always Stand", REP6of7("\x00\xFF") },
	{ "Always Lie", REP6of7("\x00\x00") },
	{ NULL }
};

DISPROPENTRY g_dpePostureTiming[] =
{
	{ "constantly (*)", "\x00\x00" },
	{ "every 1 min", "\x00\x05" },	// roughly 0x500 per minute
	{ "every 2 min", "\x00\x0A" },
	{ "every 3 min", "\x00\x0F" },
	{ "every 4 min", "\x00\x14" },
	{ "every 5 min", "\x00\x19" },
#ifdef LATER // or may never -- sit/stand/lie mode
	{ "last action", "\xFF\xFF" },
#endif
	{ NULL }
};

DISPROPENTRY g_dpeMoodFactors[] =
{
    // 10 bytes: 1 forced, 1 n/a, word compare, 3* word multiply
	{ "Standard (*)",	"\xFF\xFF\x03\x00\x01\x00\x04\x00\x01\x00" },

	{ "Balanced",		"\xFF\xFF\x03\x00\x01\x00\x01\x00\x01\x00" },
	{ "Balanced2",		"\xFF\xFF\x00\x00\x01\x00\x04\x00\x01\x00" },
	{ "Mostly Hyper",	"\xFF\xFF\x00\x00\x01\x00\x00\x00\x04\x00" },
	{ "Mostly Happy",	"\xFF\xFF\x00\x00\x01\x00\x00\x00\x00\x00" },
	{ "Force Hyper",	"\x00\xFF\x03\x00\x01\x00\x04\x00\x01\x00" },
	{ "Force Happy",	"\x01\xFF\x03\x00\x01\x00\x04\x00\x01\x00" },
	{ "Force Sleepy",	"\x02\xFF\x03\x00\x01\x00\x04\x00\x01\x00" },
	{ "Force Sad",		"\x03\xFF\x03\x00\x01\x00\x04\x00\x01\x00" },
	{ "Force Sick",		"\x04\xFF\x03\x00\x01\x00\x04\x00\x01\x00" },
	{ NULL }
};

DISPROPENTRY g_dpeLightCheck[] =
{
	{ "every 1 min", "\x01" },	
	{ "every 2 min", "\x02" },	
	{ "every 3 min", "\x03" },	
	{ "every 4 min", "\x04" },	
	{ "every 5 min", "\x05" },	
	{ "every 10 min", "\x0A" },	
	{ "every 20 min", "\x14" },	
	{ "every 30 min", "\x1E" },	
	{ "always (expt'l)",	"\x00" },
	{ NULL }
};

DISPROPENTRY g_dpeDarkOrLight[] =
{
	{ "disabled (*)", "\xFF" },	
	{ "0/16", "\x00" },	
	{ "1/16", "\x10" },	
	{ "2/16", "\x20" },	
	{ "3/16", "\x30" },	
	{ "4/16", "\x40" },	
	{ "5/16", "\x50" },	
	{ "6/16", "\x60" },	
	{ "7/16", "\x70" },	
	{ "8/16", "\x80" },	
	{ "9/16", "\x90" },	
	{ "10/16", "\xA0" },	
	{ "11/16", "\xB0" },	
	{ "12/16", "\xC0" },	
	{ "13/16", "\xD0" },	
	{ "14/16", "\xE0" },	
	{ "15/16", "\xF0" },	
	{ "16/16", "\xFE" },	// FF used for disable
	{ NULL }
};


void InitGeneralProperties(CSelectList& idleTimeChoice,
	CSelectList& stretchChoice,
	CSelectList& wagTailChoice,
	CSelectList& mfChoice,
	CSelectList& sslChoice,
	CSelectList& sslTiming,
	CButton& checkDisableWUC,
    CButton& checkMute)
{
	idleTimeChoice.Init(&g_cart.GetYD()->bIdleDelay, g_dpeIdleTime, 1);
	stretchChoice.Init(&g_cart.GetYD()->bStretchCount, g_dpeStretch, 1);
	wagTailChoice.Init(&g_cart.GetYD()->bWagTime, g_dpeWagTail, 1);
	mfChoice.Init(g_cart.GetYD()->mood_data, g_dpeMoodFactors, 10);
	sslChoice.Init(g_cart.Lookup(g_cart.GetYD()->addrSitStandNewData), g_dpeSitStand, 2*7);
	sslTiming.Init((byte*)&(g_cart.GetYD()->wSslSkipCount), g_dpePostureTiming, 2);
	checkDisableWUC.SetCheck(g_cart.GetYD()->boolDisableCharger);
	checkMute.SetCheck(g_cart.GetYD()->boolMute);
}

void CSelectList::Init(byte* pData, DISPROPENTRY const* dpe, int cb)
{
	m_pData = pData;	// Lookup value
	m_dpe = dpe;
	m_cb = cb;
	m_bEditable = false;

	// fill listbox with strings
	for ( ; dpe->szName != NULL; dpe++)
	{
		int iItem = AddString(dpe->szName);
		ASSERT(dpe->pbMatch != NULL);
		SetItemData(iItem, (LPARAM)dpe->pbMatch);
	}
}

bool CSelectList::Update()
{
	// find a match
	int n = GetCount();
	for (int i = 0; i < n; i++)
	{
		const byte* pb = (const byte*)GetItemData(i);
		if (memcmp(m_pData, pb, m_cb) == 0)
		{
			SetCurSel(i);
			m_bEditable = true;
			return true;
		}
	}
	m_bEditable = false;
	return false;
}

void CSelectList::Select()
{
	if (!m_bEditable)
	{
		AfxMessageBox("Old value was unknown - unsafe to edit");
		return;
	}
	int iItem = GetCurSel();
	if (iItem < 0)
		return;
	const byte* pb = (const byte*)GetItemData(iItem);
	memcpy(m_pData, pb, m_cb);
	g_cart.SetDirty(true);
}



////////////////////////////////////////////////////////////

static int g_version = 0;

void PrepareImportData()
{
	g_version = 0;
	g_cart.SetDirty(true);	// assume we will change something
}

static bool ParseHex(byte* pb, int cb, const char* szHex)
{
	if ((int)strlen(szHex) != cb*2)
		return false;
	while (cb--)
	{
		char szT[3];
		memcpy(szT, szHex, 2);
		szT[0] = '\0';
		int val;
		if (sscanf(szHex, "%02X", &val) != 1 || val < 0 || val > 255)
			return false;
		*pb++ = (byte)val;
		szHex += 2;
	}
	return true;
}


int __cdecl int_compare(const void *elem1, const void *elem2)
{
	int a = *(int*)elem1;
	int b = *(int*)elem2;
	if (a == b)
		return 0;
	else if (a < b)
		return -1;
	else
		return 1;

}

void SortSelections(int selections[], int nSort)
{
	if (nSort == 0)
		return;
	// sort using order in array (not iStore number which is historical)
	qsort(selections, nSort, sizeof(int), int_compare);
}


static int ParseActionStorageNumbers(int selections[MAX_ACTIONS_PER_COND], const char* szList)
{
	int n = 0;
	while (1)
	{
		if (*szList == ' ' || *szList == '\t')
			szList++;
		if (*szList == '\0')
			break;
		int iStore = atoi(szList);
		if (iStore == 0 && szList[0] != '0')
			return -1;	// probably atoi error

		int iAct = FindActionByNumber(iStore);
		if (iAct < 0)
			return -1;
		ASSERT(iAct >= 0 && iAct < NUM_UNAME);
		if (n >= MAX_ACTIONS_PER_COND)
			return -1;
		selections[n++] = iAct;
		char* pch = strchr(szList, ',');
		if (pch == NULL)
			break;
		szList = pch+1;
	}
	return n;
}

static int FindProbActOrAct(COND_TYPE ctFind, int indexFind)
{
	for (int iCond = 0; iCond < NUM_COND; iCond++)
	{
		CONDITION const& cond = g_conditions[iCond];
		if (cond.ct == ctFind && cond.index == indexFind)
			return iCond;
	}
	return -1;
}

bool ImportDataLine(const char* szLine)
{
	char* pchEq = strchr(szLine, '=');
	if (pchEq == NULL)
		return false;
	*pchEq = '\0';

	char* pchParen = strchr(szLine, '(');
	int index = -1;
	if (pchParen != NULL)
	{
		*pchParen = '\0';
		index = atoi(pchParen+1);
		if (index < 0)
			return false;
	}
	CString sym = szLine;

	const char* szVal = pchEq+1;
	TRACE("IMPORT %s [%d] = %s\n", (const char*)sym, index, szVal);

	if (sym == "YICT_VERSION")
	{
		g_version = atoi(szVal);
		if (g_version != 201 && g_version != 202)
		{
			TRACE("Version not supported\n");
			return false;
		}
	}
	else if (sym == "STRETCH")
	{
		if (index != -1)
			return false;
		g_cart.GetYD()->bStretchCount = atoi(szVal);
	}
	else if (sym == "WAGTAIL")
	{
		if (index != -1)
			return false;
		g_cart.GetYD()->bWagTime = atoi(szVal);
	}
	else if (sym == "MOODDATA")
	{
		if (index != -1)
			return false;
		if (!ParseHex(g_cart.GetYD()->mood_data, 10, szVal))
			return false;
	}
	else if (sym == "SSL_NEWDATA")
	{
		if (index != -1)
			return false;
		if (!ParseHex(g_cart.Lookup(g_cart.GetYD()->addrSitStandNewData), 2*7, szVal))
			return false;
	}
	else if (sym == "SSL_SKIPCOUNT")
	{
		if (index != -1)
			return false;
		g_cart.GetYD()->wSslSkipCount = atoi(szVal);
	}
	else if (sym == "PROBACT")
	{
		int iCond = FindProbActOrAct(CT_PROBACTION, index);
		if (iCond < 0)
			return false;
		CONDITION const& cond = g_conditions[iCond];
		int selections[MAX_ACTIONS_PER_COND];
		int nNew = ParseActionStorageNumbers(selections, szVal);
		if (nNew == -1)
			return false;

		SortSelections(selections, nNew);

		// replace
		int selectionsOld[MAX_ACTIONS_PER_COND];
	    int nOld = GetCurrentActions(iCond, selectionsOld);
		if (nOld == -1)
		{
			TRACE("Unknown action used. Can't edit this safely\n");
            return false;
		}

		for (int i = nOld-1; i >= 0; i--)
			DeletePerformance(iCond, i);
		for (/*int*/ i = 0; i < nNew; i++)
			AddPerformance(iCond, selections[i]);
		RebalanceProbabilities(iCond);
	}
	else if (sym == "ACT")
	{
		int iCond = FindProbActOrAct(CT_ACTION, index);
		if (iCond < 0)
			return false;
		CONDITION const& cond = g_conditions[iCond];
		int iAct = FindActionByNumber(atoi(szVal));
		if (iAct < 0)
			return false;
		ASSERT(iAct >= 0 && iAct < NUM_UNAME);
		ChangeSinglePerformance(iCond, iAct);
	}
	else
	{
		TRACE("BAD LINE\n");
		return false;
	}
	return true;
}


static void DumpHex(FILE* pf, const char* szPre, const byte* pb, int cb)
{
	fprintf(pf, "%s=", szPre);
	while (cb--)
		fprintf(pf, "%02X", *pb++);
	fprintf(pf, "\n");
}

bool ExportData(FILE* pf)
{
	fprintf(pf, "YICT_VERSION=202\n");	// new format

	// general properties
	fprintf(pf, "; General\n");
	fprintf(pf, "STRETCH=%d\n", g_cart.GetYD()->bStretchCount);	// BYTE
	fprintf(pf, "WAGTAIL=%d\n", g_cart.GetYD()->bWagTime);	// BYTE
	DumpHex(pf, "MOODDATA", g_cart.GetYD()->mood_data, 10);
	DumpHex(pf, "SSL_NEWDATA", g_cart.Lookup(g_cart.GetYD()->addrSitStandNewData), 2*7);
	fprintf(pf, "SSL_SKIPCOUNT=%d\n", g_cart.GetYD()->wSslSkipCount);	// WORD

#ifdef LATER
// add others
#endif
	// disable WUC not saved

	// PROBACTIONs
	fprintf(pf, "; ProbAction\n");
	for (int iCond = 0; iCond < NUM_COND; iCond++)
	{
		CONDITION const& cond = g_conditions[iCond];
		if (cond.ct != CT_PROBACTION)
			continue;
		int selections[MAX_ACTIONS_PER_COND];
		int nSel = GetCurrentActions(iCond, selections);
		fprintf(pf, "PROBACT(%d)=", cond.index);	// iProbAction
		for (int i = 0; i < nSel; i++)
		{
			if (i != 0)
				fprintf(pf, ",");
			fprintf(pf, "%d", GetStorageNum(selections[i]));
		}
		fprintf(pf, "\n");
	}

	// ACTIONs
	fprintf(pf, "; Action\n");
	for (/*int*/ iCond = 0; iCond < NUM_COND; iCond++)
	{
		CONDITION const& cond = g_conditions[iCond];
		if (cond.ct != CT_ACTION)
			continue;
		fprintf(pf, "ACT(%d)=%d\n", cond.index, GetStorageNum(GetCurrentAction(iCond)));	// iAction
	}

	fprintf(pf, "; EOF\n");
	return true;
}


////////////////////////////////////////////////////////////
