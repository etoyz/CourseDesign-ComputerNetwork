#include <winsock2.h>
#include <iostream>
#include <sstream>
#pragma comment (lib, "ws2_32.lib")

using namespace std;

SOCKET server_socket_descriptor;
bool SendToServer(int, char*, const char* Msg);
void HandleDataFromServer();
string get_all_sensor_data();

/*
	[1][3][3][2][[1][3][1][1]][1][1]
	共 17位
	房间状态：有无人。只上传
	温度传感器：采集房间温度信息。只上传
	湿度传感器：采集房间湿度信息。只上传
	灯光：开闭状态；至少要有两路：卧室、卫生间；
	空调：当前电源状态、设置的温度、风速、模式（制冷还是制热）。
	门窗状态：开闭状态；
	窗帘状态：开闭幕状态；
*/
int main()
{
	char ip[20];
	cout << "请输入IP地址：";
	cin >> ip;
	while (true) {
		string str = get_all_sensor_data();
		if (SendToServer(81, ip, str.c_str()))
			cout << "发送传感器数据：" << str.c_str() << endl;
		else
		{
			cout << "Sent failed!" << endl;
			exit(1);
		}
		HandleDataFromServer();
		Sleep(1000);
	}
	// 释放资源
	closesocket(server_socket_descriptor);
	WSACleanup();
	system("pause");
}

/*
* 发送指定数据到指定IP端口
*/
bool SendToServer(int PortNo, char* IPAddress, const char* Msg)
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
	server_socket_descriptor = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); //Create socket
	if (server_socket_descriptor == INVALID_SOCKET)
	{
		return false; //Couldn't create the socket
	}

	// 尝试与服务器连接
	if (connect(server_socket_descriptor, (SOCKADDR*)&target, sizeof(target)) == SOCKET_ERROR)
	{
		return false;
	}
	else {
		send(server_socket_descriptor, Msg, strlen(Msg), 0);
		return true;
	}
}

/*
* 处理从服务器传回的控制数据
*/
void HandleDataFromServer()
{
	char buff[100] = { 0 };
	int size = recv(server_socket_descriptor, buff, sizeof(buff), MSG_WAITALL);
	cout << "接受到控制信息：" << buff << endl;
	cout << "----------------------" << endl;
	cout << "发出控制信号如下:\n";
	if (buff[0] == '1')
		cout << "关闭所有设备\n";
	if (buff[1] == '1')
		cout << "关闭门窗\n";
	if (buff[2] == '1')
		cout << "打开卧室灯光\n";
	if (buff[3] == '1')
		cout << "关闭卫生间灯光\n";
	if (buff[4] == '1') {
		cout << "打开空调\n";
		if (buff[5] == '0')
			cout << "开启空调制冷，";
		else
			cout << "开启空调制热，";
		cout << "设定温度为" << buff[6] << buff[7] << "." << buff[8] << "℃，";
		if (buff[9] == '0')
		{
			if (buff[10] == '0')
				cout << "风速为低风";
			else
				cout << "风速为中风";
		}
		else
			cout << "风速为高风";
		cout << "\n控制信号解析完毕！\n";
	}

}

string get_all_sensor_data() {
	srand(time(NULL));
	stringstream data;
	data << rand() % 2; // 有无人
	data << (rand() % 5) << (rand() % 10) << (rand() % 10); // 温度00.0 - 49.9 ℃
	data << (rand() % 5) << (rand() % 10) << (rand() % 10); // 湿度00.0 - 49.9 ℃
	data << rand() % 2 << rand() % 2; // 控制2个灯光
	int ac = rand() % 2; // 空调状态
	data << rand() % 2;
	if (ac == 1) {
		data << (rand() % 5) << (rand() % 10) << (rand() % 10); // 温度00.0 - 49.9 ℃
		data << (rand() % 3) + 1 ; // 风速  1 2 3
		data << (rand() % 2); // 0制冷 1制热
	}
	else
		data << "xxxxx";
	data << rand() % 2; // 门窗状态
	data << rand() % 2; // 窗帘状态

	return data.str();
}