#pragma once

#include "pluginmain.h"

//plugin data
#define PLUGIN_NAME "$safeprojectname$"
#define PLUGIN_VERSION 1

//functions
bool pluginInit(PLUG_INITSTRUCT* initStruct);
bool pluginStop();
void pluginSetup();
//encoding
std::string ToUtf8(std::wstring text);
std::wstring FromLCP(std::string text);
std::wstring FromResourceToUtf16(int id);
std::string FromResourceToUtf8(int id);