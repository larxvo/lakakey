// Manual event smoke-test: LakakeyMascotEvent.exe welcome|vn|en|unicode|telex|vni|typing|success|notice|follow|sleep|error
#define UNICODE
#define _UNICODE
#include <windows.h>
#include <cwchar>
#include <string>
int wmain(int argc,wchar_t** argv){
 if(argc!=2)return 2;
 const wchar_t* names[]={L"welcome",L"vn",L"en",L"unicode",L"telex",L"vni",L"typing",L"success",L"notice",L"follow",L"sleep",L"error"};
 DWORD event=0;
 for(DWORD i=0;i<12;i++)if(_wcsicmp(argv[1],names[i])==0)event=i+1;
 if(!event)return 3;
 HWND hwnd=FindWindowW(L"LakakeyMascotRuntimeV06",nullptr);
 if(!hwnd)return 4;
 COPYDATASTRUCT packet{0x4C4B4D35,sizeof(event),&event};
 DWORD_PTR result=0;
 if(!SendMessageTimeoutW(hwnd,WM_COPYDATA,0,(LPARAM)&packet,SMTO_ABORTIFHUNG|SMTO_BLOCK,250,&result))return 5;
 return result?0:6;
}
