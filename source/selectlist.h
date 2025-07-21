struct DISPROPENTRY;
typedef unsigned long addr_t;

class CSelectList : public CComboBox
{
public:
	void Init(byte* pData, DISPROPENTRY const* dpe, int cb);

	bool Update();
	void Select();
protected:
	byte* m_pData;	// direct pointer to image data
	DISPROPENTRY const* m_dpe;
	int m_cb;
	bool m_bEditable;
};

