//#pragma once
//
//
//
//
//
//
//class miniUDP {
//private:
//	enum Status {
//		status_none = 0,
//		status_client = 1,
//		status_server = 2
//	};
//
//	short _cur_status;
//	int sock;
//	struct sockaddr_in addr;
//	int _port;
//	WSAData wsaData;
//
//
//	/* ----------------   CLIENT   --------------*/
//	std::string _server_ip;
//	uint8_t self_id;
//	struct timeval tv;
//	struct sockaddr_in addr_server;
//
//
//	/* ----------------   SERVER   --------------*/
//	struct sockaddr_in client_addr_list[10];
//	short client_num;
//
//	// setupの時のみ使用
//	int sock_temp;
//	struct sockaddr_in addr_temp;
//
//
//public:
//	inline void setStatus(short status) { _cur_status = status; }
//	inline short getStatus() { return _cur_status; }
//
//	inline void setup(int port, std::string server_ip) {
//		switch (_cur_status) {
//		case status_client:
//			/* ----------------   CLIENT   --------------*/
//			WSAStartup(MAKEWORD(2, 0), &wsaData);   //MAKEWORD(2, 0)はwinsockのバージョン2.0ってこと
//			sock = socket(AF_INET, SOCK_DGRAM, 0);  //AF_INETはIPv4、SOCK_DGRAMはUDP通信、0は？
//
//			tv.tv_sec = 0;
//			tv.tv_usec = 10;
//
//			std::random_device rnd;     // 非決定的な乱数生成器を生成
//			std::mt19937 mt(rnd());     //  メルセンヌ・ツイスタの32ビット版、引数は初期シード値
//			std::uniform_int_distribution<> rand100(200, 250);        // [0, 99] 範囲の一様乱数
//			self_id = rand100(mt);
//
//			_server_ip = server_ip;
//			_port = port;
//
//			addr_server.sin_family = AF_INET;  //IPv4
//			addr_server.sin_port = htons(_port);   //通信ポート番号設定
//			addr_server.sin_addr.S_un.S_addr = inet_addr(_server_ip.c_str()); // 送信アドレスを127.0.0.1に設定
//
//			if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&tv, sizeof(tv)) < 0) { perror("Error"); }
//			break;
//
//
//		case status_server:
//			/* ----------------   SERVER   --------------*/
//			client_num = 0;
//			_port = port;
//			/* 10秒でタイムアウトするようにします */
//			tv.tv_sec = 0;
//			tv.tv_usec = 100000;
//
//
//			//setup用の設定
//			WSAStartup(MAKEWORD(2, 0), &wsaData);   //MAKEWORD(2, 0)はwinsockのバージョン2.0ってこと
//			sock_temp = socket(AF_INET, SOCK_DGRAM, 0);
//			addr_temp.sin_family = AF_INET;
//			addr_temp.sin_port = htons(_port);
//			addr_temp.sin_addr.s_addr = INADDR_ANY;
//			bind(sock_temp, (struct sockaddr *)&addr_temp, sizeof(addr_temp));
//
//			u_long val = 1;
//			ioctlsocket(sock_temp, FIONBIO, &val);// ここで、ノンブロッキングに設定しています
//
//			if (setsockopt(sock_temp, SOL_SOCKET, SO_RCVTIMEO, (char*)&tv, sizeof(tv)) < 0) {
//				perror("Error");
//			}
//			break;
//		}
//	}
//
//	inline void listten_bind() {
//		switch (_cur_status) {
//		case status_client:
//			//データを送信
//			char data = self_id;
//			sendto(sock, &data, 1, 0, (struct sockaddr *)&addr_server, sizeof(addr_server));
//			break;
//
//		case status_server:
//			char senderstr[16];
//			char data;
//			int addrlen = sizeof(addr_temp);
//			recvfrom(sock_temp, &data, 1, 0, (struct sockaddr *)&addr_temp, &addrlen);
//			uint8_t u_data = (uint8_t)data;
//
//			printf("recv data: %d, address=%d\n", u_data, ntohs(addr_temp.sin_addr.S_un.S_addr));
//
//			if (u_data > 200) {
//				printf("new client ->%d\n", addr_temp.sin_addr.S_un.S_addr);
//				// アドレス等格納
//				client_addr_list[client_num].sin_family = AF_INET;  //IPv4
//				client_addr_list[client_num].sin_port = htons(_port);   //通信ポート番号設定
//				client_addr_list[client_num].sin_addr.S_un.S_addr = addr_temp.sin_addr.S_un.S_addr; // 送信アドレスを127.0.0.1に設定
//				client_num++;
//			}
//		}
//	}
//
//	inline void start_main_process() {
//		switch (_cur_status) {
//		case status_client:
//			// setupの終了処理
//			closesocket(sock);
//			WSACleanup();
//
//			//start!!
//			WSAStartup(MAKEWORD(2, 0), &wsaData);   //MAKEWORD(2, 0)はwinsockのバージョン2.0ってこと
//			sock = socket(AF_INET, SOCK_DGRAM, 0);  //AF_INETはIPv4、SOCK_DGRAMはUDP通信、0は？
//			addr.sin_family = AF_INET;  //IPv4
//			addr.sin_port = htons(_port);   //通信ポート番号設定
//			addr.sin_addr.S_un.S_addr = INADDR_ANY; // INADDR_ANYはすべてのアドレスからのパケットを受信する
//			bind(sock, (struct sockaddr *)&addr, sizeof(addr));
//
//			u_long val = 1;
//			ioctlsocket(sock, FIONBIO, &val);// ここで、ノンブロッキングに設定しています。
//			break;
//
//		case status_server:
//			//setupの後処理
//			closesocket(sock_temp);
//			WSACleanup();
//			//start!!
//			WSAStartup(MAKEWORD(2, 0), &wsaData);   //MAKEWORD(2, 0)はwinsockのバージョン2.0ってこと
//			sock = socket(AF_INET, SOCK_DGRAM, 0);  //AF_INETはIPv4、SOCK_DGRAMはUDP通信、0は？
//			break;
//		}
//	}
//
//
//
//
//	/*-----------    CLIENT     --------------*/
//	inline uint8_t receive_byte() {
//		if (_cur_status != status_client) return 0xFF;
//
//		char buf = 0xFF;
//		if (recv(sock, &buf, 1, 0) < 0) return 0xFF;
//		return (uint8_t)buf;
//	}
//
//	inline uint8_t getSelfID() { return self_id; }
//
//
//
//	/*-----------    SERVER     --------------*/
//	inline void send_all(uint8_t data_) {
//		if (_cur_status != status_server) return;
//
//		char data = data_;
//		for (short i = 0; i < client_num; i++) {
//			sendto(sock, &data, 1, 0, (struct sockaddr *)&client_addr_list[i], sizeof(client_addr_list[i]));
//		}
//	}
//
//	inline short getClientNum() { return client_num; }
//	inline void close() { closesocket(sock); WSACleanup(); }
//};
//
//
//
//
////
////
////class client {
////private:
////	int sock;
////	struct sockaddr_in addr;
////	WSAData wsaData;
////	int _port;
////	std::string _server_ip;
////	uint8_t self_id;
////	struct timeval tv;
////	struct sockaddr_in addr_server;
////
////public:
////	void setup(int port, std::string server_ip) {
////		WSAStartup(MAKEWORD(2, 0), &wsaData);   //MAKEWORD(2, 0)はwinsockのバージョン2.0ってこと
////		sock = socket(AF_INET, SOCK_DGRAM, 0);  //AF_INETはIPv4、SOCK_DGRAMはUDP通信、0は？
////
////		tv.tv_sec = 0;
////		tv.tv_usec = 10;
////
////		std::random_device rnd;     // 非決定的な乱数生成器を生成
////		std::mt19937 mt(rnd());     //  メルセンヌ・ツイスタの32ビット版、引数は初期シード値
////		std::uniform_int_distribution<> rand100(200, 250);        // [0, 99] 範囲の一様乱数
////		self_id = rand100(mt);
////
////		_server_ip = server_ip;
////		_port = port;
////
////		addr_server.sin_family = AF_INET;  //IPv4
////		addr_server.sin_port = htons(_port);   //通信ポート番号設定
////		addr_server.sin_addr.S_un.S_addr = inet_addr(_server_ip.c_str()); // 送信アドレスを127.0.0.1に設定
////
////		if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&tv, sizeof(tv)) < 0) { perror("Error"); }
////	}
////
////
////	inline void WaitForConnected() {
////		//データを送信
////		char data = self_id;
////		sendto(sock, &data, 1, 0, (struct sockaddr *)&addr_server, sizeof(addr_server));
////	}
////
////
////	inline void start_main_process() {
////		// setupの終了処理
////		closesocket(sock);
////		WSACleanup();
////
////		//start!!
////		WSAStartup(MAKEWORD(2, 0), &wsaData);   //MAKEWORD(2, 0)はwinsockのバージョン2.0ってこと
////		sock = socket(AF_INET, SOCK_DGRAM, 0);  //AF_INETはIPv4、SOCK_DGRAMはUDP通信、0は？
////		addr.sin_family = AF_INET;  //IPv4
////		addr.sin_port = htons(_port);   //通信ポート番号設定
////		addr.sin_addr.S_un.S_addr = INADDR_ANY; // INADDR_ANYはすべてのアドレスからのパケットを受信する
////		bind(sock, (struct sockaddr *)&addr, sizeof(addr));
////
////		u_long val = 1;
////		ioctlsocket(sock, FIONBIO, &val);// ここで、ノンブロッキングに設定しています。
////	}
////
////
////
////	inline uint8_t receive_byte() {
////		char buf = 0xFF;
////		if (recv(sock, &buf, 1, 0) < 0) return 0xFF;
////		return (uint8_t)buf;
////	}
////
////	inline uint8_t getSelfID() { return self_id; }
////	inline void close() { closesocket(sock); WSACleanup(); }
////};
////
////
////
////int main(int argc, char **argv) {
////	client udp0;
////	udp0.setup(50007, "127.0.0.1");
////
////	std::cout << "connecting....\n";
////
////	for (int i = 0; i < 10; i++) {
////		udp0.WaitForConnected();
////		Sleep(50);
////	}
////
////	udp0.start_main_process();
////
////	uint8_t data;
////
////	while (1) {
////		/*std::string rdata = udp0.recieve();
////		printf("recv:%s\n", rdata.c_str());*/
////		data = udp0.receive_byte();
////		printf("### %d ###\n", data);
////		Sleep(30);
////	}
////	return 0;
////}
////
////
////
////
////class server {
////private:
////	int sock;
////	struct sockaddr_in addr;
////	WSAData wsaData;
////	struct sockaddr_in client_addr_list[10];
////	short client_num;
////	int _port;
////	struct timeval tv;
////
////	// setupの時のみ使用
////	int sock_temp;
////	struct sockaddr_in addr_temp;
////
////
////public:
////	inline void init(int port) {
////		client_num = 0;
////		_port = port;
////		/* 10秒でタイムアウトするようにします */
////		tv.tv_sec = 0;
////		tv.tv_usec = 100000;
////
////
////		//setup用の設定
////		WSAStartup(MAKEWORD(2, 0), &wsaData);   //MAKEWORD(2, 0)はwinsockのバージョン2.0ってこと
////		sock_temp = socket(AF_INET, SOCK_DGRAM, 0);
////		addr_temp.sin_family = AF_INET;
////		addr_temp.sin_port = htons(_port);
////		addr_temp.sin_addr.s_addr = INADDR_ANY;
////		bind(sock_temp, (struct sockaddr *)&addr_temp, sizeof(addr_temp));
////
////		u_long val = 1;
////		ioctlsocket(sock_temp, FIONBIO, &val);// ここで、ノンブロッキングに設定しています
////
////		if (setsockopt(sock_temp, SOL_SOCKET, SO_RCVTIMEO, (char*)&tv, sizeof(tv)) < 0) {
////			perror("Error");
////		}
////	}
////
////
////	inline void ListenClients() {
////		char senderstr[16];
////		char data;
////		int addrlen = sizeof(addr_temp);
////		recvfrom(sock_temp, &data, 1, 0, (struct sockaddr *)&addr_temp, &addrlen);
////		uint8_t u_data = (uint8_t)data;
////
////		printf("recv data: %d, address=%d\n", u_data, ntohs(addr_temp.sin_addr.S_un.S_addr));
////
////		if (u_data > 200) {
////			printf("new client ->%d\n", addr_temp.sin_addr.S_un.S_addr);
////			// アドレス等格納
////			client_addr_list[client_num].sin_family = AF_INET;  //IPv4
////			client_addr_list[client_num].sin_port = htons(_port);   //通信ポート番号設定
////			client_addr_list[client_num].sin_addr.S_un.S_addr = addr_temp.sin_addr.S_un.S_addr; // 送信アドレスを127.0.0.1に設定
////			client_num++;
////		}
////	}
////
////
////	inline void start_main_process() {
////		//setupの後処理
////		closesocket(sock_temp);
////		WSACleanup();
////
////
////		//start!!
////		WSAStartup(MAKEWORD(2, 0), &wsaData);   //MAKEWORD(2, 0)はwinsockのバージョン2.0ってこと
////		sock = socket(AF_INET, SOCK_DGRAM, 0);  //AF_INETはIPv4、SOCK_DGRAMはUDP通信、0は？
////
////	}
////
////	inline void send_all(uint8_t data_) {
////		char data = data_;
////		for (short i = 0; i < client_num; i++) {
////			sendto(sock, &data, 1, 0, (struct sockaddr *)&client_addr_list[i], sizeof(client_addr_list[i]));
////		}
////	}
////
////	inline short getClientNum() { return client_num; }
////	inline void close() { closesocket(sock); WSACleanup(); }
////};
////
////
////
////int main(int argc, char *argv[]) {
////	server udp0;
////	udp0.init(50007);
////	bool loop = true;
////
////	while (loop) {
////		udp0.ListenClients();
////		Sleep(50);
////		if (udp0.getClientNum() > 1) loop = false;
////	}
////
////	udp0.start_main_process();
////
////	const char aa[] = "hello!";
////	for (uint8_t i = 0; i < 255; i++) {
////		Sleep(300);
////		udp0.send_all(i);
////		//udp0.send(aa);
////		printf("send->%d\n", i);
////	}
////	udp0.close();
////	return 0;
////}
////
