#include "plugin.h"

enum
{
    MENU_TEST,
    MENU_DISASM_ADLER32,
    MENU_DUMP_ADLER32,
    MENU_STACK_ADLER32
};

static void Adler32Menu(int hWindow)
{
    if(!DbgIsDebugging())
    {
        _plugin_logputs("[" PLUGIN_NAME "] You need to be debugging to use this command");
        return;
    }
    SELECTIONDATA sel;
    GuiSelectionGet(hWindow, &sel);
    duint len = sel.end - sel.start + 1;
    unsigned char* data = new unsigned char[len];
    if(DbgMemRead(sel.start, data, len))
    {
        DWORD a = 1, b = 0;
        for(duint index = 0; index < len; ++index)
        {
            a = (a + data[index]) % 65521;
            b = (b + a) % 65521;
        }
        delete[] data;
        DWORD checksum = (b << 16) | a;
        _plugin_logprintf("[" PLUGIN_NAME "] Adler32 of %p[%X] is: %08X\n", sel.start, len, checksum);
    }
    else
        _plugin_logputs("[" PLUGIN_NAME "] DbgMemRead failed...");
}

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
    _plugin_logprintf("\"%s\"\n", ToUtf8(text).c_str());
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
    _plugin_logprintf("\"%s\"\n", ToUtf8(text).c_str());
    delete[]A;
    return true;
}


PLUG_EXPORT void CBMENUENTRY(CBTYPE cbType, PLUG_CB_MENUENTRY* info)
{
    switch(info->hEntry)
    {
    case MENU_TEST:
        MessageBoxA(hwndDlg, "Test Menu Entry Clicked!", PLUGIN_NAME, MB_ICONINFORMATION);
        break;

    case MENU_DISASM_ADLER32:
        Adler32Menu(GUI_DISASSEMBLY);
        break;

    case MENU_DUMP_ADLER32:
        Adler32Menu(GUI_DUMP);
        break;

    case MENU_STACK_ADLER32:
        Adler32Menu(GUI_STACK);
        break;

    default:
        break;
    }
}

//Initialize your plugin data here.
bool pluginInit(PLUG_INITSTRUCT* initStruct)
{
    _plugin_registercommand(pluginHandle, "loga", cbLogA, false);
    _plugin_registercommand(pluginHandle, "logw", cbLogW, false);

    return true; //Return false to cancel loading the plugin.
}

//Deinitialize your plugin data here (clearing menus optional).
bool pluginStop()
{
    _plugin_unregistercommand(pluginHandle, PLUGIN_NAME);
    _plugin_menuclear(hMenu);
    _plugin_menuclear(hMenuDisasm);
    _plugin_menuclear(hMenuDump);
    _plugin_menuclear(hMenuStack);
    return true;
}

//Do GUI/Menu related things here.
void pluginSetup()
{
    //_plugin_menuaddentry(hMenu, MENU_TEST, "&Menu Test");
    //_plugin_menuaddentry(hMenuDisasm, MENU_DISASM_ADLER32, "&Adler32 Selection");
    //_plugin_menuaddentry(hMenuDump, MENU_DUMP_ADLER32, "&Adler32 Selection");
    //_plugin_menuaddentry(hMenuStack, MENU_STACK_ADLER32, "&Adler32 Selection");
}
