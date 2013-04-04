
// PdfClimateDoc.h : interface of the CPdfClimateDoc class
//


#pragma once


class CPdfClimateDoc : public CDocument
{
protected: // create from serialization only
    CPdfClimateDoc();
    DECLARE_DYNCREATE(CPdfClimateDoc)

// Attributes
public:

// Operations
public:

// Overrides
public:
    virtual BOOL OnNewDocument();
    virtual void Serialize(CArchive& ar);

// Implementation
public:
    virtual ~CPdfClimateDoc();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
    DECLARE_MESSAGE_MAP()
};


