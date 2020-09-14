#define _WINSOCK_DEPRECATED_NO_WARNINGS true


#define CURL_STATICLIB
#include <curl.h>

#include <conio.h>
#include <iostream>
#include <stdio.h>
#include <string>
#include <thread>
#include <WinSock.h>


using namespace std;

static bool _sWaitForExit = true;
static bool _sConnected = false;
static bool _sRestartService = false;
static bool _sChangeMode = true;
static string _sStrGoProIP = "";
static int _sMode = 2;

int GetLastIndexOf(const string& str, const char c)
{
    int result = -1;
    const char* cstr = str.c_str();
    for (unsigned int i = 0; i < str.size(); ++i)
    {
        if (cstr[i] == c)
        {
            result = i;
        }
    }
    return result;
}

string GetGoProWebcam()
{
    WORD wVersionRequested;
    WSADATA wsaData;
    char name[255];
    PHOSTENT hostinfo;
    wVersionRequested = MAKEWORD(1, 1);
    char* ip;

    if (WSAStartup(wVersionRequested, &wsaData) == 0)
    {
        if (gethostname(name, sizeof(name)) == 0)
        {
            //printf("Host name: %s\n", name);

            if ((hostinfo = gethostbyname(name)) != NULL)
            {
                int nCount = 0;
                while (hostinfo->h_addr_list[nCount])
                {
                    ip = inet_ntoa(*(
                        struct in_addr*)hostinfo->h_addr_list[nCount]);

                    string strIP = ip;
                    if (strIP.rfind("172.", 0) == 0) {
                        int lastDot = GetLastIndexOf(strIP, '.');
                        if (lastDot > 0)
                        {
                            string strGoProIP = strIP.substr(0, lastDot);
                            strGoProIP += ".51";
                            return strGoProIP;
                        }
                    }

                    ++nCount;
                }
            }
        }
    }
    return "";
}

size_t CurlWrite_CallbackFunc_StdString(void* contents, size_t size, size_t nmemb, string* s)
{
    size_t newLength = size * nmemb;
    try
    {
        s->append((char*)contents, newLength);
    }
    catch (std::bad_alloc&)
    {
        //handle memory problem
        return 0;
    }
    return newLength;
}

void RequestGoProWebcamEndpoint(const string path)
{
    curl_global_init(CURL_GLOBAL_DEFAULT);
    auto curl = curl_easy_init();
    if (curl) {
        string url = "http://";
        url += _sStrGoProIP;
        url += path;
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
        curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 50L);
        curl_easy_setopt(curl, CURLOPT_TCP_KEEPALIVE, 1L);

        std::string response_string;
        std::string header_string;
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CurlWrite_CallbackFunc_StdString);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);
        curl_easy_setopt(curl, CURLOPT_HEADERDATA, &header_string);

        curl_easy_perform(curl);
        cout << "Request: " << path << " Response: ===" << endl << response_string << endl;
        curl_easy_cleanup(curl);
        curl_global_cleanup();
        curl = NULL;
    }
}

void HandleInput()
{
    while (_sWaitForExit)
    {
        int input = _getch();
        switch (input)
        {
        case 27: //ESC
            _sWaitForExit = false;
            break;
        case 'M':
        case 'm':
            _sMode = (_sMode + 1) % 3;
            _sChangeMode = true;
            break;
        case 13: //ENTER
            _sChangeMode = true;
            break;
        case 'R':
        case 'r':
            _sRestartService = true;
            break;
        }
        Sleep(0);
    }
}


void PrintLegend()
{
    system("cls");
    if (_sConnected)
    {
        cout << "GoPro WebCam " << _sStrGoProIP << " [Connected]" << endl;

        cout << "Modes:" << endl;
        cout << ((_sMode == 0) ? "[ * ]" : "[   ]") << " Wide" << endl;
        cout << ((_sMode == 1) ? "[ * ]" : "[   ]") << " Large" << endl;
        cout << ((_sMode == 2) ? "[ * ]" : "[   ]") << " Narrow" << endl;
    }
    else
    {
        printf("GoPro WebCam [Disconnected]\r\n");
    }

    cout << "Press `ESC` to quit." << endl;
    cout << "Press `R` to restart GoPro Webcam Service." << endl;
    cout << "Press `M` to toggle camera mode." << endl;
    cout << "Press `ENTER` to reset mode." << endl;

    cout << endl;

    cout << "Troubleshooting:" << endl;
    cout << "  * Try restarting the camera." << endl;
    cout << "  * Try reconnecting the USB camera cable." << endl;
    cout << "  * Try checking the camera settings and make sure USB is in MTP mode." << endl;
    cout << "  * You may need to restart the GoPro application to detect the camera." << endl;
}

void RestartGoProWebcamService()
{
    //restart service requires admin
    system("net stop \"GoPro Webcam Service\"");
    system("net start \"GoPro Webcam Service\"");
}

// ref - https://www.codeguru.com/csharp/csharp/cs_network/article.php/c6045/Obtain-all-IP-addresses-of-local-machine.htm
int main(int argc, char* argv[])
{
    thread thread(HandleInput);
    string lastGoProIP = "";
    while (_sWaitForExit)
    {
        if (_sRestartService)
        {
            _sRestartService = false;
            RestartGoProWebcamService();
            Sleep(1000);

            PrintLegend();
        }

        _sStrGoProIP = GetGoProWebcam();
        if (lastGoProIP.compare(_sStrGoProIP) != 0)
        {
            lastGoProIP = _sStrGoProIP;
            _sChangeMode = true;
            _sConnected = !_sStrGoProIP.empty();
        }

        if (_sConnected &&
            _sChangeMode)
        {
            _sChangeMode = false;

            //change default mode from WIDE to NARROW
            RequestGoProWebcamEndpoint("/gp/gpWebCam/STOP"); //STOP
            switch (_sMode)
            {
            case 0:
                RequestGoProWebcamEndpoint("/gp/gpControl/setting/43/0"); //MODE
                break;
            case 1:
                RequestGoProWebcamEndpoint("/gp/gpControl/setting/43/4"); //MODE
                break;
            case 2:
                RequestGoProWebcamEndpoint("/gp/gpControl/setting/43/6"); //MODE
                break;
            }
            RequestGoProWebcamEndpoint("/gp/gpWebcam/START"); //START

            Sleep(500);

            cout << "Ready to use Webcam!" << endl;
        }

        PrintLegend();

        Sleep(1000);
    }
    thread.join();
    return 0;
}

#ifdef _DEBUG
#pragma comment( lib, "msvcrtd" )
#else
#pragma comment( lib, "msvcrt" )
#endif
#pragma comment( lib, "Wldap32" )
#pragma comment( lib, "Ws2_32" )
#pragma comment (lib, "crypt32")
#pragma comment( lib, "libcurl" )
#pragma comment( lib, "libssh2" )
#pragma comment( lib, "libcrypto" )
#pragma comment( lib, "libssl" )
#pragma comment( lib, "zlibstat" )
#pragma comment (lib, "jsoncpp")
#pragma comment(lib, "wsock32.lib")
