////////////////////////////////////////////////////////////
// customization functions

#include "cart.h"

extern CART_DATA g_cart; // cartridge being edited

// these only work on editing CART_DATA

bool SaveChanges(bool bKeepOrig = false);


////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
// Higher level pretty printing

const char* GetCondName(int iCond, const char* & szCatRet);

bool ExtractAllPerfData();

int StartPerfIterate(int iCond); // single or multi
void ParsePerfData(int iCond, int& index, int& percentOut, CString& strStdOut, CString& strActionNameOut);

////////////////////////////////////////////////////////////
// Editing

// iAct = index into array, not StorageNumber

bool IsMultiCond(int iCond);

// for single selection ACTIONS
int GetCurrentAction(int iCond); // single
int PromptAddItem(int iSelectOld = -1); // change one or add one
void ChangeSinglePerformance(int iCond, int iAction);

// for multi selection PROBACTIONs
#define MAX_ACTIONS_PER_COND 32

int GetCurrentActions(int iCond, int selections[MAX_ACTIONS_PER_COND]);
    // return count, or -1 if error
int PromptChangeItems(int selections[MAX_ACTIONS_PER_COND], int nOld);
    // return nNew or -1 if cancel

// bool CanAddMore(int iCond);
void DeletePerformance(int iCond, int iItem);
void AddPerformance(int iCond, int iAction);
void RebalanceProbabilities(int iCond);


int GetStorageNum(int iAct);
int FindStdName(const char* szStdName);
int FindSimilarName(int iActionBase, const char* szTag);



////////////////////////////////////////////////////////////
// General properties

class CSelectList;
void InitGeneralProperties(CSelectList& idleTimeChoice, CSelectList& stretchChoice, CSelectList& wagTailChoice,
	CSelectList& mfChoice, CSelectList& sslChoice, CSelectList& sslTiming,
//	CSelectList& sslLightCheck, CSelectList& sslDarkness, CSelectList& sslLightness,
	CButton& checkDisableWUC, CButton& checkMute);


////////////////////////////////////////////////////////////
// Import and Export

void PrepareImportData();
bool ImportDataLine(const char* szLine);
bool ExportData(FILE* pf);

bool DoImport(const char* szFile, bool bReport);

////////////////////////////////////////////////////////////
