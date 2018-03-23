//GUI
//Copyright (C) 2017 VeriFone Systems Inc
// Message.cpp : implementation file
//

#include "stdafx.h"
#include "GVRgui.h"
#include "Message.h"

//Supress warnings
#pragma warning(disable : 4800)	//Int to bool
#pragma warning(disable : 4996)	//Scanf
#pragma warning(disable : 4267)
#pragma warning(disable : 4309)

// CMessage dialog

IMPLEMENT_DYNAMIC(CMessage, CDialog)

//Constructor
CMessage::CMessage(CWnd* pParent /*=NULL*/)
	: CDialog(CMessage::IDD, pParent)
{
	MessageType=0;
	MessageText=_T("");
}

//Descturctor
CMessage::~CMessage()
{
}

BOOL CMessage::OnInitDialog()
{
	CClientDC dc(this);
	CFont font;
	CButton *pMessage=new CButton;

	//Kill the exit button
	CMenu* pSM=GetSystemMenu(FALSE);
	if(pSM) pSM->EnableMenuItem(SC_CLOSE,MF_BYCOMMAND|MF_GRAYED|MF_DISABLED);
	//Change the font
	font.CreatePointFont(500,_T("Arial Black"),&dc); 	//Slightly larger/bold font	
	pMessage=reinterpret_cast<CButton*>(GetDlgItem(IDC_MESSAGE));	
	pMessage->SetFont(&font);
	MessageTicked=false;
	switch(MessageType)
	{
		case MESSAGE_OK:
			GetDlgItem(IDC_ALERT)->ShowWindow(1);
			GetDlgItem(IDOK)->ShowWindow(1);
			SetWindowText(_T("ATTENTION"));
			SetDlgItemText(IDC_MESSAGE,_T("ATTENTION!"));
			GetDlgItem(IDOK)->SetFocus();
			break;
		case MESSAGE_PROMPT:
			GetDlgItem(IDC_PROMPT)->ShowWindow(1);
			GetDlgItem(IDOK2)->ShowWindow(1);
			GetDlgItem(IDOK3)->ShowWindow(1);
			SetWindowText(_T("PASS/FAIL INQUIRY"));
			SetDlgItemText(IDC_MESSAGE,_T("(P/F)?"));
			GetDlgItem(IDOK2)->SetFocus();
			break;
		case MESSAGE_YN:
			GetDlgItem(IDC_PROMPT)->ShowWindow(1);
			GetDlgItem(IDOK2)->ShowWindow(1);
			SetDlgItemText(IDOK2,_T("NO"));
			GetDlgItem(IDOK3)->ShowWindow(1);
			SetDlgItemText(IDOK3,_T("YES"));
			SetWindowText(_T("YES/NO INQUIRY"));
			SetDlgItemText(IDC_MESSAGE,_T("(Y/N)?"));
			GetDlgItem(IDOK3)->SetFocus();
			break;
		case MESSAGE_PASS:
			GetDlgItem(IDC_PASS)->ShowWindow(1);
			GetDlgItem(IDOK)->ShowWindow(1);
			GetDlgItem(IDC_GPASS)->ShowWindow(1);
			SetWindowText(_T("RESULT"));
			GetDlgItem(IDOK)->SetFocus();
			break;
		case MESSAGE_FAIL:
			GetDlgItem(IDC_FAIL)->ShowWindow(1);
			GetDlgItem(IDOK)->ShowWindow(1);
			GetDlgItem(IDC_RFAIL)->ShowWindow(1);
			SetWindowText(_T("RESULT"));
			GetDlgItem(IDOK)->SetFocus();
			break;	
		case MESSAGE_ERROR:
			GetDlgItem(IDC_ERROR)->ShowWindow(1);
			GetDlgItem(IDOK)->ShowWindow(1);
			SetWindowText(_T("ERROR"));
			SetDlgItemText(IDC_MESSAGE,_T("ERROR"));
			GetDlgItem(IDOK)->SetFocus();
			break;	
		case MESSAGE_ADMIN:
			GetDlgItem(IDC_ADMIN)->ShowWindow(1);
			GetDlgItem(IDOK)->ShowWindow(1);
			SetWindowText(_T("ERROR"));
			SetDlgItemText(IDC_MESSAGE,_T("ERROR"));
			GetDlgItem(IDOK)->SetFocus();
			break;	
		case MESSAGE_SERIAL:
			GetDlgItem(IDOK)->ShowWindow(1);
			GetDlgItem(ID_CAN)->ShowWindow(1);
			GetDlgItem(IDC_SCAN)->ShowWindow(1);
			GetDlgItem(IDC_DOSCAN)->ShowWindow(1);
			static_cast<CEdit*>(GetDlgItem(IDC_SCAN))->LimitText(SERIAL_LEN);
			SetWindowText(_T("PLEASE SCAN THE SERIAL NUMBER"));
			SetDlgItemText(IDC_MESSAGE,_T("PLEASE SCAN THE SERIAL NUMBER"));
			MessageTicked=true;
			//If SN is passed in
			if(MessagePreload!="")
			{
				SetDlgItemText(IDC_SCAN,MessagePreload);
				((CEdit*)GetDlgItem(IDC_SCAN))->SetSel(0,-1);
			}
			break;
		case MESSAGE_MAC:
			GetDlgItem(IDOK)->ShowWindow(1);
			GetDlgItem(ID_CAN)->ShowWindow(1);
			GetDlgItem(IDC_SCAN)->ShowWindow(1);
			GetDlgItem(IDC_DOSCAN)->ShowWindow(1);
			static_cast<CEdit*>(GetDlgItem(IDC_SCAN))->LimitText(MAC_LEN);
			SetWindowText(_T("PLEASE SCAN THE MAC"));
			SetDlgItemText(IDC_MESSAGE,_T("PLEASE SCAN THE MAC"));
			MessageTicked=true;
			//If MAC is passed in
			if(MessagePreload!="")
			{
				SetDlgItemText(IDC_SCAN,MessagePreload);
				((CEdit*)GetDlgItem(IDC_SCAN))->SetSel(0,-1);
			}
			break;	
	}
	if(MessageText!="") SetDlgItemText(IDC_MESSAGE,MessageText);
	SetTimer(1,1000,NULL);
	//Flush
	memset(&EditBuff[0],0,25);
	font.DeleteObject();
	Beep(1000,100);

	return TRUE;
}

//=====================================================================================
//===================================[ Set Message ]===================================
//=====================================================================================
//Set parameters
void CMessage::SetMessage(int Type,const char* Text)
{
	MessageText=CString(&Text[0]);
	MessagePreload="";
	MessageType=Type;
	RetBuffer=NULL;
}

//=====================================================================================
//===================================[ Set Message ]===================================
//=====================================================================================
//Set parameters overload
void CMessage::SetMessage(int Type,const char* Text,char* Buffer)
{
	if(Type==MESSAGE_SERIAL)	
	{
		MessagePreload=CString(&Text[0]);
		MessageText=CString("PLEASE SCAN THE SERIAL NUMBER");
	}
	else { MessagePreload=""; MessageText=CString(&Text[0]); }
	MessageType=Type;
	RetBuffer=Buffer;
}


void CMessage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CMessage, CDialog)
	ON_BN_CLICKED(IDOK, &CMessage::OnBnClickedOk)
	ON_BN_CLICKED(IDOK2, &CMessage::OnBnClickedOk2)
	ON_BN_CLICKED(IDOK3, &CMessage::OnBnClickedOk3)
	ON_EN_CHANGE(IDC_SCAN, &CMessage::OnEnChangeScan)
	ON_BN_CLICKED(ID_CAN, &CMessage::OnBnClickedCan)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CMessage message handlers

//=====================================================================================
//===================================[ OK Button ]=====================================
//=====================================================================================
//OK
void CMessage::OnBnClickedOk()
{
	//Copy the text
	if((MessageType==MESSAGE_SERIAL)&&(RetBuffer!=NULL)) { OnEnChangeScan(); memcpy(RetBuffer,&EditBuff[0],21); }
	if((MessageType==MESSAGE_MAC   )&&(RetBuffer!=NULL)) { OnEnChangeScan(); memcpy(RetBuffer,&EditBuff[0],17); }
	//Close the window
	OnOK();
}

//=====================================================================================
//===================================[ PASS Button ]===================================
//=====================================================================================
//PASS
void CMessage::OnBnClickedOk3()
{
	OnCancel();
}

//=====================================================================================
//===================================[ FAIL Button ]===================================
//=====================================================================================
//FAIL
void CMessage::OnBnClickedOk2()
{
	OnOK();
}

//=====================================================================================
//===================================[ Cancel Button ]=================================
//=====================================================================================
//Abort
void CMessage::OnBnClickedCan()
{
	if(RetBuffer!=NULL)
	{
		RetBuffer[0]=(char)0xFF;
		RetBuffer[1]=(char)0x00;
		RetBuffer[2]=(char)0x00;
	}
	//Close the window
	OnOK();	
}


//=====================================================================================
//===================================[ Scan Event ]====================================
//=====================================================================================
//Scan has changed
void CMessage::OnEnChangeScan()
{
	wchar_t wbuff[50];
	
	GetDlgItem(IDC_SCAN)->GetWindowText(&wbuff[0],22);
	memset(&EditBuff[0],0,25);							//Flush
	wcstombs(&EditBuff[0],&wbuff[0],21);				//Convert from wcat_t to char
}

//=====================================================================================
//===================================[ Timer Event ]===================================
//=====================================================================================
//Do timer once every 1000 mS
void CMessage::OnTimer(UINT_PTR nIDEvent)
{
	//Set focus on scan
	if(MessageTicked) 
	{ 
		MessageTicked=false; 
		if(MessageType==MESSAGE_SERIAL) GetDlgItem(IDC_SCAN)->SetFocus(); 
		if(MessageType==MESSAGE_MAC) GetDlgItem(IDC_SCAN)->SetFocus(); 
	}

	CDialog::OnTimer(nIDEvent);
}

