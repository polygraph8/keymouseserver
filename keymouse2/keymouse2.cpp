// keymouse2.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "keymouse2.h"
#include "webserver.h"
#include "mylib.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// Ψһ��Ӧ�ó������

CWinApp theApp;

using namespace std;

void run_script_node(char* scriptnode,char* inputtext) {
	int deltax = m_deltax;
	int deltay = m_deltay;
	vector<COMMAND> commandlist = commandlistmap[scriptnode];
	for (COMMAND command : commandlist)
	{
		command.x += deltax;
		command.y += deltay;

		if (command.op == "clickAt") 		clickAt(command.x, command.y);
		if (command.op == "doubleClick") 	doubleclickAt(command.x, command.y);
		if (command.op == "Sleep") 			Sleep(command.sleeptime);
		if (command.op == "pullUp") 		pullup(command.x1, command.y1, command.x2, command.y2);
		if (command.op == "mouseWheel")		mouseWheel(command.x, command.y);
		if (command.op == "Input")
			inputText3((char*)command.text.c_str(), inputtext);
		if (command.op == "Winexec") {
			char *p = (char*)command.text.c_str();
//			char path[] = "C:\\Program Files\\Nox\\bin\\Nox.exe";
//			p = path;
			WinExec(p, SW_SHOW);
		}

	}
}

void startSimulator() {
	int r = WinExec("C:\\Program Files\\Nox\\bin\\Nox.exe", SW_SHOW);
}

void run_script() {
//	startSimulator();
   	 run_script_node("openAnroidSimulator");
	 run_script_node("killAndroidSimulator");
	 run_script_node("steps");
	 run_script_node("TextInput");
	 run_script_node("killAndroidSimulator");
//	 run_script_node("TextInput");
//	 run_script_node("SearchLiveRoom");
//   run_script_node("loginAndSendVerifycode");

}
void test() {
	runHttpServer();
	//run_script();
//	startSimulator();
}

int main()
{
    int nRetCode = 0;
	ReadIniFile();
	ReadSciprtFile();
    HMODULE hModule = ::GetModuleHandle(nullptr);

    if (hModule != nullptr)
    {
        // ��ʼ�� MFC ����ʧ��ʱ��ʾ����
        if (!AfxWinInit(hModule, nullptr, ::GetCommandLine(), 0))
        {
            // TODO: ���Ĵ�������Է���������Ҫ
            wprintf(L"����: MFC ��ʼ��ʧ��\n");
            nRetCode = 1;
        }
        else
        {
			test();
            // TODO: �ڴ˴�ΪӦ�ó������Ϊ��д���롣
        }
    }
    else
    {
        // TODO: ���Ĵ�������Է���������Ҫ
        wprintf(L"����: GetModuleHandle ʧ��\n");
        nRetCode = 1;
    }

    return nRetCode;
}
