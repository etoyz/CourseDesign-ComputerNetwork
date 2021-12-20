//Don't forget to link libws2_32.a to your program as well
#include <winsock2.h>
#include <iostream>
#pragma comment (lib, "ws2_32.lib")

using namespace std;

SOCKET server_socket_descriptor; //Socket handle

bool SendToHost(int PortNo, char* IPAddress, char* Msg)
{
	//Start up Winsock��
	WSADATA wsadata;

	/*
	* If successful, the WSAStartup function returns zero. Otherwise, it returns one of the error codes
	*/
	if (WSAStartup(0x0202, &wsadata) != 0)
		return false;

	//Fill out the information needed to initialize a socket��
	SOCKADDR_IN target;               //Socket address information

	target.sin_family = AF_INET;      // address family Internet
	target.sin_port = htons(PortNo); //Port to connect on
	target.sin_addr.s_addr = inet_addr(IPAddress); //Target IP

	server_socket_descriptor = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); //Create socket
	if (server_socket_descriptor == INVALID_SOCKET)
	{
		return false; //Couldn't create the socket
	}

	//Try connecting...
	if (connect(server_socket_descriptor, (SOCKADDR*)&target, sizeof(target)) == SOCKET_ERROR)
	{
		return false;                 //Couldn't connect
	}
	else {
		send(server_socket_descriptor, Msg, strlen(Msg), 0);

		char buff[100] = { 0 };
		int size = recv(server_socket_descriptor, buff, strlen(buff), MSG_WAITALL);
		cout << buff << endl;

		return true;                  //Success
	}
}

void HandleDataFromHost()
{
	char buff[100] = { 0 };
	int size = recv(server_socket_descriptor, buff, sizeof(buff), MSG_WAITALL);
	cout << "���ܵ�������Ϣ��" << buff << endl;
	cout << "----------------------" << endl;
	cout << "���������ź�����:\n";
	if(buff[0] == '1')
		cout << "�ر������豸\n";
	if (buff[1] == '1')
		cout << "�ر��Ŵ�\n";
	if (buff[2] == '1')
		cout << "�����ҵƹ�\n";
	if (buff[3] == '1')
		cout << "�ر�������ƹ�\n";
	if (buff[4] == '1') {
		cout << "�򿪿յ�\n";
		if(buff[5] == '0')
			cout << "�����յ����䣬";
		else
			cout << "�����յ����ȣ�";
		cout << "�趨�¶�Ϊ" << buff[6] << buff[7] << "." << buff[8] << "�棬";
		if (buff[9] == '0')
		{
			if (buff[10] == '0')
				cout << "����Ϊ�ͷ�";
			else
				cout << "����Ϊ�з�";
		} else 
			cout << "����Ϊ�߷�";
		cout << "\n�����źŷ�����ϣ�\n";
	}
		
}

//CLOSECONNECTION �C shuts down the socket and closes any connection on it
void CloseConnection()
{
	//Close the socket if it exists
	if (server_socket_descriptor)
		closesocket(server_socket_descriptor);

	WSACleanup();                     //Clean up Winsock
}

/*
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
	if (SendToHost(81, ip, msg))
		cout << "���ʹ��������ݣ�" << msg << endl;
	else
		cout << "Sent failed!" << endl;
	HandleDataFromHost();
	//CloseConnection();
	system("pause");
}