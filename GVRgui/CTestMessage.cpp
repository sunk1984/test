//GUI
//Copyright (C) 2017 VeriFone Systems Inc
//CTestMessage.cpp : implementation file
//

#include "stdafx.h"
#include "GVRgui.h"
#include "CTestMessage.h"

//Supress warnings
#pragma warning(disable : 4800)	//Int to bool
#pragma warning(disable : 4996)	//Scanf
#pragma warning(disable : 4267)
#pragma warning(disable : 4309)

IMPLEMENT_DYNAMIC(CTestMessage, CDialog)

//Constructor
CTestMessage::CTestMessage(CWnd* pParent /*=NULL*/)
	: CDialog(CTestMessage::IDD, pParent)
{

}

//Destructor
CTestMessage::~CTestMessage()
{
	m_czMessage[0]=0;
	m_bReturn=NULL;
}

BOOL CTestMessage::OnInitDialog()
{

	Beep(1000,100);

	return TRUE;
}

void CTestMessage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CTestMessage, CDialog)
	ON_BN_CLICKED(IDOK, &CTestMessage::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CTestMessage::OnBnClickedCancel)
END_MESSAGE_MAP()

//=====================================================================================
//===================================[ Set Return ]====================================
//=====================================================================================
//Send a pointer to the result
void CTestMessage::SetReturn(bool* Return)
{
	m_bReturn=Return;
}

//=====================================================================================
//===================================[ Set Messages ]==================================
//=====================================================================================
//Set the messages
void CTestMessage::SetMessage(int Type,char* Caption, char* Message)
{
	m_iPic=Type;
	sprintf(&m_czCaption[0],"%s",&Caption[0]);
	sprintf(&m_czMessage[0],"%s",&Message[0]);
}


//=====================================================================================
//===================================[ PASS Button ]===================================
//=====================================================================================
//PASS
void CTestMessage::OnBnClickedOk()
{
	*(m_bReturn)=true;
	OnOK();
}

//=====================================================================================
//===================================[ FAIL Button ]===================================
//=====================================================================================
//FAIL
void CTestMessage::OnBnClickedCancel()
{
	*(m_bReturn)=false;
	OnCancel();
}


//https://www.codeproject.com/Articles/24969/An-MFC-picture-control-to-dynamically-show-picture
//https://docs.microsoft.com/en-us/dotnet/framework/winforms/controls/how-to-set-pictures-at-run-time-windows-forms
//https://stackoverflow.com/questions/2339702/setting-resized-bitmap-file-to-an-mfc-picture-control

