#pragma once



// CGraphFormView form view

class CGraphFormView : public CFormView
{
	DECLARE_DYNCREATE(CGraphFormView)

protected:
    CGraphFormView(UINT nIDTemplate);
	CGraphFormView();           // protected constructor used by dynamic creation
	virtual ~CGraphFormView();

public:
	enum { IDD = 0 };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

public:
    CImage *getSrcImage() { return m_pSrcImage; }
    CBitmap *getSrcBitmap() { return m_pSrcBitmap; }
protected:
    CImage *m_pSrcImage;
    CBitmap *m_pSrcBitmap;
    CList<CWnd*> m_lstCtrlGroup1;
    CStatic m_ctrlMainImage;
    CStatic m_ctrlGraphInfo;
    CStatic m_ctrlCoordinateInfo;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
    virtual void OnDraw(CDC* /*pDC*/);
};


