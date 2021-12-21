#include <winsock2.h>
#include <WS2tcpip.h>
#include <io.h>
#include <iostream>
#pragma comment (lib, "ws2_32.lib")

using namespace std;

#define PORT 81 //监听的端口
#define MAXIMUM_CONNECTION 10 // 最大连接数

int connection_cnt = 0;
string IP;
string parse(string, int);
string get_local_ip();

int main()
{
	//使用Windows系统API
	WSADATA wsadata;
	if (WSAStartup(0x0202, &wsadata) != 0)
		return false;

	// 创建一个socket套接字
	SOCKET local_socket_descriptor = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (local_socket_descriptor == INVALID_SOCKET)
	{
		cout << "服务器Socket创建失败！" << endl;
		exit(-1);
	}
	cout << "服务器Socket创建成功！" << endl;

	/*
	* 填写初始化套接字所需的信息
	* sockaddr_in结构体：可以存储一套网络地址（包括IP与端口）,此处存储本机IP地址与本地的一个端口
	*/
	struct sockaddr_in local_addr;
	local_addr.sin_family = AF_INET;
	local_addr.sin_port = htons(PORT);  //绑定特定端口
	local_addr.sin_addr.s_addr = inet_addr(get_local_ip().c_str()); //绑定服务器本地IP地址

	/*
	* 初始化套接字
	* bind()： 将一个网络地址与一个套接字绑定，此处将服务器本地地址绑定到一个套接字上
	*/
	int res = bind(local_socket_descriptor, (struct sockaddr*)&local_addr, sizeof(local_addr));
	if (res == SOCKET_ERROR)
	{
		cout << "Socket初始化失败！" << endl;
		exit(-1);
	}
	cout << "Socket初始化成功！" << endl;
	cout << "本服务器IP地址为：" << get_local_ip() << endl;

	/*
	* listen()函数：监听试图连接本机的客户端
	* 参数二：监听的进程数
	*/
	listen(local_socket_descriptor, MAXIMUM_CONNECTION);
	cout << "正在等待客户端的连接...." << endl;

	while (true)//循环接收客户端的请求
	{
		// 5.创建一个sockaddr_in结构体，用来存储客户机的地址
		struct sockaddr_in client_addr;
		socklen_t len = sizeof(client_addr);
		// accept()函数：阻塞运行，直到收到某一客户机的连接请求，并返回客户机的描述符
		SOCKET client_socket_descriptor = accept(local_socket_descriptor, (struct sockaddr*)&client_addr, &len);
		if (client_socket_descriptor == SOCKET_ERROR)
		{
			cout << "Unable to create client socket!\n" << endl;
			exit(-1);
		}

		// 输出客户机的信息
		string ip = inet_ntoa(client_addr.sin_addr);
		cout << "\n\n\n\n======================================================================" << endl;
		connection_cnt++;
		cout << "接收到第 " << connection_cnt << " 个连接请求! 源主机IP: " << ip << endl;

		// 处理客户机请求的数据
		char buff[2048] = { 0 };
		int size = recv(client_socket_descriptor, buff, sizeof(buff), 0);
		string recv_data;
		recv_data.assign(buff, buff + size);
		string cmd_to_client = parse(recv_data, size);

		// 使用第6步accept()返回socket描述符，即客户机的描述符，进行通信。
		cout << "\n向客户端发送控制数据： " + cmd_to_client;
		send(client_socket_descriptor, cmd_to_client.c_str(), cmd_to_client.size(), 0); //返回给客户端数据
		// get
		//cout << "\n一秒后发送GET信号";
		//Sleep(1000);
		//send(client_socket_descriptor, "GET", 3, 0); //返回给客户端数据

		// 关闭该客户的socket
		shutdown(client_socket_descriptor, 2);
	}
	shutdown(local_socket_descriptor, 2);
	WSACleanup();
	return 0;
}

/*
* 解析客户端的请求数据, 并生成应返回的控制数据
*/
string parse(string data, int size)
{
	// 输出解析信息到标准输出设备
	cout << "请求数据:\n" << data << endl;
	cout << "共 " << size << " 字节。" << endl;

	// 解析
	cout << "---------------------------------------------" << endl;
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
	if (data[9] == '1') {
		cout << "开启状态";
		cout << "(" << data[10] << data[11] << "." << data[12]
			<< "℃, 风速:" << data[13]
			<< ", 模式:" << data[14] << ")\n";
	}
	else
		cout << "关闭状态\n";
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

	// Action
	/*
		[1][1][1][1][[1][3][1][1]]
		是否关闭所有设备
		门窗
		卧室灯光
		卫生间灯光
		空调：当前电源状态、设置的温度、风速、模式（制冷还是制热）。
	*/
	string cmd_to_client; // 发回客户端的控制信息
	cout << "---------------------------------------------" << endl;
	cout << "根据解析结果发送控制信息: " << endl;
	cmd_to_client += "SET";
	if (data[0] == '0') { // 无人
		cout << "关闭所有设备\n";
		cmd_to_client += "1";
	}
	else cmd_to_client += "0";
	if (data[0] == '1' && data[9] == '1') { // 有人且空调开启
		cout << "关闭门窗\n";
		cmd_to_client += "1";
	}
	else cmd_to_client += "0";
	if (data[0] == '1' && data[16] == '0') { // 有人且窗帘关闭
		cout << "打开卧室灯光\n";
		cmd_to_client += "1";
	}
	else cmd_to_client += "0";
	if (data[0] == '1' && data[16] == '1') { // 有人且窗帘打开
		cout << "关闭卫生间灯光\n";
		cmd_to_client += "1";
	}
	else cmd_to_client += "0";
	if (data[0] == '1') { // 有人
		string tempe_str = string(1, data[10]) + string(1, data[11]) + '.' + string(1, data[12]);
		float t;
		sscanf(tempe_str.c_str(), "%f", &t);
		cout << "温度为 " + tempe_str + " ℃，";
		if (t > 30) { // 且温度高于30度
			cout << "开启空调制冷，设定温度为24度，风速为中风\n";
			cmd_to_client += "10242";
		}
		else if (t < 15) { // 且温度低于15度
			cout << "开启空调制热，设定温度为26度，风速为高风\n";
			cmd_to_client += "11263";
		}
		else {
			cout << "空调维持原状态\n";
			cmd_to_client += "0XXXX";
		}
	}
	else cmd_to_client += "0XXXX";

	return cmd_to_client;
}

/*
* 获取本机IP
*/
string get_local_ip() {
	char host[256];
	int hostname = gethostname(host, sizeof(host));
	struct hostent* host_entry;
	host_entry = gethostbyname(host);
	return inet_ntoa(*((struct in_addr*)host_entry->h_addr_list[2]));
}