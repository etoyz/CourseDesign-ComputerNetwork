#include <winsock2.h>
#include <WS2tcpip.h>
#include <io.h>
#include <iostream>
#pragma comment (lib, "ws2_32.lib")

using namespace std;

#define IP "127.0.0.1" //�󶨵�IP
#define port 81 //�����Ķ˿ڣ������ڷ�Χ�������趨
#define MAXIMUM_CONNECTION 10 // ���������
fd_set client_socket_descriptors; // ���ӳ�

/*
* �����ͻ����������Ϣ, �����Ϳ�������
*/
string parse(string data, int size)
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

	// Action
	/*
		[1][1][1][1][[1][3][1][1]]
		�Ƿ�ر������豸
		�Ŵ�
		���ҵƹ�
		������ƹ�
		�յ�����ǰ��Դ״̬�����õ��¶ȡ����١�ģʽ�����仹�����ȣ���
	*/
	string cmd_to_client;
	cout << "----------------------" << endl;
	cout << "���ݽ���������Ϳ�����Ϣ: " << endl;
	if (data[0] == '0') { // ����
		cout << "�Ƿ�ر������豸\n";
		cmd_to_client += "1";
	}
	else cmd_to_client += "0";
	if (data[0] == '1' && data[9] == '1') { // �����ҿյ�����
		cout << "�Ƿ�ر��Ŵ�\n";
		cmd_to_client += "1";
	}
	else cmd_to_client += "0";
	if (data[0] == '1' && data[16] == '0') { // �����Ҵ����ر�
		cout << "�Ƿ�����ҵƹ�\n";
		cmd_to_client += "1";
	}
	else cmd_to_client += "0";
	if (data[0] == '1' && data[16] == '1') { // �����Ҵ�����
		cout << "�Ƿ�ر�������ƹ�\n";
		cmd_to_client += "1";
	}
	else cmd_to_client += "0";
	if (data[0] == '1') { // ����
		cmd_to_client += "1";
		string tempe_str = string(1, data[10]) + string(1, data[11]) + '.' + string(1, data[12]);
		float t;
		sscanf(tempe_str.c_str(), "%f", &t);
		cout << "�¶�Ϊ " + tempe_str + " �棬";
		if (t > 30) { // ���¶ȸ���30��
			cout << "�����յ����䣬�趨�¶�Ϊ24�ȣ�����Ϊ�з�";
			cmd_to_client += "02401";
		}
		else if (t < 15) { // ���¶ȵ���15��
			cout << "�����յ����ȣ��趨�¶�Ϊ26�ȣ�����Ϊ�߷�";
			cmd_to_client += "12610";
		}
	}
	else cmd_to_client += "0XXXXX";

	cout << "\n���Ϳ�����Ϣ�� " + cmd_to_client;
	return cmd_to_client;
}

int main()
{
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	//1.����һ��socket�׽���
	SOCKET local_socket_descriptor = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (local_socket_descriptor == INVALID_SOCKET)
	{
		cout << "Server local socket created failed!" << endl;
		exit(-1);
	}
	cout << "Server local socket created successfully!" << endl;

	/*
	* 2.��д��ʼ���׽����������Ϣ
	* sockaddr_in�ṹ�壺���Դ洢һ�������ַ������IP��˿ڣ�,�˴��洢����IP��ַ�뱾�ص�һ���˿�
	*/
	struct sockaddr_in local_addr;
	local_addr.sin_family = AF_INET;
	local_addr.sin_port = htons(port);  //���ض��˿�
	local_addr.sin_addr.s_addr = inet_addr(IP); //���ض�IP��ַ

	/*
	* 3.��ʼ���׽���
	* bind()�� ��һ�������ַ��һ���׽��ְ󶨣��˴������������ص�ַ�󶨵�һ���׽�����
	*/
	int res = bind(local_socket_descriptor, (struct sockaddr*)&local_addr, sizeof(local_addr));
	if (res == SOCKET_ERROR)
	{
		cout << "Socket initialization failed!(check whether IP address is valid)" << endl;
		exit(-1);
	}
	cout << "Socket initialization successfully!" << endl;

	//4.listen()������������ͼ���ӱ����Ŀͻ���
	//�������������Ľ�����
	listen(local_socket_descriptor, MAXIMUM_CONNECTION);
	cout << "Waiting for a connection...." << endl;

	while (true)//ѭ�����տͻ��˵�����
	{
		//5.����һ��sockaddr_in�ṹ�壬�����洢�ͻ����ĵ�ַ
		struct sockaddr_in client_addr;
		socklen_t len = sizeof(client_addr);
		//6.accept()�������������У�ֱ���յ�ĳһ�ͻ������������󣬲����ؿͻ�����������
		SOCKET client_socket_descriptor = accept(local_socket_descriptor, (struct sockaddr*)&client_addr, &len);
		//WSAAsyncSelect(local_socket_descriptor,NULL,NULL,FD_ACCEPT)
		if (client_socket_descriptor == SOCKET_ERROR)
		{
			cout << "Unable to create client socket!\n" << endl;
			exit(-1);
		}

		//7.����ͻ�������Ϣ
		string ip = inet_ntoa(client_addr.sin_addr);
		cout << "\n\n========================================================" << endl;
		cout << "�ͻ���: " << ip << " ���ӵ���������!" << endl;
		cout << client_socket_descriptors.fd_count;

		//8.����ͻ����������Ϣ
		char buff[100] = { 0 };
		int size = recv(client_socket_descriptor, buff, sizeof(buff), 0);
		string cmd_to_client = parse(buff, size);

		//9.ʹ�õ�6��accept()����socket�����������ͻ�����������������ͨ�š�
		send(client_socket_descriptor, cmd_to_client.c_str(), cmd_to_client.size(), 0);//���ظ��ͻ�����Ϣ

		//10.�رտͻ��˵�socket
		shutdown(client_socket_descriptor, 2);
	}
	shutdown(local_socket_descriptor, 2);
	WSACleanup();
	return 0;
}

