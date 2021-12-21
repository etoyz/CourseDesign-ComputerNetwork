#include <winsock2.h>
#include <iostream>
#include <sstream>
#pragma comment (lib, "ws2_32.lib")

using namespace std;

#define PORT 81 // 端口
char ip[20];	// 存储服务器IP
SOCKET server_socket; // 服务器Socket
bool send_to_server(int, char*, const char*); // 向服务器发送数据
void handle_data_from_server(); // 处理从服务器发回的指令
string current_sersor_data; // 当前传感器数据
string get_all_sensor_data(); // 获取全部传感器数据

// 此结构体存储服务器传回的指令
struct Cmd_From_Server
{
	string head;
	string data;
};

int main()
{
	cout << "请输入服务器IP地址：";
	cin >> ip;
	while (true) {
		cout << "\n\n\n\n======================================================================" << endl;
		current_sersor_data = get_all_sensor_data();
		if (send_to_server(PORT, ip, ("DATA" + current_sersor_data).c_str()))
			cout << "发送传感器数据：" << "DATA" + current_sersor_data << endl;
		else {
			cout << "发送失败!" << endl;
			exit(1);
		}
		handle_data_from_server();
		if (send_to_server(PORT, ip, ("STATACK")))
			cout << "发送传感器数据：" << "STATACK" << endl;
		else {
			cout << "发送失败!" << endl;
			exit(1);
		}
		Sleep(500);
	}
	// 释放资源
	closesocket(server_socket);
	WSACleanup();
	system("pause");
}

// 发送指定数据到指定IP端口
bool send_to_server(int PortNo, char* IPAddress, const char* Msg)
{
	//使用Windows系统API
	WSADATA wsadata;
	if (WSAStartup(0x0202, &wsadata) != 0)
		return false;

	// 填写初始化套接字所需的信息
	SOCKADDR_IN target;
	target.sin_family = AF_INET;
	target.sin_port = htons(PortNo);
	target.sin_addr.s_addr = inet_addr(IPAddress);
	// 创建套接字
	server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); //Create socket
	if (server_socket == INVALID_SOCKET)
	{
		return false; //Couldn't create the socket
	}

	// 尝试与服务器连接
	if (connect(server_socket, (SOCKADDR*)&target, sizeof(target)) == SOCKET_ERROR)
	{
		return false;
	}
	else {
		send(server_socket, Msg, strlen(Msg), 0);
		return true;
	}
}

// 处理从服务器传回的控制数据
void handle_data_from_server()
{
	char buff[100] = { 0 };
	int size = recv(server_socket, buff, sizeof(buff), MSG_WAITALL);
	cout << "---------------------------------------------" << endl;
	cout << "接受到来自服务器的控制数据：" << buff << endl;
	cout << "解析服务器发出的控制数据..." << endl;
	Cmd_From_Server cmd_from_server;
	cmd_from_server.head.assign(buff, buff + 3);
	cmd_from_server.data.assign(buff + 3, sizeof(buff));
	string data = cmd_from_server.data;
	cout << "---------------------------------------------" << endl;
	if (cmd_from_server.head == "SET") {
		cout << "根据服务器指示，做出如下动作:\n";
		if (data[0] == '1') {
			cout << "关闭所有设备\n";
			current_sersor_data[7] = 0;
			current_sersor_data[8] = 0;
			current_sersor_data[9] = 0;
			current_sersor_data[15] = 0;
			current_sersor_data[16] = 0;
		}
		if (data[1] == '1') {
			cout << "关闭门窗\n";
			current_sersor_data[15] = 0;
			current_sersor_data[16] = 0;
		}
		if (data[2] == '1') {
			cout << "打开卧室灯光\n";
			current_sersor_data[7] = 1;
		}
		if (data[3] == '1') {
			cout << "关闭卫生间灯光\n";
			current_sersor_data[8] = 0;
		}
		if (data[4] == '1') {
			cout << "打开空调\n";
			current_sersor_data[9] = 1;
			if (data[5] == '0') {
				cout << "开启空调制冷，";
				current_sersor_data[14] = 0;
			}
			else {
				cout << "开启空调制热，";
				current_sersor_data[14] = 1;
			}
			cout << "设定温度为" << data[6] << data[7] << "." << data[8] << "℃，";
			current_sersor_data[10] = data[6];
			current_sersor_data[11] = data[7];
			current_sersor_data[12] = 0;
			if (data[8] == '1')
					cout << "风速为低风";
			else if (data[8] == '2')
				cout << "风速为中风";
			else if (data[8] == '3')
				cout << "风速为高风";
			current_sersor_data[13] = data[8]; // 风速
		}
	}
	else if (cmd_from_server.head == "GET") {
		cout << "根据服务器指示，返回当前传感器数据:\n";
		send_to_server(PORT,ip, current_sersor_data.c_str());
	}

	cout << "\n\n控制信号解析完毕！";
}

// 获取全部传感器数据
string get_all_sensor_data() {
	srand(time(NULL));
	stringstream data;
	data << rand() % 2; // 有无人
	data << (rand() % 2 + 1) << (rand() % 10) << (rand() % 10); // 温度00.0 - 49.9 ℃
	data << (rand() % 3) << (rand() % 10) << (rand() % 10); // 湿度00.0 - 49.9
	data << rand() % 2 << rand() % 2; // 控制2个灯光
	int ac = rand() % 2; // 空调状态
	data << rand() % 2;
	if (ac == 1) {
		data << (rand() % 3 + 1) << (rand() % 10) << (rand() % 10); // 温度00.0 - 49.9 ℃
		data << (rand() % 3) + 1; // 风速  1 2 3
		data << (rand() % 2); // 0制冷 1制热
	}
	else
		data << "XXXXX";
	data << rand() % 2; // 门窗状态
	data << rand() % 2; // 窗帘状态

	return data.str();
}