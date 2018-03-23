// GVRguiDlg.h : header file
//

#pragma once


// CGVRguiDlg dialog
class CGVRguiDlg : public CDialog
{
// Construction
public:
	CGVRguiDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_GVRGUI_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedOk2();
	afx_msg void OnBnClickedOk3();

	void SetControls(int Status);
	void SetStatus(char* Message);
	void SetStatus(int Color,char* Message);
	void doEvents(void);
	void doWait(DWORD Delay);
	void ClearTime(void);
	void ToLog(char* Message, bool ToDisplay);
	bool OpenUUT(void);
	void CloseUUT(void);
	char* GetTimeStamp(void);
	void UpdateStatusUser(void);
	bool SendUUT(char* Message,char* Expect,int Timeout);	//Send, and wait for answer
	void SendUUT(char* Message);							//Send, and no wait
	bool WaitUUT(char* Expect,int Timeout);					//Wait for message
	bool GetINI(void);
	bool WrtINI(void);
	void ShowSetting(void);
	void WrtTape(char* Message,bool Screen,bool Print);
	void WrtLog(char* Text);
	bool RunAvatar(int MasterOrSlave);
	bool GetSerial(void);
	bool GetMac(void);
	bool DoMessage(void);
	void fixMac(void);
	void fixSerial(void);
	bool ShowMessage(int Type,const char* Text);
	void TapeLog(void);
	bool checkAdmin(void);
	afx_msg void OnBnClickedOk4();
	afx_msg void OnBnClickedProgKL82();
	afx_msg void OnEnChangeSerial();
	afx_msg void OnEnChangePrinter();
	afx_msg void OnEnChangeStation();
	afx_msg void OnBnClickedStop();
	afx_msg void OnBnClickedEeprom();
	afx_msg void OnBnClickedA001();
	afx_msg void OnBnClickedA002();
};
