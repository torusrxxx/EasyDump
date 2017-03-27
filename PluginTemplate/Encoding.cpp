#include <Windows.h>
#include <string>

extern HMODULE hModule;

std::string ToUtf8(std::wstring text)
{
    size_t size = text.size() * 2;
    char* buffer = new char[size];
    memset(buffer, 0, size);
    while(WideCharToMultiByte(CP_UTF8, 0, text.c_str(), text.size(), buffer, size, NULL, NULL) == 0)
    {
        switch(GetLastError())
        {
        case ERROR_INSUFFICIENT_BUFFER:
            size = size + 64;
            delete[] buffer;
            buffer = new char[size];
            memset(buffer, 0, size);
            break;
        default:
            delete[] buffer;
            return "(error)";
        }
    }
    std::string returnVal = std::string(buffer, size);
    delete[] buffer;
    return returnVal;
}

std::wstring FromLCP(std::string text)
{
    size_t bufsize = text.size();
    wchar_t* buffer = new wchar_t[bufsize];
    memset(buffer, 0, bufsize * sizeof(wchar_t));
    while(MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, text.c_str(), text.size(), buffer, bufsize) == 0)
    {
        switch(GetLastError())
        {
        case ERROR_INSUFFICIENT_BUFFER:
            bufsize = bufsize + 64;
            delete[] buffer;
            buffer = new wchar_t[bufsize];
            memset(buffer, 0, bufsize * sizeof(wchar_t));
            break;
        default:
            delete[] buffer;
            return L"(error)";
        }
    }
    std::wstring returnVal = std::wstring(buffer, bufsize);
    delete[] buffer;
    return returnVal;
}

std::wstring FromResourceToUtf16(int id)
{

    wchar_t p[512];
    int len;
    memset(p, 0, sizeof(p));
    if((len = LoadString(hModule, id, (LPWSTR)p, 512)) == 0)
    {
        return L"";
    }
    else
    {
        std::wstring utf16line(p, len);
        return utf16line;
    }
}

std::string FromResourceToUtf8(int id)
{
    return ToUtf8(FromResourceToUtf16(id));
}