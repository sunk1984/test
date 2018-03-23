#pragma once

class CTestMessage : public CDialog
{
	DECLARE_DYNAMIC(CTestMessage)

public:
	CTestMessage(CWnd* pParent = NULL);   // standard constructor
	virtual ~CTestMessage();


// Dialog Data
	enum { IDD = IDD_TEST_DIALOG };

protected:
	bool* m_bReturn;
	int m_iPic;
	char m_czMessage[200];
	char m_czCaption[50];
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	DECLARE_MESSAGE_MAP()
public:
	BOOL CTestMessage::OnInitDialog();
	void SetReturn(bool* Return);
	void SetMessage(int Type,char* Caption, char* Message);
	void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
};
