#include <winsock2.h>
#include <iostream>
#include <sstream>
#pragma comment (lib, "ws2_32.lib")

using namespace std;

#define PORT 81 // �˿�
char ip[20];	// �洢������IP
SOCKET server_socket; // ������Socket
bool send_to_server(int, char*, const char*); // ���������������
void handle_data_from_server(); // ����ӷ��������ص�ָ��
string current_sersor_data; // ��ǰ����������
string get_all_sensor_data(); // ��ȡȫ������������

// �˽ṹ��洢���������ص�ָ��
struct Cmd_From_Server
{
	string head;
	string data;
};

int main()
{
	cout << "�����������IP��ַ��";
	cin >> ip;
	while (true) {
		cout << "\n\n\n\n======================================================================" << endl;
		current_sersor_data = get_all_sensor_data();
		if (send_to_server(PORT, ip, ("DATA" + current_sersor_data).c_str()))
			cout << "���ʹ��������ݣ�" << "DATA" + current_sersor_data << endl;
		else {
			cout << "����ʧ��!" << endl;
			exit(1);
		}
		handle_data_from_server();
		if (send_to_server(PORT, ip, ("STATACK")))
			cout << "���ʹ��������ݣ�" << "STATACK" << endl;
		else {
			cout << "����ʧ��!" << endl;
			exit(1);
		}
		Sleep(500);
	}
	// �ͷ���Դ
	closesocket(server_socket);
	WSACleanup();
	system("pause");
}

// ����ָ�����ݵ�ָ��IP�˿�
bool send_to_server(int PortNo, char* IPAddress, const char* Msg)
{
	//ʹ��WindowsϵͳAPI
	WSADATA wsadata;
	if (WSAStartup(0x0202, &wsadata) != 0)
		return false;

	// ��д��ʼ���׽����������Ϣ
	SOCKADDR_IN target;
	target.sin_family = AF_INET;
	target.sin_port = htons(PortNo);
	target.sin_addr.s_addr = inet_addr(IPAddress);
	// �����׽���
	server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); //Create socket
	if (server_socket == INVALID_SOCKET)
	{
		return false; //Couldn't create the socket
	}

	// ���������������
	if (connect(server_socket, (SOCKADDR*)&target, sizeof(target)) == SOCKET_ERROR)
	{
		return false;
	}
	else {
		send(server_socket, Msg, strlen(Msg), 0);
		return true;
	}
}

// ����ӷ��������صĿ�������
void handle_data_from_server()
{
	char buff[100] = { 0 };
	int size = recv(server_socket, buff, sizeof(buff), MSG_WAITALL);
	cout << "---------------------------------------------" << endl;
	cout << "���ܵ����Է������Ŀ������ݣ�" << buff << endl;
	cout << "���������������Ŀ�������..." << endl;
	Cmd_From_Server cmd_from_server;
	cmd_from_server.head.assign(buff, buff + 3);
	cmd_from_server.data.assign(buff + 3, sizeof(buff));
	string data = cmd_from_server.data;
	cout << "---------------------------------------------" << endl;
	if (cmd_from_server.head == "SET") {
		cout << "���ݷ�����ָʾ���������¶���:\n";
		if (data[0] == '1') {
			cout << "�ر������豸\n";
			current_sersor_data[7] = 0;
			current_sersor_data[8] = 0;
			current_sersor_data[9] = 0;
			current_sersor_data[15] = 0;
			current_sersor_data[16] = 0;
		}
		if (data[1] == '1') {
			cout << "�ر��Ŵ�\n";
			current_sersor_data[15] = 0;
			current_sersor_data[16] = 0;
		}
		if (data[2] == '1') {
			cout << "�����ҵƹ�\n";
			current_sersor_data[7] = 1;
		}
		if (data[3] == '1') {
			cout << "�ر�������ƹ�\n";
			current_sersor_data[8] = 0;
		}
		if (data[4] == '1') {
			cout << "�򿪿յ�\n";
			current_sersor_data[9] = 1;
			if (data[5] == '0') {
				cout << "�����յ����䣬";
				current_sersor_data[14] = 0;
			}
			else {
				cout << "�����յ����ȣ�";
				current_sersor_data[14] = 1;
			}
			cout << "�趨�¶�Ϊ" << data[6] << data[7] << "." << data[8] << "�棬";
			current_sersor_data[10] = data[6];
			current_sersor_data[11] = data[7];
			current_sersor_data[12] = 0;
			if (data[8] == '1')
					cout << "����Ϊ�ͷ�";
			else if (data[8] == '2')
				cout << "����Ϊ�з�";
			else if (data[8] == '3')
				cout << "����Ϊ�߷�";
			current_sersor_data[13] = data[8]; // ����
		}
	}
	else if (cmd_from_server.head == "GET") {
		cout << "���ݷ�����ָʾ�����ص�ǰ����������:\n";
		send_to_server(PORT,ip, current_sersor_data.c_str());
	}

	cout << "\n\n�����źŽ�����ϣ�";
}

// ��ȡȫ������������
string get_all_sensor_data() {
	srand(time(NULL));
	stringstream data;
	data << rand() % 2; // ������
	data << (rand() % 2 + 1) << (rand() % 10) << (rand() % 10); // �¶�00.0 - 49.9 ��
	data << (rand() % 3) << (rand() % 10) << (rand() % 10); // ʪ��00.0 - 49.9
	data << rand() % 2 << rand() % 2; // ����2���ƹ�
	int ac = rand() % 2; // �յ�״̬
	data << rand() % 2;
	if (ac == 1) {
		data << (rand() % 3 + 1) << (rand() % 10) << (rand() % 10); // �¶�00.0 - 49.9 ��
		data << (rand() % 3) + 1; // ����  1 2 3
		data << (rand() % 2); // 0���� 1����
	}
	else
		data << "XXXXX";
	data << rand() % 2; // �Ŵ�״̬
	data << rand() % 2; // ����״̬

	return data.str();
}