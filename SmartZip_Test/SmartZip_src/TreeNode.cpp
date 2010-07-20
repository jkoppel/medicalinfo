// TreeNode.cpp: implementation of the CTreeNode class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "ZipEX.h"
#include "smartzip.h"
#include "TreeNode.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTreeNode::CTreeNode()
{
	zip_index=-1;
}

CTreeNode::~CTreeNode()
{
	int m=Childrens.GetSize ();
	for(int i=0;i<m;i++)
		delete Childrens[i];
	POSITION pos=fIndex.GetStartPosition ();
	while(pos)
	{
		CString s;CObject*pOpj;
		fIndex.GetNextAssoc (pos,s,pOpj);
		delete pOpj;
	}
}
