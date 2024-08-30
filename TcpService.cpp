#include "TcpService.h"
#include <iostream>
#include <stdio.h>

C_TCP_SERVICE::C_TCP_SERVICE(int l_num, int h_num, int index)
{
	// socket初始化
	socketInit(l_num, h_num);

	// 创建对应的TCP端 套接字 服务端
	if (index == E_TCP::SERVER)
	{
		m_socket = socket(AF_INET, SOCK_STREAM, 0);
		m_addrSrv.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
		m_addrSrv.sin_family = AF_INET;
		m_addrSrv.sin_port = htons(6000);
		// 绑定套接字
		bind(m_socket, (SOCKADDR*)&m_addrSrv, sizeof(SOCKADDR));
	}
	// 客户端
	else
	{
		m_socket = socket(AF_INET, SOCK_STREAM, 0);
		m_addrSrv.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
		m_addrSrv.sin_family = AF_INET;
		m_addrSrv.sin_port = htons(6000);
	}
}

C_TCP_SERVICE::~C_TCP_SERVICE()
{
	WSACleanup();
}

// Socket 初始化
void C_TCP_SERVICE::socketInit(int l_num, int h_num)
{
	WORD wVersionRequested;
	int err;

	// 指定使用socket版本为 1.1
	wVersionRequested = MAKEWORD(l_num, h_num);

	// 绑定对应版本的Socket库，相应信息存储到wsaData中
	err = WSAStartup(wVersionRequested, &m_wsaData);
	if (err != 0)
	{
		printf("Socket Lib Bind error!\n");
		return;
	}

	// 判断版本信息是否对应
	if (LOBYTE(m_wsaData.wVersion) != l_num || HIBYTE(m_wsaData.wVersion) != h_num)
	{
		WSACleanup();
		printf("Version match error!\n");
		return;
	}
	printf("Bind Socket Lib Success!\n");
	return;
}

bool C_TCP_SERVICE::s_startServer()
{
	bool is_connect = false, is_recv = true;
	is_connect = s_waitConnect();

	// 是否连接成功
	if (is_connect)
		return true;
	else
		return false;
}

void C_TCP_SERVICE::s_endServer()
{
	if (m_connect == INVALID_SOCKET)
		return;
	// 关闭套接字
	closesocket(m_connect);
}

bool C_TCP_SERVICE::s_waitConnect()
{
	int res, addr_len;
	// 将套接字设为监听模式，准备接收客户请求
	res = listen(m_socket, _MAX_CONNECT_NUM);
	if (res != 0)
		return false;
	addr_len = sizeof(SOCKADDR);
	printf("等待客户端连接......\n");
	// 等待客户请求到来
	while (true)
	{
		m_connect = accept(m_socket, (SOCKADDR*)&m_addrClient, &addr_len);
		if (m_connect != INVALID_SOCKET)
		{
			printf("客户端【%s】已连接!\n", inet_ntoa(m_addrClient.sin_addr));
			break;
		}
	}

	return true;
}

bool C_TCP_SERVICE::c_startClient()
{
	bool is_connect = false, is_recv = true;
	is_connect = c_connect();

	// 是否连接成功
	if (is_connect)
		return true;
	else
		return false;

	//// 是否连接成功
	//if (is_connect)
	//{
	//	char sendBuf[_MAX_SEND_LEN];
	//	char recvBuf[_MAX_RECV_LEN];
	//	while (true)
	//	{
	//		memset(sendBuf, 0, _MAX_SEND_LEN);
	//		sprintf_s(sendBuf, "I'm %s ，I come!", inet_ntoa(m_addrSrv.sin_addr));
	//		// 发送数据
	//		send(m_socket, sendBuf, strlen(sendBuf) + 1, 0);


	//		memset(recvBuf, 0, _MAX_RECV_LEN);
	//		// 接收数据
	//		is_recv = recvAllMsg(m_socket, recvBuf, 50);
	//		// 打印接收的数据
	//		if (is_recv)
	//			printf("%s\n", recvBuf);
	//	}
	//}
}

void C_TCP_SERVICE::c_endClient()
{
	if (m_socket == INVALID_SOCKET)
		return;
	// 关闭套接字
	closesocket(m_socket);
}

bool C_TCP_SERVICE::c_connect()
{
	int res;
	// 向服务器发出连接请求
	res = connect(m_socket, (SOCKADDR*)&m_addrSrv, sizeof(SOCKADDR));
	if (res == SOCKET_ERROR)
		return false;

	return true;
}

bool C_TCP_SERVICE::sendAllMsg(const SOCKET& t_socket, const char* buf, int len)
{
	int size = len;
	while (size > 0)
	{
		int res;
		// 成功返回字节数，连接结束时返回 0 ，连接失败时返回 -1
		res = send(t_socket, buf, size, 0);
		if (res == SOCKET_ERROR)
			return false;
		size = size - res;	// 循环发送，直至发送完毕
		buf = buf + res;	// 已发送偏移量
	}
	return true;
	// 本质是将数据copy到 TCP协议的发送缓冲区，只负责写入，协议负责发送
}

bool C_TCP_SERVICE::recvAllMsg(const SOCKET& t_socket, char* buf, int len)
{
	int count_Recv0 = 0, count_Recv = 0;
	int size = len;
	char* recvBuf = buf;
	while (size > 0)
	{
		int res;
		// 成功返回字节数，连接结束时返回 0 ，连接失败时返回 -1
		res = recv(t_socket, recvBuf, size, 0);
		if (res == SOCKET_ERROR)
			return false;
		if (res == 0)
		{
			count_Recv0++;
			if (count_Recv0 >= 3)
			{
				count_Recv0 = 0;
				break;
			}
		}
		size -= res;		// 循环发送，直至发送完毕
		recvBuf += res;		// 已发送偏移量
		count_Recv += res;
	}

	buf = recvBuf - count_Recv;
	count_Recv = 0;
	return true;
	// 本质是将数据copy到 buf缓冲区，只负责读出，协议负责接收
}

SOCKET C_TCP_SERVICE::getSocket()
{
	return m_socket;
}

SOCKET C_TCP_SERVICE::getConSocket()
{
	return m_connect;
}

SOCKADDR_IN C_TCP_SERVICE::getSrvADDR()
{
	return m_addrSrv;
}

SOCKADDR_IN C_TCP_SERVICE::getCliADDR()
{
	return m_addrClient;
}

WSADATA C_TCP_SERVICE::getWSAData()
{
	return m_wsaData;
}
