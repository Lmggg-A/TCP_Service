#pragma once
#include <WinSock2.h>

constexpr auto _MAX_CONNECT_NUM = 5;	// �ȴ����ӵ������г���
constexpr auto _MAX_SEND_LEN = 4096;	// ����ͳ���4096�ֽ�
constexpr auto _MAX_RECV_LEN = 4096;	// �����ճ���4096�ֽ�

enum E_TCP
{
	SERVER = 1,
	CLIENT = 2,
};

class C_TCP_SERVICE
{
public:
	C_TCP_SERVICE(int l_num, int h_num, int index);
	~C_TCP_SERVICE();

	bool s_startServer();
	void s_endServer();
	
	bool c_startClient();
	void c_endClient();
	
	bool sendAllMsg(const SOCKET &t_socket, const char* buf, int len);
	bool recvAllMsg(const SOCKET &t_socket, char* buf, int len);

	SOCKET getSocket();
	SOCKET getConSocket();
	SOCKADDR_IN getSrvADDR();
	SOCKADDR_IN getCliADDR();
	WSADATA getWSAData();
private:
	void socketInit(int l_num, int h_num);
	bool s_waitConnect();
	bool c_connect();

private:
	WSADATA m_wsaData;
	SOCKET m_socket;
	SOCKADDR_IN m_addrSrv;
	SOCKET m_connect;
	SOCKADDR_IN m_addrClient;
};

