// Generalized CART_DATA
    // multiple instances
    // primarily for image being edited
    // also for Import Sounds
        // maybe for binary import of Behavior

// globals - per image
#define ROM_SIZE        (256*1024)
#define ADDR_BASE       0x200000

#define LEN_CARTTYPE 16 /* 15 real characters */

#include "../yictdata202.h"


class CART_DATA
{
public:
    CART_DATA();

	bool LoadRomImage(const char* szName, bool& bOrig);
    YICT_DATA* GetYD();

	void GetRomName(char szName[LEN_CARTTYPE], char szBaseName[LEN_CARTTYPE]);

	const char* GetBaseName() { return m_baseName; }
	void SetBaseName(const char* szT);
	bool IsDirty() { return m_bDirty; }
	void SetDirty(bool bFlag) { m_bDirty = bFlag; }

protected:
	bool LoadHalf(const char* szName, const char* szExt, int iSpan);

	bool m_bDirty;
	char m_baseName[_MAX_PATH];

public:	// if you know what you are doing
	byte m_image[ROM_SIZE];

	byte* Lookup(addr_t addr);
	addr_t* LookupAddr(addr_t addr);
};

