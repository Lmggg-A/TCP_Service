#include "TcpService.h"
#include <iostream>
#include <stdio.h>

C_TCP_SERVICE::C_TCP_SERVICE(int l_num, int h_num, int index)
{
	// socket��ʼ��
	socketInit(l_num, h_num);

	// ������Ӧ��TCP�� �׽��� �����
	if (index == E_TCP::SERVER)
	{
		m_socket = socket(AF_INET, SOCK_STREAM, 0);
		m_addrSrv.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
		m_addrSrv.sin_family = AF_INET;
		m_addrSrv.sin_port = htons(6000);
		// ���׽���
		bind(m_socket, (SOCKADDR*)&m_addrSrv, sizeof(SOCKADDR));
	}
	// �ͻ���
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

// Socket ��ʼ��
void C_TCP_SERVICE::socketInit(int l_num, int h_num)
{
	WORD wVersionRequested;
	int err;

	// ָ��ʹ��socket�汾Ϊ 1.1
	wVersionRequested = MAKEWORD(l_num, h_num);

	// �󶨶�Ӧ�汾��Socket�⣬��Ӧ��Ϣ�洢��wsaData��
	err = WSAStartup(wVersionRequested, &m_wsaData);
	if (err != 0)
	{
		printf("Socket Lib Bind error!\n");
		return;
	}

	// �жϰ汾��Ϣ�Ƿ��Ӧ
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

	// �Ƿ����ӳɹ�
	if (is_connect)
		return true;
	else
		return false;
}

void C_TCP_SERVICE::s_endServer()
{
	if (m_connect == INVALID_SOCKET)
		return;
	// �ر��׽���
	closesocket(m_connect);
}

bool C_TCP_SERVICE::s_waitConnect()
{
	int res, addr_len;
	// ���׽�����Ϊ����ģʽ��׼�����տͻ�����
	res = listen(m_socket, _MAX_CONNECT_NUM);
	if (res != 0)
		return false;
	addr_len = sizeof(SOCKADDR);
	printf("�ȴ��ͻ�������......\n");
	// �ȴ��ͻ�������
	while (true)
	{
		m_connect = accept(m_socket, (SOCKADDR*)&m_addrClient, &addr_len);
		if (m_connect != INVALID_SOCKET)
		{
			printf("�ͻ��ˡ�%s��������!\n", inet_ntoa(m_addrClient.sin_addr));
			break;
		}
	}

	return true;
}

bool C_TCP_SERVICE::c_startClient()
{
	bool is_connect = false, is_recv = true;
	is_connect = c_connect();

	// �Ƿ����ӳɹ�
	if (is_connect)
		return true;
	else
		return false;

	//// �Ƿ����ӳɹ�
	//if (is_connect)
	//{
	//	char sendBuf[_MAX_SEND_LEN];
	//	char recvBuf[_MAX_RECV_LEN];
	//	while (true)
	//	{
	//		memset(sendBuf, 0, _MAX_SEND_LEN);
	//		sprintf_s(sendBuf, "I'm %s ��I come!", inet_ntoa(m_addrSrv.sin_addr));
	//		// ��������
	//		send(m_socket, sendBuf, strlen(sendBuf) + 1, 0);


	//		memset(recvBuf, 0, _MAX_RECV_LEN);
	//		// ��������
	//		is_recv = recvAllMsg(m_socket, recvBuf, 50);
	//		// ��ӡ���յ�����
	//		if (is_recv)
	//			printf("%s\n", recvBuf);
	//	}
	//}
}

void C_TCP_SERVICE::c_endClient()
{
	if (m_socket == INVALID_SOCKET)
		return;
	// �ر��׽���
	closesocket(m_socket);
}

bool C_TCP_SERVICE::c_connect()
{
	int res;
	// �������������������
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
		// �ɹ������ֽ��������ӽ���ʱ���� 0 ������ʧ��ʱ���� -1
		res = send(t_socket, buf, size, 0);
		if (res == SOCKET_ERROR)
			return false;
		size = size - res;	// ѭ�����ͣ�ֱ���������
		buf = buf + res;	// �ѷ���ƫ����
	}
	return true;
	// �����ǽ�����copy�� TCPЭ��ķ��ͻ�������ֻ����д�룬Э�鸺����
}

bool C_TCP_SERVICE::recvAllMsg(const SOCKET& t_socket, char* buf, int len)
{
	int count_Recv0 = 0, count_Recv = 0;
	int size = len;
	char* recvBuf = buf;
	while (size > 0)
	{
		int res;
		// �ɹ������ֽ��������ӽ���ʱ���� 0 ������ʧ��ʱ���� -1
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
		size -= res;		// ѭ�����ͣ�ֱ���������
		recvBuf += res;		// �ѷ���ƫ����
		count_Recv += res;
	}

	buf = recvBuf - count_Recv;
	count_Recv = 0;
	return true;
	// �����ǽ�����copy�� buf��������ֻ���������Э�鸺�����
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
