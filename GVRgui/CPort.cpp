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
//* Version 20170726 - Add char capture mode to seek
//******************************************************************************

#include "stdafx.h"
#include "CPort.h"

//Supress warnings
#pragma warning(disable : 4996)
#pragma warning(disable : 4800)

#define T_RESOLUTION	5

//Constructor (Default some critical settings)
CPort::CPort()
{
	m_Port=0;
	m_Baud=0;
	m_hIDComDev=NULL;
	m_RTSenable=true;
	m_DTRenable=true;
	m_mSdelay=0;
	m_InQueue=0;
	memset(&m_dcb,0,sizeof(m_dcb));
	//Flush working seek buffer - 20161030
	memset(&m_CasheBuff[0],0,9);		
	m_CashePtr=0;
	m_DataFlag1=false;
	m_DataFlag2=false;
	m_FlagChar=0x10;
	m_Buffer=NULL;	//20170726
}

//Destructor
CPort::~CPort()
{
	Close();
}

//=====================================================================================
//===================================[ Open ]==========================================
//=====================================================================================
//Open port member method - All parameters
//Success = true 
bool CPort::Open(int nPort,int nBaud,char bParity,char bBits,char bStop,char bRts,char bDtr)
{
	char szPort[20];

	if(nPort!=m_Port) Close();				//20160401	
		
	if(m_hIDComDev!=NULL) return true;		//Already open
	//Claim the port if possible
	sprintf((LPSTR)&szPort,"\\\\.\\COM%d",nPort);
	m_hIDComDev=CreateFileA((LPSTR)&szPort,GENERIC_READ|GENERIC_WRITE,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL|FILE_FLAG_OVERLAPPED,NULL);
	if(m_hIDComDev==NULL) return false;	//Return fail if deviec is not available
	//Remember the baud rate - 20170726
	m_Baud=nBaud;
	//Flush the butters
	memset(&m_OverlappedRead,0,sizeof(OVERLAPPED));
 	memset(&m_OverlappedWrite,0,sizeof(OVERLAPPED));
	//Crap required by windows to use the serial port
	COMMTIMEOUTS CommTimeOuts;
	CommTimeOuts.ReadIntervalTimeout=0xFFFFFFFF;
	CommTimeOuts.ReadTotalTimeoutMultiplier=0;
	CommTimeOuts.ReadTotalTimeoutConstant=0;
	CommTimeOuts.WriteTotalTimeoutMultiplier=0;
	CommTimeOuts.WriteTotalTimeoutConstant=5000;
	SetCommTimeouts(m_hIDComDev,&CommTimeOuts);
	//Enable windows events for overlapping TX, and RX
	m_OverlappedRead.hEvent=CreateEvent(NULL,true,false,NULL);
	m_OverlappedWrite.hEvent=CreateEvent(NULL,true,false,NULL);
	//Create the default DCB
	m_dcb.DCBlength=sizeof(DCB);
	GetCommState(m_hIDComDev,&m_dcb);
	//Set speed
	m_dcb.BaudRate=nBaud;
	//Binary - Only mode presently supported by Windows
	m_dcb.fBinary=true;
	//Handle partity
	switch(bParity&0xDF)
	{
		case 'E': { m_dcb.Parity=EVENPARITY; break; }
		case 'O': { m_dcb.Parity=ODDPARITY;  break; }
		default:  { m_dcb.Parity = NOPARITY; break; }
	}
	//Handle the word length
	bBits&=0x0F;	
	if(bBits>8) bBits=9;
	m_dcb.ByteSize=bBits;
	//Handle stop bits - Only one or two is supported
	if(bStop==2) m_dcb.StopBits=2;		//Two stop bits
	else m_dcb.StopBits=0;				//One stop bit
	//Handle RTS
	m_RTSenable=false;
	switch(bRts&0xDF)	//Set up RTS using info given
	{
		case 'E':
			m_dcb.fDtrControl=RTS_CONTROL_ENABLE;
			m_RTSenable=true;							
			break;
		case 'H':
			m_dcb.fDtrControl=RTS_CONTROL_HANDSHAKE;
			break;
		case 'T':
			m_dcb.fDtrControl=RTS_CONTROL_TOGGLE;
			break;
		default:
			m_dcb.fRtsControl=RTS_CONTROL_DISABLE;
	}
	//Handle DTR
	m_DTRenable=false;
	switch(bDtr&0xDF)	//Set up DTR using info given
	{
		case 'E':
			m_dcb.fDtrControl=DTR_CONTROL_ENABLE;
			m_DTRenable=true;		
			break;
		case 'H':
			m_dcb.fDtrControl=DTR_CONTROL_HANDSHAKE;
			break;
		default:
			m_dcb.fDtrControl=DTR_CONTROL_DISABLE;
	}
	//Send all the settings to the OS driver
	if(!SetCommState(m_hIDComDev,&m_dcb)||!SetupComm(m_hIDComDev,10000,10000)||m_OverlappedRead.hEvent==NULL||m_OverlappedWrite.hEvent==NULL)
	{
		if(m_OverlappedRead.hEvent!=NULL) CloseHandle(m_OverlappedRead.hEvent);
		if(m_OverlappedWrite.hEvent!=NULL) CloseHandle(m_OverlappedWrite.hEvent);
		CloseHandle(m_hIDComDev);
		m_hIDComDev=NULL;
		return false;	//Init failed
	}
	//Success
	m_Port=nPort;		//Save port - 20160401

	//Flush working seek buffer - 20161030
	m_InQueue=0;
	memset(&m_CasheBuff[0],0,9);		
	m_CashePtr=0;
	m_DataFlag1=false;
	m_DataFlag2=false;
	m_FlagChar=0x10;

	return true;
}

//=====================================================================================
//===================================[ Open ]==========================================
//=====================================================================================
//Open port member method overload
//Same as above but ignore RTS and DTR 
//Success = true 
bool CPort::Open(int nPort,int nBaud,char bParity,char bBits,char bStop)
{
	return Open(nPort,nBaud,bParity,bBits,bStop,'D','D');
}

//=====================================================================================
//===================================[ Open ]==========================================
//=====================================================================================
//Open port member method overload
//Same as above, but only basic serial port control
bool CPort::Open(int nPort,int nBaud)
{
	return Open(nPort,nBaud,'N','8','1','D','D');
}

//=====================================================================================
//===================================[ Close ]=========================================
//=====================================================================================
//Close port member method
void CPort::Close(void)
{
	if((m_hIDComDev!=NULL)&&(m_Port!=0))
	{
		if(m_OverlappedRead.hEvent!=NULL) CloseHandle(m_OverlappedRead.hEvent);
		if(m_OverlappedWrite.hEvent!=NULL) CloseHandle(m_OverlappedWrite.hEvent);
		CloseHandle(m_hIDComDev);
		m_hIDComDev=NULL;
		m_RTSenable=false;
		m_DTRenable=false;	
		m_Port=0;			//20160401
		m_Ptr=0;							//20170726
		m_Size=0;							//20170726
		if(m_Buffer!=NULL) free(m_Buffer);	//20170726
	}
}

//=====================================================================================
//Check if open
//=====================================================================================
//Return the port status - 20160401
bool CPort::IsOpen(void)
{
	if(m_hIDComDev==NULL) return false;
	else return true;
}

//=====================================================================================
//Private primitive Write byte
//=====================================================================================
bool CPort::WriteByte(unsigned char ucByte)
{
	BOOL bWriteStat;
	DWORD dwBytesWritten;

	bWriteStat=WriteFile(m_hIDComDev,(LPSTR)&ucByte,1,&dwBytesWritten,&m_OverlappedWrite);
	if(!bWriteStat&&(GetLastError()==ERROR_IO_PENDING))
	{
	  	if(WaitForSingleObject(m_OverlappedWrite.hEvent,1000))dwBytesWritten=0;
	  	else
		{
			GetOverlappedResult(m_hIDComDev,&m_OverlappedWrite,&dwBytesWritten,false);
			m_OverlappedWrite.Offset+=dwBytesWritten;
		}
	}
	//Now handle the delay
	if(m_mSdelay) Sleep(m_mSdelay);

	return true;
}

//=====================================================================================
//Private primitive wait for write queue to empty
//=====================================================================================
void CPort::WaitQueue(void)
{
	COMSTAT ComStat;
	DWORD dwErrorFlags;
	
	while(1)	
	{
		ClearCommError(m_hIDComDev,&dwErrorFlags,&ComStat);	//Update status
		if(ComStat.cbOutQue==0) break;
	}
}

//=====================================================================================
//===================================[ Write ]=========================================
//=====================================================================================
//Write binary data member method
//Return the number of bytes written
int CPort::Write(const char* String,int Size)
{
	bool bWriteStat;
	DWORD dwBytesWritten;

	bWriteStat=WriteFile(m_hIDComDev,(LPSTR)&String[0],Size,&dwBytesWritten,&m_OverlappedWrite);
	if(!bWriteStat&&(GetLastError()==ERROR_IO_PENDING))
	{
	  	if(WaitForSingleObject(m_OverlappedWrite.hEvent,1000))dwBytesWritten=0;
	  	else
		{
			GetOverlappedResult(m_hIDComDev,&m_OverlappedWrite,&dwBytesWritten,false);
			m_OverlappedWrite.Offset+=dwBytesWritten;
		}
	}
	return (int)dwBytesWritten;
}

//=====================================================================================
//===================================[ Write ]=========================================
//=====================================================================================
//Write binary data member method overload with wait
int CPort::Write(const char* String,int Size, bool bWait)
{
	DWORD dwBytesWritten;
		
	dwBytesWritten=Write(String,Size);
	if(bWait)  WaitQueue();

	return((int)dwBytesWritten);
}

//=====================================================================================
//===================================[ Write ]=========================================
//=====================================================================================
//Write string member method overload
//For ASCIIZ string
//20160401 Now returns the number of bytes written
int CPort::Write(const char* czString)
{
	BOOL bWriteStat;
	DWORD dwBytesWritten;
	DWORD dwBytesToWrite=(DWORD)strlen(&czString[0]);

	bWriteStat=WriteFile(m_hIDComDev,(LPSTR)&czString[0],dwBytesToWrite,&dwBytesWritten,&m_OverlappedWrite);
	if(!bWriteStat&&(GetLastError()==ERROR_IO_PENDING))
	{
	  	if(WaitForSingleObject(m_OverlappedWrite.hEvent,1000))dwBytesWritten=0;
	  	else
		{
			GetOverlappedResult(m_hIDComDev,&m_OverlappedWrite,&dwBytesWritten,false);
			m_OverlappedWrite.Offset+=dwBytesWritten;
		}
	}
	return((unsigned int)dwBytesWritten);
}

//=====================================================================================
//===================================[ Write ]=========================================
//=====================================================================================
//Write string member method overload with wait
//Same as above, with wait
void CPort::Write(const char* czString, bool bwait)
{
	BOOL bWriteStat;
	DWORD dwBytesWritten;
	DWORD dwBytesToWrite=(DWORD)strlen(&czString[0]);

	bWriteStat=WriteFile(m_hIDComDev,(LPSTR)&czString[0],dwBytesToWrite,&dwBytesWritten,&m_OverlappedWrite);
	if(!bWriteStat&&(GetLastError()==ERROR_IO_PENDING))
	{
	  	if(WaitForSingleObject(m_OverlappedWrite.hEvent,1000))dwBytesWritten=0;
	  	else
		{
			GetOverlappedResult(m_hIDComDev,&m_OverlappedWrite,&dwBytesWritten,false);
			m_OverlappedWrite.Offset+=dwBytesWritten;
		}
	}
	//Now handle the delay
	if(m_mSdelay) Sleep(m_mSdelay);
	if(bwait)  WaitQueue();
}

//=====================================================================================
//===================================[ Peek ]==========================================
//=====================================================================================
//Check for data member method
//Return the number of bytes in the queue
int CPort::Peek(void)
{
	DWORD dwErrorFlags;
	COMSTAT ComStat;

	if(m_hIDComDev==NULL) return 0;
	ClearCommError(m_hIDComDev,&dwErrorFlags,&ComStat);	//Update status
	//Get count - 20161030
	m_InQueue=(int)ComStat.cbInQue;
	return m_InQueue;
}

//=====================================================================================
//===================================[ DataCheck ]=====================================
//=====================================================================================
//Wait for the flag to be seen in position zero, and if the minimum number or char are seen, then return the buffer content.
bool CPort::DataCheck(int Min,char* Buff)
{
	unsigned char c;

	while(Peek())
	{
		c=ReadByte();				//Get a byte
		if(m_Buffer!=NULL) { if(m_Ptr<m_Size) m_Buffer[m_Ptr++]=c; }	//20170726
		if((m_DataFlag1)&&(m_CashePtr>=Min))
		{
			//If the data was there, return all but the flag charactor
			m_CasheBuff[8]=0;
			if(Buff!=NULL) memcpy(&Buff[0],&m_CasheBuff[1],8);
			return true;		//Flag charactor found
		}
	}
	return false;	//Flag charactor not found
}

//=====================================================================================
//===================================[ DataCheck ]=====================================
//=====================================================================================
//Wait for the flag to be seen in position zero, then return the 8 char
bool CPort::DataCheck(char* Buff)
{
	unsigned char c;
	
	while(Peek())
	{
		c=ReadByte();				//Get a byte
		if(m_Buffer!=NULL) { if(m_Ptr<m_Size) m_Buffer[m_Ptr++]=c; }	//20170726
		if((m_DataFlag1))
		{
			//If the data was there, return all but the flag charactor
			m_CasheBuff[8]=0;
			if(Buff!=NULL) memcpy(&Buff[0],&m_CasheBuff[1],8);
			return true;		//Flag charactor found
		}
	}
	return false;	//Flag charactor not found
}

//=====================================================================================
//===================================[ DataCheck ]=====================================
//=====================================================================================
//Return true if the flag is seen, and false if not
bool CPort::DataCheck(void)
{
	unsigned char c;

	while(Peek())
	{
		c=ReadByte();				//Get a byte
		if(m_Buffer!=NULL) { if(m_Ptr<m_Size) m_Buffer[m_Ptr++]=c; }	//20170726
		if((m_DataFlag2)) return true;		//Flag charactor found
	}
	return false;							//Flag charactor not found
}


//=====================================================================================
//===================================[ DataCheck ]=====================================
//=====================================================================================
//Wait for the flag to be seen in position zero, and the specified string
bool CPort::DataSeek(char* String)
{
	int len=(int)strlen(&String[0]);
	unsigned char c;

	while(Peek())
	{
		c=ReadByte();				//Get a byte
		if(m_Buffer!=NULL) { if(m_Ptr<m_Size) m_Buffer[m_Ptr++]=c; }	//20170726
		if((m_DataFlag1))
		{
			if(memcmp(&String[0],&m_CasheBuff[1],len)==0) return true;		//Flag charactor found
		}
	}
	return false;	//Flag charactor not found
}

//=====================================================================================
//===================================[ Check with wildcard ]===========================
//=====================================================================================
//Wait for the flag to be seen in position zero, and the specified string with wildcards
bool CPort::WildcardSeek(char* String)
{
	int len=(int)strlen(&String[0]);
	int i,j=1;
	char c;
	while(Peek())
	{
		c=ReadByte();				//Get a byte
		if(m_Buffer!=NULL) { if(m_Ptr<m_Size) m_Buffer[m_Ptr++]=c; }	//20170726
		if((m_DataFlag1))
		{
			for(i=0;i<len;i++)
			{
				c=m_CasheBuff[j++];
				if(c=='?') continue;
				if(c!=String[i]) return false;
			}
		}
		return true;
	}
	return false;	//Flag charactor not found
}


//=====================================================================================
//===================================[ DataCheck ]=====================================
//=====================================================================================
//Return data
bool CPort::DataGet(char* Buff, int Len)
{
	if((m_DataFlag1))							//If present
	{
		memcpy(&Buff[0],&m_CasheBuff[1],Len);	//Get 
		Buff[Len]=0;							//Truncate
		return true;
	}

	return false;	//Flag charactor not found
}


//=====================================================================================
//===================================[ Flush ]=========================================
//=====================================================================================
//Flush data
//Return the number of bytes flushed
int CPort::Flush(void)
{
	DWORD dwErrorFlags;
	COMSTAT ComStat;
	int cnt=0;
	char buffer[12];

	if(m_hIDComDev==NULL) return 0;

	while(1) 	//Wait until no more charactors or available
	{
		ClearCommError(m_hIDComDev,&dwErrorFlags,&ComStat);	//Update status
		if(ComStat.cbInQue)	cnt+=Read(&buffer[0],1); 
		else break;
	}
	
	//Flush working seek buffer  - 20161030
	m_InQueue=0;
	memset(&m_CasheBuff[0],0,9);		
	m_CashePtr=0;
	m_DataFlag1=false;
	m_DataFlag2=false;
	m_FlagChar=0x10;
	m_Ptr=0;							//20170726
	if(m_Buffer!=NULL) free(m_Buffer);	//20170726

	return cnt;
}

//=====================================================================================
//===================================[ New Data Seek ]=================================
//=====================================================================================
//Flush data, and start seek with no char - 20160913
void CPort::NewSeek(void)
{
	//Flush working seek buffer - 20160913
	memset(&m_CasheBuff[0],0,9);		
	m_CashePtr=0;
	m_DataFlag1=false;
	m_DataFlag2=false;
	m_FlagChar=0x10;					//Default
	m_Ptr=0;							//20170726
	if(m_Buffer!=NULL) free(m_Buffer);	//20170726
}

//=====================================================================================
//===================================[ New Data Seek ]=================================
//=====================================================================================
//Flush data, and start seek with char given - 20160913
void CPort::NewSeek(char Flag)
{
	//Flush working seek buffer - 20160913
	memset(&m_CasheBuff[0],0,9);		
	m_CashePtr=0;
	m_DataFlag1=false;
	m_DataFlag2=false;
	m_FlagChar=Flag;
	m_Ptr=0;							//20170726
	if(m_Buffer!=NULL) free(m_Buffer);	//20170726
}

//=====================================================================================
//===================================[ New Data Seek ]=================================
//=====================================================================================
//Flush data, and start seek with char given, and set up buffer - 20170726
bool CPort::NewSeek(char Flag, int Size)
{
	if(m_Buffer!=NULL) free(m_Buffer);
	m_Buffer=NULL;
	memset(&m_CasheBuff[0],0,9);		
	m_CashePtr=0;
	m_DataFlag1=false;
	m_DataFlag2=false;
	m_FlagChar=Flag;
	m_Size=Size;
	m_Ptr=0;	//Reset pointer into buffer
	m_Buffer=(char*)malloc(Size);
	if(m_Buffer!=NULL) return true;	//If buffer set, then return success
	
	return false;
}

//=====================================================================================
//===================================[ Wait ]==========================================
//=====================================================================================
//Wait for first byte member method
bool CPort::Wait(unsigned int unTimeout)
{
	DWORD dwErrorFlags;
	COMSTAT ComStat;
	unsigned int cnt;

	if(m_hIDComDev==NULL) return false;

	if(unTimeout<T_RESOLUTION) unTimeout=T_RESOLUTION;
	cnt=unTimeout/T_RESOLUTION;

	while(cnt--)
	{	
		Sleep(T_RESOLUTION);	
		ClearCommError(m_hIDComDev,&dwErrorFlags,&ComStat);	//Update status
		if(ComStat.cbInQue) return true;
	}
	return false;
}

//=====================================================================================
//===================================[ Wait ]==========================================
//=====================================================================================
//Wait for data member method overload
//Return false if timeout or port closed
//Return true if data rx
bool CPort::Wait(unsigned int unTimeout, int nBytes)
{
	DWORD dwErrorFlags;
	COMSTAT ComStat;
	unsigned int cnt;

	if(m_hIDComDev==NULL) return false;

	if(unTimeout<T_RESOLUTION) unTimeout=T_RESOLUTION;
	cnt=unTimeout/T_RESOLUTION;

	while(cnt--)
	{	
		Sleep(T_RESOLUTION);	
		ClearCommError(m_hIDComDev,&dwErrorFlags,&ComStat);	//Update status
		if(ComStat.cbInQue==nBytes) return true;
	}
	return false;
}

//=====================================================================================
//===================================[ Read ]==========================================
//=====================================================================================
//Read binary data member method
//Return number of bytes
//No terminating zero
int CPort::Read(void* buffer,int limit)
{
	BOOL bReadStatus;
	DWORD dwBytesRead,dwErrorFlags;
	COMSTAT ComStat;

	if(m_hIDComDev==NULL) return 0;

	ClearCommError(m_hIDComDev,&dwErrorFlags,&ComStat);	//Update status
	if(!ComStat.cbInQue) return 0;	

	dwBytesRead=(DWORD)ComStat.cbInQue;
	if(dwBytesRead>(DWORD)limit) dwBytesRead=(DWORD)limit;
	bReadStatus=ReadFile(m_hIDComDev,buffer,dwBytesRead,&dwBytesRead,&m_OverlappedRead);
	if(!bReadStatus)
	{
		if(GetLastError()==ERROR_IO_PENDING)
		{
			WaitForSingleObject(m_OverlappedRead.hEvent,2000);
			return((int)dwBytesRead);
		}
		return 0;
	}
	return((int)dwBytesRead);
}

//=====================================================================================
//=================================[ ReadByte ]========================================
//=====================================================================================
//Read byte member method
char CPort::ReadByte(void)
{
	DWORD dwErrorFlags;
	COMSTAT ComStat;

	while(1) 	
	{
		ClearCommError(m_hIDComDev,&dwErrorFlags,&ComStat);	//Update status
		if(ComStat.cbInQue)	
		{
			Read(&m_Char,1);				//Read when avialable
			break;
		}
	}
	
	//Save in cashe - 20161030
	if(m_CashePtr!=8) { m_CasheBuff[m_CashePtr++]=m_Char; }
	else
	{
		m_CasheBuff[0]=m_CasheBuff[1];
		m_CasheBuff[1]=m_CasheBuff[2];
		m_CasheBuff[2]=m_CasheBuff[3];
		m_CasheBuff[3]=m_CasheBuff[4];
		m_CasheBuff[4]=m_CasheBuff[5];
		m_CasheBuff[5]=m_CasheBuff[6];
		m_CasheBuff[6]=m_CasheBuff[7];
		m_CasheBuff[7]=m_Char;
	}
	//Flag if needed
	if(m_CasheBuff[0]==m_FlagChar) m_DataFlag1=true;	//If seen if position zero
	if(m_Char==m_FlagChar) m_DataFlag2=true;			//If seen at all

	return m_Char;
}

//=====================================================================================
//=================================[ ReadLine ]========================================
//=====================================================================================
//Read line member method
//Reads in a zero terminated string as TTY
//Carrage return tuncates input.  Backspace is handled
//Buffer must be limit+1
int CPort::ReadLine(char* cBuffer,int Limit)
{
	char c;
	int count=0;

	if(m_hIDComDev==NULL) return 0;

	while(1)	//Wait loop for charactor input
	{
		while(1)  //Wait for a printable, CR or BKSP
		{
			if(Peek())	//20160913 
			{
				c=ReadByte();
				if((c>=' ')&&(c<='z')&&(count<Limit)) break;
				if((c==0x08)&&(count>0)) break;
				if(c==0x0D) break;
			}
		}
		if(c==0x0D) //Carrage return
		{
			cBuffer[count]=0;
			break;
		}
		else
		{
			if(c==0x08) count--; //Backspace
			else cBuffer[count++]=c;
		}
	}
	return count;
}

//=====================================================================================
//===================================[ Read Data from Buffer ]=========================
//=====================================================================================
//Get data from buffer - 20170726
void CPort::GetBuffer(char* Destination)
{
	if(m_Buffer!=NULL) 
	{
		//Grab data
		if(m_Ptr) memcpy(&Destination[0],&m_Buffer[0],m_Ptr);
		else Destination[0]=0;
		//Release
		free(m_Buffer);
		m_Buffer=NULL;
		m_Ptr=0;
		
	}
}

#ifdef WindowsXP	//These two methods work on XP or later
//=====================================================================================
//===================================[ Set RTS() ]=====================================
//=====================================================================================
//Drive RTS - 20131210 (Verified)
bool CPort::SetRTS(void)
{
	DWORD dwFunc=SETRTS;

	if(m_RTSenable)
	{
		if(m_hIDComDev==NULL) return false;
		return (bool)EscapeCommFunction(m_hIDComDev,dwFunc);
	}
	else return false;
}

//=====================================================================================
//===================================[ Clear RTS() ]===================================
//=====================================================================================
//Clear RTS - 20131210 (Verified)
bool CPort::ClearRTS(void)
{
	DWORD dwFunc=CLRRTS;

	if(m_RTSenable)
	{
		if(m_hIDComDev==NULL) return false;
		return (bool)EscapeCommFunction(m_hIDComDev,dwFunc);
	}
	else return false;
}

//=====================================================================================
//===================================[ Set DTR() ]=====================================
//=====================================================================================
//Drive DTR - 20131210 (Verified)
bool CPort::SetDTR(void)
{
	DWORD dwFunc=SETDTR;

	if(m_DTRenable)
	{
		if(m_hIDComDev==NULL) return false;
		return (bool)EscapeCommFunction(m_hIDComDev,dwFunc);
	}
	else return false;
}

//=====================================================================================
//===================================[ Clear DTR() ]===================================
//=====================================================================================
//Clear DTR - 20131210 (Verified)
bool CPort::ClearDTR(void)
{
	DWORD dwFunc=CLRDTR;

	if(m_DTRenable)
	{
		if(m_hIDComDev==NULL) return false;
		return (bool)EscapeCommFunction(m_hIDComDev,dwFunc);
	}
	else return false;
}

//=====================================================================================
//===================================[ SetHanshake()]==================================
//=====================================================================================
//Drive RTS, DTR, etc (Verified)
//Enumerated methods = SETXON,SETXOFF,SETRTS,CLRRTS,SETDTR,CLRDTR,SETBREAK,CLRBREAK from WinBase.h
bool CPort::SetHandshake(DWORD dwFunc)
{
#pragma warning(disable:4800)
	if(m_DTRenable||m_RTSenable)
	{
		if(m_hIDComDev==NULL) return false;
		return (bool)EscapeCommFunction(m_hIDComDev,dwFunc);
	}
	else return false;
}

//=====================================================================================
//===================================[ Get Modem Status ]==============================
//=====================================================================================
//Read CTS, DSR, RING, RLSD - Not working as of 21031210
//Returns -1 on fail
//Returns MS_CTS_ON(0x0010), MS_DSR_ON(0x0020), MS_RING_ON(0x0040), MS_RLSD_ON(0x0080) from WinBase.h
int CPort::GetModemStat(void)
{
	DWORD mdmStat=0;

	if(m_hIDComDev==NULL) return -1;
	if(GetCommModemStatus(m_hIDComDev,&mdmStat)==0) return -1;
	
	return (int)mdmStat;  //&0x00F0;	
}

//=====================================================================================
//===================================[ Set Delay ]=====================================
//=====================================================================================
//Set charactor delay
//0-500 mS
void CPort::SetDelay(int NewDelay)
{
	if(NewDelay<=0) NewDelay=0;			//Min 0mS
	if(NewDelay>500) NewDelay=500;		//Max 500mS
	m_mSdelay=NewDelay;
}

#endif
