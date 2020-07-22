#pragma once
#include "mylog.h"
#include "config.h"
#include<winsock2.h>
#include <atlstr.h>
#include <Ws2tcpip.h>  

#include <stdio.h>
#include <time.h>
#include <sys/timeb.h>

#pragma comment(lib,"WS2_32")

void run_script_node(char* scriptnode, char* inputtext=NULL);
//================================
char req_act[200];
char req_name[200];
char req_oldroom[200]="";
char req_room[200];
char req_text[200];
char req_password[200];
char req_verificode[200];
char opresult[1024];
char myip[16] = "";
char response[256];


#define IsHexNum(c) ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f'))

char * CString2char(CString str) {

	CString str1 = str;
	int len = WideCharToMultiByte(CP_ACP, 0, str1, -1, NULL, 0, NULL, NULL);
	char ptxtTemp[1024];
	WideCharToMultiByte(CP_ACP, 0, str1, -1, ptxtTemp, len, NULL, NULL);
	return ptxtTemp;
}

CString Utf8ToStringT(LPSTR str)
{
	_ASSERT(str);
	USES_CONVERSION;
	WCHAR buf[1024];
	int length = MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0);
	ZeroMemory(buf, (length + 1) * sizeof(WCHAR));
	MultiByteToWideChar(CP_UTF8, 0, str, -1, buf, length);
	//	printf("%s \n", buf);
	return (CString(W2T(buf)));
}

//url编码字符串解码函数
CString UrlDecode(char* url)
{
	_ASSERT(url);
	USES_CONVERSION;
	LPSTR _url = url;
	int i = 0;
	int length = (int)strlen(_url);
	CHAR buf[1024];
	ZeroMemory(buf, length);
	LPSTR p = buf;
	char tmp[4];
	while (i < length)
	{
		if (i <= length - 3 && _url[i] == '%' && IsHexNum(_url[i + 1]) && IsHexNum(_url[i + 2]))
		{
			memset(tmp, 0, sizeof(tmp));
			memcpy(tmp, _url + i + 1, 2);
			sscanf_s(tmp, "%x", p++);
			i += 3;
		}
		else
		{
			*(p++) = _url[i++];
		}
	}
	*p = '\0';
	//return CString(buf);
	return Utf8ToStringT(buf);
}

//================================
long getMilliTimeStamp()
{
	timeb t;
	ftime(&t);
	return t.time * 1000 + t.millitm;
}

bool isSafeIP(sockaddr_in  addrlink) {
	WCHAR ip[16] = TEXT("");  //初始化是个好习惯
							  //const char *inet_ntop(int af, const void *src, char *dst, socklen_t cnt);
	if (InetNtop(AF_INET, &addrlink.sin_addr, ip, 100) == NULL)
	{
		printf("inetNtop error\n");;
		return false;
	}
	WCHAR ip2[20] = TEXT("");  //初始化是个好习惯
	wcscat_s(ip2, _T(","));  	wcscat_s(ip2, ip); // 兼容IP段	wcscat_s(ip2, _T(","));
	if (wcsstr(m_allowip, ip2) || wcsstr(m_allowip, L",0.0.0.0,"))
	{
		wprintf(L"allowip contains %s \r\n", ip);
		return true;
	}
	wprintf(L"allowip not contains %s \r\n", ip);
	return false;
}


bool isSafeControlIP(sockaddr_in  addrlink) {
	WCHAR ip[16] = TEXT("");  //初始化是个好习惯

							  //const char *inet_ntop(int af, const void *src, char *dst, socklen_t cnt);
	if (InetNtop(AF_INET, &addrlink.sin_addr, ip, 100) == NULL)
	{
		printf("inetNtop error\n");;
		return false;
	}
	WCHAR ip2[20] = TEXT("");  //初始化是个好习惯
	wcscat_s(ip2, _T(","));  	wcscat_s(ip2, ip); // 兼容IP段	wcscat_s(ip2, _T(","));

	if (wcsstr(m_controlip, ip2))
	{
		wprintf(L"controlip contains %s \r\n", ip);
		return true;
	}
	wprintf(L"controlip not contains %s \r\n", ip);
	return false;
}

void process_http_req(char*s);
bool is_controlip = false;

int runHttpServer()
{
	WSADATA swadata;
	WORD sockVersion = MAKEWORD(2, 0);//winsock的版本2.0
	WSAStartup(sockVersion, &swadata);//加载WInsock库，swadata返回库的信息

									  //创建套接字
	SOCKET s = ::socket(AF_INET, SOCK_STREAM, 0);

	if (s == -1)
	{
		printf("Creat scoket is failed!\n");
		::WSACleanup();
		system("pause");
		return 0;
	}
	//填充socaddr_in结构
	sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(80);//1024--65535的端口号
	sin.sin_addr.S_un.S_addr = INADDR_ANY;
	//绑定这个套接字到本地地址
	if (::bind(s, (SOCKADDR*)&sin, sizeof(sin)) == SOCKET_ERROR)
	{
		printf("bind is failed!\n");
		::WSACleanup();
		system("pause");
		return 0;
	}
	//监听模式
	if (::listen(s, 2) == SOCKET_ERROR)
	{
		printf("listen is failed!\n");
		::WSACleanup();
		system("pause");
		return 0;
	}
	unsigned long ul = 1;
//	::ioctlsocket(s, FIONBIO, &ul);
	printf("正在监听....\n");

	//循环接受连接请求

	//	char anser[] = "HTTP/1.1 200 OK\r\n\r\n<html><head><title></title></head><body>yours</body></html>\r\n";
	char anser[500000] = "";
	char anser0[] = "HTTP/1.1 200 OK\r\n\r\n";
	char anserpic0[200] = "HTTP/1.1 200 OK\r\nContent-Type: image/jpeg\r\nContent-Length: ";
	char anserpic1[200]	="\r\nAccept-Ranges: bytes\r\n";
	char anser1[] = "\r\n";

	char buf[1025];
	SOCKET client;
	while (1)
	{
		mylog("accepting ", "127.0.0.1");
		is_controlip = false;

		sockaddr_in addrlink;
		int Len = sizeof(addrlink);

		client = ::accept(s, (SOCKADDR*)&addrlink, &Len);

		if (client == INVALID_SOCKET)
		{
			//			printf("accept is failed!");
			Sleep(10);
			continue;
		}

		struct sockaddr_in myaddr;
		socklen_t len = sizeof(myaddr);

		getsockname(client, (struct sockaddr *)&myaddr, &len);


		//const char *inet_ntop(int af, const void *src, char *dst, socklen_t cnt);
		inet_ntop(AF_INET, &myaddr.sin_addr, myip, 16);
		printf(myip);
		printf("accept a socket");
		unsigned long ul2 = 1;
		::ioctlsocket(client, FIONBIO, &ul2);  //设置为非堵塞，避免扫描
		int n;
		int lop = 0;
		while ((n = ::recv(client, buf, 1024, 0)) == 0 && lop < 2) {
			Sleep(5);
			lop = lop + 1;
		};
		printf("\n recv %d bytes\n", n);
		if (n < 1) {
			n = 0;
		}
		buf[n] = '\0';

		WCHAR ip[16] = TEXT("0");  //初始化是个好习惯

								   //const char *inet_ntop(int af, const void *src, char *dst, socklen_t cnt);
		if (InetNtop(AF_INET, &addrlink.sin_addr, ip, 100) == NULL)
		{
			printf("inetNtop error\n");;
			closesocket(client);
			continue;
		}
		mylog(buf, CString2char(ip));
		wprintf(L"Incoming IP is : %s\n", ip);
		if (!isSafeIP(addrlink)) {
			strcpy_s(anser, anser0);
			strcat_s(anser, "ip denyed:");
			strcat_s(anser, CString2char(ip));
			strcat_s(anser, anser1);
			::send(client, anser, strlen(anser), 0);
			::closesocket(client);
			continue;
		}
		if (isSafeControlIP(addrlink))
			is_controlip = true;

								//		printf("客户:(%s)", inet_ntoa(addrlink.sin_addr));
		printf("\n%s\n", buf);
		if (n < 50) {
			::closesocket(client);
		}
		else {
			long t0 = getMilliTimeStamp();
			process_http_req(buf);

			long t1 = getMilliTimeStamp();
			if (strstr(opresult, "getscreen") != NULL)
			{
				struct _stat buf;
				char fn[100] = "c:/screen.png";
				int ret= _stat(fn, &buf);
				int filesize = 0;
				char filesizestr[100];

				if (ret != 0)
				{
					::closesocket(client);
					continue;
				}
				filesize = buf.st_size;
				strcpy_s(anser, anserpic0);
				sprintf_s(filesizestr, "%d", filesize);
				strcat_s(anser, filesizestr);
				strcat_s(anser, anserpic1);
				strcat_s(anser, anser1);
				::send(client, anser, strlen(anser), 0);

				FILE* fp; 
				fopen_s(&fp, fn, "rb");
				if (fp != NULL) {
					fread(anser, filesize, 1, fp);
					::send(client, anser, filesize, 0);
				}
				::closesocket(client);
				continue;
			}
			strcpy_s(anser, anser0);
			strcat_s(anser, opresult);
			strcat_s(anser, anser1);
			::send(client, anser, strlen(anser), 0);
			::closesocket(client);
			printf("current_request spend time : %d, %d ,%d \n", t1 - t0, t1, t0);
		}
	}
	::closesocket(s);
	::WSACleanup();
	return 0;
}



CString getdecodeurl(char*s) {
	char url[1024];
	char* pstart = s + 5;
	char* pend = strstr(s, " HTTP/");
	if (pend == NULL)
		return  CString("");
	int len = pend - pstart;
	strncpy_s(url, pstart, len);
	url[len] = 0;


	CString cs = UrlDecode(url);
	//	CString2char(cs);
	return cs;
}


bool verify(char* verifycode, char* name) {

	Sleep(1000);
	return false;
}
int login(char* req_name, char* req_password) {
	return 1;
}


//curl "http://127.0.0.1/act=textinput&text=1234567&room=第一财经直播室&name=18610817416"
int textinput_process( char* req_text, char *req_room, char *req_name) {

	HWND hwnd = FindWindow(NULL, L"夜神模拟器");
	if (hwnd == NULL)
	{
		run_script_node("openAnroidSimulator");
		run_script_node("OpenDouyin");
	}
	if (*req_room == '\x0' || strstr(req_oldroom, req_room) == NULL) {
	    run_script_node("SearchLiveRoom");
		strcpy_s(req_oldroom, req_room);
	}
	run_script_node("TextInput",req_text);
	return 1;
}
char* doRemoteOp(char *req_act, char *req_name, char *req_password, char * req_verificode, char* req_text,char *req_room ) {

	CString window_name = CString(req_name);
	CString opresult;
	window_name = window_name + _T("的工作台");

	if (strcmp(req_act, "textinput") == 0) {		// logined ,needverifycode
		textinput_process(req_text, req_room, req_name);

	}


	if (strcmp(req_act, "login") == 0) {		// logined ,needverifycode
		int ret = login(req_name, req_password);
		switch (ret) {
		case 0:
			memset(response, 0, 256);
			sprintf_s(response, "%s%s%s", "remote response:", "remote response:login success:", myip);
			return response;
		}
	}

	if (strcmp(req_act, "getscreen") == 0) {		// logined ,needverifycode
		memset(response, 0, 256);
		sprintf_s(response, "%s", "getscreen");
		return response;
	}

	if (strcmp(req_act, "verify") == 0) {  // launched , notluanched
		if (verify(req_verificode, req_name))
			return "remote response:verify success, launched";
		else
			return "remote response:verify failed, launched failed";
	}

	char rets[1024] = "remote response:unknown command:";
	strcat_s(rets, req_act);
	return rets;
}


// curl "http://127.0.0.1/act=textinput&text=1234567&room=第一财经直播室&name=18610817416" 

void parseUrlArgAndOp(char *str) {
	printf(myip); printf("\n");
	*req_act = *req_name = *req_password = *req_verificode = *req_room= *req_text = '\0';
	char * pch, *pch2, *pch3;
	char * nexttoken, *nexttoken2;
	pch = strtok_s(str, "&", &nexttoken);
	while (pch != NULL)
	{
		printf("%s\n", pch);
		pch2 = strtok_s(pch, "=", &nexttoken2);
		if (pch2 != NULL)
		{

			//			printf("%s\n", pch2);
			pch3 = strtok_s(NULL, "=", &nexttoken2);
			if (pch3 != NULL) {       //strncpy_s(s, sizeof(s), "12345678901", sizeof(s) - 1); //
				if (strcmp(pch2, "act") == 0) 	strncpy_s(req_act, sizeof(req_act), pch3, sizeof(req_act) - 1);
				if (strcmp(pch2, "name") == 0) 	strncpy_s(req_name, sizeof(req_name), pch3, sizeof(req_name) - 1);
				if (strcmp(pch2, "pwd") == 0) 	strncpy_s(req_password, sizeof(req_password), pch3, sizeof(req_password) - 1);
				if (strcmp(pch2, "text") == 0) 	strncpy_s(req_text, sizeof(req_text), pch3, sizeof(req_text) - 1);
				if (strcmp(pch2, "room") == 0) 	strncpy_s(req_room, sizeof(req_room), pch3, sizeof(req_room) - 1);
				if (strcmp(pch2, "verifycode") == 0) strncpy_s(req_verificode, sizeof(req_verificode), pch3, sizeof(req_verificode) - 1);
			}
		}
		pch = strtok_s(NULL, "&", &nexttoken);
	}
	char * p = doRemoteOp(req_act, req_name, req_password, req_verificode,req_text ,req_room);  // 做实际的操作！
	strcpy_s(opresult, p);
	printf(opresult); printf("\n");
	return;
}

void process_http_req(char*s) {
	CString url = getdecodeurl(s);
	char *p = CString2char(url) ; printf("%s\n", p);
	parseUrlArgAndOp(p);

	//	Sleep(4000);
}

