// generalized CART routines

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

CART_DATA::CART_DATA()
{
    m_bDirty = false;
}

YICT_DATA* CART_DATA::GetYD()
{
	return (YICT_DATA*)&m_image[YICT_CUSTOMIZATION_ADDRESS-CART_BASE];
}

byte* CART_DATA::Lookup(addr_t addr)
{
    ASSERT(addr >= ADDR_BASE && addr < ADDR_BASE + ROM_SIZE);
    return &m_image[addr - ADDR_BASE];
}

addr_t* CART_DATA::LookupAddr(addr_t addr)
{
    ASSERT(addr >= ADDR_BASE && addr < ADDR_BASE + ROM_SIZE);
    return (addr_t*)&m_image[addr - ADDR_BASE];
}


bool CART_DATA::LoadHalf(const char* szName, const char* szExt, int iSpan)
	// if failure, report error then return false
{
	char szT[_MAX_PATH];
	sprintf(szT, "%s%s", szName, szExt);
	FILE* pf = fopen(szT, "rb");
	if (pf == NULL)
	{
		CString str;
		str.Format("ERROR: Can't open file '%s' for reading", szT);
		AfxMessageBox(str);
		return false;
	}
	byte* loadImage = new byte[ROM_SIZE/2];
	byte bEnd;
	if (fread(loadImage, ROM_SIZE/2, 1, pf) != 1 ||
		fread(&bEnd, 1, 1, pf) != 0)
	{
		CString str;
		str.Format("ERROR: Invalid image file '%s'", szT);
		AfxMessageBox(str);
		fclose(pf);
		delete [] loadImage;
		return false;
	}
	fclose(pf);
	for (int i = 0; i < ROM_SIZE/2; i++)
		m_image[i*2+iSpan] = loadImage[i];
	delete [] loadImage;
	return true;
}


bool CART_DATA::LoadRomImage(const char* szName, bool& bOrig)
{
	strcpy(m_baseName, szName);
    strlwr(m_baseName);

    char* pch;
    if ((pch = strstr(m_baseName, "-l.bin")) != NULL)
        *pch = '\0';
    else if ((pch = strstr(m_baseName, "-h.bin")) != NULL)
        *pch = '\0';

    if (!LoadHalf(m_baseName, "-l.bin", 0))
        return false;
    if (!LoadHalf(m_baseName, "-h.bin", 1))
        return false;

    if (memcmp(GetYD()->yict_id, YICT_ID, YICTTAG_SIZE) != 0 ||
		memcmp(GetYD()->end_tag, YICTEND_ID, YICTTAG_SIZE) != 0)
    {
        AfxMessageBox("Image is not compatible with this version of YICT\n    [[" YICT_ID "]]");
        return false;
    }

	m_bDirty = false;
	bOrig = (GetYD()->generation == 1);

	return true;
}


void CART_DATA::SetBaseName(const char* szName)
{
	ASSERT(strlen(szName) <= 15);
	strcpy(m_baseName, szName);
	memset(GetYD()->name, 0, 16);
	strcpy(GetYD()->name, szName);
	m_bDirty = true;
}

void CART_DATA::GetRomName(char szName[LEN_CARTTYPE], char szBaseName[LEN_CARTTYPE])
{
	strcpy(szName, GetYD()->name);
	strcpy(szBaseName, GetYD()->basename);
}

//////////////////////////////////////////////
