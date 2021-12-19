#include <winsock2.h>
#include <WS2tcpip.h>
#include <io.h>
#include <iostream>
#pragma comment (lib, "ws2_32.lib")

#define IP "127.0.0.1"
#define port 81 //�����Ķ˿ڣ������ڷ�Χ�������趨
#define message "HTTP server is ready!"//���ظ��ͻ�������Ϣ

using namespace std;

int parse(char* data, int size)
{
	// Output to the terminal
	cout << "��������:\n" << data << endl;
	cout << "�� " << size << " �ֽڡ�" << endl;

	// Parsing
	cout << "----------------------" << endl;
	cout << "�����ͻ��˷��͹�������Ϣ: " << endl;
	cout << "����״̬��\t";
	if (data[0] == '1')
		cout << "����\n";
	else
		cout << "����\n";
	cout << "�����¶ȣ�\t";
	cout << data[1] << data[2] << "." << data[3] << "��\n";
	cout << "����ʪ�ȣ�\t";
	cout << data[4] << data[5] << "." << data[6] << "%rh\n";
	cout << "���ҵƹ⣺\t";
	if (data[7] == '1')
		cout << "����״̬\n";
	else
		cout << "�ر�״̬\n";
	cout << "������ƹ⣺\t";
	if (data[8] == '1')
		cout << "����״̬\n";
	else
		cout << "�ر�״̬\n";
	cout << "�յ���\t\t";
	if (data[9] == '1')
		cout << "����״̬";
	else
		cout << "�ر�״̬";
	cout << "(" << data[10] << data[11] << "." << data[12] 
		<< "��, ����:" << data[13] 
		<< ", ģʽ:" << data[14] << ")\n";
	cout << "�Ŵ�״̬��\t";
	if (data[15] == '1')
		cout << "����״̬\n";
	else
		cout << "�ر�״̬\n";
	cout << "����״̬��\t";
	if (data[16] == '1')
		cout << "��Ļ״̬\n";
	else
		cout << "��Ļ״̬\n";

	return 0;
}

int main()
{
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	//1.����һ��socket�׽���
	SOCKET local_socket_descriptor = socket(AF_INET, SOCK_STREAM, 0);
	if (local_socket_descriptor == -1)
	{
		cout << "Local socket created failed!" << endl;
		exit(-1);
	}
	cout << "Local socket created successfully!" << endl;

	//2.sockaddr_in�ṹ�壺���Դ洢һ�������ַ������IP��˿ڣ�,�˴��洢����IP��ַ�뱾�ص�һ���˿�
	struct sockaddr_in local_addr;
	local_addr.sin_family = AF_INET;
	local_addr.sin_port = htons(port);  //���ض��˿�
	local_addr.sin_addr.s_addr = inet_addr(IP); //���ض�IP��ַ

	//3.bind()�� ��һ�������ַ��һ���׽��ְ󶨣��˴������ص�ַ�󶨵�һ���׽�����
	int res = bind(local_socket_descriptor, (struct sockaddr*)&local_addr, sizeof(local_addr));
	if (res == -1)
	{
		cout << "Bind failed!(check whether IP address is valid)" << endl;
		exit(-1);
	}
	cout << "Bind successfully!" << endl;

	//4.listen()������������ͼ���ӱ����Ŀͻ���
	//�������������Ľ�����
	listen(local_socket_descriptor, 10);
	cout << "Waiting for a connection...." << endl;

	while (true)//ѭ�����տͻ��˵�����
	{
		//5.����һ��sockaddr_in�ṹ�壬�����洢�ͻ����ĵ�ַ
		struct sockaddr_in client_addr;
		socklen_t len = sizeof(client_addr);
		//6.accept()�������������У�ֱ���յ�ĳһ�ͻ������������󣬲����ؿͻ�����������
		SOCKET client_socket_descriptor = accept(local_socket_descriptor, (struct sockaddr*)&client_addr, &len);
		if (client_socket_descriptor == -1)
		{
			cout << "Unable to create client socket!\n" << endl;
			exit(-1);
		}

		//7.����ͻ�������Ϣ
		char* ip = inet_ntoa(client_addr.sin_addr);
		cout << "------------------------------------------------------" << endl;
		cout << "------------------------------------------------------" << endl;
		cout << "�ͻ���: " << ip << " ���ӵ���������!" << endl;

		//8.����ͻ����������Ϣ
		char buff[100] = { 0 };
		int size = recv(client_socket_descriptor, buff, sizeof(buff), 0);
		parse(buff, size);

		//9.ʹ�õ�6��accept()����socket�����������ͻ�����������������ͨ�š�
		send(client_socket_descriptor, message, sizeof(message), 0);//����message

		//10.�رտͻ��˵�socket
		shutdown(client_socket_descriptor, 2);
	}
	shutdown(local_socket_descriptor, 2);
	WSACleanup();
	return 0;
}

