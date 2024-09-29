#pragma once
#include <windows.h>
#include <winhttp.h>
#include <iostream>
#include <string>
#pragma comment(lib, "winhttp.lib")

BOOL SendWebRequest(std::string strDomain, std::string strURI);
