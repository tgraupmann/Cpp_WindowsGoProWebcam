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
            }
        }
        Sleep(1000);
    }
    return 0;
}

#pragma comment(lib, "wsock32.lib")
