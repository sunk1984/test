//******************************************************************************
//* CConsole.h 
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
//* 20160226 - 
//******************************************************************************
//Defines
#define CONSOLE_COLOR_BLACK		0
#define CONSOLE_COLOR_RED		1
#define CONSOLE_COLOR_GREEN		2
#define CONSOLE_COLOR_YELLOW	3
#define CONSOLE_COLOR_BLUE		4
#define CONSOLE_COLOR_MAGENTA	5
#define CONSOLE_COLOR_CYAN		6
#define CONSOLE_COLOR_GRAY		7	//20151027
#define CONSOLE_COLOR_GREY		7	//20151027
#define CONSOLE_COLOR_WHITE		8
#define CONSOLE_COLOR_NULL		9
#define CONSOLE_COLOR_DEFAULTF		CONSOLE_COLOR_WHITE
#define CONSOLE_COLOR_DEFAULTB		CONSOLE_COLOR_BLACK

#define CONSOLE_VERSION			0x20160926

class CConsole
{

public:
	CConsole();													//Plain constructor
	CConsole(TCHAR* Title);										//Constructor with tile
	CConsole(unsigned int Width,unsigned int Hieght);			//Constructor with char hieght and width
	~CConsole();

	void Show();												//This must be called once before most functions will work
	void Show(TCHAR* Title);									// "  "  only change the title
	void SetTitle(TCHAR* Title);								//Set the title (may be called at any time)
	void SetToBack(bool Back);									//Set default behavior (foreground or backgound) - 20151102
	void SetPosition(unsigned int X,unsigned int Y);			//Set the position of the console
	//Any of these functions can be called afer the first Show
	void Hide();												//Makes the console invisible
	void SetColor(unsigned int Fcolor);							//Set the foreground color
	void SetColor(unsigned int Fcolor,unsigned int Bcolor);		//Set the foreground, and background color
	void Write(char* Message);									//Write message using current forgrouond color
	void Write(unsigned int Fcolor,char* Message);				//Write message using given foreground color
	void Write(CString Message);								//Write a CString using current forground color
	void Write(unsigned int Fcolor,CString Message);			//Write message using given foreground color
	void Clear();												//Clear the console to the current backgoround color
	void Clear(unsigned int Bcolor);							//Clear the console to the given background color
	void ThrowFocus();											//Throw the focus to the console
	bool IsOpen();												//Return true if the console is open - 20151027
	bool IsVisible();											//Return true if the consile is visible - 20151027
	bool IsEbabled();											//Return true if the console is enabled - 20151102
	void Disable();												//Disable and hide - 20151102
	void Enable();												//Re-Enable - 20151102
	unsigned int GetVersion();									//Return hex console version as unsigned int - 20160226
	bool GetHandle(HWND*);										//Return the handle if window is open - 20160226
	bool IsKey();												//Return true of a key is ready, or false if console is not avilable - 20160226
	char WaitKey();												//Waits for a key, and returns it, returns zero if console is not avilable
	unsigned int GetString(char* Buffer,unsigned int Max);		//Waits for a string, and returns the number of char 

protected:
	unsigned int m_iCurrentFcolor;	//Foreground color
	unsigned int m_iCurrentBcolor;	//Background color
	bool m_bConsoleOpen;			//Open or closed
	bool m_bConsoleHidden;			//Visible or not
	bool m_bConsoleDisable;			//Disable flag  - 20151102
	bool m_bConsoleBack;			//Set to backgound - 20151102
	bool m_bCtrlC;					
	FILE *m_fStdIn,*m_fStdOut;
	HWND m_hWindow;
	HANDLE m_Screen;				
	CONSOLE_CURSOR_INFO curInf;
	unsigned int m_iWidth;			//Console width in charactors
	unsigned int m_iHieght;			//Console back buffer size in lines
	unsigned int m_iX;				//Console X pos
	unsigned int m_iY;				//Console Y pos
	TCHAR m_szTitle[200];			//Name

	void consoleOn(void);
	bool console_ctrl_handler(DWORD dwCtrlType);
	void doEvents(void);
};

