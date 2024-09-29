#include "NetworkMisc.h"

BOOL SendWebRequest(std::string strDomain, std::string strURI)
{
    // Initialize WinHTTP
    std::wstring userAgent = L"Mozilla/5.0 (Windows NT 10.0) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/104.0.0.0 Safari/537.36";
    HINTERNET hSession = WinHttpOpen(userAgent.c_str(), WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);

    if (hSession == NULL)
    {
        OutputDebugStringA("WinHttpOpen failed: ");
        OutputDebugStringA(std::to_string(GetLastError()).c_str());
        return FALSE;
    }

    // Specify the target server and port
    std::wstring wstr(strDomain.begin(), strDomain.end()); //Unicode project, chuyển nhanh qua wstring, **không an toàn**
    HINTERNET hConnect = WinHttpConnect(hSession, (LPCWSTR)wstr.c_str(), INTERNET_DEFAULT_HTTPS_PORT, 0);
    if (hConnect == NULL)
    {
        OutputDebugStringA("WinHttpConnect failed: ");
        OutputDebugStringA(std::to_string(GetLastError()).c_str());
        WinHttpCloseHandle(hSession);
        return FALSE;
    }
    // Create a request
    std::wstring wstrURI(strURI.begin(), strURI.end());
    HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"GET", (LPCWSTR)wstrURI.c_str(), NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_SECURE);

    if (hRequest == NULL)
    {
        OutputDebugStringA("WinHttpOpenRequest failed: ");
        OutputDebugStringA(std::to_string(GetLastError()).c_str());
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return FALSE;
    }

    // Set the option to ignore SSL errors
    DWORD dwFlags = SECURITY_FLAG_IGNORE_UNKNOWN_CA | SECURITY_FLAG_IGNORE_CERT_CN_INVALID | SECURITY_FLAG_IGNORE_CERT_DATE_INVALID;
    WinHttpSetOption(hRequest, WINHTTP_OPTION_SECURITY_FLAGS, &dwFlags, sizeof(dwFlags));

    // Send the request
    if (!WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0))
    {
        OutputDebugStringA("WinHttpSendRequest failed: ");
        OutputDebugStringA(std::to_string(GetLastError()).c_str());
        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return FALSE;
    }
    // Wait for the response
    if (!WinHttpReceiveResponse(hRequest, NULL))
    {
        OutputDebugStringA("WinHttpReceiveResponse failed: ");
        OutputDebugStringA(std::to_string(GetLastError()).c_str());
        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return FALSE;
    }

    // Read the response
    DWORD dwSize = 0;
    DWORD dwDownloaded = 0;
    LPSTR pszOutBuffer;
    LPSTR pszHeader = NULL;
    BOOL  bResults = FALSE;

    do
    {
        dwSize = 0;
        bResults = WinHttpQueryDataAvailable(hRequest, &dwSize);
        if (!bResults)
        {
            OutputDebugStringA("WinHttpQueryDataAvailable failed: ");
            OutputDebugStringA(std::to_string(GetLastError()).c_str());
            break;
        }

        pszOutBuffer = new char[dwSize + 1];
        if (!pszOutBuffer)
        {
            std::cerr << "Out of memory" << std::endl;
            dwSize = 0;
        }
        else
        {
            ZeroMemory(pszOutBuffer, dwSize + 1);
            bResults = WinHttpReadData(hRequest, (LPVOID)pszOutBuffer, dwSize, &dwDownloaded);
            if (!bResults)
            {
                OutputDebugStringA("WinHttpReadData failed: ");
                OutputDebugStringA(std::to_string(GetLastError()).c_str());
            }
            else
            {
                //std::cout << pszOutBuffer; // Output the response
            }
            delete[] pszOutBuffer;
        }
    } while (dwSize > 0);

    // Clean up
    WinHttpCloseHandle(hRequest);
    WinHttpCloseHandle(hConnect);
    WinHttpCloseHandle(hSession);

	return TRUE;
}

