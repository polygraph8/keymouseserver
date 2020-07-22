#pragma once
#include <Gdiplus.h>
#include <atlimage.h>
#include <atlstr.h>
#include <time.h>
#include <sys/timeb.h>

#include <unordered_map>
#include <vector>
#include <string>
#include <regex>

using namespace Gdiplus;
using namespace std;

typedef struct COMMAND {
	string op; ;
	int sleeptime;
	int x1, x2, y1, y2;
	int x, y;
	string text;
};


#define SCRIPTMAP unordered_map<string, vector<string>> 
#define COMMANDLISTMAP unordered_map<string, vector<COMMAND>> 

SCRIPTMAP scriptmap{};
COMMANDLISTMAP commandlistmap{};


void dumpstr(char* s) {
	printf("\n--- %s  ---\n", s);
	for (int i = 0; i < strlen(s); i++) {
		printf("%d %c 0x%x ", i, s[i], s[i]);
	}
}
void dumpmap(SCRIPTMAP scriptmap) {
	printf("\ndump scriptmap : %d\n", scriptmap.size());
	
	for (auto& x : scriptmap) {
		cout << "-" << x.first << "-" << endl;
		for (auto& y : x.second) {
			cout << y << endl;
		}
	}
}




void dumpcommand(vector<COMMAND> commandlist) {
	printf("\ndump commandlist  \n");
	for (auto& x : commandlist) {
		cout << "-" << x.op << "-" << endl;
	}
}

void string_replace(string&s1, const string&s2, const string&s3)
{
	string::size_type pos = 0;
	string::size_type a = s2.size();
	string::size_type b = s3.size();
	while ((pos = s1.find(s2, pos)) != string::npos)
	{
		s1.replace(pos, a, s3);
		pos += b;
	}
}

vector<string> string_split(const string& in, const string& delim) {
	std::regex re{ delim };
	// 调用 std::vector::vector (InputIterator first, InputIterator last,const allocator_type& alloc = allocator_type())
	// 构造函数,完成字符串分割
	return std::vector<std::string> {
		std::sregex_token_iterator(in.begin(), in.end(), re, -1),
			std::sregex_token_iterator()
	};
}
 

COMMAND get_command(string s) {
	COMMAND command{};
//	printf("get_command \n");
	vector<string>  arr = string_split(s, "::");

	for (auto& x : arr){
//		cout << "-"<< x << "-" << endl;
	}
	command.op = arr[0];
	if (arr.size() == 1)
		return command;
	vector<string> args{};
	args = string_split(arr[1], ",");
	int i = args.size();

	if (command.op == "clickAt" || command.op == "doubleClick") {

		command.x = atoi(args[0].c_str()); 
		command.y = atoi(args[1].c_str());
	}
	if (command.op == "Sleep") {
		command.sleeptime = atoi(args[0].c_str());
	}
	if (command.op == "Input" || command.op=="Winexec") {
		command.text = args[0];
	}
	if (command.op == "pullUp") {
		command.x1 = atoi(args[0].c_str());
		command.y1 = atoi(args[1].c_str());
		command.x2 = atoi(args[2].c_str());
		command.y2 = atoi(args[3].c_str());
	}
	return command;
}

vector<COMMAND> get_command_list(string whatisup="steps") {
//	printf("=========get_command_list %s\n", whatisup.c_str());
	vector<COMMAND> commandv{};
	COMMAND command;
	vector<string> ops = scriptmap[whatisup];
	vector<string> ops2 = scriptmap["whatisup"];
	int i = 0;
	for (auto op : ops) {
		if (scriptmap[op].size() > 0) {
			vector<COMMAND> op_commandv = get_command_list(op);
			commandv.insert(commandv.end(), op_commandv.begin(),op_commandv.end());
		}
		else {
			command = get_command(op);
			commandv.push_back(command);
//			cout << ops.size()<<"-" <<i++ <<":" << op << endl;
		}
	}
//	printf("=========get_command_list result: %s\n", whatisup.c_str());

//	for (auto c : commandv) {
//		cout << c.op << endl;
//	}
	return commandv;
}


void getcommandlistmap(SCRIPTMAP scriptmap) {
	printf("\ndump scriptmap \n");
	for (auto& x : scriptmap) {
		string key = x.first;
//		cout << "-" << x.first << "-" << endl;
		commandlistmap[key] = get_command_list(key);
		printf("%s has total %d ops\n", key.c_str(), commandlistmap[key].size());
	}
}

CString m_allowip;
CString m_controlip;
int m_deltax=0, m_deltay=0;

void ReadIniFile()
{
	WCHAR path[2000];
	GetModuleFileName(NULL, path, MAX_PATH);        //获取到完整路径如：E:\Tools\qq.exe
	GetCurrentDirectory(1024, path);
	CString cspath = path;
	cspath = cspath + _T("\\keymouse2.ini");
	*wcsrchr(path, '\\') = '\0';
	::GetPrivateProfileString(_T("ip"), _T("allowip"), _T(""), m_allowip.GetBuffer(1000000), 1000000, cspath);
	::GetPrivateProfileString(_T("ip"), _T("controlip"), _T(""), m_controlip.GetBuffer(100), 100, cspath);
	m_deltax = ::GetPrivateProfileInt(_T("coordinator"), _T("deltax"),  0, cspath);
	m_deltay = ::GetPrivateProfileInt(_T("coordinator"), _T("deltay"), 0, cspath);

	return;
}

void ReadSciprtFile()
{
	char s[10024];
	string key;
	vector<string> ops;
	FILE* f;
	fopen_s(&f, "script.ini", "r");
	if (f != NULL){
		key = "";
		while(!feof(f)){
			fgets(s, 1024, f);
//			dumpstr(s);
			if (strchr(s, '[') != NULL && strchr(s, ']') != NULL) {
				if (key !="")
					scriptmap[key] = ops;
				key = s;

				string_replace(key, "[", "");
				string_replace(key, "]", "");
				string_replace(key, "\xa", "");
				
				string_replace(key, " ", "");
				printf("commandlist %s\n", key.c_str());
				ops = {};
			}
			else {
				if (*s == '#')
					continue;
				string v = s;
				string_replace(v, "\xa", "");
				
				//string_replace(v, "#", "\0");

				if (v !="")
				   ops.push_back(v);
				//				get_command(v); 
			}
		}
		scriptmap[key] = ops;
		fclose(f);
	}
	printf("\ndump scriptmap : %d\n", scriptmap.size());
	getcommandlistmap(scriptmap);

//	printf("\ndump scriptmap : %d\n", scriptmap.size());
/*
	vector<string> ops2 = scriptmap["whatisup1"];
	ops2 = scriptmap["whatisup2"];
	ops2 = scriptmap["whatisup3"];
	printf("\ndump scriptmap : %d\n", scriptmap.size());
	printf("----------------dump map ------");
	dumpmap(scriptmap);


	dumpcommand(commandlistmap["InputPincodeAndEnter"]);
	dumpcommand(commandlistmap["openAnroidSimulator"]);
	dumpcommand(commandlistmap["steps"]);
*/
	return;
}

