#include "utilconfig.h"

#include <ctype.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <pwd.h>
#include <sstream>
#include "util/LoggerUtil.h"
#include "util/Utils.h"

namespace br {
    namespace ufscar {
        namespace lince {
                namespace util {

                        string Utils::timeToString(double time) {
                            ostringstream out;
                            if (time > 59.0) {
                                int hours = time / 3600;
                                int minutes = (time - (hours * 3600)) / 60;
                                int seconds = time - (hours * 3600) - (minutes * 60);

                                if (hours < 10)
                                    out << "0";
                                out << hours;
                                out << ":";
                                if (minutes < 10)
                                    out << "0";
                                out << minutes;
                                out << ":";
                                if (seconds < 10)
                                    out << "0";
                                out << seconds;
                            } else {
                                out << "00:00:";
                                if (time < 10.0)
                                    out << "0";
                                out << time;
                            }

                            return out.str();
                        }

                        double Utils::stringToTime(string str) {
                            string time = str;
                            double hours, minutes, seconds, milliseconds;

                            hours = atof(time.substr(0, time.find(":")).c_str());
                            time = time.substr(time.find(":") + 1);
                            minutes = atof(time.substr(0, time.find(":")).c_str());
                            time = time.substr(time.find(":") + 1);
                            seconds = atof(time.substr(0, time.find(".")).c_str());
                            time = time.substr(time.find(".") + 1);
                            if (time.find("/") != string::npos) {
                                double f0, f1;
                                f0 = atof(time.substr(0, time.find("/")).c_str());
                                time = time.substr(time.find("/") + 1);
                                f1 = atof(time.c_str());
                                milliseconds = f0 / f1;
                            } else {
                                milliseconds = atof(time.c_str());
                            }

                            return (double) (
                                    (hours * 3600) +
                                    (minutes * 60) +
                                    (seconds) +
                                    (milliseconds / 1000));
                        }

                        const char* Utils::stringReplaceChars(string &str, const char *from, const char *to) {
                            size_t i = 0;
                            size_t pos = 0;
                            string fromStr = from;
                            string toStr = to;
                            while ((i = str.find(fromStr, pos)) != string::npos) {
                                str.replace(i, fromStr.size(), toStr);
                                pos = i + toStr.size();
                            }
                            return str.c_str();
                        }

                        const char* Utils::parseXMLChars(string &str){
                            stringReplaceChars(str, "&amp;", "&");
                            stringReplaceChars(str, "&lt;", "<");
                            stringReplaceChars(str, "&gt;", ">");
                            stringReplaceChars(str, "&quot;", "\"");
                            stringReplaceChars(str, "&apos;", "'");
                            return str.c_str();
                        }

                        const char* Utils::encodeXMLChars(string &str){
                            stringReplaceChars(str, "&", "&amp;");
                            stringReplaceChars(str, "<", "&lt;");
                            stringReplaceChars(str, ">", "&gt;");
                            stringReplaceChars(str, "\"", "&quot;");
                            stringReplaceChars(str, "'", "&apos;");
                            return str.c_str();
                        }

                        const char* Utils::stringToLowerCase(string &str){
                            int length = str.length();
                            for(int i = 0; i < length; i++)
                                str[i] = tolower(str.at(i));
                            return str.c_str();
                        }

                        bool Utils::equalsIgnoreCase(const char* str1, const char* str2){
                            if((str1 != NULL) && (str2 != NULL)){
                                string s1 = str1;
                                string s2 = str2;
                                stringToLowerCase(s1);
                                stringToLowerCase(s2);
                                return (s1.compare(s2.c_str()) == 0);
                            } else {
                                return false;
                            }
                        }

                        bool Utils::stringEndsWith(const char* str, const char* end){
                            if((str != NULL) && (end != NULL)){
                                string str1 = str;
                                string str2 = end;
                                int length1 = str1.length();
                                int length2 = str2.length();
                                if(length1 >= length2){
                                    return (str2.compare(str1.substr(length1 - length2, length2).c_str()) == 0);
                                }
                            }
                            return false;
                        }

                        bool Utils::isAbsoluteURL(string url){
                            string tmp = url;
                            stringToLowerCase(tmp);
                            return (tmp.substr(0, tmp.find("://")).compare("http") == 0);
                        }

                        char* Utils::getHostURL(const char* url){
                            string tmp = url;
                            if(isAbsoluteURL(tmp)){
                                // busca depois do "http://"
                                tmp = tmp.substr(7, tmp.find("/", 7) - 7);
                                tmp = tmp.substr(0, tmp.find_last_of(":"));
                                return strdup(tmp.c_str());
                            } else {
                                return NULL;
                            }
                        }

                        int Utils::getPortURL(const char* url){
                            string tmp = url;
                            if(isAbsoluteURL(tmp)){
                                // busca depois do "http://"
                                tmp = tmp.substr(7, tmp.find("/", 7) - 7);
                                tmp = tmp.substr(tmp.find_last_of(":") + 1);
                                return stringToInteger(tmp.c_str());
                            } else {
                                return 80;
                            }
                        }

                        char* Utils::getRelativeURL(const char *url){
                            if(url != NULL){
                                string url1 = url;
                                string relative;
                                if(isAbsoluteURL(url1)){
                                    // busca depois de "http://"
                                    relative = url1.substr(url1.find("/", 7));
                                    if(stringEndsWith(relative.c_str(), "/"))
                                        relative = relative.substr(0, relative.length() - 1);
                                } else {
                                    if((url1.length() > 0) && (url1[0] != '/')){
                                        relative = "/";
                                        relative.append(url1);
                                    } else {
                                        relative = url1;
                                    }
                                }
                                return strdup(relative.c_str());
                            } else {
                                return NULL;
                            }
                        }

                        bool Utils::equalsURL(const char* absoluteUrl, const char* url) {
                            if ((absoluteUrl != NULL) && (url != NULL)) {
                                string urlStr = url;
                                if (urlStr.compare(absoluteUrl) == 0) {
                                    return true;
                                } else {
                                    string relative = getRelativeURL(absoluteUrl);
                                    return (relative.compare(url) == 0);
                                }
                            } else {
                                return false;
                            }
                        }

                        void Utils::wait(long mtime){
                            usleep(mtime*1000);
                        }

                        char* Utils::integerToString(int value){
                            ostringstream out;
                            out << value;
                            return strdup(out.str().c_str());
                        }

                        char* Utils::longToString(long value){
                            ostringstream out;
                            out << value;
                            return strdup(out.str().c_str());
                        }

                        char* Utils::offtToString(off_t value) {
                            ostringstream out;
                            out << value;
                            return strdup(out.str().c_str());
                        }

                        int Utils::stringToInteger(const char* value){
                            return atoi(value);
                        }

                        char* Utils::createRandomUUID(){
                            char uuid[21];
                            time_t t1 = time(NULL);
                            time_t t2 = (time_t)((double)time(NULL) * ((double)rand() / (double)RAND_MAX));
                            sprintf(uuid, "%04x-%04x-%04x-%04x",
                                    (int)(t1 & 0xFFFF),
                                    (int)(((t1 >> 31) | 0xA000) & 0xFFFF),
                                    (int)(t2 & 0xFFFF),
                                    (int)(((t2 >> 31) | 0xE000) & 0xFFFF));
                            return strdup(uuid);
                        }

                        char* Utils::getHostAddress(int n){
                            HLoggerPtr logger = LoggerUtil::getLogger("br.ufscar.lince.ginga.util.utils");
                            LoggerUtil_info(logger, "Entrando no metodo getHostAddress(n)");

                            int count = 0;

                            int sock = socket(AF_INET, SOCK_DGRAM, 0);
                            if (sock >= 0){
                                char buf[8192];
                                ifconf ifc;
                                ifreq ifr;

                                LoggerUtil_debug(logger, "socket created!");

                                ifc.ifc_len = sizeof(buf);
                                ifc.ifc_buf = buf;

                                if(ioctl(sock, SIOCGIFCONF, &ifc) >= 0) {
                                    int size = ifc.ifc_len / sizeof(ifreq);
                                    for(int i = 0; i < size; i++){
                                        ifr = ifc.ifc_req[i];
                                        if( (ioctl(sock, SIOCGIFFLAGS, &ifr) >= 0) && (ifr.ifr_flags & IFF_UP) && !(ifr.ifr_flags & IFF_LOOPBACK) && (ioctl(sock, SIOCGIFADDR, &ifr) >= 0) ){
                                            if(count == n){
                                                char ip[INET_ADDRSTRLEN];
                                                inet_ntop(AF_INET, &(((sockaddr_in*)&ifr.ifr_addr)->sin_addr), ip, INET_ADDRSTRLEN);

                                                LoggerUtil_debug(logger, "ip: " << ip);

                                                return strdup(ip);
                                            }
                                            count++;
                                        }
                                    }
                                }
                                shutdown(sock,SHUT_RDWR);
                                close(sock);
                            }
                            sock = socket(AF_INET6, SOCK_DGRAM, 0);
                            if (sock >= 0){
                                char buf[8192];
                                ifconf ifc;
                                ifreq ifr;

                                LoggerUtil_debug(logger, "socket created!");

                                ifc.ifc_len = sizeof(buf);
                                ifc.ifc_buf = buf;

                                if(ioctl(sock, SIOCGIFCONF, &ifc) >= 0) {
                                    int size = ifc.ifc_len / sizeof(ifreq);
                                    for(int i = 0; i < size; i++){
                                        ifr = ifc.ifc_req[i];
                                        if( (ioctl(sock, SIOCGIFFLAGS, &ifr) >= 0) && (ifr.ifr_flags & IFF_UP) && !(ifr.ifr_flags & IFF_LOOPBACK) && (ioctl(sock, SIOCGIFADDR, &ifr) >= 0) ){
                                            if(count == n){
                                                char ip[INET6_ADDRSTRLEN];
                                                inet_ntop(AF_INET6, &(((sockaddr_in6*)&ifr.ifr_addr)->sin6_addr), ip, INET6_ADDRSTRLEN);

                                                LoggerUtil_debug(logger, "ip: " << ip);

                                                return strdup(ip);
                                            }
                                            count++;
                                        }
                                    }
                                }
                                shutdown(sock,SHUT_RDWR);
                                close(sock);
                            }

                            return NULL;
                        }

                        int Utils::getHostInterfaces(){
                            HLoggerPtr logger = LoggerUtil::getLogger("br.ufscar.lince.ginga.util.utils");
                            LoggerUtil_info(logger, "Entrando no metodo getHostInterfaces()");

                            int count = 0;

                            int sock = socket(AF_INET, SOCK_DGRAM, 0);
                            if (sock >= 0){
                                char buf[8192];
                                ifconf ifc;
                                ifreq ifr;

                                LoggerUtil_debug(logger, "socket created!");

                                ifc.ifc_len = sizeof(buf);
                                ifc.ifc_buf = buf;

                                if(ioctl(sock, SIOCGIFCONF, &ifc) >= 0) {
                                    int size = ifc.ifc_len / sizeof(ifreq);
                                    for(int i = 0; i < size; i++){
                                        ifr = ifc.ifc_req[i];
                                        if( (ioctl(sock, SIOCGIFFLAGS, &ifr) >= 0) && (ifr.ifr_flags & IFF_UP) && !(ifr.ifr_flags & IFF_LOOPBACK) && (ioctl(sock, SIOCGIFADDR, &ifr) >= 0) ){
                                            count++;
                                        }
                                    }
                                }
                                shutdown(sock,SHUT_RDWR);
                                close(sock);
                            }
                            sock = socket(AF_INET6, SOCK_DGRAM, 0);
                            if (sock >= 0){
                                char buf[8192];
                                ifconf ifc;
                                ifreq ifr;

                                LoggerUtil_debug(logger, "socket created!");
                                
                                ifc.ifc_len = sizeof(buf);
                                ifc.ifc_buf = buf;

                                if(ioctl(sock, SIOCGIFCONF, &ifc) >= 0) {
                                    int size = ifc.ifc_len / sizeof(ifreq);
                                    for(int i = 0; i < size; i++){
                                        ifr = ifc.ifc_req[i];
                                        if( (ioctl(sock, SIOCGIFFLAGS, &ifr) >= 0) && (ifr.ifr_flags & IFF_UP) && !(ifr.ifr_flags & IFF_LOOPBACK) && (ioctl(sock, SIOCGIFADDR, &ifr) >= 0) ){
                                            count++;
                                        }
                                    }
                                }
                                shutdown(sock,SHUT_RDWR);
                                close(sock);
                            }

                            return count;
                        }

                        bool Utils::isIPv4(const char* ip){
                            if(ip == NULL) {
                                return false;
                            } else {
                                sockaddr_in sa;
                                return (inet_pton(AF_INET, ip, &(sa.sin_addr)) == 1);
                            }
                        }

                        bool Utils::isIPv6(const char* ip){
                            if(ip == NULL) {
                                return false;
                            } else {
                                sockaddr_in6 sa;
                                return (inet_pton(AF_INET6, ip, &(sa.sin6_addr)) == 1);
                            }
                        }

                        int Utils::socketServer(const char* ip, int* port){
                            int sock = -1;
                            if((ip == NULL) || (strlen(ip) == 0) || isIPv4(ip)){
                                sock = socket(PF_INET, SOCK_STREAM, 0);
                                if (sock >= 0){
                                    sockaddr_in sa;
                                    memset(&sa, 0, sizeof(sa));
                                    sa.sin_family = PF_INET;
                                    sa.sin_addr.s_addr = INADDR_ANY;
                                    sa.sin_port = htons(*port);
                                    if((bind(sock, (sockaddr*) &sa, sizeof(sa)) < 0 ) || ( listen(sock, 1024) < 0 )){
                                        shutdown(sock,SHUT_RDWR);
                                        close(sock);
                                        sock = -1;
                                    }

                                    socklen_t length = sizeof(sa);
                                    if(getsockname(sock, (sockaddr*) &sa, &length ) != -1){
                                        *port = (int) ntohs(sa.sin_port);
                                    }
                                    
                                }
                            }else if(isIPv6(ip)){
                                sock = socket(PF_INET6, SOCK_STREAM, 0);
                                if (sock >= 0){
                                    sockaddr_in6 sa;
                                    memset(&sa, 0, sizeof(sa));
                                    sa.sin6_family = PF_INET6;
                                    sa.sin6_addr = in6addr_any;
                                    sa.sin6_port = htons(*port);
                                    if((bind(sock, (sockaddr*) &sa, sizeof(sa)) < 0) || (listen(sock, 1024) < 0)){
                                        shutdown(sock,SHUT_RDWR);
                                        close(sock);
                                        sock = -1;
                                    }

                                    socklen_t length = sizeof(sa);
                                    if(getsockname(sock, (sockaddr*) &sa, &length ) != -1){
                                        *port = (int) ntohs(sa.sin6_port);
                                    }

                                }
                            }

                            return sock;
                        }

                        int Utils::socketConnect(const char* ip, int port){
                            int sock = -1;
                            if(isIPv4(ip)){
                                sock = socket(AF_INET, SOCK_STREAM, 0);
                                if (sock >= 0){
                                    sockaddr_in sa;
                                    memset(&sa, 0, sizeof(sa));
                                    inet_pton(AF_INET, ip, &(sa.sin_addr));
                                    sa.sin_family = AF_INET;
                                    sa.sin_port = htons(port);
                                    if(connect(sock, (sockaddr*) &sa, sizeof(sa)) != 0){
                                        shutdown(sock,SHUT_RDWR);
                                        close(sock);
                                        sock = -1;
                                    }
                                }
                            }else if(isIPv6(ip)){
                                sock = socket(AF_INET6, SOCK_STREAM, 0);
                                if (sock >= 0){
                                    sockaddr_in6 sa;
                                    memset(&sa, 0, sizeof(sa));
                                    inet_pton(AF_INET6, ip, &(sa.sin6_addr));
                                    sa.sin6_family = AF_INET6;
                                    sa.sin6_port = htons(port);
                                    if(connect(sock, (sockaddr*) &sa, sizeof(sa)) != 0){
                                        shutdown(sock,SHUT_RDWR);
                                        close(sock);
                                        sock = -1;
                                    }
                                }
                            }

                            return sock;
                        }

                        off_t Utils::socketSend(int sock, const char* data, off_t length){
                            off_t pos = (off_t) 0;
                            if (length > 0) {
                                int retry = 0;
                                do {
                                    off_t sent = send(sock, data + pos, length, MSG_NOSIGNAL);
                                    if (sent <= 0) {
                                        if (retry >= 10)
                                            return pos;
                                        wait(1000);
                                        retry++;
                                    } else {
                                        pos += sent;
                                        length -= sent;
                                        retry = 0;
                                    }
                                } while (length > 0);
                            }
                            return pos;
                        }

                        const char* Utils::getHomeDir() {
                            static const char* homeDir = NULL;

                            if (homeDir == NULL) {
                                passwd* pw = getpwuid(getuid());

                                if (pw != NULL){
                                    homeDir = pw->pw_dir;
                                } else {
                                    char* var = getenv("HOME");
                                    if (var != NULL)
                                        homeDir = var;
                                }
                                if (homeDir == NULL)
                                    homeDir = "/tmp";
                            }

                            return homeDir;
                        }

            }
        }
    }
}
