// sounddlg.cpp : implementation file
//

#include "stdafx.h"
#include "yict.h"
#include "sounddlg.h"

#include "custom.h"

#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSoundPreviewDlg dialog


CSoundPreviewDlg::CSoundPreviewDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSoundPreviewDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSoundPreviewDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CSoundPreviewDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSoundPreviewDlg)
	DDX_Control(pDX, IDC_LIST1, m_list);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSoundPreviewDlg, CDialog)
	//{{AFX_MSG_MAP(CSoundPreviewDlg)
	ON_LBN_DBLCLK(IDC_LIST1, OnPlay)
	ON_BN_CLICKED(IDC_PLAY, OnPlay)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

const char* soundNames[30] =
{
 "01 SOUND_BEEP",
 "02 SOUND_YIP",
 "03 SOUND_YUP",
 "04 SOUND_BARK",
 "05 SOUND_BARK2",
 "06 SOUND_BARK3",
 "07 SOUND_BURP",
 "08 SOUND_LICK",
 "09 SOUND_IN_PAIN",
 "10 SOUND_PANT",
 "11 SOUND_DRINK",
 "12 SOUND_GROWL",
 "13 SOUND_YOU_ASS_HOLE",
 "14 SOUND_HOWL",
 "15 SOUND_RURUFF",
 "16 SOUND_PANT2",
 "17 SOUND_PANT3",
 "18 SOUND_WATER_SFX",
 "19 SOUND_RURUFF2",
 "20 SOUND_MELODY",
 "21 SOUND_LICK_ONCE",
 "22 SOUND_SNEEZE",
 "23 SOUND_SNIFF",
 "24 SOUND_SNORE",
 "25 SOUND_PEE_SFX",
 "26 SOUND_WHINE",
 "27 SOUND_WHINE2",
 "28 SOUND_RURUFF3",
 "29 SOUND_YAWN_RUFF",
 "30 SOUND_SCREECH",
};

const byte* LookupSound(int iSnd)
{
	ASSERT(iSnd >= 30 && iSnd <= 59);	// zero based number
	addr_t addr = *g_cart.LookupAddr(g_cart.GetYD()->addrExtraAudioPointers + (iSnd-30)*4);
	if (addr == NULL)
		return NULL;
	return g_cart.Lookup(addr);
}

BOOL CSoundPreviewDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	// two different ways of looking at it
	m_list.AddString("=== Replaced System Sounds ===");

	byte* pLookup30 = g_cart.Lookup(g_cart.GetYD()->addrSysAudioLookup);
	for (int i = 0; i < 30; i++)
	{
		int iSnd = pLookup30[i];	// 0 based numbers 0->29, 30->59
		if (iSnd >= 30 && iSnd <= 59)
		{
			const byte* pSound = LookupSound(iSnd);
			if (pSound != NULL)
			{
				char szT[128];
				sprintf(szT, "%s = Custom Sound %d", soundNames[i], 1+iSnd);
				int iItem = m_list.AddString(szT);
				ASSERT(iItem >= 0);
				m_list.SetItemData(iItem, (LONG)pSound);	// store direct pointer
			}
		}
	}

	m_list.AddString("=== Custom Sounds ===");

	for (int iSnd = 30; iSnd <= 59; iSnd++)
	{
		const byte* pSound = LookupSound(iSnd);
		if (pSound != NULL)
		{
			char szT[128];
			sprintf(szT, "Custom Sound %d", 1+iSnd);
			int iItem = m_list.AddString(szT);
			ASSERT(iItem >= 0);
			m_list.SetItemData(iItem, (LONG)pSound);	// store direct pointer
		}
	}


	if (m_list.GetCount() <= 2)
	{
		AfxMessageBox("No custom sounds");
		EndDialog(IDCANCEL);
	}

	return TRUE;
}


#include "sunadpcm.c_"

const byte rgbRiffHdr[] =
{
    'R', 'I', 'F', 'F',
    0, 0, 0, 0, // cbSize
    'W', 'A', 'V', 'E',
    'f', 'm', 't', ' ',
    16, 0, 0, 0,
    1, 0, 1, 0, // 16bit mono, 8KHz
    0x40, 0x1f, 0, 0,
    0x80, 0x3e, 0, 0,
    2, 0, 16, 0,
    'd', 'a', 't', 'a',
    0, 0, 0, 0, // cbData
};
#define IBRIFF_TOTAL_SIZE       4
#define IBRIFF_DATA_SIZE        0x28

////////////////////////////////////////////

int qindex;
signed char samp8BitAudio;

void PrepareForConvert()
{
    qindex = 0;
	samp8BitAudio = 0;
}

signed short ConvertTo16(byte bNib)
{
    ASSERT(bNib >= 0 && bNib <= 15);
    int val = (bNib >> 1);
    bool bNeg = (bNib & 1) != 0;

    int slope_index = (qindex + val) & QTABLE_MASK;
	byte slope = slope_table[slope_index];
    signed char samp8Old = samp8BitAudio;
    if (bNeg)
        samp8BitAudio -= slope;
    else
        samp8BitAudio += slope;
#if 0
    printf("%d -> %d\n", samp8Old, samp8BitAudio);
#endif
    qindex = next_step[slope_index];
    return samp8BitAudio * 0x100;
}


byte* ConvertToWav(const byte* pbSound)
{
	long cbAud = *(long*)pbSound;
	pbSound += 4;
	ASSERT(cbAud < 64*1024);

    int cbOut = sizeof(rgbRiffHdr) + cbAud*4;
    byte* pbOut = new byte[cbOut];

    memcpy(pbOut, rgbRiffHdr, sizeof(rgbRiffHdr));
	long l = cbAud * 4;
	memcpy(&pbOut[IBRIFF_DATA_SIZE], &l, 4);
	l += 0x24;
	memcpy(&pbOut[IBRIFF_TOTAL_SIZE], &l, 4);

	PrepareForConvert();

    signed short* psamp = (signed short*)(pbOut + sizeof(rgbRiffHdr));
    for (int iAud = 0; iAud < cbAud; iAud++)
    {
        byte bAud = *pbSound++;  // 2 nibbles

        // low nibble first
		*psamp++ = ConvertTo16(bAud & 0xF);
		*psamp++ = ConvertTo16((bAud >> 4) & 0xF);
    }
    return pbOut; // caller must delete
}

void CSoundPreviewDlg::OnPlay() 
{
	int iItem = m_list.GetCurSel();
	if (iItem < 0)
	{
		AfxMessageBox("Please select a custom sound first\nOr just double click it");
		return;
	}
	const byte* pbSound = (const byte*)m_list.GetItemData(iItem);

	if (pbSound == NULL)
	{
		AfxMessageBox("Double click on a sound to hear it\n"
			"Sounds are categorized two ways\n"
			"#1) how the builtin sounds are usually replaced\n"
			"#2) all the custom sounds");
		return;
	}
    byte* pbWav = ConvertToWav(pbSound);
    PlaySound((LPCSTR)pbWav, NULL, SND_SYNC | SND_MEMORY | SND_PURGE);
    delete [] pbWav;
}

/////////////////////////////////////////////////////////////////////////////

