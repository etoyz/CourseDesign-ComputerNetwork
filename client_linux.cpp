#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <cstring>

using namespace std;

#define PORT 81 // 端口
char ip[20];	// 存储服务器IP
int server_socket; // 服务器Socket
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
    char ip[20];
	cout << "请输入IP地址：";
	cin >> ip;
	char msg[] = "DATA02051091012701110";
	if (send_to_server(81, ip, msg))
		cout << "发送传感器数据：" << msg << endl;
	else
		cout << "Sent failed!" << endl;
	handle_data_from_server();
	close(server_socket);
	return 0;
}


bool send_to_server(int PortNo, char* IPAddress, char* Msg)
{
	cout << Msg;
	sockaddr_in target;

	target.sin_family = AF_INET;
	target.sin_port = htons(PortNo);
	target.sin_addr.s_addr = inet_addr(IPAddress);

	server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (server_socket < 0)
	{
		return false;
	}

	if (connect(server_socket, (sockaddr*)&target, sizeof(target)) < 0)
	{
		return false;
	}
	else {
		write(server_socket, Msg, strlen(Msg));

		char buff[100] = { 0 };
		int size = read(server_socket, buff, strlen(buff));
		cout << buff << endl;

		return true;
	}
}

void handle_data_from_server()
{
	char buff[100] = { 0 };
	int size = read(server_socket, buff, sizeof(buff));
	cout << "接受到控制信息：" << buff << endl;
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