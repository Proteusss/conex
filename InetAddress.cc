/**
 * @file InetAddress.cc
 * @author zjb
 * @brief 
 * @date 2021-03-27
 */
#include<arpa/inet.h>
#include<strings.h>
#include<iostream>
#include "InetAddress.h"

using namespace conex;
InetAddress::InetAddress(const std::string &ip,uint16_t port)
{
    bzero(&addr_,sizeof addr_);
    addr_.sin_family = AF_INET;
    int ret = ::inet_pton(AF_INET,ip.c_str(),&addr_.sin_addr.s_addr);
    if( ret != 1)
    {
        //SYSFATAL
    }
    addr_.sin_port = htons(port);
}
InetAddress::InetAddress(uint16_t port, bool any)
{
    bzero(&addr_,sizeof addr_);
    addr_.sin_family = AF_INET;
    addr_.sin_port = htons(port);
    auto ip = any ? INADDR_ANY :INADDR_LOOPBACK;
    addr_.sin_addr.s_addr = htonl(ip);
}
std::string InetAddress::toIp() const
{
    char buf[INET_ADDRSTRLEN];
    const char* ret = inet_ntop(AF_INET,&addr_.sin_addr,buf,sizeof buf);
    if(ret == nullptr)
    {
        buf[0] = '\0';
        //SYSERR
    }
    return buf;
}
uint16_t InetAddress::toPort() const
{
    return ntohs(addr_.sin_port);
}
//format: ip:port
std::string InetAddress::toIpPort() const
{
    std::string addr = toIp();
    addr = addr + ':';
    addr += std::to_string(toPort());
    return addr; 
}

//int main()
//{
    //InetAddress addr(8080);
    //std::cout<<addr.toIpPort()<<std::endl;
    //auto addr2(addr);
    //std::cout<<addr2.toIpPort()<<std::endl;
    //return 0;
//}