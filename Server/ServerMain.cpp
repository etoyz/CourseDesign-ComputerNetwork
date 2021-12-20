#include <winsock2.h>
#include <WS2tcpip.h>
#include <io.h>
#include <iostream>
#pragma comment (lib, "ws2_32.lib")

using namespace std;

#define port 81 //�����Ķ˿�
#define MAXIMUM_CONNECTION 10 // ���������

string IP;
string parse(string, int);
string get_local_ip();

int main()
{
	//ʹ��WindowsϵͳAPI
	WSADATA wsadata;
	if (WSAStartup(0x0202, &wsadata) != 0)
		return false;

	// ����һ��socket�׽���
	SOCKET local_socket_descriptor = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (local_socket_descriptor == INVALID_SOCKET)
	{
		cout << "Server local socket created failed!" << endl;
		exit(-1);
	}
	cout << "Server local socket created successfully!" << endl;

	/*
	* ��д��ʼ���׽����������Ϣ
	* sockaddr_in�ṹ�壺���Դ洢һ�������ַ������IP��˿ڣ�,�˴��洢����IP��ַ�뱾�ص�һ���˿�
	*/
	struct sockaddr_in local_addr;
	local_addr.sin_family = AF_INET;
	local_addr.sin_port = htons(port);  //���ض��˿�
	local_addr.sin_addr.s_addr = inet_addr(get_local_ip().c_str()); //�󶨷���������IP��ַ

	/*
	* ��ʼ���׽���
	* bind()�� ��һ�������ַ��һ���׽��ְ󶨣��˴������������ص�ַ�󶨵�һ���׽�����
	*/
	int res = bind(local_socket_descriptor, (struct sockaddr*)&local_addr, sizeof(local_addr));
	if (res == SOCKET_ERROR)
	{
		cout << "Socket initialization failed!(check whether IP address is valid)" << endl;
		exit(-1);
	}
	cout << "Socket initialization successfully!" << endl;
	cout << "��������IP��ַΪ��" << get_local_ip() << endl;

	/*
	* listen()������������ͼ���ӱ����Ŀͻ���
	* �������������Ľ�����
	*/
	listen(local_socket_descriptor, MAXIMUM_CONNECTION);
	cout << "Waiting for a connection...." << endl;

	while (true)//ѭ�����տͻ��˵�����
	{
		// 5.����һ��sockaddr_in�ṹ�壬�����洢�ͻ����ĵ�ַ
		struct sockaddr_in client_addr;
		socklen_t len = sizeof(client_addr);
		// accept()�������������У�ֱ���յ�ĳһ�ͻ������������󣬲����ؿͻ�����������
		SOCKET client_socket_descriptor = accept(local_socket_descriptor, (struct sockaddr*)&client_addr, &len);
		if (client_socket_descriptor == SOCKET_ERROR)
		{
			cout << "Unable to create client socket!\n" << endl;
			exit(-1);
		}

		// ����ͻ�������Ϣ
		string ip = inet_ntoa(client_addr.sin_addr);
		cout << "\n\n========================================================" << endl;
		cout << "�ͻ���: " << ip << " ���ӵ���������!" << endl;
		//cout << client_socket_descriptors.fd_count;

		// ����ͻ������������
		char buff[2048] = { 0 };
		int size = recv(client_socket_descriptor, buff, sizeof(buff), 0);
		string recv_data;
		recv_data.assign(buff, buff + size);
		string cmd_to_client = parse(recv_data, size);

		// ʹ�õ�6��accept()����socket�����������ͻ�����������������ͨ�š�
		send(client_socket_descriptor, cmd_to_client.c_str(), cmd_to_client.size(), 0); //���ظ��ͻ�������

		// �رոÿͻ���socket
		shutdown(client_socket_descriptor, 2);
	}
	shutdown(local_socket_descriptor, 2);
	WSACleanup();
	return 0;
}

/*
* �������������, �����Ϳ�������
*/
string parse(string data, int size)
{
	// ���������Ϣ����׼����豸
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
	if (data[9] == '1') {
		cout << "����״̬";
		cout << "(" << data[10] << data[11] << "." << data[12]
			<< "��, ����:" << data[13]
			<< ", ģʽ:" << data[14] << ")\n";
	}
	else
		cout << "�ر�״̬";
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
		cout << "�ر������豸\n";
		cmd_to_client += "1";
	}
	else cmd_to_client += "0";
	if (data[0] == '1' && data[9] == '1') { // �����ҿյ�����
		cout << "�ر��Ŵ�\n";
		cmd_to_client += "1";
	}
	else cmd_to_client += "0";
	if (data[0] == '1' && data[16] == '0') { // �����Ҵ����ر�
		cout << "�����ҵƹ�\n";
		cmd_to_client += "1";
	}
	else cmd_to_client += "0";
	if (data[0] == '1' && data[16] == '1') { // �����Ҵ�����
		cout << "�ر�������ƹ�\n";
		cmd_to_client += "1";
	}
	else cmd_to_client += "0";
	if (data[0] == '1') { // ����
		string tempe_str = string(1, data[10]) + string(1, data[11]) + '.' + string(1, data[12]);
		float t;
		sscanf(tempe_str.c_str(), "%f", &t);
		cout << "�¶�Ϊ " + tempe_str + " �棬";
		if (t > 30) { // ���¶ȸ���30��
			cout << "�����յ����䣬�趨�¶�Ϊ24�ȣ�����Ϊ�з�";
			cmd_to_client += "102401";
		}
		else if (t < 15) { // ���¶ȵ���15��
			cout << "�����յ����ȣ��趨�¶�Ϊ26�ȣ�����Ϊ�߷�";
			cmd_to_client += "112610";
		}
		else {
			cmd_to_client += "0XXXXX";
		}
	}
	else cmd_to_client += "0XXXXX";

	cout << "\n���Ϳ������ݣ� " + cmd_to_client;
	return cmd_to_client;
}

/*
* ��ȡ����IP
*/
string get_local_ip() {
	char host[256];
	int hostname = gethostname(host, sizeof(host));
	struct hostent* host_entry;
	host_entry = gethostbyname(host);
	return inet_ntoa(*((struct in_addr*)host_entry->h_addr_list[3]));
}