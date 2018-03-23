// GVRguiDlg.cpp : implementation file
//

#include "stdafx.h"
#include "GVRgui.h"
#include "GVRguiDlg.h"
#include "Message.h"
#include "CTestMessage.h"
#include "CPort.h"
#include "CConsole.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//Suppress warnings
#pragma warning(disable : 4800)	//Int to bool
#pragma warning(disable : 4996)	//Scanf
#pragma warning(disable : 4267)
#pragma warning(disable : 4309)

#define VERSION 0x20180125

#define TEST_IDLE		0
#define TEST_RUN		1
#define TEST_STOP		2
#define TEST_ERROR		3
#define TEST_FAIL		4
#define TEST_PASS		5
#define TEST_EXIT		6
#define TEST_FAULT		7
#define TEST_SERIAL		8
#define TEST_ADMIN		9

#define CONTROLS_OFF	0
#define CONTROLS_ON		1

#define QUICK_WAIT		100
#define WAIT_1S			1000
#define WAIT_2S			2000
#define WAIT_4S			4000
#define WAIT_6S			6000
#define WAIT_10S		10000
#define WAIT_15S		15000
#define WAIT_30S		30000
#define WAIT_45S		45000
#define WAIT_60S		60000
#define WAIT_1000S		1000000

#define CONSOLE		1
#define NO_CONSOLE	0
#define SCREEN		1
#define NO_SCREEN	0
#define PRINT		1
#define NO_PRINT	0

enum test {
TEST_IMPED=1,	//Check impeadance 4.3.1
TEST_VOLTAGE,	//Voltage 4.3.2
PROG_KL82,		//KL82 Programming 4.3.3
TEST_RESET,		//Reset switch 4.3.4
TEST_BATT,		//Battery 4.3.5
TEST_UBOOT,		//Uboot 4.3.6
PROG_EEPROM,	//Program EEPROM 4.3.6.1
TEST_LEDS,		//LEDs 4.3.6.2
TEST_KL82,		//Test KL82 chip 4.3.6.3
TEST_POWER,		//Controlled power 4.3.6.4 and 4.3.6.5
TEST_SOURCE,	//Current Source 4.3.6.6
TEST_STORAGE,	//Storage Validation 4.3.6.7
TEST_SENSOR,	//Temp Sensor 4.3.6.8
TEST_NET,		//Ethernet Switch 4.3.6.11
TEST_DISMOUNT,	//LCD Dismount 4.3.6.9
TEST_BOOT,		//Linux Boot 4.3.7
TEST_LVDS,		//LVDS Video out 4.3.6.10
TEST_RTC,		//RTC 4.3.7.1
TEST_USB,		//USB 4.3.7.2
TEST_AUDIO,		//Audio 4.3.6.3
TEST_LOOP,		//Current Loop 4.3.7.4
};

#define BUFFER_SIZE	5000

bool m_bTest1=true;		//Impeadance 4.3.1
bool m_bTest2=true;		//Voltage 4.3.2
bool m_bTest3=true;		//KL82 Programming 4.3.3
bool m_bTest4=true;		//Reset switch 4.3.4
bool m_bTest5=true;		//Battery 4.3.5
bool m_bTest6=true;		//Uboot 4.3.6
bool m_bTest7=true;		//Program EEPROM 4.3.6.1
bool m_bTest8=true;		//LED 4.3.6.2
bool m_bTest9=true;		//Test KL82 4.3.6.3
bool m_bTest10=true;	//Controlled power 4.3.6.4 and 4.3.6.5
bool m_bTest11=true;	//Current Source 4.3.6.6
bool m_bTest12=true;	//Storage Validation 4.3.6.7
bool m_bTest13=true;	//Temp Sensor 4.3.6.8
bool m_bTest14=true;	//LCD Dismount 4.3.6.9
bool m_bTest15=true;	//LVDS Video out 4.3.6.10
bool m_bTest16=true;	//Ethernet Switch 4.3.6.11
bool m_bTest17=true;	//Linux Boot 4.3.7
bool m_bTest18=true;	//RTC 4.3.7.1
bool m_bTest19=true;	//USB 4.3.7.2
bool m_bTest20=true;	//Audio 4.3.6.3
bool m_bTest21=true;	//Current Loop 4.3.7.4

CPort* FixturePort=new CPort();		//Fixture port
CPort* PrinterPort=new CPort();		//Printer port
CConsole* Console=new CConsole(80,200);

bool	m_bTestRunning=false;
bool	m_bFixturePortOpen=false;	//Fixture port
bool	m_bPrinterOpen=false;		//Printer port
bool	m_bProgKL82=true;
bool	m_bProgEEPROM=true;
bool	m_bA001=true;
bool	m_bA002=false;
int		m_iStatus=TEST_IDLE;

int		m_iFixturePort=84;			//Fixture port
int		m_iPrinterPort=1;			//Printer port
char	m_czStation[5];

//Device info
char	m_czUUTserial[25]="";		//Serial for EEPROM
char	m_czUUTmac[25]="";			//MAC for EEPROM
char	m_czBoardVer[10]="";		//Board rev for EEPROM
char	m_czRSofwareRev[10]="";		//Software rev for EEPROM
char	m_czMFRdate[10]="";			//Manufacture date
char	m_czMFRdateA[20]="";		//Manufacture date in ASCII
char	m_czDispBuffer[BUFFER_SIZE];	
char	m_czTapeBuffer[BUFFER_SIZE];	
char	m_czPrintBuffer[BUFFER_SIZE];	
char	m_czVersion[50];
bool	m_bTimer;			//Timer on
int		m_iAbsSec;			//Total seconds
int		m_iAvSec;
int		m_iSec;				
int		m_iMin;
int		m_iTested;
char	m_czIsoTime[20];	//ISO timestamp
char	m_czTime[20];		//Time of day
char	m_czAsciiTime[20];	//Test time
DWORD	m_dwTime=0;			//Current tick
char	m_czTimeStamp[20];	//Timestamp  mm:ss:ms
bool	m_bStopOnFail=true;
bool	m_bCOnChange=false;		//Configuration changed
bool	m_bConfEnable=false;	//Allow changes to the configuration
int		m_iCounts[8];			//Past counts
char	m_czResultLog[100]="";
char	m_czTapeLog[100]="";



// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CGVRguiDlg dialog




CGVRguiDlg::CGVRguiDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGVRguiDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CGVRguiDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CGVRguiDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, &CGVRguiDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDOK2, &CGVRguiDlg::OnBnClickedOk2)
	ON_BN_CLICKED(IDOK3, &CGVRguiDlg::OnBnClickedOk3)
	ON_BN_CLICKED(IDOK4, &CGVRguiDlg::OnBnClickedOk4)
	ON_BN_CLICKED(IDC_KL82, &CGVRguiDlg::OnBnClickedProgKL82)
	ON_EN_CHANGE(IDC_SERIAL, &CGVRguiDlg::OnEnChangeSerial)
	ON_EN_CHANGE(IDC_PRINTER, &CGVRguiDlg::OnEnChangePrinter)
	ON_EN_CHANGE(IDC_STATION, &CGVRguiDlg::OnEnChangeStation)
	ON_BN_CLICKED(IDC_STOP, &CGVRguiDlg::OnBnClickedStop)
	ON_BN_CLICKED(IDC_EEPROM, &CGVRguiDlg::OnBnClickedEeprom)
	ON_BN_CLICKED(IDC_A001, &CGVRguiDlg::OnBnClickedA001)
	ON_BN_CLICKED(IDC_A002, &CGVRguiDlg::OnBnClickedA002)
END_MESSAGE_MAP()


// CGVRguiDlg message handlers

BOOL CGVRguiDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	CClientDC dc(this);
	CButton *pMessage=new CButton;
	CFont font;
	font.CreatePointFont(400,_T("Consolas"),&dc);
	
	pMessage=reinterpret_cast<CButton*>(GetDlgItem(IDC_RESULT));	
	pMessage->SetFont(&font);

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	//Kill the exit button
	CMenu* pSM=GetSystemMenu(FALSE);
	if(pSM) pSM->EnableMenuItem(SC_CLOSE,MF_BYCOMMAND|MF_GRAYED|MF_DISABLED);
	//Test for and create a directory for the ini
	if(GetFileAttributes(_T("./ini/"))==INVALID_FILE_ATTRIBUTES) CreateDirectory(_T("./ini"),NULL);
	//Test for and create a directory for the logd
	if(GetFileAttributes(_T("./log/"))==INVALID_FILE_ATTRIBUTES) CreateDirectory(_T("./log"),NULL);
	//Test for and create a sub directory for the test logs
	if(GetFileAttributes(_T("./log/result"))==INVALID_FILE_ATTRIBUTES) CreateDirectory(_T("./log/result"),NULL);
	//Test for and create a sub directory for the tape logs 
	if(GetFileAttributes(_T("./log/tape"))==INVALID_FILE_ATTRIBUTES) CreateDirectory(_T("./log/tape"),NULL);
	//Fix the tile
	sprintf(&m_czVersion[0],"Omnia Test - %08X",VERSION);
	this->SetWindowTextW(CString(&m_czVersion[0]));
	//Start the console
	Console->Show(_T("Omnia - Debug Console "));	
	Console->SetPosition(0,200);
	sprintf(m_czVersion,"%08X",VERSION);
	Console->Write(CONSOLE_COLOR_WHITE,"Omina GUI (C) 2018 VSI\nVersion: ");
	Console->Write(CONSOLE_COLOR_CYAN,&m_czVersion[0]);
	Console->Write(CONSOLE_COLOR_WHITE,"\n\n\n");
	//Set up dialog
	static_cast<CEdit*>(GetDlgItem(IDC_SERIAL))->LimitText(4);
	static_cast<CEdit*>(GetDlgItem(IDC_PRINTER))->LimitText(4);
	static_cast<CEdit*>(GetDlgItem(IDC_STATION))->LimitText(3);
	static_cast<CEdit*>(GetDlgItem(IDC_REV))->LimitText(1);
	static_cast<CEdit*>(GetDlgItem(IDC_REV))->SetWindowTextW(CString("D"));
	//Read the config
	Console->Write(CONSOLE_COLOR_WHITE,"Reading INI file\n");
	GetINI();
	//Misc setup
	ClearTime();
	SetTimer(1,1000,NULL);
	for(int i=0;i<8;i++) m_iCounts[i]=0;	//Clear past counts
	m_iAvSec=0;
	m_iTested=0;
	//Check for morons
	if(checkAdmin()==false)
	{
		m_iStatus=TEST_ADMIN;
		UpdateStatusUser();
	}
	else Console->Write(CONSOLE_COLOR_WHITE,"GUI Ready!\n\n");

//--DEBUG----------------------------------------------------------------------------------
//bool result=true;
//result=DoMessage();
//result=ShowMessage(MESSAGE_PROMPT,"You are about to test this part.");
//-----------------------------------------------------------------------------------------
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CGVRguiDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CGVRguiDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CGVRguiDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


//=====================================================================================
//===================================[ Clear Time ]====================================
//=====================================================================================
void CGVRguiDlg::ClearTime(void)
{
	m_bTimer=false;
	m_czTime[0]=0;
	m_iAbsSec=0;
	m_iSec=0;
	m_iMin=0;
	sprintf(&m_czAsciiTime[0],"00:00");
	sprintf(&m_czTimeStamp[0],"00:00.00 ");
	m_dwTime=GetTickCount();
}

//=====================================================================================
//===================================[ Do Events ]=====================================
//=====================================================================================
//Keep the user interface warm
void CGVRguiDlg::doEvents(void)
{
	MSG msg;

	while(PeekMessage(&msg,NULL,NULL,NULL,PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

//=====================================================================================
//===================================[ Do Wait ]=======================================
//=====================================================================================
//Sleep, but keep the GUI warm
void CGVRguiDlg::doWait(DWORD Delay)
{
	DWORD target;

	target=GetTickCount()+Delay;
	while(GetTickCount()<target) doEvents();
}

//=====================================================================================
//===================================[ Get Timestamp ]=================================
//=====================================================================================
////Return the timestamp +mm:ss.hh 
char* CGVRguiDlg::GetTimeStamp(void)
{
	char buf[10];
	DWORD mil=GetTickCount()-m_dwTime;	//Get number of milliseconds since last tick
	mil/=10;	//Make it hundreths
	sprintf(&buf[0],"%02i",mil);	
	buf[2]=0; 
	sprintf(&m_czTimeStamp[0],"+%s.%s ",&m_czAsciiTime[0],&buf[0]);

	return &m_czTimeStamp[0];
}

//=====================================================================================
//===================================[ Status ]========================================
//=====================================================================================
//Set the status screen
void CGVRguiDlg::SetStatus(char* Message)
{
	sprintf(&m_czPrintBuffer[0],"%s\n",&Message[0]);
//	Console->Write(CONSOLE_COLOR_WHITE,&m_czPrintBuffer[0]);
	SetDlgItemText(IDC_STATUS,CString(&Message[0]));
	ToLog(&m_czPrintBuffer[0],NO_CONSOLE);
	doWait(200);
}

//=====================================================================================
//===================================[ Status ]========================================
//=====================================================================================
//Set the status screen
void CGVRguiDlg::SetStatus(int Color,char* Message)
{
	sprintf(&m_czPrintBuffer[0],"%s\n",&Message[0]);
	Console->Write(Color,&m_czPrintBuffer[0]);
	Console->Write(CONSOLE_COLOR_WHITE,"\n\n");
	SetDlgItemText(IDC_STATUS,CString(&Message[0]));
	ToLog(&m_czPrintBuffer[0],NO_CONSOLE);
	doWait(200);
}

//Exit
void CGVRguiDlg::OnBnClickedOk()
{
	m_bTestRunning=false;		
	m_iStatus=TEST_EXIT;
	if(m_bCOnChange) WrtINI();	//Save
	Sleep(200);	
	FixturePort->Close();			//No serial port
	delete FixturePort;
	PrinterPort->Close();			//No printer
	delete PrinterPort;
	Console->Hide();			//No console
	Sleep(200);
	delete Console;

	OnOK();
}

void CGVRguiDlg::ToLog(char* Message, bool ToDisplay)
{
	if(ToDisplay) Console->Write(CONSOLE_COLOR_WHITE,&Message[0]);
	WrtLog(&Message[0]);
}

void CGVRguiDlg::UpdateStatusUser(void)
{
	int i;

	switch(m_iStatus)
	{
		case TEST_PASS:
			SetStatus(CONSOLE_COLOR_GREEN,"** TEST PASSED **\n");
			ShowMessage(MESSAGE_PASS,"");
			if(m_iTested<8) m_iTested++;	//One more
			//Save
			m_iCounts[7]=m_iCounts[6]; m_iCounts[6]=m_iCounts[5];
			m_iCounts[5]=m_iCounts[4]; m_iCounts[4]=m_iCounts[3];
			m_iCounts[3]=m_iCounts[2]; m_iCounts[2]=m_iCounts[1];
			m_iCounts[1]=m_iCounts[0]; m_iCounts[0]=m_iAbsSec;
			//Compute average
			m_iAvSec=0;
			for(i=0;i<8;i++) m_iAvSec+=m_iCounts[i];
			m_iAvSec/=m_iTested;
			//Show
			sprintf(&m_czAsciiTime[0],"%02d:%02d",m_iAvSec/60,m_iAvSec%60);
			SetDlgItemText(IDC_AVERAGE,CString(&m_czAsciiTime[0]));
			break;
		case TEST_FAIL:
			SetStatus(CONSOLE_COLOR_RED,"** TEST FAILED **\n");
			ShowMessage(MESSAGE_FAIL,"");
			break;
		case TEST_FAULT:
			SetStatus(CONSOLE_COLOR_MAGENTA,"** TEST FAULT **\n");
			ShowMessage(MESSAGE_ERROR,"TEST FAULT!");
			break;
		case TEST_STOP:
			SetStatus(CONSOLE_COLOR_RED,"** OPERATOR INTERRUPT **\n");
			ShowMessage(MESSAGE_ERROR,"OPERATOR INTERRUPT!");
			break;
		case TEST_ERROR:
			SetStatus(CONSOLE_COLOR_MAGENTA,"** TEST ERROR **\n");
			ShowMessage(MESSAGE_ERROR,"TEST FAULT!");
			break;
		case TEST_SERIAL:
			SetStatus(CONSOLE_COLOR_RED,"** SERIAL COMMUNICATIONS FAULT **\n");
			ShowMessage(MESSAGE_ERROR,"SERIAL COMMUNICATIONS FAULT!");
			break;
		case TEST_ADMIN:
			SetControls(CONTROLS_OFF);
			GetDlgItem(IDOK )->EnableWindow(true);			//Enable Exit
			SetStatus(CONSOLE_COLOR_WHITE,"Plugh!\n\n");
			Beep(500,500);
			SetStatus(CONSOLE_COLOR_RED,"*** ADMINISTRATIVE ACCESS REQUIRED ***\n");
			Beep(500,500);
			SetStatus(CONSOLE_COLOR_WHITE,"*** ADMINISTRATIVE ACCESS REQUIRED ***");
			Beep(500,500);
			SetStatus(CONSOLE_COLOR_BLUE,"*** ADMINISTRATIVE ACCESS REQUIRED ***");
			Beep(500,500);
			SetStatus(CONSOLE_COLOR_GREEN,"*** ADMINISTRATIVE ACCESS REQUIRED ***");
			Beep(500,500);
			SetStatus(CONSOLE_COLOR_YELLOW,"*** ADMINISTRATIVE ACCESS REQUIRED ***");
			Beep(500,500);
			SetStatus(CONSOLE_COLOR_WHITE,"Phred will not permit any software he his responcible for to run without admin rights!\nDo not call Phred, he does not care.\nTake this up with management!");
			Beep(500,500);
			ShowMessage(MESSAGE_ADMIN,"ADMINISTRATIVE ACCESS REQUIRED!\nPLEASE CONTACT YOUR HELP DESK.");
			break;
	}
}

//Start Test
void CGVRguiDlg::OnBnClickedOk2() 
{
	int stateCount=0;
	bool runningResult=true;
	bool localResult=false;
	char localBuffer[200];
	CString version;
	
	//Check that we are running the correct device
	if(m_bA001) { localResult=ShowMessage(MESSAGE_PROMPT,"Are You Testing\n\nM15758A001?"); }
	if(m_bA002) { localResult=ShowMessage(MESSAGE_PROMPT,"Are You Testing\n\nM15758A002?"); }
	if(localResult==false) { runningResult=localResult; return; }
	else localResult=false;
	//Get board rev
	static_cast<CEdit*>(GetDlgItem(IDC_REV))->GetWindowTextW(version);
	sprintf(&m_czBoardVer[0],"%s",version.GetBuffer());
	//Save settings
	if(m_bCOnChange) WrtINI();
	SetControls(CONTROLS_OFF);
	Console->Write(CONSOLE_COLOR_GREEN,"** Test Start **\n");
	//Send version
	sprintf_s(&m_czPrintBuffer[0],50,"Test executive: %s\n",&m_czVersion[0]);
	ToLog(&m_czPrintBuffer[0],NO_CONSOLE);
	//Get serial
	m_iStatus=TEST_RUN;
	runningResult=GetSerial();					//Serial
	if(runningResult) 
	{
		Console->Write(CONSOLE_COLOR_WHITE,"  Serial Number: ");
		Console->Write(CONSOLE_COLOR_CYAN,&m_czUUTserial[0]);
		Console->Write(CONSOLE_COLOR_WHITE,"\n");
	}
	if(runningResult) 
	{
		Console->Write(CONSOLE_COLOR_WHITE,"       Revision: ");
		Console->Write(CONSOLE_COLOR_CYAN,&m_czBoardVer[0]);
		Console->Write(CONSOLE_COLOR_WHITE,"\n");
	}

	if(runningResult) { runningResult=GetMac();	}				//MAC address
	if(runningResult)
	{
		Console->Write(CONSOLE_COLOR_WHITE,"    MAC Address: ");
		Console->Write(CONSOLE_COLOR_CYAN,&m_czUUTmac[0]);
		Console->Write(CONSOLE_COLOR_WHITE,"\n");
	}
	if(runningResult)
	{
		Console->Write(CONSOLE_COLOR_WHITE,"Manufacture Lot: ");
		Console->Write(CONSOLE_COLOR_CYAN,&m_czMFRdateA[0]);
		Console->Write(CONSOLE_COLOR_WHITE,"\n");
	}

	if(runningResult)	//If Serial
	{
		//Create logs
		sprintf(&m_czResultLog[0],"./log/result/Ominia_%s_%s_%s.log",&m_czUUTserial[0],&m_czIsoTime[0],&m_czStation[0]); 
		sprintf(&m_czTapeLog[0],"./log/tape/TapeLog_%s_%s_%s.log",&m_czUUTserial[0],&m_czIsoTime[0],&m_czStation[0]);
		//Begin
		m_bTestRunning=true;
		ClearTime();
		m_bTimer=true;
		ShowMessage(MESSAGE_OK,"Mount the UUT in the fixture");
		//Flush
		m_czDispBuffer[0]=0;
		m_czTapeBuffer[0]=0;
		SetDlgItemText(IDC_RESULT,_T(""));
		WrtTape("----------------------------------------\n",NO_SCREEN,PRINT);
		if(m_bA001) WrtTape("       Model: M15758A001\n",SCREEN,PRINT);
		if(m_bA002) WrtTape("       Model: M15758A002\n",SCREEN,PRINT);
		sprintf(&m_czPrintBuffer[0],"    Revision: %s\n",&m_czBoardVer[0]);
		WrtTape(&m_czPrintBuffer[0],SCREEN,PRINT);		
		sprintf(&m_czPrintBuffer[0],"      Serial: %s\n",&m_czUUTserial[0]);
		WrtTape(&m_czPrintBuffer[0],SCREEN,PRINT);
				sprintf(&m_czPrintBuffer[0],"         MAC: %s\n",&m_czUUTmac[0]);
		WrtTape(&m_czPrintBuffer[0],SCREEN,PRINT);		
		sprintf(&m_czPrintBuffer[0],"   Timestamp: %s\n",&m_czIsoTime[0]);
		WrtTape(&m_czPrintBuffer[0],SCREEN,PRINT);
		sprintf(&m_czPrintBuffer[0],"  Station ID: %s\n",&m_czStation[0]);
		WrtTape(&m_czPrintBuffer[0],NO_SCREEN,PRINT);
		WrtTape("----------------------------------------\n\n",SCREEN,PRINT);
		
		//Let the user know
		Console->Write(CONSOLE_COLOR_WHITE,"\n");
		//Dispatcher
		while(m_bTestRunning)
		{
			switch(++stateCount)
			{
				//----------------------------------------------------------------------------------------
				case TEST_IMPED:	//Check impeadance 4.3.1, and power on
					SetStatus("Power On...");
					ToLog("Send: I\r\n",false);
					localResult=SendUUT("I\r\n","<PASS>",WAIT_15S);
					Console->Write(&m_czPrintBuffer[0]);
					ToLog(&m_czPrintBuffer[0],false);
					runningResult=localResult;
					//Power on
					if(localResult)
					{
						SetStatus("Power On...");
						doWait(200);
						ToLog("Send: N\r\n",false);
						localResult=SendUUT("N\r\n","<PASS>",WAIT_4S); //20180118 - Wass "<DONE>"
						Console->Write(&m_czPrintBuffer[0]);
						ToLog(&m_czPrintBuffer[0],false);
						runningResult=localResult;
					}
					//Adjust the power off
					if(localResult)
					{
						doWait(200);
						ToLog("Send: Y\r\n",false);
						localResult=SendUUT("Y\r\n","<DONE>",WAIT_4S);
						Console->Write(&m_czPrintBuffer[0]);
						ToLog(&m_czPrintBuffer[0],false);
						runningResult=localResult;
					}
					if(localResult) WrtTape("Impeadance Test/Power On------------PASS\n",SCREEN,NO_PRINT);
					else 			WrtTape("Impeadance Test/Power On------------FAIL\n",SCREEN,PRINT);
					if(m_bTestRunning==false) continue;
					break;
				//----------------------------------------------------------------------------------------
				case TEST_VOLTAGE:	//Check the voltages 4.3.2
					if(m_bTest2)
					{
						SetStatus("Checking voltages...");
						ToLog("Send: V\r\n",false);
						localResult=SendUUT("V\r\n","<PASS>",WAIT_15S);
						Console->Write(&m_czPrintBuffer[0]);
						ToLog(&m_czPrintBuffer[0],false);
						runningResult=localResult;
						if(localResult) WrtTape("Voltages----------------------------PASS\n",SCREEN,NO_PRINT);
						else 			WrtTape("Voltages----------------------------FAIL\n",SCREEN,PRINT);
						if(m_bTestRunning==false) continue;
					}
					break;
				//----------------------------------------------------------------------------------------
				case PROG_KL82:	//KL82 configure 4.3.3
					if((m_bProgKL82)&&(m_bTest3))
					{
						SetStatus("KL82 Programming...");
						sprintf(&m_czPrintBuffer[0],"KL82 Programming\r\n");
						Console->Write(&m_czPrintBuffer[0]);
						ToLog(&m_czPrintBuffer[0],false);
						localResult=DoMessage();
						runningResult=localResult;
						if(localResult) WrtTape("KL82 Programming--------------------PASS\n",SCREEN,NO_PRINT);
						else 			WrtTape("KL82 Programming--------------------FAIL\n",SCREEN,PRINT);
						if(m_bTestRunning==false) continue;
					}
					break;
				//----------------------------------------------------------------------------------------
				case TEST_RESET:	//Reset switch 4.3.4
					if(m_bTest4)
					{
						SetStatus("Checking reset...");
						ShowMessage(MESSAGE_OK,"Please Click, OK, then Press the Reset Button on the card under test.");
						ToLog("Send: W\r\n",false);
						localResult=SendUUT("W\r\n","<PASS>",WAIT_15S);
						sprintf(&m_czPrintBuffer[0],"Reset test\r\n");
						Console->Write(&m_czPrintBuffer[0]);
						ToLog(&m_czPrintBuffer[0],false);
						runningResult=localResult;
						if(localResult) WrtTape("Reset test--------------------------PASS\n",SCREEN,NO_PRINT);
						else 			WrtTape("Reset test--------------------------FAIL\n",SCREEN,PRINT);
						if(m_bTestRunning==false) continue;
					}
					break;
					//----------------------------------------------------------------------------------------
				case TEST_BATT:	//Battery Test 4.3.5
					if(m_bTest5)
					{
						SetStatus("Battery test...");
						ToLog("Send: B\r\n",false);
						localResult=SendUUT("B\r\n","<PASS>",WAIT_2S);
						Console->Write(&m_czPrintBuffer[0]);
						ToLog(&m_czPrintBuffer[0],false);
						runningResult=localResult;
						if(localResult) WrtTape("Battery Test------------------------PASS\n",SCREEN,NO_PRINT);
						else 			WrtTape("Battery Test------------------------FAIL\n",SCREEN,PRINT);
						if(m_bTestRunning==false) continue;
					}
					break;
				//----------------------------------------------------------------------------------------
				case TEST_UBOOT:	//U-boot Test 4.3.6
					if(m_bTest6)
					{
						SetStatus("U-Boot test...");
						ToLog("Send: H\r\n",false);
						localResult=SendUUT("H\r\n","<PASS>",WAIT_4S);
						Console->Write(&m_czPrintBuffer[0]);
						ToLog(&m_czPrintBuffer[0],false);
						runningResult=localResult;
						if(localResult) WrtTape("U-Boot Test-------------------------PASS\n",SCREEN,NO_PRINT);
						else 			WrtTape("U-Boot Test-------------------------FAIL\n",SCREEN,PRINT);
						if(m_bTestRunning==false) continue;
					}
					break;
				//----------------------------------------------------------------------------------------
				case PROG_EEPROM:	//EEPROM program 4.3.6.1
					if((m_bProgEEPROM)&&(m_bTest7))
					{
						SetStatus("EEPROM programming...");
						//Show info
						if(m_bA001) sprintf(&m_czPrintBuffer[0],"          Device: M15758A001\r\n");
						if(m_bA001) sprintf(&m_czPrintBuffer[0],"          Device: M15758A002\r\n");
						ToLog(&m_czPrintBuffer[0],false);
						sprintf(&m_czPrintBuffer[0],"   Board Version: %s\r\n",&m_czBoardVer[0]);
						ToLog(&m_czPrintBuffer[0],false);
						sprintf(&m_czPrintBuffer[0],"Software Version: %s\r\n",&m_czRSofwareRev[0]);
						ToLog(&m_czPrintBuffer[0],false);
						sprintf(&m_czPrintBuffer[0],"   Serial Number: %s\r\n",&m_czUUTserial[0]);
						ToLog(&m_czPrintBuffer[0],false);
						sprintf(&m_czPrintBuffer[0],"     MAC Address: %s\r\n",&m_czUUTmac[0]);
						ToLog(&m_czPrintBuffer[0],false);
						//20170118 - Added lot code 
						//sprintf(&m_czPrintBuffer[0]," Manufacture Lot: 0x%02X 0x%02X 0x%02X\r\n",m_czMFRdateA[0],m_czMFRdate[1],m_czMFRdate[2]);
						sprintf(&m_czPrintBuffer[0], " Manufacture Lot: %sr\n", &m_czMFRdateA[0]);
						ToLog(&m_czPrintBuffer[0],false);
						//Assemble command - 20180122
						// a=M15758A001 or M15758A002
						//eep i 3 -> (hardcoded-allows application code to detect the EEPROM version-this is the first byte in the EEPROM) 
						//n -> INXXXXXXXX (board serial number-unique to each board) 
						//b ->  D or revision letter corresponding to the gerber package/drawing (board assembly revision)
						//a -> Gilbarco part number (M15758A001 for development or M15758A002 for production) 
						//d 180116 -> 2018/01/16 -> 0x18 0x01 0x16 saved in EEPROM
						//c -> VeriFone (hardcoded)
						//v ->              (leave blank since IAC is not programming application code into Omnia. Currently, this will be done at GSO since boards will be programmed here) 
						//p -> 1  (program the # of Ethernet ports that have a MAC-there is only one which is the i.MX6)
						//X -> XX:XX:XX:XX:XX:XX (unique to each board) 
						//if(m_bA001) sprintf(&localBuffer[0],"P a=M15758A001 c=Verifone i=3 b=%s v=%s p=1 0=%s n=%s d=%s \r\n",&m_czBoardVer[0],&m_czRSofwareRev[0],&m_czUUTmac[0],&m_czUUTserial[0],&m_czMFRdate[0]);
						//if(m_bA002) sprintf(&localBuffer[0],"P a=M15758A002 c=Verifone i=3 b=%s v=%s p=1 0=%s n=%s d=%s \r\n",&m_czBoardVer[0],&m_czRSofwareRev[0],&m_czUUTmac[0],&m_czUUTserial[0],&m_czMFRdate[0]);
						if(m_bA001) sprintf(&localBuffer[0],"P a=M15758A001 c=Verifone i=3 b=%s v=%s p=1 0=%s n=%s d=%s \r\n",&m_czBoardVer[0],&m_czRSofwareRev[0],&m_czUUTmac[0],&m_czUUTserial[0],&m_czMFRdateA[0]);
						if(m_bA002) sprintf(&localBuffer[0],"P a=M15758A002 c=Verifone i=3 b=%s v=%s p=1 0=%s n=%s d=%s \r\n",&m_czBoardVer[0],&m_czRSofwareRev[0],&m_czUUTmac[0],&m_czUUTserial[0],&m_czMFRdateA[0]);
						sprintf(&m_czPrintBuffer[0],"Send: %s",&localBuffer[0]);
						ToLog(&m_czPrintBuffer[0],false);
						localResult=SendUUT(&localBuffer[0],"<DONE>", WAIT_15S);
						Console->Write(&m_czPrintBuffer[0]);
						ToLog(&m_czPrintBuffer[0],false);
						runningResult=localResult;
						if(localResult) WrtTape("EEPROM Programming------------------PASS\n",SCREEN,NO_PRINT);
						else 			WrtTape("EEPROM Programming------------------FAIL\n",SCREEN,PRINT);
						if(m_bTestRunning==false) continue;
					}
					break;
				//----------------------------------------------------------------------------------------
				case TEST_LEDS:	//LED test 4.3.6.2
					if(m_bTest8)
					{
						SetStatus("LED test...");
						ToLog("Send: L\r\n",false);
						localResult=SendUUT("L\r\n","<PASS>",WAIT_30S);
						Console->Write(&m_czPrintBuffer[0]);
						ToLog(&m_czPrintBuffer[0],false);
						runningResult=localResult;
						if(localResult) WrtTape("LED Test----------------------------PASS\n",SCREEN,NO_PRINT);
						else 			WrtTape("LED Test----------------------------FAIL\n",SCREEN,PRINT);
						if(m_bTestRunning==false) continue;
					}
					break;
				//----------------------------------------------------------------------------------------
				case TEST_KL82:	//LK82 test 4.3.6.3
					if(m_bTest9)
					{
						SetStatus("KL82 test...");
						ToLog("Send: K\r\n",false);
						localResult=SendUUT("K\r\n","<PASS>",WAIT_15S);
						Console->Write(&m_czPrintBuffer[0]);
						ToLog(&m_czPrintBuffer[0],false);
						runningResult=localResult;
						if(localResult) WrtTape("KL82 Test---------------------------PASS\n",SCREEN,NO_PRINT);
						else 			WrtTape("KL82 Test---------------------------FAIL\n",SCREEN,PRINT);
						if(m_bTestRunning==false) continue;
					}
					break;
				//----------------------------------------------------------------------------------------
				case TEST_POWER:	//Controlled power 4.3.6.4 and 4.3.6.5
					if(m_bTest10)
					{
						SetStatus("Controlled power test...");
						ToLog("Send: O\r\n",false);
						localResult=SendUUT("O\r\n","<PASS>",WAIT_4S);
						Console->Write(&m_czPrintBuffer[0]);
						ToLog(&m_czPrintBuffer[0],false);
						runningResult=localResult;
						if(localResult) WrtTape("Controlled Power Test---------------PASS\n",SCREEN,NO_PRINT);
						else 			WrtTape("Controlled Power Test---------------FAIL\n",SCREEN,PRINT);
						if(m_bTestRunning==false) continue;
					}
					break;
				//----------------------------------------------------------------------------------------
				case TEST_SOURCE:	//Current source test 4.3.6.6
					if(m_bTest11)
					{
						SetStatus("Current source test...");
						ToLog("Send: T\r\n",false);
						localResult=SendUUT("T\r\n","<PASS>",WAIT_4S);
						Console->Write(&m_czPrintBuffer[0]);
						ToLog(&m_czPrintBuffer[0],false);
						runningResult=localResult;
						if(localResult) WrtTape("Current Source Test-----------------PASS\n",SCREEN,NO_PRINT);
						else 			WrtTape("Current Source Test-----------------FAIL\n",SCREEN,PRINT);
						if(m_bTestRunning==false) continue;
					}
					break;
				//----------------------------------------------------------------------------------------
				case TEST_STORAGE:	//Storage test 4.3.6.7
					if(m_bTest12)
					{
						SetStatus("Storage test...");
						ToLog("Send: S\r\n",false);
						localResult=SendUUT("S\r\n","<PASS>",WAIT_4S);
						Console->Write(&m_czPrintBuffer[0]);
						ToLog(&m_czPrintBuffer[0],false);
						runningResult=localResult;
						if(localResult) WrtTape("Storage Test------------------------PASS\n",SCREEN,NO_PRINT);
						else 			WrtTape("Storage Test------------------------FAIL\n",SCREEN,PRINT);
						if(m_bTestRunning==false) continue;
					}
					break;
				//----------------------------------------------------------------------------------------
				case TEST_SENSOR:	//Temp Sensor test 4.3.6.8
					if(m_bTest13)
					{
						SetStatus("Temp Sensor test...");
						ToLog("Send: M\r\n",false);
						localResult=SendUUT("M\r\n","<PASS>",WAIT_4S);
						Console->Write(&m_czPrintBuffer[0]);
						ToLog(&m_czPrintBuffer[0],false);
						runningResult=localResult;
						if(localResult) WrtTape("Temp Sensor Test--------------------PASS\n",SCREEN,NO_PRINT);
						else 			WrtTape("Temp Sensor Test--------------------FAIL\n",SCREEN,PRINT);
						if(m_bTestRunning==false) continue;
					}
					break;
				//----------------------------------------------------------------------------------------
				case TEST_DISMOUNT:	//LCD Dismount test 4.3.6.9
					if(m_bTest14)
					{
						SetStatus("LCD Dismount test...");
						ToLog("Send: D\r\n",false);
						localResult=SendUUT("D\r\n","<PASS>",WAIT_15S);
						Console->Write(&m_czPrintBuffer[0]);
						ToLog(&m_czPrintBuffer[0],false);
						runningResult=localResult;
						if(localResult) WrtTape("LCD Dismount Test-------------------PASS\n",SCREEN,NO_PRINT);
						else 			WrtTape("LCD Dismount Test-------------------FAIL\n",SCREEN,PRINT);
						if(m_bTestRunning==false) continue;
					}
					break;
				//----------------------------------------------------------------------------------------
				case TEST_LVDS:	//LVDS Video test 4.3.6.10
					if(m_bTest15)
					{
						SetStatus("LVDS Video test...");
						ToLog("Send: Q\r\n",false);
						localResult=SendUUT("Q\r\n","<PASS>",WAIT_15S);
						Console->Write(&m_czPrintBuffer[0]);
						ToLog(&m_czPrintBuffer[0],false);
						if(localResult)
						{
							localResult=localResult=ShowMessage(MESSAGE_PROMPT,"Did the video test pass?");
						}
						runningResult=localResult;
						if(localResult) WrtTape("LVDS Video Test---------------------PASS\n",SCREEN,NO_PRINT);
						else 			WrtTape("LVDS Video Test---------------------FAIL\n",SCREEN,PRINT);
						if(m_bTestRunning==false) continue;
					}
					break;
				//----------------------------------------------------------------------------------------
				case TEST_NET:	//Ethernet Switch test 4.3.6.11
					if(m_bTest16)
					{
						SetStatus("Ethernet Switch test...");
						ToLog("Send: E\r\n",false);
						localResult=SendUUT("E\r\n","<PASS>",WAIT_15S);
						Console->Write(&m_czPrintBuffer[0]);
						ToLog(&m_czPrintBuffer[0],false);
						runningResult=localResult;
						if(localResult) WrtTape("Ethernet Test-----------------------PASS\n",SCREEN,NO_PRINT);
						else 			WrtTape("Ethernet Test-----------------------FAIL\n",SCREEN,PRINT);
						if(m_bTestRunning==false) continue;
					}
					break;
				//----------------------------------------------------------------------------------------
				case TEST_BOOT:	//Linux Boot 4.3.7
					if(m_bTest17)
					{
						SetStatus("Linux Boot test...");
						ToLog("Send: X\r\n",false);
						localResult=SendUUT("X\r\n","<PASS>",WAIT_30S);
						Console->Write(&m_czPrintBuffer[0]);
						ToLog(&m_czPrintBuffer[0],false);
						runningResult=localResult;
						if(localResult) WrtTape("Linux Boot Test---------------------PASS\n",SCREEN,NO_PRINT);
						else 			WrtTape("Linux Boot Test---------------------FAIL\n",SCREEN,PRINT);
						if(m_bTestRunning==false) continue;
					}
					break;
				//----------------------------------------------------------------------------------------
				case TEST_RTC:	//RTC Test 4.3.7.1
					if(m_bTest18)
					{
						SetStatus("RTC test...");
						ToLog("Send: R\r\n",false);
						localResult=SendUUT("R\r\n","<PASS>",WAIT_15S);
						Console->Write(&m_czPrintBuffer[0]);
						ToLog(&m_czPrintBuffer[0],false);
						runningResult=localResult;
						if(localResult) WrtTape("RTC Test----------------------------PASS\n",SCREEN,NO_PRINT);
						else 			WrtTape("RTC Test----------------------------FAIL\n",SCREEN,PRINT);
						if(m_bTestRunning==false) continue;
					}
					break;
				//----------------------------------------------------------------------------------------
				case TEST_USB:	//USB Test 4.3.7.2
					if(m_bTest19)
					{
						SetStatus("USB test...");
						ToLog("Send: U\r\n",false);
						localResult=SendUUT("U\r\n","<PASS>",WAIT_4S);
						Console->Write(&m_czPrintBuffer[0]);
						ToLog(&m_czPrintBuffer[0],false);
						runningResult=localResult;
						if(localResult) WrtTape("USB Test----------------------------PASS\n",SCREEN,NO_PRINT);
						else 			WrtTape("USB Test----------------------------FAIL\n",SCREEN,PRINT);
						if(m_bTestRunning==false) continue;
					}
					break;
				//----------------------------------------------------------------------------------------
				case TEST_AUDIO:	//Audio Test 4.3.7.3
					if(m_bTest20)
					{
						SetStatus("Audio test...");
						ToLog("Send: A\r\n",false);
						localResult=SendUUT("A\r\n","<PASS>",WAIT_10S);
						Console->Write(&m_czPrintBuffer[0]);
						ToLog(&m_czPrintBuffer[0],false);
						runningResult=localResult;
						if(localResult) WrtTape("Audio Test--------------------------PASS\n",SCREEN,NO_PRINT);
						else 			WrtTape("Audio Test--------------------------FAIL\n",SCREEN,PRINT);
						if(m_bTestRunning==false) continue;
					}
					break;
				//----------------------------------------------------------------------------------------
				case TEST_LOOP:	//Current Loop 4.3.7.4
					if(m_bTest21)
					{
						SetStatus("Current Loop test...");
						ToLog("Send: C\r\n",false);
						localResult=SendUUT("C\r\n","<PASS>",WAIT_30S);
						Console->Write(&m_czPrintBuffer[0]);
						ToLog(&m_czPrintBuffer[0],false);
						runningResult=localResult;
						if(localResult) WrtTape("Current Loop Test-------------------PASS\n",SCREEN,NO_PRINT);
						else 			WrtTape("Current Loop Test-------------------FAIL\n",SCREEN,PRINT);
						if(m_bTestRunning==false) continue;
					}
					break;
				//----------------------------------------------------------------------------------------
				default:	//Done, now exit
					m_bTestRunning=false;
					continue;
			}
			//----------------------------------------------------------------------------------------
			if(m_bStopOnFail)	//See if we need to stop
			{
				if(runningResult==false) break;
			}
	} //End while

		//Elapsed time
		m_bTimer=false;
		sprintf(&m_czPrintBuffer[0],"           Test time:  %s\n",&m_czAsciiTime[0]);
		WrtTape(&m_czPrintBuffer[0],NO_SCREEN,PRINT);
		m_bTestRunning=false;
		//Show pass or fail
		if(runningResult==false)
		{
			if(m_iStatus!=TEST_SERIAL) m_iStatus=TEST_FAIL;
			WrtTape("\n          *** TEST FAILED ***\n",SCREEN,PRINT);
		}
		else
		{
			if(m_iStatus!=TEST_SERIAL) m_iStatus=TEST_PASS;
			WrtTape("\n          *** TEST PASSED ***\n",SCREEN,PRINT);
		}
		//Done
		ToLog("Powering OFF...\n",true);
		ToLog("Send: F\r\n",false);	
		SendUUT("F\r\n","<DONE>",WAIT_2S);	//Power off
		ToLog(&m_czPrintBuffer[0],false);
		CloseUUT();							//Close serial port
		SetControls(CONTROLS_ON);
		WrtTape("\n\n\n\n\n\n",NO_SCREEN,PRINT);
		TapeLog();	//log the tape
	}
	else
	{
		m_iStatus=TEST_STOP;	//User skipped serial
		m_bTimer=false;
		ClearTime();
		SetDlgItemText(IDC_TIME,CString(&m_czAsciiTime[0]));
	}
	//Done
	UpdateStatusUser();
	//Update log
	if(runningResult==false)
	{
		//Rename the log
		sprintf(&m_czPrintBuffer[0],"%s.fail",&m_czResultLog[0]);
		rename(&m_czResultLog[0],&m_czPrintBuffer[0]);
	}
	else
	{
		//Rename the log
		sprintf(&m_czPrintBuffer[0],"%s.pass",&m_czResultLog[0]);
		rename(&m_czResultLog[0],&m_czPrintBuffer[0]);	
	}
}

//Stop Test
void CGVRguiDlg::OnBnClickedOk3()
{
	m_bTestRunning=false;
	m_iStatus=TEST_STOP;
}

//Reset fixture (Power off)
void CGVRguiDlg::OnBnClickedOk4()
{
	SendUUT("F\r\n","<DONE>",WAIT_6S);	//Power off
	CloseUUT();						//Close serial port
}

void CGVRguiDlg::CloseUUT(void)
{
	if(m_bFixturePortOpen)
	{
		m_bFixturePortOpen=false;
		FixturePort->Close();
	}
}

bool CGVRguiDlg::OpenUUT(void)
{
	if(m_bFixturePortOpen==false)
	{
		if(FixturePort->Open(m_iFixturePort,9600)) m_bFixturePortOpen=true;
		else 
		{
			m_iStatus=TEST_SERIAL;
			return false;
		}
	}
	return true;
}

//=====================================================================================
//===================================[ Send UUT ]======================================
//=====================================================================================
//Send, and wait for answer
bool CGVRguiDlg::SendUUT(char* Message,char* Expect,int Timeout)
{
	bool result=false;
	int count=0;
	int msgPtr=-1;
	unsigned char c;
	bool looking=true;

	//Open port
	if(OpenUUT()==false) return false;
	//Manage the timeout
	if(Timeout<WAIT_2S) Timeout=WAIT_2S;
	Timeout/=50;

	m_czPrintBuffer[0]=0;
	FixturePort->Flush();
	FixturePort->Write(&Message[0],true);
	while(looking)
	{
		doWait(50);		//Wait
		if((--Timeout)==0)
		{
			ToLog("Message Timeout!\n",CONSOLE);
			m_czPrintBuffer[count]=0;
			return false;
		}
		while(FixturePort->Peek())
		{
			if(count>=BUFFER_SIZE)	
			{
				ToLog("Buffer overrun!\n",CONSOLE);
				m_czPrintBuffer[count]=0;
				return false;
			}
			FixturePort->Read(&c,1);
			if(c==0x10) { msgPtr=count; }
			else m_czPrintBuffer[count++]=c;
			if((msgPtr!=-1)&&(c==0x0A)) { looking=false; break; }
		}
	}
	if(msgPtr!=-1)
	{
		if(!memcmp(&m_czPrintBuffer[msgPtr],&Expect[0],6)) { result=true; }
	}
	m_czPrintBuffer[count]=0;

	return result;
}

//=====================================================================================
//===================================[ Send UUT ]======================================
//=====================================================================================
//Send, and do not wait
void CGVRguiDlg::SendUUT(char* Message)
{
	FixturePort->Flush();
	FixturePort->Write(&Message[0],true);
}

//=====================================================================================
//===================================[ Send UUT ]======================================
//=====================================================================================
//Send, and wait for answer

bool CGVRguiDlg::WaitUUT(char* Expect,int Timeout)
{
	bool result=false;
	int count=0;
	int msgPtr=-1;
	unsigned char c;
	bool looking=true;

	//Open port
	if(OpenUUT()==false) return false;
	//Manage the timeout
	if(Timeout<WAIT_2S) Timeout=WAIT_2S;
	Timeout/=50;

	m_czPrintBuffer[0]=0;
	while(looking)
	{
		doWait(50);		//Wait
		if((--Timeout)==0)
		{
			ToLog("Message Timeout!\n",CONSOLE);
			m_czPrintBuffer[count]=0;
			return false;
		}
		while(FixturePort->Peek())
		{
			if(count>=BUFFER_SIZE)	
			{
				ToLog("Buffer overrun!\n",CONSOLE);
				m_czPrintBuffer[count]=0;
				return false;
			}
			FixturePort->Read(&c,1);
			if(c==0x10) { msgPtr=count; }
			else m_czPrintBuffer[count++]=c;
			if((msgPtr!=-1)&&(c==0x0A)) { looking=false; break; }
		}
	}
	if(msgPtr!=-1)
	{
		if(!memcmp(&m_czPrintBuffer[msgPtr],&Expect[0],6)) { result=true; }
	}
	m_czPrintBuffer[count]=0;

	return result;
}


void CGVRguiDlg::SetControls(int Status)
{
	if(Status)	//Idle mode
	{
		GetDlgItem(IDOK )->EnableWindow(true);			//Enable Exit
		GetDlgItem(IDOK2)->EnableWindow(true);			//Enable Start
		GetDlgItem(IDOK3)->EnableWindow(false);			//Disable Stop
		GetDlgItem(IDOK4)->EnableWindow(true);			//Enable Reset
		GetDlgItem(IDC_SERIAL)->EnableWindow(true);		//Enable Port
		GetDlgItem(IDC_PRINTER)->EnableWindow(true);	//Enable Port
		GetDlgItem(IDC_STATION)->EnableWindow(true);	//Enable Station
		GetDlgItem(IDC_KL82)->EnableWindow(true);		//Enable KL82 programming
		GetDlgItem(IDC_STOP)->EnableWindow(true);		//Enable Stop on fail
		GetDlgItem(IDC_EEPROM)->EnableWindow(true);		//Enable EEPROM programming
		GetDlgItem(IDC_A001)->EnableWindow(true);		//Enable Select A001
		GetDlgItem(IDC_A002)->EnableWindow(true);		//Enable Select A002
		GetDlgItem(IDC_REV1)->EnableWindow(true);		//Enable Revision
		GetDlgItem(IDC_REV)->EnableWindow(true);		// "  :"
	}
	else	//Test running
	{
		GetDlgItem(IDOK )->EnableWindow(false);			//Disable Exit
		GetDlgItem(IDOK2)->EnableWindow(false);			//Disable Start
		GetDlgItem(IDOK3)->EnableWindow(true);			//Enable Stop
		GetDlgItem(IDOK4)->EnableWindow(false);			//Disable Reset
		GetDlgItem(IDC_SERIAL)->EnableWindow(false);	//Disable Port
		GetDlgItem(IDC_PRINTER)->EnableWindow(false);	//Disable Port
		GetDlgItem(IDC_STATION)->EnableWindow(false);	//Disable Station
		GetDlgItem(IDC_KL82)->EnableWindow(false);		//Disable KL82 programming
		GetDlgItem(IDC_STOP)->EnableWindow(false);		//Disable Stop on fail
		GetDlgItem(IDC_EEPROM)->EnableWindow(false);	//Disable EEPROM programming
		GetDlgItem(IDC_A001)->EnableWindow(false);		//Disable Select A001
		GetDlgItem(IDC_A002)->EnableWindow(false);		//Disable Select A002
		GetDlgItem(IDC_REV1)->EnableWindow(false);		//Disable Revision
		GetDlgItem(IDC_REV)->EnableWindow(false);		// "  :"
	}
}

//=====================================================================================
//===================================[ Timer Event ]===================================
//=====================================================================================
//Do timer once every 1000 mS
void CGVRguiDlg::OnTimer(UINT_PTR nIDEvent)
{
	time_t timeGet;
	struct tm *time1;
	int days[]={0,31,59,90,120,151,181,212,243,273,304,334};
	char c;

	//Get time
	timeGet=time(NULL);
	time1=localtime(&timeGet);
	//Post 
	sprintf(&m_czTime[0],"%04d-%02d-%02d %02d:%02d",(time1->tm_year+1900),time1->tm_mon+1,time1->tm_mday,time1->tm_hour,time1->tm_min);
	sprintf(&m_czIsoTime[0],"%04d%02d%02d%02d%02d%02d",(time1->tm_year+1900),time1->tm_mon+1,time1->tm_mday,time1->tm_hour,time1->tm_min,time1->tm_sec);
	//Make the ASCII datestamp
	sprintf(&m_czMFRdateA[0],"%02d%02d%02d",(time1->tm_year-100),time1->tm_mon+1,time1->tm_mday);
	//Make the BCD datestamp
	sprintf(&m_czMFRdate[0],"%02d%02d%02d",(time1->tm_year-100),time1->tm_mon+1,time1->tm_mday);
	c=(m_czMFRdate[0]&0x0F)<<4; c|=(m_czMFRdate[1]&0x0F); m_czMFRdate[0]=c;	//YY to BCD
	c=(m_czMFRdate[2]&0x0F)<<4; c|=(m_czMFRdate[3]&0x0F); m_czMFRdate[1]=c;	//MM to BCD
	c=(m_czMFRdate[4]&0x0F)<<4; c|=(m_czMFRdate[5]&0x0F); m_czMFRdate[2]=c;	//DD to BCD
	m_czMFRdate[3]=0; m_czMFRdate[4]=0; m_czMFRdate[5]=0;
	//Make the ASCII datestamp
	//sprintf(&m_czMFRdateA[0],"%02d%02d%02d (0x%02X,0x%02X,0x%02X)",(time1->tm_year-100),time1->tm_mon+1,time1->tm_mday,m_czMFRdate[0],m_czMFRdate[1],m_czMFRdate[2]);
	//Tick
	if(m_bTimer)
	{
		m_dwTime=GetTickCount();		//Clear millisecond counter
		m_iAbsSec++;					//Total seconds
		if(m_iAbsSec>6000) m_iAbsSec=0;	//Rollover if more than 100 minuets
		m_iSec++;						//Seconds as partial minuets
		if(m_iSec==60) { m_iSec=0; m_iMin++; if(m_iMin>99) m_iMin=0; }
		sprintf(&m_czAsciiTime[0],"%02d:%02d",m_iMin,m_iSec);
		SetDlgItemText(IDC_TIME,CString(&m_czAsciiTime[0]));
	}	
	else { m_iSec=0; m_iMin=0; }
	
	doEvents();										//Keep UI warm
	CDialog::OnTimer(nIDEvent);
}

//=============================================================================
//============================[ Show INI ]=====================================
//=============================================================================
//Show the INI info
void CGVRguiDlg::ShowSetting(void)
{
	Console->Write(CONSOLE_COLOR_WHITE,"--Settings------------------");
	//Show the fixture port
	Console->Write(CONSOLE_COLOR_GRAY,"\n  Fixture Port: ");
	sprintf(&m_czPrintBuffer[0],"%03i",m_iFixturePort);
	GetDlgItem(IDC_SERIAL)->SetWindowTextW(CString(&m_czPrintBuffer[0]));
	Console->Write(CONSOLE_COLOR_CYAN,&m_czPrintBuffer[0]);
	//Show the printer port
	Console->Write(CONSOLE_COLOR_GRAY,"\n  Printer Port: ");
	sprintf(&m_czPrintBuffer[0],"%03i",m_iPrinterPort);
	GetDlgItem(IDC_PRINTER)->SetWindowTextW(CString(&m_czPrintBuffer[0]));
	Console->Write(CONSOLE_COLOR_CYAN,&m_czPrintBuffer[0]);
	//Show the station ID
	Console->Write(CONSOLE_COLOR_GRAY,"\n    Station ID: ");
	Console->Write(CONSOLE_COLOR_CYAN,&m_czStation[0]);
	GetDlgItem(IDC_STATION)->SetWindowTextW(CString(&m_czStation[0]));
	//Show board type
	Console->Write(CONSOLE_COLOR_GRAY,"\n    Board type: ");
	Console->Write(CONSOLE_COLOR_CYAN,"M15758A001");
	Console->Write(CONSOLE_COLOR_WHITE,"\n----------------------------\n\n");
}

//=============================================================================
//============================[ Get INI ]======================================
//=============================================================================
//Read the INI file
bool CGVRguiDlg::GetINI(void)
{
	FILE* fp=NULL;
	char buff[100];
	bool result=false;
	//Flush
	//Open INI file (default the values)
	fp=fopen("./ini/Omnia.ini","r");
	m_iFixturePort=84;
	m_iPrinterPort=1;
	sprintf(&m_czStation[0],"00");
	//See if it opened 
	if(fp!=NULL)
	{
		while(!feof(fp))
		{
			fgets(&buff[0],100,fp);
			if(!memcmp(&buff[0],"FixturePort=",11)) m_iFixturePort=atoi(&buff[12]); 
			if(!memcmp(&buff[0],"PrinterPort=",11)) m_iPrinterPort=atoi(&buff[12]); 
			if(!memcmp(&buff[0],"Station=",8)) { memcpy(&m_czStation[0],&buff[8],2); m_czStation[2]; }
			if(!memcmp(&buff[0],"Test1=",6))   { if(buff[6]) m_bTest1=true;	  else m_bTest1=false;  }
			if(!memcmp(&buff[0],"Test2=",6))   { if(buff[6]) m_bTest2=true;   else m_bTest2=false;  }
			if(!memcmp(&buff[0],"Test3=",6))   { if(buff[6]) m_bTest3=true;	  else m_bTest3=false;  }
			if(!memcmp(&buff[0],"Test4=",6))   { if(buff[6]) m_bTest4=true;   else m_bTest4=false;  }
			if(!memcmp(&buff[0],"Test5=",6))   { if(buff[6]) m_bTest5=true;   else m_bTest5=false;  }
			if(!memcmp(&buff[0],"Test6=",6))   { if(buff[6]) m_bTest6=true;   else m_bTest6=false;  }
			if(!memcmp(&buff[0],"Test7=",6))   { if(buff[6]) m_bTest7=true;   else m_bTest7=false;  }
			if(!memcmp(&buff[0],"Test8=",6))   { if(buff[6]) m_bTest8=true;   else m_bTest8=false;  }
			if(!memcmp(&buff[0],"Test9=",6))   { if(buff[6]) m_bTest9=true;   else m_bTest9=false;  }
			if(!memcmp(&buff[0],"Test10=",7))  { if(buff[7]) m_bTest10=true;  else m_bTest10=false; }
			if(!memcmp(&buff[0],"Test11=",7))  { if(buff[7]) m_bTest11=true;  else m_bTest11=false; }
			if(!memcmp(&buff[0],"Test12=",7))  { if(buff[7]) m_bTest12=true;  else m_bTest12=false; }
			if(!memcmp(&buff[0],"Test13=",7))  { if(buff[7]) m_bTest13=true;  else m_bTest13=false; }
			if(!memcmp(&buff[0],"Test14=",7))  { if(buff[7]) m_bTest14=true;  else m_bTest14=false; }
			if(!memcmp(&buff[0],"Test15=",7))  { if(buff[7]) m_bTest15=true;  else m_bTest15=false; }
			if(!memcmp(&buff[0],"Test16=",7))  { if(buff[7]) m_bTest16=true;  else m_bTest16=false; }
			if(!memcmp(&buff[0],"Test17=",7))  { if(buff[7]) m_bTest17=true;  else m_bTest17=false; }
			if(!memcmp(&buff[0],"Test18=",7))  { if(buff[7]) m_bTest18=true;  else m_bTest18=false; }
			if(!memcmp(&buff[0],"Test19=",7))  { if(buff[7]) m_bTest19=true;  else m_bTest19=false; }
			if(!memcmp(&buff[0],"Test20=",7))  { if(buff[7]) m_bTest20=true;  else m_bTest20=false; }
			if(!memcmp(&buff[0],"Test21=",7))  { if(buff[7]) m_bTest21=true;  else m_bTest21=false; }
		}
		fclose(fp);
		result=true;
	}
	else result=WrtINI();
	//Show settings
	if(result) ShowSetting();
	else Console->Write(CONSOLE_COLOR_RED,"Configuration read failed!");
	
	//Update checkboxes, not saved
	static_cast<CButton*>(GetDlgItem(IDC_KL82))->SetCheck(true);	
	static_cast<CButton*>(GetDlgItem(IDC_STOP))->SetCheck(true);
	static_cast<CButton*>(GetDlgItem(IDC_EEPROM))->SetCheck(true);
	static_cast<CButton*>(GetDlgItem(IDC_A001))->SetCheck(true);
	doWait(100);
	m_bConfEnable=true;

	return result;
}

//=============================================================================
//============================[ Write INI ]====================================
//=============================================================================
//Write the INI file
bool CGVRguiDlg::WrtINI(void)
{
	FILE* fp=NULL;
	bool result=false;

	fp=fopen("./ini/Omnia.ini","w");
	//See if it opened 
	if(fp!=NULL)
	{
		fprintf(fp,"FixturePort=%03i\n",m_iFixturePort);
		fprintf(fp,"PrinterPort=%03i\n",m_iPrinterPort);
		fprintf(fp,"Station=%s\n",&m_czStation[0]);
		//Get type
		fprintf(fp,"Version=%s\n",m_czVersion);
//-----------------------------------------------------------------
		//Power
		if(m_bTest1)	fprintf(fp,"Test1=1  - Impeadance 4.3.1\n");
		else			fprintf(fp,"Test1=0  - Impeadance 4.3.1\n"); 
		//Voltage
		if(m_bTest2)	fprintf(fp,"Test2=1  - Voltage 4.3.2\n");
		else			fprintf(fp,"Test2=0  - Voltage 4.3.2\n"); 
		//FPGA Configure
		if(m_bTest3)	fprintf(fp,"Test3=1  - KL82 Programming 4.3.3\n");
		else			fprintf(fp,"Test3=0  - KL82 Programming 4.3.3\n"); 
		//FPGA EEPROM program
		if(m_bTest4)	fprintf(fp,"Test4=1  - Reset switch 4.3.4\n");
		else			fprintf(fp,"Test4=0  - Reset switch 4.3.4\n"); 
		//INT Backlight Test
		if(m_bTest5)	fprintf(fp,"Test5=1  - Battery 4.3.5\n");
		else			fprintf(fp,"Test5=0  - Battery 4.3.5\n"); 
		//EXT Backlight Test
		if(m_bTest6)	fprintf(fp,"Test6=1  - Uboot 4.3.6\n");
		else			fprintf(fp,"Test6=0  - Uboot 4.3.6\n"); 
		//Display Ground Sense
		if(m_bTest7)	fprintf(fp,"Test7=1  - Program EEPROM 4.3.6.1\n");
		else			fprintf(fp,"Test7=0  - Program EEPROM 4.3.6.1\n"); 
		//Display Ground Sense
		if(m_bTest8)	fprintf(fp,"Test8=1  - LED 4.3.6.2\n");
		else			fprintf(fp,"Test8=0  - LED 4.3.6.2\n"); 
		//Display Ground Sense
		if(m_bTest9)	fprintf(fp,"Test9=1  - Test KL82 4.3.6.3\n");
		else			fprintf(fp,"Test9=0  - Test KL82 4.3.6.3\n"); 
		//Video Test
		if(m_bTest10)	fprintf(fp,"Test10=1  - Controlled power 4.3.6.4 and 4.3.6.5\n");
		else			fprintf(fp,"Test10=0  - Controlled power 4.3.6.4 and 4.3.6.5\n"); 
		//USB enumeration
		if(m_bTest11)	fprintf(fp,"Test11=1 - Current Source 4.3.6.6\n");
		else			fprintf(fp,"Test11=0 - Current Source 4.3.6.6\n"); 
		//UART Enumeration
		if(m_bTest12)	fprintf(fp,"Test12=1 - Storage Validation 4.3.6.7\n");
		else			fprintf(fp,"Test12=0 - Storage Validation 4.3.6.7\n"); 
		//USB Port R/W
		if(m_bTest13)	fprintf(fp,"Test13=1 - Temp Sensor 4.3.6.8\n");
		else			fprintf(fp,"Test13=0 - Temp Sensor 4.3.6.8\n"); 
		//USB Power
		if(m_bTest14)	fprintf(fp,"Test14=1 - LCD Dismount 4.3.6.9\n");
		else			fprintf(fp,"Test14=0 - LCD Dismount 4.3.6.9\n"); 
		//USB Overcurrent
		if(m_bTest15)	fprintf(fp,"Test15=1 - LVDS Video out 4.3.6.10\n");
		else			fprintf(fp,"Test15=0 - LVDS Video out 4.3.6.10\n"); 
		//BNA Port Test
		if(m_bTest16)	fprintf(fp,"Test16=1 - Ethernet Switch 4.3.6.11\n");
		else			fprintf(fp,"Test16=0 - Ethernet Switch 4.3.6.11\n"); 
		//BNA Latch Test
		if(m_bTest17)	fprintf(fp,"Test17=1 - Linux Boot 4.3.7\n");
		else			fprintf(fp,"Test17=0 - Linux Boot 4.3.7\n"); 
		//TRIND Port Test
		if(m_bTest18)	fprintf(fp,"Test18=1 - RTC 4.3.7.1\n");
		else			fprintf(fp,"Test18=0 - RTC 4.3.7.1\n"); 
		//POS Port Test
		if(m_bTest19)	fprintf(fp,"Test19=1 - USB 4.3.7.2\n");
		else			fprintf(fp,"Test19=0 - USB 4.3.7.2\n"); 
		//PUMP Port Test
		if(m_bTest20)	fprintf(fp,"Test20=1 - Audio 4.3.6.3\n");
		else			fprintf(fp,"Test20=0 - Audio 4.3.6.3\n"); 
		//Beeper Test
		if(m_bTest21)	fprintf(fp,"Test21=1 - Current Loop 4.3.7.4\n");
		else			fprintf(fp,"Test21=0 - Current Loop 4.3.7.4\n"); 
//-----------------------------------------------------------------
		fclose(fp);
		result=true;
		m_bCOnChange=false;
	}
	else result=false;
	
	return result;
}

//-----------------------------------------------------------------
//Event handlers
//-----------------------------------------------------------------
void CGVRguiDlg::OnBnClickedA001()
{
	m_bA001=!m_bA001;
	m_bA002=!m_bA002;
	if(m_bA001)
	{
		static_cast<CButton*>(GetDlgItem(IDC_A001))->SetCheck(true);
		static_cast<CButton*>(GetDlgItem(IDC_A002))->SetCheck(false);	
	}
	else
	{
		static_cast<CButton*>(GetDlgItem(IDC_A001))->SetCheck(false);
		static_cast<CButton*>(GetDlgItem(IDC_A002))->SetCheck(true);
	}
}

void CGVRguiDlg::OnBnClickedA002()
{
	m_bA001=!m_bA001;
	m_bA002=!m_bA002;
	if(m_bA002)
	{
		static_cast<CButton*>(GetDlgItem(IDC_A002))->SetCheck(true);
		static_cast<CButton*>(GetDlgItem(IDC_A001))->SetCheck(false);	
	}
	else
	{
		static_cast<CButton*>(GetDlgItem(IDC_A002))->SetCheck(false);
		static_cast<CButton*>(GetDlgItem(IDC_A001))->SetCheck(true);
	}
}


void CGVRguiDlg::OnBnClickedEeprom()
{
	m_bProgEEPROM=!m_bProgEEPROM;
	if(m_bProgEEPROM) static_cast<CButton*>(GetDlgItem(IDC_EEPROM))->SetCheck(true);
	else static_cast<CButton*>(GetDlgItem(IDC_EEPROM))->SetCheck(false);
}

void CGVRguiDlg::OnBnClickedProgKL82()
{
	m_bProgKL82=!m_bProgKL82;
	if(m_bProgKL82) static_cast<CButton*>(GetDlgItem(IDC_KL82))->SetCheck(true);
	else static_cast<CButton*>(GetDlgItem(IDC_KL82))->SetCheck(false);
}

void CGVRguiDlg::OnBnClickedStop()
{
	m_bStopOnFail=!m_bStopOnFail;
	if(m_bStopOnFail) static_cast<CButton*>(GetDlgItem(IDC_STOP))->SetCheck(1);
	else static_cast<CButton*>(GetDlgItem(IDC_STOP))->SetCheck(0);
}

void CGVRguiDlg::OnEnChangeSerial()
{
	wchar_t buff[10];

	if(m_bConfEnable)
	{
		GetDlgItem(IDC_SERIAL)->GetWindowText(&buff[0],4);
		wcstombs(&m_czPrintBuffer[0],&buff[0],9);			//Convert from wcat_t to char
		m_iFixturePort=atoi(&m_czPrintBuffer[0]);
		m_bCOnChange=true;
	}
}

void CGVRguiDlg::OnEnChangePrinter()
{
	wchar_t buff[10];

	if(m_bConfEnable)
	{
		GetDlgItem(IDC_PRINTER)->GetWindowText(&buff[0],4);
		wcstombs(&m_czPrintBuffer[0],&buff[0],9);			//Convert from wcat_t to char
		m_iPrinterPort=atoi(&m_czPrintBuffer[0]);
		m_bCOnChange=true;
	}
}

void CGVRguiDlg::OnEnChangeStation()
{
	wchar_t buff[10];

	if(m_bConfEnable)
	{
		GetDlgItem(IDC_STATION)->GetWindowText(&buff[0],3);
		wcstombs(&m_czStation[0],&buff[0],9);	//Convert from wcat_t to char
		m_czStation[3]=0;
		m_bCOnChange=true;
	}
}

void CGVRguiDlg::TapeLog(void)
{
	FILE* fp=NULL;

	//Write to log
	fp=fopen(&m_czTapeLog[0],"w");
	if(fp!=NULL)
	{
		fprintf(fp,"%s",&m_czTapeBuffer[0]);
		fclose(fp);
	}
}

void CGVRguiDlg::WrtTape(char* Message,bool Screen,bool Print)
{
	//write to display
	if(Screen)
	{
		strcat(&m_czDispBuffer[0],&Message[0]);
		SetDlgItemText(IDC_RESULT,CString(&m_czDispBuffer[0]));
		WrtLog(&Message[0]);		//Add to result log
	}
	//Write to printer
	if(Print)
	{
		if(m_iPrinterPort)
		{
			if (m_bPrinterOpen == false) { PrinterPort->Open(m_iPrinterPort, 115200); m_bPrinterOpen = true;}
			if(m_bPrinterOpen) PrinterPort->Write(&Message[0]);
		}
		strcat(&m_czTapeBuffer[0],&Message[0]);
	}
}

//=====================================================================================
//===================================[ Fix Serial ]====================================
//=====================================================================================
//Adjust the Serial Number, and zero if not usable
//Do not lead with zeros - 20180123
void CGVRguiDlg::fixSerial(void)
{
	char buff[17];
	int len=(int)strlen(&m_czUUTserial[0]);

	if(len==0)
	{
		memset(&m_czUUTserial[0],0,17);
		memset(&buff[0],0,17);	
	}
}

//=====================================================================================
//===================================[ Fix MAC ]=======================================
//=====================================================================================
//Adjust the MAC, and zero if not usable
void CGVRguiDlg::fixMac(void)
{
	char buff[17];
	int i=0,j=0;
	char c;

	while(1)	//Copy to local excluding all but 1-9 and A-F
	{
		c=m_czUUTmac[i++];
		if(c==0) break;
		if(((c>='0')&&(c<='9'))||((c>='A')&&(c<='F'))) buff[j++]=c;
	}
	buff[j]=0;	//Truncate
	//Bounce it if the length is not 12 digits
	if(strlen(&buff[0])!=12) { m_czUUTmac[0]=0; return; } 
	//Put in the colons as apropriate
	m_czUUTmac[ 0]=buff[ 0];
	m_czUUTmac[ 1]=buff[ 1];
	m_czUUTmac[ 2]=':';
	m_czUUTmac[ 3]=buff[ 2];
	m_czUUTmac[ 4]=buff[ 3];
	m_czUUTmac[ 5]=':';
	m_czUUTmac[ 6]=buff[ 4];
	m_czUUTmac[ 7]=buff[ 5];
	m_czUUTmac[ 8]=':';
	m_czUUTmac[ 9]=buff[ 6];
	m_czUUTmac[10]=buff[ 7];
	m_czUUTmac[11]=':';
	m_czUUTmac[12]=buff[ 8];
	m_czUUTmac[13]=buff[ 9];
	m_czUUTmac[14]=':';
	m_czUUTmac[15]=buff[10];
	m_czUUTmac[16]=buff[11];
	m_czUUTmac[17]=0;
}

//=====================================================================================
//===================================[ ShowMessage ]===================================
//=====================================================================================
//Show a message of type with text
bool CGVRguiDlg::ShowMessage(int Type,const char* Text)
{
	int result=0;

	CMessage dlg;

	switch(Type)
	{
		case MESSAGE_MAC:				//Pass in the buffer
			dlg.SetMessage(Type,Text,&m_czUUTmac[0]);
			result=(int)dlg.DoModal();	//Do the deed
			if(m_czUUTmac[0]==(char)0xFF) m_iStatus=TEST_STOP;
			else fixMac();
			break;	
		case MESSAGE_SERIAL:			//Pass in the buffer	
			dlg.SetMessage(Type,Text,&m_czUUTserial[0]);
			result=(int)dlg.DoModal();	//Do the deed
			if(m_czUUTserial[0]==(char)0xFF) m_iStatus=TEST_STOP;
			else fixSerial();
			break;
		default:						//No buffer needed
			dlg.SetMessage(Type,&Text[0]);
			result=(int)dlg.DoModal();	//Do the deed
			break;
	}
	delete dlg;
	doWait(200);						//Pause
	if(result==2) return true;			//OnCancel
	return false;						//OnOK
}

//=====================================================================================
//===================================[ Serial Number ]=================================
//=====================================================================================
bool CGVRguiDlg::GetSerial(void)
{
	bool result=true;

	m_czUUTserial[0]=0;
	while(1)
	{
		ShowMessage(MESSAGE_SERIAL,NULL);
		if(m_iStatus==TEST_STOP) { SetControls(CONTROLS_ON); result=false; break; }
		if(m_czUUTserial[0]!=0) break;
	}

	return result;
}

//=====================================================================================
//===================================[ MAC address ]==================================
//=====================================================================================
bool CGVRguiDlg::GetMac(void)
{
	bool result=true;

	m_czUUTmac[0]=0;
	while(1)
	{
		ShowMessage(MESSAGE_MAC,NULL);
		if(m_iStatus==TEST_STOP) { SetControls(CONTROLS_ON); result=false; break; }
		if(m_czUUTmac[0]!=0) break;
	}

	return result;
}

//=====================================================================================
//===================================[ Check Admin ]===================================
//=====================================================================================
//Yup, this is pollitical
bool CGVRguiDlg::checkAdmin(void)
{
    bool result=false;
	FILE* fp=NULL;
	
	//Check token
	result=IsUserAnAdmin();
	
	return result;
}

//=====================================================================================
//===================================[ Log Update ]====================================
//=====================================================================================
//Write to the log
void CGVRguiDlg::WrtLog(char* Text)
{
	FILE* fp;

	fp=fopen(&m_czResultLog[0],"a+");
	if(fp!=NULL)
	{
		fprintf(fp,"%s%s",GetTimeStamp(),&Text[0]);
		fclose(fp);
	}
}

//=====================================================================================
//===================================[ Post Message ]==================================
//=====================================================================================
bool CGVRguiDlg::DoMessage(void)
{
	bool result=false;
	CTestMessage dlg;
	
	dlg.SetReturn(&result);
	dlg.DoModal();
	delete dlg;
	
	return result;
}



