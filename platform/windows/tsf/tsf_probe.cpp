// Lakakey TSF v0.6: diagnostic only, NOT an installed input method.
#define UNICODE
#define _UNICODE
#include <windows.h>
#include <msctf.h>
#include <cstdio>
int wmain(){
 HRESULT hr=CoInitializeEx(nullptr,COINIT_APARTMENTTHREADED);
 if(FAILED(hr)){std::wprintf(L"COM initialization failed: 0x%08lx\n",(unsigned long)hr);return 1;}
 ITfThreadMgr* manager=nullptr;
 hr=CoCreateInstance(CLSID_TF_ThreadMgr,nullptr,CLSCTX_INPROC_SERVER,IID_ITfThreadMgr,(void**)&manager);
 if(FAILED(hr)){std::wprintf(L"TSF thread manager unavailable: 0x%08lx\n",(unsigned long)hr);CoUninitialize();return 2;}
 TfClientId client=TF_CLIENTID_NULL;
 hr=manager->Activate(&client);
 if(SUCCEEDED(hr)){
  std::wprintf(L"TSF manager activated, client id: %lu\n",(unsigned long)client);
  manager->Deactivate();
 } else std::wprintf(L"TSF activation failed: 0x%08lx\n",(unsigned long)hr);
 manager->Release();CoUninitialize();return SUCCEEDED(hr)?0:3;
}
