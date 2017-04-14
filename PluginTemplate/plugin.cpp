#include "plugin.h"
#include <iostream>
#include <fstream>
std::ofstream outfile;

static bool cbLogA(int argc, char* argv[])
{
    if(argc < 2)
    {
        _plugin_logputs(FromResourceToUtf8(IDS_ARGLESS).c_str());
        return false;
    }
    duint addr = 0;
    if(!DbgIsValidExpression(argv[1]))
    {
        _plugin_logputs(FromResourceToUtf8(IDS_ARGERR).c_str());
        return false;
    }
    addr = DbgValFromString(argv[1]);
    char* A;
    size_t size;
    std::wstring text;
    if(argc >= 3)
    {
        if(!DbgIsValidExpression(argv[2]))
        {
            _plugin_logputs(FromResourceToUtf8(IDS_ARGERR).c_str());
            return false;
        }
        size = DbgValFromString(argv[2]);
        A = new char[size + 1];
        if(!DbgMemRead(addr, A, size))
        {
            delete[]A;
            _plugin_logputs(FromResourceToUtf8(IDS_MEMERR).c_str());
            return false;
        }
        A[size] = 0;
        text = FromLCP(std::string(A, size));
    }
    else
    {
        size = 512;
        A = new char[size];
        if(!DbgMemRead(addr, A, size))
        {
            delete[]A;
            _plugin_logputs(FromResourceToUtf8(IDS_MEMERR).c_str());
            return false;
        }
        A[size - 1] = 0;
        text = FromLCP(std::string(A));
    }
    _plugin_logprintf("%p: \"%s\"\n", addr, ToUtf8(text).c_str());
    if(outfile.is_open())
        outfile.write((const char*)text.c_str(), text.size() * sizeof(wchar_t));
    delete []A;
    return true;
}

static bool cbLogW(int argc, char* argv[])
{
    if(argc < 2)
    {
        _plugin_logputs(FromResourceToUtf8(IDS_ARGLESS).c_str());
        return false;
    }
    duint addr = 0;
    if(!DbgIsValidExpression(argv[1]))
    {
        _plugin_logputs(FromResourceToUtf8(IDS_ARGERR).c_str());
        return false;
    }
    addr = DbgValFromString(argv[1]);
    wchar_t* A;
    size_t size;
    std::wstring text;
    if(argc >= 3)
    {
        if(!DbgIsValidExpression(argv[2]))
        {
            _plugin_logputs(FromResourceToUtf8(IDS_ARGERR).c_str());
            return false;
        }
        size = DbgValFromString(argv[2]) / sizeof(wchar_t);
        A = new wchar_t[size + 1];
        if(!DbgMemRead(addr, A, size * sizeof(wchar_t)))
        {
            delete[]A;
            _plugin_logputs(FromResourceToUtf8(IDS_MEMERR).c_str());
            return false;
        }
        for(size_t i = 0; i < size; i++)
            if(A[i] < 0x1f && A[i] != 10 && A[i] != 13 && A[i] != 9)
                A[i] = L'.';
        A[size] = 0;
        text = std::wstring(A, size);
    }
    else
    {
        size = 512;
        A = new wchar_t[size];
        if(!DbgMemRead(addr, A, size * sizeof(wchar_t)))
        {
            delete[]A;
            _plugin_logputs(FromResourceToUtf8(IDS_MEMERR).c_str());
            return false;
        }
        A[size - 1] = 0;
        text = std::wstring(A);
    }
    _plugin_logprintf("%p: L\"%s\"\n", addr, ToUtf8(text).c_str());
    if(outfile.is_open())
        outfile.write((const char*)text.c_str(), text.size() * sizeof(wchar_t));
    delete[]A;
    return true;
}

static bool cbLogData(int argc, char* argv[])
{
    if(argc < 3)
    {
        _plugin_logputs(FromResourceToUtf8(IDS_ARGLESS).c_str());
        return false;
    }
    if(DbgIsValidExpression(argv[1]) && DbgIsValidExpression(argv[2]))
    {
        duint addr = DbgValFromString(argv[1]);
        duint size = DbgValFromString(argv[2]);
        if(size > 0x7fffffff)
            return false;
        if(!DbgMemIsValidReadPtr(addr))
            return false;
        char* buffer = new char[size];
        if(!outfile.is_open() || DbgMemRead(addr, buffer, size))
        {
            if(outfile.is_open())
                outfile.write(buffer, size);
        }
        delete[] buffer;
        return true;
    }
    else
        return false;
}

static bool cbSetLogFile(int argc, char* argv[])
{
    if(argc < 2)
    {
        if(outfile.is_open())
            outfile.close();
        return true;
    }
    if(outfile.is_open())
        outfile.close();
    outfile.open(FromUtf8(argv[1]).c_str(), std::ios_base::app|std::ios_base::binary);
    return true;
}

//Initialize your plugin data here.
bool pluginInit(PLUG_INITSTRUCT* initStruct)
{
    _plugin_registercommand(pluginHandle, "loga", cbLogA, true);
    _plugin_registercommand(pluginHandle, "logw", cbLogW, true);
    _plugin_registercommand(pluginHandle, "logdata", cbLogData, true);
    _plugin_registercommand(pluginHandle, "setlogfile", cbSetLogFile, false);

    return true; //Return false to cancel loading the plugin.
}

//Deinitialize your plugin data here (clearing menus optional).
bool pluginStop()
{
    _plugin_unregistercommand(pluginHandle, "loga");
    _plugin_unregistercommand(pluginHandle, "logw");
    _plugin_menuclear(hMenu);
    _plugin_menuclear(hMenuDisasm);
    _plugin_menuclear(hMenuDump);
    _plugin_menuclear(hMenuStack);
    if(outfile.is_open())
        outfile.close();
    return true;
}

//Do GUI/Menu related things here.
void pluginSetup()
{
}
