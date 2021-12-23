#include <winsock2.h>
#include <WS2tcpip.h>
#include <io.h>
#include <iostream>
#include <thread>
#pragma comment (lib, "ws2_32.lib")

using namespace std;

#define PORT 81 //监听的端口
#define MAXIMUM_CONNECTION 10 // 最大连接数

void connection_handler(SOCKET local_socket, SOCKET client_socket);
SOCKET local_socket; // 服务器本地Socket
int connection_cnt = 0; // 当前累积连接数
bool initialize_socket(); // 初始化本地Socket
SOCKET wait_for_connection(); // 阻塞等待客户端连接
int WSAAPI send_data(SOCKET, string); // 向客户端发送数据
string receive_data(SOCKET); // 从客户端接收数据
string parse(string); // 解析客户端传感器数据，并生成控制指令
void print_all_interface_ip(); // 输出本服务器全部网络接口的IP

// 此结构体存储客户端发来的请求
struct Data_From_Client
{
	string head;
	string data;
};

int main() {
	// 初始化服务器本地套接字
	if (!initialize_socket())
		exit(-1);

	cout << "正在等待客户端的连接...." << endl;

	SOCKET client_socket;
	while ((client_socket = wait_for_connection()) != SOCKET_ERROR) {//循环接收客户端的请求
		thread thread(&connection_handler, local_socket, client_socket);
		thread.detach();
	}
	closesocket(local_socket);
	WSACleanup();
	return 0;
}

void connection_handler(SOCKET local_socket, SOCKET client_socket) {
	// 接受客户端发来的数据
	string data_rv = receive_data(client_socket);

	// 解析客户端请求数据
	string ret_p = parse(data_rv);

	// 向客户端发送控制指令
	if (ret_p != "") { // 如果不是STAT请求，即是DATA请求时，则对客户端发送返回的指令
		int WSAAPI ret_s = send_data(client_socket, ret_p);
		if (ret_s != SOCKET_ERROR) {
			cout << "\t共 " << ret_s << " 字节。\t";
		}
	}
	else {
		cout << "  客户端异常!\n\n";
	}

	// 关闭该客户的socket
	closesocket(client_socket);
}

/*
* 解析客户端的请求数据
*
* @returns 如果是DATA请求，生成对应的控制指令并返回; 如果是STAT请求，则返回空串
*/
string parse(string data)
{
	if (data.empty())
		return "";
	string ret; // 返回值

	// 解析
	Data_From_Client data_from_client;
	data_from_client.head = data.substr(0, 4);
	data_from_client.data = data.substr(4, data.size());
	cout << "请求数据:\n" << data << endl;
	cout << "共 " << data.size() << " 字节。\t\t(" << data_from_client.head << "请求)" << endl;
	data = data_from_client.data;
	if (data_from_client.head == "DATA") {
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
			空调：当前电源状态、设置的模式（制冷还是制热）、温度、风速、。
		*/
		cout << "---------------------------------------------" << endl;
		cout << "根据解析结果发送控制信息: " << endl;
		ret += "SET";
		if (data[0] == '0') { // 无人
			cout << "关闭所有设备\n";
			ret += "1";
		}
		else ret += "0";
		if (data[0] == '1' && data[9] == '1') { // 有人且空调开启
			cout << "关闭门窗\n";
			ret += "1";
		}
		else ret += "0";
		if (data[0] == '1' && data[16] == '0') { // 有人且窗帘关闭
			cout << "打开卧室灯光\n";
			ret += "1";
		}
		else ret += "0";
		if (data[0] == '1' && data[16] == '1') { // 有人且窗帘打开
			cout << "关闭卫生间灯光\n";
			ret += "1";
		}
		else ret += "0";
		if (data[0] == '1') { // 有人
			string tempe_str = string(1, data[10]) + string(1, data[11]) + '.' + string(1, data[12]);
			float t;
			sscanf(tempe_str.c_str(), "%f", &t);
			cout << "温度为 " + tempe_str + " ℃，";
			if (t > 30) { // 且温度高于30度
				cout << "开启空调制冷，设定温度为24度，风速为中风\n";
				ret += "10242";
			}
			else if (t < 15) { // 且温度低于15度
				cout << "开启空调制热，设定温度为26度，风速为高风\n";
				ret += "11263";
			}
			else {
				cout << "空调维持原状态\n";
				ret += "0XXXX";
			}
		}
		else ret += "0XXXX";
	}
	else if (data_from_client.head == "STAT") {
		cout << "---------------------------------------------" << endl;
		cout << "解析客户端发送过来的信息: " << endl;
		if (data == "ACK")
			cout << "客户端回复：指令执行成功！ 此次会话终止！";
		else if (data == "ERR") {
			cout << "客户端回复：指令执行失败！";
		}
		return "";
	}

	return ret;
}

// 初始化服务器本地套接字，将其置为监听状态
bool initialize_socket() {
	//使用Windows系统API
	WSADATA wsadata;
	if (WSAStartup(0x0202, &wsadata) != 0)
		return false;

	// 创建socket套接字
	local_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (local_socket == INVALID_SOCKET)
	{
		cout << "服务器Socket创建失败！" << endl;
		return false;
	}
	cout << "服务器Socket创建成功！" << endl;

	/*
	* 填写初始化套接字所需的信息
	* sockaddr_in结构体：可以存储一套网络地址（包括IP与端口）,此处存储本机IP地址与本地的一个端口
	*/
	struct sockaddr_in local_addr;
	local_addr.sin_family = AF_INET;
	local_addr.sin_port = htons(PORT);  //绑定特定端口
	local_addr.sin_addr.s_addr = INADDR_ANY; //绑定服务器全部网卡的IP地址

	/*
	* 初始化套接字
	* bind()： 将一个网络地址与一个套接字绑定，此处将服务器本地地址绑定到一个套接字上
	*/
	int res = bind(local_socket, (struct sockaddr*)&local_addr, sizeof(local_addr));
	if (res == SOCKET_ERROR)
	{
		cout << "Socket初始化失败！" << endl;
		return false;
	}
	cout << "Socket初始化成功！" << endl;
	print_all_interface_ip();

	/*
	* listen()函数：将指定套接字置为监听状态
	*/
	listen(local_socket, MAXIMUM_CONNECTION);
	return true;
}

// 阻塞等待客户端的连接请求，连接发生后返回客户端的Socket
SOCKET wait_for_connection() {
	SOCKET s;
	// 创建一个sockaddr_in结构体，用来存储客户机的地址
	struct sockaddr_in client_addr;
	socklen_t len = sizeof(client_addr);
	// accept()函数：阻塞运行，直到收到某一客户机的连接请求，并返回客户机的描述符
	s = accept(local_socket, (struct sockaddr*)&client_addr, &len);
	if (s == SOCKET_ERROR)
	{
		cout << "无法为客户端创建Socket!\n" << endl;
		return SOCKET_ERROR;
	}
	// 输出客户机的信息
	string ip = inet_ntoa(client_addr.sin_addr);
	cout << "\n\n\n\n======================================================================" << endl;
	connection_cnt++;
	cout << "接收到第 " << connection_cnt << " 个连接请求! 源主机IP: " << ip << endl;

	return s;
}

// 从Socket中接收数据并返回
string receive_data(SOCKET s) {
	char buff[2048] = { 0 };
	int size = recv(s, buff, sizeof(buff), 0);
	string recv_data;
	recv_data.assign(buff, buff + size);

	return recv_data;
}

// 向客户端发送数据
int WSAAPI send_data(SOCKET s, string data) {
	cout << "\n向客户端发送控制数据： " + data;
	return send(s, data.c_str(), data.size(), 0); //返回给客户端数据
}

void print_all_interface_ip() {
	char host[50];
	gethostname(host, sizeof(host));
	struct hostent* host_entry = gethostbyname(host);
	cout << "网络适配器接口列表：\n";
	int i = 0;
	while (host_entry->h_addr_list[i] != 0) {
		printf("  网卡 %d. IPv4: %s\n", i + 1, inet_ntoa(*((struct in_addr*)host_entry->h_addr_list[i])));
		i++;
	}
}