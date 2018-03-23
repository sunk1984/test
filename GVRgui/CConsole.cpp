//******************************************************************************
//* CConsole.cpp
//* George "Phred" Nees  (George.Nees@VeriFone.com)
//*	General Purpose Console Object for use with GUI apps (requires MFC).
//*    _____                      _      
//*   / ____|                    | |     
//*  | |     ___  _ __  ___  ___ | | ___ 
//*  | |    / _ \| '_ \/ __|/ _ \| |/ _ \
//*  | |___| (_) | | | \__ \ (_) | |  __/
//*   \_____\___/|_| |_|___/\___/|_|\___|
//*                                     
//* 20151020 - First incarnation
//* 20151027 - Added IsOpen, IsVisible, add GRAY, and remove console on close
//* 20151102 - Add enable/disable
//* 20160226 - Allow the console to be moved, and other changes.
//******************************************************************************

#include "stdafx.h"
#include "conio.h"
#include <windows.h>
#include "CConsole.h"
#include <tchar.h>
#include <string.h>

//Supress warnings
#pragma warning(disable : 4996)

//=============================================================================
//============================[ Constructor ]==================================
//=============================================================================
//Create a console 80x500 console window
CConsole::CConsole(void)
{
	m_iCurrentFcolor=CONSOLE_COLOR_DEFAULTF;
	m_iCurrentBcolor=CONSOLE_COLOR_DEFAULTB;
	m_bConsoleOpen=false;		//20151027
	m_bConsoleHidden=false;		//20151027
	m_bConsoleDisable=false;	//20151102
	m_bCtrlC=false;
	m_fStdIn=NULL;
	m_fStdOut=NULL;
	m_hWindow=NULL;
	m_Screen=NULL;
	m_iWidth=80;
	m_iHieght=500;
	m_szTitle[0]=0;
}

//=============================================================================
//============================[ Constructor ]==================================
//=============================================================================
//Overload - Create a console 80x500 console window with a new title
CConsole::CConsole(TCHAR* Title)
{
	m_iCurrentFcolor=CONSOLE_COLOR_DEFAULTF;
	m_iCurrentBcolor=CONSOLE_COLOR_DEFAULTB;
	m_bConsoleOpen=false;		//20151027
	m_bConsoleHidden=false;		//20151027
	m_bConsoleDisable=false;	//20151102
	m_bConsoleBack=false;		//20151102
	m_bCtrlC=false;
	m_fStdIn=NULL;
	m_fStdOut=NULL;
	m_hWindow=NULL;
	m_Screen=NULL;
	m_iWidth=80;
	m_iHieght=500;
	wsprintf(m_szTitle,_T("%s"),Title);
}

//=============================================================================
//============================[ Constructor ]==================================
//=============================================================================
//Overload - Open with defined width and height(in charactors), min and max enforced
CConsole::CConsole(unsigned int Width,unsigned int Hieght)
{
	m_iCurrentFcolor=CONSOLE_COLOR_DEFAULTF;
	m_iCurrentBcolor=CONSOLE_COLOR_DEFAULTB;
	m_bConsoleOpen=false;		//20151027
	m_bConsoleHidden=false;		//20151027
	m_bConsoleDisable=false;	//20151102
	m_bConsoleBack=false;		//20151102
	m_bCtrlC=false;
	m_fStdIn=NULL;
	m_fStdOut=NULL;
	m_hWindow=NULL;
	m_Screen=NULL;
	if(Width<40) Width=40;
	if(Width>80) Width=80;
	m_iWidth=Width;
	if(Hieght<25) Hieght=25;
	if(Hieght>2000) Hieght=2000;
	m_iHieght=Hieght;
	m_szTitle[0]=0;
}

//=============================================================================
//============================[ Destructor ]===================================
//=============================================================================
CConsole::~CConsole(void)
{
	m_bConsoleOpen=false;
	ShowWindow(m_hWindow,SW_HIDE);
	if(m_fStdIn!=NULL) fclose(m_fStdIn);
	if(m_fStdOut!=NULL) fclose(m_fStdOut);
	FreeConsole();	
}

//=============================================================================
//============================[ Is Console Open ]==============================
//=============================================================================
//Return hex console version as unsigned int - 20160226
unsigned int CConsole::GetVersion()
{
	return (unsigned int)CONSOLE_VERSION;
}


//=============================================================================
//============================[ Is Console Open ]==============================
//=============================================================================
//Return true of the console is open - 20151027
bool CConsole::IsOpen(void)
{
	return m_bConsoleOpen;
}

//=============================================================================
//============================[ Is Console Visible ]===========================
//=============================================================================
//Return true of the console is visible - 20151027
bool CConsole::IsVisible(void)
{
	return !m_bConsoleHidden;
}

//=============================================================================
//============================[ Is Console Enabled ]===========================
//=============================================================================
//Return true if the consile is enabled - 20151102
bool CConsole::IsEbabled()
{
	return !m_bConsoleDisable;
}

//=============================================================================
//============================[ Set To Back ]==================================
//=============================================================================
//Set the console default to backgound if true - 20151102
void CConsole::SetToBack(bool Back)
{
	if(Back)	//If setting to true
	{
		if((!m_bConsoleHidden)&&(!m_bConsoleDisable)) ::SetWindowPos(m_hWindow,HWND_BOTTOM,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
		m_bConsoleBack=true;
	}
	else //If setting to false
	{
		if((!m_bConsoleHidden)&&(!m_bConsoleDisable)) SetFocus(m_hWindow);
		m_bConsoleBack=false;
	}
}

//=============================================================================
//============================[ Set Position ]=================================
//=============================================================================
//Moves the console if visible
void CConsole::SetPosition(unsigned int X,unsigned int Y)
{
	if((m_bConsoleDisable==false)||(m_bConsoleOpen==true)||(m_bConsoleHidden==false))
	{
		::SetWindowPos(m_hWindow,HWND_BOTTOM,X,Y,0,0,SWP_NOZORDER|SWP_NOSIZE);
		m_iX=X;
		m_iY=Y;
	}
}

//=============================================================================
//============================[ Show Console ]=================================
//=============================================================================
//Show the console
void CConsole::Show(void)
{
	if(m_bConsoleDisable==false)						
	{
		if(m_bConsoleOpen==false) consoleOn();		//If not opened
		else
		{
			if(m_bConsoleHidden)					//If just hidden
			{
				ShowWindow(m_hWindow,SW_SHOW);
				ASSERT( ::IsWindow(m_hWindow));
				if(m_bConsoleBack)
				{
					
					::SetWindowPos(m_hWindow,HWND_BOTTOM,m_iX,m_iY,0,0,SWP_NOSIZE);	
				}
				{
					::SetWindowPos(m_hWindow,HWND_TOP,m_iX,m_iY,0,0,SWP_NOSIZE);
					SetFocus(m_hWindow);
				}
				m_bConsoleHidden=false;
			}
		}
		if(m_szTitle[0]) SetConsoleTitle(&m_szTitle[0]);
	}
}

//=============================================================================
//============================[ Show Console ]=================================
//=============================================================================
//Overload - Show the console, and set title
void CConsole::Show(TCHAR* Title)
{
	if(m_bConsoleDisable==false)
	{
		wsprintf(m_szTitle,_T("%s"),Title);
		if(m_bConsoleOpen==false) consoleOn();		//If not opened
		else
		{
			if(m_bConsoleHidden)					//If just hidden			
			{
				ShowWindow(m_hWindow,SW_SHOW);
				ASSERT( ::IsWindow(m_hWindow));
				if(m_bConsoleBack)
				{
					::SetWindowPos(m_hWindow,HWND_BOTTOM,m_iX,m_iY,0,0,SWP_NOSIZE);	
				}
				else
				{
					::SetWindowPos(m_hWindow,HWND_TOP,m_iX,m_iY,0,0,SWP_NOSIZE);
					SetFocus(m_hWindow);
				}
				m_bConsoleHidden=false;
			}
		}
	}
}

//=============================================================================
//============================[ Hide Console ]=================================
//=============================================================================
void CConsole::Hide(void)
{
	if(m_bConsoleOpen)
	{
		if(m_bConsoleHidden==false)
		{
			ShowWindow(m_hWindow,SW_HIDE);
			m_bConsoleHidden=true;
		}
	}
}

//=============================================================================
//============================[ Disable Console ]==============================
//=============================================================================
//Disable console - 20151102
void CConsole::Disable(void)
{
	if(m_bConsoleDisable==false)
	{
		if(m_bConsoleOpen)
		{
			ShowWindow(m_hWindow,SW_HIDE);
			m_bConsoleHidden=true;
			m_bConsoleDisable=true;
		}
	}
}

//=============================================================================
//============================[ Return Handle =================================
//=============================================================================
//Returns the handle if one is avialable - 20160226
bool CConsole::GetHandle(HWND* RetHandle)
{
	if(m_bConsoleOpen)
	{
		RetHandle=&m_hWindow;
		return true;
	}
	//No handle if not open
	RetHandle=NULL;	
	return false;	
}

//=============================================================================
//============================[ Enable Console ]===============================
//=============================================================================
//Enable console - 20151102
void CConsole::Enable(void)
{
	if(m_bConsoleDisable==true)
	{
		if(m_bConsoleOpen)
		{
			ShowWindow(m_hWindow,SW_SHOW);
			m_bConsoleHidden=false;
			m_bConsoleDisable=false;
			//Set focus to default - 20151102
			if(m_bConsoleBack)
			{
				ASSERT( ::IsWindow(m_hWindow));
				::SetWindowPos(m_hWindow,HWND_BOTTOM,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
			}
			else SetFocus(m_hWindow);
		}
	}
}

//=============================================================================
//============================[ setColor ]=====================================
//=============================================================================
//Foreground only
void CConsole::SetColor(unsigned int Fcolor)
{
	if(m_bConsoleOpen)
	{
		if(Fcolor<CONSOLE_COLOR_NULL)
		{
			switch(Fcolor)	//Get bits required to define forecolor
			{
				case CONSOLE_COLOR_BLACK:	{ Fcolor=0; break; }
				case CONSOLE_COLOR_RED:		{ Fcolor=FOREGROUND_RED|FOREGROUND_INTENSITY;; break; }			
				case CONSOLE_COLOR_GREEN:	{ Fcolor=FOREGROUND_GREEN; break; }		
				case CONSOLE_COLOR_YELLOW:	{ Fcolor=FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_INTENSITY; break; }			
				case CONSOLE_COLOR_BLUE:	{ Fcolor=FOREGROUND_BLUE|FOREGROUND_INTENSITY; break; }
				case CONSOLE_COLOR_MAGENTA:	{ Fcolor=FOREGROUND_RED|FOREGROUND_BLUE|FOREGROUND_INTENSITY; break; }
				case CONSOLE_COLOR_CYAN:	{ Fcolor=FOREGROUND_GREEN|FOREGROUND_BLUE|FOREGROUND_INTENSITY; break; }
				case CONSOLE_COLOR_GRAY:	{ Fcolor=FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE; break; }
				case CONSOLE_COLOR_WHITE:	{ Fcolor=FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE|FOREGROUND_INTENSITY; break; }
			}
			m_iCurrentFcolor=Fcolor;
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),Fcolor|m_iCurrentBcolor);
		}
	}
}

//=============================================================================
//============================[ setColor ]=====================================
//=============================================================================
//Overload - Forground and Background
void CConsole::SetColor(unsigned int Fcolor,unsigned int Bcolor)
{
	if(m_bConsoleOpen)
	{
		if(Fcolor<CONSOLE_COLOR_NULL)
		{
			switch(Fcolor)	//Get bits required to define forecolor
			{
				case CONSOLE_COLOR_BLACK:	{ Fcolor=0; break; }
				case CONSOLE_COLOR_RED:		{ Fcolor=FOREGROUND_RED|FOREGROUND_INTENSITY; break; }			
				case CONSOLE_COLOR_GREEN:	{ Fcolor=FOREGROUND_GREEN; break; }		
				case CONSOLE_COLOR_YELLOW:	{ Fcolor=FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_INTENSITY; break; }			
				case CONSOLE_COLOR_BLUE:	{ Fcolor=FOREGROUND_BLUE|FOREGROUND_INTENSITY; break; }
				case CONSOLE_COLOR_MAGENTA:	{ Fcolor=FOREGROUND_RED|FOREGROUND_BLUE; break; }
				case CONSOLE_COLOR_CYAN:	{ Fcolor=FOREGROUND_GREEN|FOREGROUND_BLUE|FOREGROUND_INTENSITY; break; }		
				case CONSOLE_COLOR_WHITE:	{ Fcolor=FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE; break; }
			}
			m_iCurrentFcolor=Fcolor;
		}
		else Fcolor=m_iCurrentFcolor;

		if(Bcolor<CONSOLE_COLOR_NULL)
		{
			switch(Bcolor)	//Get bits required to define backcolor
			{
				case CONSOLE_COLOR_BLACK:	{ Bcolor=0; break; }
				case CONSOLE_COLOR_RED:		{ Bcolor=BACKGROUND_RED; break; }			
				case CONSOLE_COLOR_GREEN:	{ Bcolor=BACKGROUND_GREEN; break; }		
				case CONSOLE_COLOR_YELLOW:	{ Bcolor=BACKGROUND_RED|BACKGROUND_GREEN|BACKGROUND_INTENSITY; break; }			
				case CONSOLE_COLOR_BLUE:	{ Bcolor=BACKGROUND_BLUE; break; }
				case CONSOLE_COLOR_MAGENTA:	{ Bcolor=BACKGROUND_RED|BACKGROUND_BLUE|BACKGROUND_INTENSITY; break; }
				case CONSOLE_COLOR_CYAN:	{ Bcolor=BACKGROUND_GREEN|BACKGROUND_BLUE|BACKGROUND_INTENSITY; break; }		
				case CONSOLE_COLOR_WHITE:	{ Bcolor=BACKGROUND_RED|BACKGROUND_GREEN|BACKGROUND_BLUE; break; }
			}
			m_iCurrentBcolor=Bcolor;
		}
		else Bcolor=m_iCurrentBcolor;

		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),Fcolor|Bcolor);
	}
}

//=============================================================================
//============================[ Clear the screen ]=============================
//=============================================================================
//Clear the screen to current backgrouond
void CConsole::Clear()
{
	if(m_bConsoleOpen) system("cls");
}

//=============================================================================
//============================[ Clear the screen ]=============================
//=============================================================================
//Clear the screen to given backgrouond
void CConsole::Clear(unsigned int Bcolor)
{
	SetColor(m_iCurrentFcolor,Bcolor);
	if(m_bConsoleOpen) system("cls");
}

//=============================================================================
//============================[ Set Focus ]====================================
//=============================================================================
//Send focus to console
void CConsole::ThrowFocus()
{
	if(m_bConsoleOpen&&(!m_bConsoleHidden)) SetFocus(m_hWindow);	//20151102
}

//=============================================================================
//============================[ Set Title ]====================================
//=============================================================================
//Send focus to console
void CConsole::SetTitle(TCHAR* Title)
{
	wsprintf(m_szTitle,_T("%s"),Title);
	if(m_bConsoleOpen) SetConsoleTitle(Title);					
}

//=============================================================================
//============================[ Write ]========================================
//=============================================================================
//Write to the console
void CConsole::Write(char* Message)
{
	if(m_bConsoleOpen) printf("%s",Message);
}

//=============================================================================
//============================[ Write ]========================================
//=============================================================================
//Write to the console
void CConsole::Write(unsigned int Fcolor,char* String)
{
	if(m_bConsoleOpen)
	{
		SetColor(Fcolor);
		printf("%s",String);
	}
}

//=============================================================================
//============================[ Write ]========================================
//=============================================================================
//Write CString to the console
void CConsole::Write(CString Message)
{
	if(m_bConsoleOpen) wprintf(L"%s",Message);
}

//=============================================================================
//============================[ Write ]========================================
//=============================================================================
//Write to the console
void CConsole::Write(unsigned int Fcolor,CString Message)
{
	if(m_bConsoleOpen)
	{
		SetColor(Fcolor);
		wprintf(L"%s",Message);
	}
}

//=============================================================================
//============================[ Open console ]=================================
//=============================================================================
//Start the console
void CConsole::consoleOn(void)
{
	COORD bufferSize;
	
	AllocConsole(); 
	m_hWindow=GetForegroundWindow();						//Get pointer
	if(m_bConsoleBack) SetWindowPos(m_hWindow,HWND_BOTTOM,0,0,0,0,SWP_NOMOVE);
	else 
	{
		SetWindowPos(m_hWindow,HWND_BOTTOM,0,0,0,0,SWP_NOMOVE|SWP_NOZORDER);
		SetFocus(m_hWindow);
	}
	m_fStdIn=freopen("conin$","r",stdin); 
	m_fStdOut=freopen("conout$","w",stdout);
	m_Screen=GetStdHandle(STD_OUTPUT_HANDLE);
	setvbuf(stdout,NULL,_IONBF,0);							//Turn off buffering of printf
	//TODO: Fix this!
//	SetConsoleCtrlHandler(&console_ctrl_handler(),true);	//Vector the ctl-C keypress
	//Set up cursor
	curInf.bVisible=false;
	SetConsoleCursorInfo(m_Screen,&curInf);
	//Set up console buffer
	bufferSize.X=m_iWidth;									//Width
	bufferSize.Y=m_iHieght;									//Buffer length
	SetConsoleScreenBufferSize(m_Screen,bufferSize);
	system("cls");
	//Set colors
	SetColor(CONSOLE_COLOR_DEFAULTF,CONSOLE_COLOR_DEFAULTB);
	Sleep(50);
	m_bConsoleOpen=true;
	m_bConsoleHidden=false;
	//Set title
	if(m_szTitle[0]) SetConsoleTitle(&m_szTitle[0]);
}

//=============================================================================
//============================[ break handler ]================================
//=============================================================================
//This is used to diable control-C from resulting in an exit
bool CConsole::console_ctrl_handler(DWORD dwCtrlType)
{
	if(dwCtrlType==CTRL_C_EVENT)
	{
		m_bCtrlC=true;
		return true;
	}
	return false;
}

//=====================================================================================
//===================================[ Do Events ]=====================================
//=====================================================================================
//Keep the message pump runnnig - 20160226
void CConsole::doEvents(void)
{
	MSG msg;

	while(PeekMessage(&msg,NULL,NULL,NULL,PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

//=============================================================================
//============================[ Test for key ]=================================
//=============================================================================
//Returns true if there is a key ready - 20160226
//Returns false if no key or console is not avilable
bool CConsole::IsKey()
{
	if((m_bConsoleDisable==false)||(m_bConsoleOpen==true)||(m_bConsoleHidden==false))
	{
		if(_kbhit()) return true;
	}
	return false;
}

//=============================================================================
//============================[ Wait for key ]=================================
//=============================================================================
//Waits for a key - 20160226
//Returns zero if no key or console is not avilable
char CConsole::WaitKey()
{
	if((m_bConsoleDisable==false)||(m_bConsoleOpen==true)||(m_bConsoleHidden==false))
	{
		curInf.bVisible=true;
		SetConsoleCursorInfo(m_Screen,&curInf);
		while(1)
		{
			if(_kbhit()) return (char)_getch();
			else doEvents();
		}
		curInf.bVisible=false;
		SetConsoleCursorInfo(m_Screen,&curInf);
	}
	return 0;
}

//=============================================================================
//============================[ Wait for string ]==============================
//=============================================================================
//Waits for a string - 20160226
//Returns zero if the console is unavailable
unsigned int CConsole::GetString(char* Buffer,unsigned int Max)
{
	unsigned int cnt=0;
	char c=0;

	if((m_bConsoleDisable==false)||(m_bConsoleOpen==true)||(m_bConsoleHidden==false))
	{
		curInf.bVisible=true;
		SetConsoleCursorInfo(m_Screen,&curInf);
		while(1)
		{
			if(_kbhit())
			{
				c=(char)_getch();
				if((c==0x08)&&(cnt>0)) { printf("\b \b"); Buffer[--cnt]=0; continue; }
				if(c==0x0D) { printf("\n"); break; };
				if((c>=' ')&&(cnt<Max)) { printf("%c",c); Buffer[cnt++]=c; }
			}
			else doEvents();
		}
		curInf.bVisible=false;
		SetConsoleCursorInfo(m_Screen,&curInf);
	}
	return cnt;
}
