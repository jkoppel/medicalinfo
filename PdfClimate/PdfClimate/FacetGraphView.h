#pragma once



// CFacetGraphView form view

class CFacetGraphView : public CFormView
{
	DECLARE_DYNCREATE(CFacetGraphView)

protected:
	CFacetGraphView();           // protected constructor used by dynamic creation
	virtual ~CFacetGraphView();

public:
	enum { IDD = IDD_FACET_GRAPH };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif
protected:

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

public:
    CImage *getSrcImage() { return m_pSrcImage; }
    CBitmap *getSrcBitmap() { return m_pSrcBitmap; }
    void setSrcRect(RECT &srcRect) { m_rSrcRect = srcRect; }
protected:
    CImage *m_pSrcImage;
    RECT m_rSrcRect;
    CBitmap *m_pSrcBitmap;

    DECLARE_MESSAGE_MAP()
public:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    virtual void OnDraw(CDC* /*pDC*/);
};


