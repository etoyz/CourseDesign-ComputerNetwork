//Don't forget to link libws2_32.a to your program as well
#include <winsock2.h>
#include <iostream>
#pragma comment (lib, "ws2_32.lib")

using namespace std;

SOCKET s; //Socket handle

bool ConnectToHost(int PortNo, char* IPAddress, char* Msg)
{
	//Start up Winsock��
	WSADATA wsadata;

	int error = WSAStartup(0x0202, &wsadata);

	//Did something happen?
	if (error)
		return false;

	//Fill out the information needed to initialize a socket��
	SOCKADDR_IN target;               //Socket address information

	target.sin_family = AF_INET;      // address family Internet
	target.sin_port = htons(PortNo); //Port to connect on
	target.sin_addr.s_addr = inet_addr(IPAddress); //Target IP

	s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); //Create socket
	if (s == INVALID_SOCKET)
	{
		return false; //Couldn't create the socket
	}

	//Try connecting...
	if (connect(s, (SOCKADDR*)&target, sizeof(target)) == SOCKET_ERROR)
	{
		return false;                 //Couldn't connect
	}
	else {
		send(s, Msg, strlen(Msg), 0);
		return true;                  //Success
	}
}

//CLOSECONNECTION �C shuts down the socket and closes any connection on it
void CloseConnection()
{
	//Close the socket if it exists
	if (s)
		closesocket(s);

	WSACleanup();                     //Clean up Winsock
}

/*
	head:
	[1][3][3][2][[1][3][1][1]][1][1]
	����״̬�������ˡ�ֻ�ϴ�
	�¶ȴ��������ɼ������¶���Ϣ��ֻ�ϴ�
	ʪ�ȴ��������ɼ�����ʪ����Ϣ��ֻ�ϴ�
	�ƹ⣺����״̬������Ҫ����·�����ҡ������䣻
	�յ�����ǰ��Դ״̬�����õ��¶ȡ����١�ģʽ�����仹�����ȣ���
	�Ŵ�״̬������״̬��
	����״̬������Ļ״̬��
*/
int main()
{
	char ip[] = "127.0.0.1";
	char msg[] = "111111111111111111";
	if (ConnectToHost(81, ip, msg))
		cout << "Sent successful" << endl;
	else
		cout << "Sent failed!" << endl;
	CloseConnection();
}