// TreeNode.h: interface for the CTreeNode class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TREENODE_H__5B639A65_408D_4527_A150_ACAE95CBA574__INCLUDED_)
#define AFX_TREENODE_H__5B639A65_408D_4527_A150_ACAE95CBA574__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
	struct INDEX:public CObject 
	{
		int n;
	};
class CTreeNode : public CObject  
{
public:
	CTreeNode();
	CTreeNode(CString s):itemname(s)
	{zip_index=-1;}
	virtual ~CTreeNode();
	CString itemname;
	CObArray Childrens;
	CMapStringToOb fIndex;
	int zip_index;

};

#endif // !defined(AFX_TREENODE_H__5B639A65_408D_4527_A150_ACAE95CBA574__INCLUDED_)
