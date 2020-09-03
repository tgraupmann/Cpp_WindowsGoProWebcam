#define _WINSOCK_DEPRECATED_NO_WARNINGS true


#define CURL_STATICLIB
#include <curl.h>

#include <iostream>
#include <stdio.h>
#include <string>
#include <WinSock.h>


using namespace std;

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

void RequestGoProWebcamEndpoint(const string ip, const string path)
{
    curl_global_init(CURL_GLOBAL_DEFAULT);
    auto curl = curl_easy_init();
    if (curl) {
        string url = "http://";
        url += ip;
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

// ref - https://www.codeguru.com/csharp/csharp/cs_network/article.php/c6045/Obtain-all-IP-addresses-of-local-machine.htm
int main(int argc, char* argv[])
{
    string lastGoProIP = "";
    while (true)
    {
        string strGoProIP = GetGoProWebcam();
        if (lastGoProIP.compare(strGoProIP) != 0)
        {
            lastGoProIP = strGoProIP;
            if (strGoProIP.empty())
            {
                printf("GoPro WebCam [Disconnected]\r\n");
            }
            else
            {
                printf("GoPro WebCam %s [Connected]\r\n", strGoProIP.c_str());

                //change default mode from WIDE to NARROW
                RequestGoProWebcamEndpoint(strGoProIP, "/gp/gpWebCam/STOP"); //STOP
                RequestGoProWebcamEndpoint(strGoProIP, "/gp/gpControl/setting/43/6"); //MODE
                RequestGoProWebcamEndpoint(strGoProIP, "/gp/gpWebcam/START"); //START

                Sleep(500);

                //restart service requires admin
                system("net stop \"GoPro Webcam Service\"");
                system("net start \"GoPro Webcam Service\"");

                Sleep(1000);

                cout << "Ready to use Webcam!" << endl;
                cout << "You may need to restart the GoPro application to detect the camera!" << endl;
            }
        }
        Sleep(1000);
    }
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
