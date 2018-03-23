#pragma once

//Message types
#define MESSAGE_OK		0	
#define MESSAGE_PROMPT	1	//Pass or Fail
#define MESSAGE_YN		2	//Y or N
#define MESSAGE_PASS	3	//Pass in English and Chinese	
#define MESSAGE_FAIL	4	//Fail in English and Chinese
#define MESSAGE_ERROR	5	//Error message
#define MESSAGE_ADMIN	6	//Yup politics
#define MESSAGE_MAC		7	//Scan the mac
#define MESSAGE_SERIAL	8	//Scan the serial



#define SERIAL_LEN	16
#define MAC_LEN		17

class CMessage : public CDialog
{
	DECLARE_DYNAMIC(CMessage)

public:
	CMessage(CWnd* pParent = NULL);   // standard constructor
	void CMessage::SetMessage(int Type,const char* Text);
	void CMessage::SetMessage(int Type,const char* Text,char* Buffer);
	virtual ~CMessage();


// Dialog Data
	enum { IDD = IDD_MESSAGE_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	CString MessageText;
	CString MessagePreload;
	char EditBuff[25];
	int MessageType;
	bool MessageTicked;
	char* RetBuffer;
	DECLARE_MESSAGE_MAP()
public:
	BOOL CMessage::OnInitDialog();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedOk2();
	afx_msg void OnBnClickedOk3();
	afx_msg void OnEnChangeScan();
	afx_msg void OnBnClickedCan();
};
