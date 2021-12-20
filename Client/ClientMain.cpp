//Don't forget to link libws2_32.a to your program as well
#include <winsock2.h>
#include <iostream>
#pragma comment (lib, "ws2_32.lib")

using namespace std;

SOCKET server_socket_descriptor; //Socket handle

bool SendToHost(int PortNo, char* IPAddress, char* Msg)
{
	//Start up Winsock…
	WSADATA wsadata;

	/*
	* If successful, the WSAStartup function returns zero. Otherwise, it returns one of the error codes
	*/
	if (WSAStartup(0x0202, &wsadata) != 0)
		return false;

	//Fill out the information needed to initialize a socket…
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
	cout << "接受到控制信息：" << buff << endl;
	cout << "----------------------" << endl;
	cout << "发出控制信号如下:\n";
	if(buff[0] == '1')
		cout << "关闭所有设备\n";
	if (buff[1] == '1')
		cout << "关闭门窗\n";
	if (buff[2] == '1')
		cout << "打开卧室灯光\n";
	if (buff[3] == '1')
		cout << "关闭卫生间灯光\n";
	if (buff[4] == '1') {
		cout << "打开空调\n";
		if(buff[5] == '0')
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
		} else 
			cout << "风速为高风";
		cout << "\n控制信号发送完毕！\n";
	}
		
}

//CLOSECONNECTION – shuts down the socket and closes any connection on it
void CloseConnection()
{
	//Close the socket if it exists
	if (server_socket_descriptor)
		closesocket(server_socket_descriptor);

	WSACleanup();                     //Clean up Winsock
}

/*
	[1][3][3][2][[1][3][1][1]][1][1]
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
	char ip[] = "127.0.0.1";
	char msg[] = "111111111111111111";
	if (SendToHost(81, ip, msg))
		cout << "发送传感器数据：" << msg << endl;
	else
		cout << "Sent failed!" << endl;
	HandleDataFromHost();
	//CloseConnection();
	system("pause");
}