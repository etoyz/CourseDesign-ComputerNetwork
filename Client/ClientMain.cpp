#include <winsock2.h>
#include <iostream>
#pragma comment (lib, "ws2_32.lib")

using namespace std;

SOCKET server_socket_descriptor;

bool SendToHost(int PortNo, char* IPAddress, char* Msg)
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
	char ip[] = "192.168.1.151";
	char msg[] = "111111111111111111";
	if (SendToHost(81, ip, msg))
		cout << "发送传感器数据：" << msg << endl;
	else
	{
		cout << "Sent failed!" << endl;
		exit(1);
	}
	HandleDataFromServer();
	// 释放资源
	closesocket(server_socket_descriptor);
	WSACleanup();
	system("pause");
}