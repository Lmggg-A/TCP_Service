#include <iostream>
#include "TcpService.h"
int main()
{
	bool is_recv = true;
	bool is_connect = false;
	char sendBuf[_MAX_SEND_LEN];
	char recvBuf[_MAX_RECV_LEN];

    C_TCP_SERVICE* m_tcpSrv = new C_TCP_SERVICE(1, 1, E_TCP::SERVER);
	is_connect = m_tcpSrv->s_startServer();

	while (is_connect)
	{
		memset(sendBuf, 0, _MAX_SEND_LEN);
		sprintf_s(sendBuf, "Welcome %s to %s!", inet_ntoa(m_tcpSrv->getCliADDR().sin_addr), inet_ntoa(m_tcpSrv->getSrvADDR().sin_addr));
		// 发送数据
		m_tcpSrv->sendAllMsg(m_tcpSrv->getConSocket(), sendBuf, 50);

		memset(recvBuf, 0, _MAX_RECV_LEN);
		// 接收数据
		is_recv = m_tcpSrv->recvAllMsg(m_tcpSrv->getConSocket(), recvBuf, 50);
		// 打印接收的数据
		if (is_recv)
			printf("%s\n", recvBuf);
	}

    m_tcpSrv->s_endServer();

    return 0;
}

