#include <winsock2.h>
#include <WS2tcpip.h>
#include <io.h>
#include <iostream>
#pragma comment (lib, "ws2_32.lib")

#define IP "127.0.0.1"
#define port 81 //监听的端口，可以在范围内自由设定
#define message "HTTP server is ready!"//返回给客户机的信息

using namespace std;

int parse(char* data, int size)
{
	// Output to the terminal
	cout << "请求数据:\n" << data << endl;
	cout << "共 " << size << " 字节。" << endl;

	// Parsing
	cout << "----------------------" << endl;
	cout << "解析客户端发送过来的信息: " << endl;
	cout << "房间状态：\t";
	if (data[0] == '1')
		cout << "有人\n";
	else
		cout << "无人\n";
	cout << "房间温度：\t";
	cout << data[1] << data[2] << "." << data[3] << "℃\n";
	cout << "房间湿度：\t";
	cout << data[4] << data[5] << "." << data[6] << "%rh\n";
	cout << "卧室灯光：\t";
	if (data[7] == '1')
		cout << "开启状态\n";
	else
		cout << "关闭状态\n";
	cout << "卫生间灯光：\t";
	if (data[8] == '1')
		cout << "开启状态\n";
	else
		cout << "关闭状态\n";
	cout << "空调：\t\t";
	if (data[9] == '1')
		cout << "开启状态";
	else
		cout << "关闭状态";
	cout << "(" << data[10] << data[11] << "." << data[12] 
		<< "℃, 风速:" << data[13] 
		<< ", 模式:" << data[14] << ")\n";
	cout << "门窗状态：\t";
	if (data[15] == '1')
		cout << "开启状态\n";
	else
		cout << "关闭状态\n";
	cout << "窗帘状态：\t";
	if (data[16] == '1')
		cout << "开幕状态\n";
	else
		cout << "闭幕状态\n";

	return 0;
}

int main()
{
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	//1.创建一个socket套接字
	SOCKET local_socket_descriptor = socket(AF_INET, SOCK_STREAM, 0);
	if (local_socket_descriptor == -1)
	{
		cout << "Local socket created failed!" << endl;
		exit(-1);
	}
	cout << "Local socket created successfully!" << endl;

	//2.sockaddr_in结构体：可以存储一套网络地址（包括IP与端口）,此处存储本机IP地址与本地的一个端口
	struct sockaddr_in local_addr;
	local_addr.sin_family = AF_INET;
	local_addr.sin_port = htons(port);  //绑定特定端口
	local_addr.sin_addr.s_addr = inet_addr(IP); //绑定特定IP地址

	//3.bind()： 将一个网络地址与一个套接字绑定，此处将本地地址绑定到一个套接字上
	int res = bind(local_socket_descriptor, (struct sockaddr*)&local_addr, sizeof(local_addr));
	if (res == -1)
	{
		cout << "Bind failed!(check whether IP address is valid)" << endl;
		exit(-1);
	}
	cout << "Bind successfully!" << endl;

	//4.listen()函数：监听试图连接本机的客户端
	//参数二：监听的进程数
	listen(local_socket_descriptor, 10);
	cout << "Waiting for a connection...." << endl;

	while (true)//循环接收客户端的请求
	{
		//5.创建一个sockaddr_in结构体，用来存储客户机的地址
		struct sockaddr_in client_addr;
		socklen_t len = sizeof(client_addr);
		//6.accept()函数：阻塞运行，直到收到某一客户机的连接请求，并返回客户机的描述符
		SOCKET client_socket_descriptor = accept(local_socket_descriptor, (struct sockaddr*)&client_addr, &len);
		if (client_socket_descriptor == -1)
		{
			cout << "Unable to create client socket!\n" << endl;
			exit(-1);
		}

		//7.输出客户机的信息
		char* ip = inet_ntoa(client_addr.sin_addr);
		cout << "------------------------------------------------------" << endl;
		cout << "------------------------------------------------------" << endl;
		cout << "客户机: " << ip << " 连接到本服务器!" << endl;

		//8.处理客户机请求的信息
		char buff[100] = { 0 };
		int size = recv(client_socket_descriptor, buff, sizeof(buff), 0);
		parse(buff, size);

		//9.使用第6步accept()返回socket描述符，即客户机的描述符，进行通信。
		send(client_socket_descriptor, message, sizeof(message), 0);//返回message

		//10.关闭客户端的socket
		shutdown(client_socket_descriptor, 2);
	}
	shutdown(local_socket_descriptor, 2);
	WSACleanup();
	return 0;
}

