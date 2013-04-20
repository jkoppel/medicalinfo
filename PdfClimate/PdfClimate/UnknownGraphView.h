#pragma once



// CUnknownGraphView form view

class CUnknownGraphView : public CFormView
{
	DECLARE_DYNCREATE(CUnknownGraphView)

protected:
	CUnknownGraphView();           // protected constructor used by dynamic creation
	virtual ~CUnknownGraphView();

public:
	enum { IDD = IDD_UNKNOWN_GRAPH };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

public:
    CImage *getSrcImage() { return m_pSrcImage; }
    CBitmap *getSrcBitmap() { return m_pSrcBitmap; }
    void setSrcRect(RECT &srcRect) { m_rSrcRect = srcRect; }
protected:
    CImage *m_pSrcImage;
    RECT m_rSrcRect;
    CBitmap *m_pSrcBitmap;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
    virtual void OnDraw(CDC* /*pDC*/);
};


