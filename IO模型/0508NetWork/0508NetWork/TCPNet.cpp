#include "stdafx.h"
#include "TCPNet.h"

TCPNet::TCPNet()
{
	m_sockListen = NULL;
	m_hThread = NULL;
	m_bFlagQuit = true;
	ZeroMemory(m_arySocketWaiter,sizeof(m_arySocketWaiter));
	m_nWaiterNum = 0;
	FD_ZERO(&m_fdsets);

}

TCPNet::~TCPNet()
{}


bool TCPNet::InitNetWork()
{
	//1.ѡ������  �в� ��� ���� 
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	/* Use the MAKEWORD(lowbyte, highbyte) macro declared in Windef.h */
	wVersionRequested = MAKEWORD(2, 2);

	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0) {
		/* Tell the user that we could not find a usable */
		/* Winsock DLL.                                  */
		// printf("WSAStartup failed with error: %d\n", err);
		return false;
	}

	/* Confirm that the WinSock DLL supports 2.2.*/
	/* Note that if the DLL supports versions greater    */
	/* than 2.2 in addition to 2.2, it will still return */
	/* 2.2 in wVersion since that is the version we      */
	/* requested.                                        */

	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
		/* Tell the user that we could not find a usable */
		/* WinSock DLL.                                  */

		UnInitNetWork();
		return false;
	}

	//2.�͵곤
	m_sockListen = WSASocket(AF_INET,SOCK_STREAM,IPPROTO_TCP,0,0,WSA_FLAG_OVERLAPPED);//֧���첽�¼���Socket
	if(INVALID_SOCKET  == m_sockListen)
	{
		UnInitNetWork();
		return false;
	}
	//3.�ҵط�
	sockaddr_in  addrServer;
	addrServer.sin_family =AF_INET;
	addrServer.sin_addr.S_un.S_addr = 0; //������������
	addrServer.sin_port = htons(1234);
	if( SOCKET_ERROR == bind(m_sockListen,(const sockaddr*)&addrServer,sizeof(addrServer)))
	{
		UnInitNetWork();
		return false;
	}
	//4.����
	SYSTEM_INFO si;
	GetSystemInfo(&si);//��ȡcpu
	if(SOCKET_ERROR  == listen(m_sockListen,si.dwNumberOfProcessors*2))
	{
		UnInitNetWork();
		return false;
	}
	//����IOCP
	m_hIocp=CreateIoCompletionPort(INVALID_HANDLE_VALUE,0,0,0);
	//������Socket ����IOCP
	CreateIoCompletionPort((HANDLE)m_sockListen,m_hIocp,m_sockListen,0);//�����Socket   ��ɶ˿ھ�� ��ɼ���WPARAM LPARAM��  ��ǰ�̸߳���
	//����������N��Waiter
	for(int i=0;i<si.dwNumberOfProcessors*2;i++)
	{
		if(!PostAccept())
			continue;
	}
	//�����̳߳�
	HANDLE mThread;
	for(int i=0;i<si.dwNumberOfProcessors*2;i++)
	{
		mThread=(HANDLE)_beginthreadex(0,0,&ThreadProc,this,0,0);
		if(mThread)
			m_lstThread.push_back(mThread);
	}
	return true;
}
bool TCPNet::PostAccept()
{
	MySocketEx *p=new MySocketEx;
	p->m_Type=NT_ACEPPT;
	p->m_SockWaiter=WSASocket(AF_INET,SOCK_STREAM,IPPROTO_TCP,0,0,WSA_FLAG_OVERLAPPED);
	p->m_oI.hEvent=WSACreateEvent();
	ZeroMemory(p->szbuf,sizeof(p->szbuf));
	DWORD lpdwBytesReceived;
	if(!AcceptEx(m_sockListen,p->m_SockWaiter,p->szbuf,0,sizeof(sockaddr_in)+16,sizeof(sockaddr_in)+16,&lpdwBytesReceived,&p->m_oI))
	{
		if(WSAGetLastError()!=ERROR_IO_PENDING) 
		{
			closesocket(p->m_SockWaiter);
			WSACloseEvent(p->m_oI.hEvent);
			delete p;
			p=NULL;
			return false;
		}
		m_lstSocket.push_back(p);
	}
	return true;
}
bool TCPNet::PostRecv(MySocketEx * pSockex)
{
	
	pSockex->m_Type=NT_READ;
	WSABUF wb;
	wb.buf=pSockex->szbuf;
	wb.len=sizeof(pSockex->szbuf);
	DWORD dbNumberOfBytesRecvd;//�������ݸ��� ���д�ڽṹ����
	DWORD dFlag=FALSE;//�Ƿ��޸ĺ�����Ϊ ���޸�
	if(WSARecv(pSockex->m_SockWaiter,&wb,1,&dbNumberOfBytesRecvd,&dFlag,&pSockex->m_oI,0))
	{
	  if(WSAGetLastError()!=WSA_IO_PENDING)
	  {
	  return false;
	  }
	
	}

return true;
}

unsigned  __stdcall TCPNet::ThreadProc( void * lpvoid)
{
	TCPNet *pthis = (TCPNet*)lpvoid;

	SOCKET socketWaiter;
	DWORD dyNumberOfBytes;
	SOCKET SockWaiter;
	MySocketEx *pSockex;
	DWORD bFlag;
	while(pthis->m_bFlagQuit)
	{
		//�۲�˿���Ϣ
		bFlag=GetQueuedCompletionStatus(pthis->m_hIocp,&dyNumberOfBytes,(PULONG_PTR)&SockWaiter,(LPOVERLAPPED *)&pSockex,INFINITE);
		if(!bFlag)
		{
			auto ite=pthis->m_lstSocket.begin();
			while(ite!=pthis->m_lstSocket.end())
			{
				if((*ite)==pSockex)
				{
					closesocket(pSockex->m_SockWaiter);
					WSACloseEvent(pSockex->m_oI.hEvent);
					delete pSockex;
					pSockex=NULL;
					pthis->m_lstSocket.erase(ite);
					break;
				}
			    ite++;
			}
			closesocket(SockWaiter);
			continue;
		}
		//����Socket �� pSockex
		if(SockWaiter&&pSockex)
		{
			switch (pSockex->m_Type)
			{
			case NT_ACEPPT:
				{
					pthis->PostAccept();
					pthis->PostRecv(pSockex);//WSARecv()
					CreateIoCompletionPort((HANDLE)pSockex->m_SockWaiter,pthis->m_hIocp,pSockex->m_SockWaiter,0);
				}
				break;
			case NT_READ:
				{
					pSockex->szbuf;
					pthis->PostRecv(pSockex);
				}
				break;
			default:
				break;
			}
		}
	}

	return 0;
}

unsigned  __stdcall TCPNet::ThreadRecv( void * lpvoid)
{
	TCPNet *pthis = (TCPNet*)lpvoid;
	pthis->RecvData();
	return 0;
}

void TCPNet::UnInitNetWork()
{
	m_bFlagQuit = false;
	auto ite = m_lstThread.begin();
	while(ite != m_lstThread.end())
	{
		if(WAIT_TIMEOUT == WaitForSingleObject(*ite,100))
			TerminateThread(*ite,-1);

		CloseHandle(*ite);
		*ite = NULL;
		ite++;
	}
	m_lstThread.clear();
	auto iteIoSock = m_lstSocket.begin();
	while(iteIoSock != m_lstSocket.end())
	{
		closesocket((*iteIoSock)->m_SockWaiter);
		WSACloseEvent((*iteIoSock)->m_oI.hEvent);
		delete (*iteIoSock);

		*iteIoSock = NULL;
		iteIoSock++;
	}
	m_lstSocket.clear();



	if(m_sockListen)
	{
		closesocket(m_sockListen);
		m_sockListen = NULL;
	}
	WSACleanup();
}

//���� udp --IP,PORT TCP -SOCKET
bool TCPNet::SendData(char *szbuf,int nLen,unsigned sockIp,unsigned short nport)
{
	if(!sockIp || !szbuf || nLen <=0)
		return false;
	//���Ͱ���С
	if(send(sockIp,(const char *)&nLen,sizeof(int),0) <=0)
		return false;
	//���Ͱ�����
	if(send(sockIp,szbuf,nLen,0) <=0)
		return false;

	return true;
}

void TCPNet::RecvData()
{
	//SOCKET sockWaiter = m_mapThreadIDToSocket[GetCurrentThreadId()];

	int nRelReadNum;
	char szbuf[1024] = {0};
	fd_set fdtemp;
	timeval tv;
	tv.tv_sec =0;
	tv.tv_usec = 100;
	while(m_bFlagQuit)
	{
		fdtemp = m_fdsets;

		//��fdtemp ����select ����
		select(0,&fdtemp,0,0,&tv);

		for(int i =0 ;i < m_nWaiterNum;i++)
		{

			if(FD_ISSET(m_arySocketWaiter[i],&fdtemp))
			{
				nRelReadNum = recv(m_arySocketWaiter[i],szbuf,sizeof(szbuf),0);
				if(nRelReadNum >0)
				{
					//����
				}
			}

		}

		Sleep(10);

	}
}