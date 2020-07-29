#ifndef _TCPNET_H
#define _TCPNET_H

#include <winsock2.h>
#include <process.h>
#include <map>
#include <list>
#include<Mswsock.h>
using namespace std;
#pragma comment(lib,"ws2_32.lib")

#pragma comment(lib,"Mswsock.lib")
#define MAXNUM   64
#define MAXSIZE 1024
enum NetType{NT_ACEPPT,NT_READ};
struct MySocketEx
{
	OVERLAPPED m_oI;//�¼� �ص�io
	SOCKET m_SockWaiter;//�׽���
	char szbuf[MAXSIZE];//������
	NetType m_Type;//��� �Ƿ���ʲô���͵�Socket
};
class TCPNet 
{
public:
	TCPNet();
	virtual ~TCPNet();
public:
	bool InitNetWork();
	void UnInitNetWork();
	              //���� udp --IP,PORT TCP -SOCKET
	bool SendData(char *szbuf,int nLen,unsigned sockIp,unsigned short nport=0);
	void RecvData();
public:
	static  unsigned  __stdcall ThreadProc( void * );
	static  unsigned  __stdcall ThreadRecv( void * );
	bool PostAccept();
	bool PostRecv(MySocketEx *);
private:
	SOCKET m_sockListen;
	HANDLE m_hThread;
	bool   m_bFlagQuit;
	map< unsigned ,SOCKET> m_mapThreadIDToSocket;
	list<HANDLE>  m_lstThread;
	SOCKET m_arySocketWaiter[MAXNUM];
	int   m_nWaiterNum;
	fd_set  m_fdsets;
	//��ɶ˿�
	HANDLE m_hIocp;
	list<MySocketEx *> m_lstSocket;
};


#endif