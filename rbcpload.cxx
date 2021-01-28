#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <cctype>
#include <algorithm>

#include "rbcp.h"


std::vector<std::string> strsplit(std::string str)
{
	std::vector<std::string> vstr;
	std::string separator = std::string(" ");
	int separator_len = separator.length();
	size_t offset = std::string::size_type(0);
	while(true) {
		size_t pos = str.find(separator, offset);
		if (pos == std::string::npos) {
			if (str.substr(offset).length() > 0) {
				vstr.push_back(str.substr(offset));
			}
			break;
		} else {
			if (pos - offset > 0) {
				vstr.push_back(str.substr(offset, pos - offset));
			}
			offset = pos + separator_len;
		}
		
	}

	return vstr;
}

int readfile(char *filename)
{
	int port = 0x1234;
	std::string host("192.168.10.16");

	std::ifstream ifs(filename);
	std::string str;
	if (ifs.fail()) {
		std::cerr << "Fail to read " << filename << std::endl;
		return -1;
	}
	while (std::getline(ifs, str)) {
		//std::cout << "> " << str << std::endl;
		std::vector<std::string> list = strsplit(str);
		/*
		std::cout << "#";
		for (unsigned int i = 0 ; i < list.size() ; i++) {
			std::cout << "/" << list[i];
		}
		std::cout << "/" << std::endl;
		*/

		for (unsigned int i = 0 ; i < list.size() ; i++) {
			if ((list.size() > i + 1) &&
				((list[i] == "HOST") || list[i] == "host")) {
				host = list[++i];
				std::cout << "HOST : " << host << std::endl;
			}
			if ((list.size() > i + 1) &&
				((list[i] == "PORT") || list[i] == "port")) {
				std::istringstream ss(list[++i]);
				ss >> port;
				std::cout << "PORT : " << port << std::endl;
			}
		}

		if (list.size() > 1) {
		try {
			unsigned int addr = std::stoi(list[0], NULL, 0);
			std::cout << std::setw(8) << addr << " : ";
			std::vector<int> values;
			for (unsigned int i = 1 ; i <  list.size() ; i++) {
				try {
					values.push_back(std::stoi(list[i], NULL, 0));
					std::cout << " " << values.back();
				} catch (const std::exception &e) {
					std::cout << "#E " << e.what() << std::endl;
				}
			}
			std::cout << std::endl;
			if (values.size() > 0) {
				char buf[values.size() + 16];
				for (unsigned int i = 0 ; i < values.size() ; i++) {
					buf[i] = static_cast<char>(values[i] & 0xff);
				}
				int sock = rbcp_open(const_cast<char *>(host.c_str()), port);
				rbcp_write(sock, buf, addr, values.size());
				rbcp_close(sock);
			}

		} catch (const std::exception &e) {
		}
		}

	}

	return 0;
}

int main(int argc, char* argv[])
{
	for (int i = 1 ; i < argc ; i++) {
		readfile(argv[i]);
	}

	return 0;
}
