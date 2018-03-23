//******************************************************************************
//* CPort.cpp: 
//* George "Phred" Nees (Phredog@Yahoo.com)
//* This module is the interlectual property of George "Phred" Nees
//* It is free for anyone to use under GPL.
//* Please respect my authoriship.
//*	General Purpose Serial Port Object (requires MFC).
//*   ___          _      _   ___         _   
//*  / __| ___ _ _(_)__ _| | | _ \___ _ _| |_ 
//*  \__ \/ -_) '_| / _` | | |  _/ _ \ '_|  _|
//*  |___/\___|_| |_\__,_|_| |_| \___/_|  \__|
//*                                          
//* WindowsXP must be defined to enable driving RTS and DTR
//* Test built with VC++ 6.0, and VC++ 2005
//*
//* Version 20060810 - First Incarnation
//* Version 20080112 - Previously 7 data bits would not work
//* Version 20080915 - Flush queue methoid added. 
//* Version 20080918 - Fixed a bug where the party does not get updated if none. 
//* Version 20110826 - Support com ports above 9 
//* Version 20111203 - First attempt to add RTS, DTR control
//* Version 20111207 - Flush returns the number of bytes, and now you can read handshake signals
//* Version 20120103 - Fixed some warnings under VC++ 2005  I hope this does not cause a problem
//* Version 20140301 - Add charactor delay setting
//* Version 20140919 - Higher resolution for wait set as a constant
//* Version 20160401 - Send strings as continious blocks, and add the IsOpen method, detect of port number is changed
//******************************************************************************

#define WindowsXP

#undef UNICODE
#undef _UNICODE

class CPort
{

public:
	CPort();
	~CPort();

	bool	Open(int,int,char,char,char,char,char); //Port,Baud,Parity,Data bits,stop bits,rts,dtr
	bool	Open(int,int,char,char,char);			//Port,Baud,Parity,Data bits,stop bits, All handshake singnals are disabled
	bool	Open(int,int);							//Port,Baud, Defaults to N,8,1
	
	void	Close(void);							//Close the port

	int		Read(void*,int);						//Read data
	int		ReadLine(char* buffer,int limit);		//Read string as TTY
	bool	WaitString(char *String, int Timeout);	//Wait for string
	int		Write(const char*, int);				//Writes binary data
	int		Write(const char*, int, bool);			//Writes binary data, and waits for completion
	int		Write(const char*);						//Writes a string
	void	Write(const char*, bool);				//Writes a string and waits for a completion
	int		Peek(void);								//Returns the number of char in the queue
	bool	DataCheck(int, char*);					//Check for flag, and minimum data - 20161030
	bool	DataCheck(char*);						//Check for flag, and data - 20161030
	bool	DataCheck(void);						//Check for flag - 20161030
	bool	DataSeek(char*);						//Check for flag and string - 20161030
	bool	WildcardSeek(char*);					//Check for flag and string with wildcards - 20161030
	bool	DataGet(char*, int);					//Get data if present - 20161030
	int		Flush(void);							//Flush everything in all queue
	void	NewSeek(void);							//Flush the seek queue - 20161030
	void	NewSeek(char);							//Flush the seek queue, and set flag - 20161030
	bool	NewSeek(char,int);						//Flush the seek queue, enable buffer, and set flag - 20170726
	bool	Wait(unsigned int);						//Waits for given time
	bool	Wait(unsigned int,int);					//Waits for given time and min char
	void	SetDelay(int);							//Sets charactor delay 0-500mS
	bool	WriteByte(unsigned char);				//Single byte write
	char	ReadByte(void);							//Single byte read
	bool	IsOpen(void);							//Return status
	void	SetMask(bool);							//Skip all data with bit 7 set - 20160913
	void	GetBuffer(char*);						//Read data and release buffer
	

#ifdef	WindowsXP									//Only on Windows XP or later supports the following:								
	bool	SetHandshake(DWORD dwFunc);				//Controls DTR (only on Windows XP or later)
	int		GetModemStat(void);						//Return the modem status bits
	bool	SetRTS(void);							//Set RTS
	bool	ClearRTS(void);							//Clear RTS
	bool	SetDTR(void);							//Set DTR
	bool	ClearDTR(void);							//Clear DTR
#endif

protected:
	void	WaitQueue(void);						//Wait for write quene to flush
	int		m_Port;									//Current open port
	int		m_Baud;									//Current baud rate - 20170726
	bool	m_RTSenable;							//RTS is enabled
	bool	m_DTRenable;							//DTR is enabled	
	int		m_mSdelay;								//Intra charactor delay
	char	m_CasheBuff[10];						//Buffer of last 8 char  - 20161030
	int		m_CashePtr;								//Pointer into seek buffer
	bool	m_DataFlag1;							//Flag charactor found flag 1 - 20161030
	bool	m_DataFlag2;							//Flag charactor found flag 2 - 20161030
	char	m_FlagChar;								//Flag charactor - 20161030 
	int		m_InQueue;								//Count - 20161030
	char	m_Char;									//Last char - 20161030
	char*   m_Buffer;								//Save charactors - 20170726
	int		m_Ptr;									//Pointer into buffer - 20170726
	int		m_Size;									//Buffer size
	
	HANDLE m_hIDComDev;
	DCB m_dcb;
	OVERLAPPED m_OverlappedRead,m_OverlappedWrite;
};

