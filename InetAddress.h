/**
 * @file InetAddress.h
 * @author zjb
 * @brief   封装后的地址
 * @date 2021-03-27
 */
#ifndef CONEX_INETADDRESS_H
#define CONEX_INETADDRESS_H

#include <netinet/in.h>
#include <string>

namespace conex
{

class InetAddress
{
public:
    InetAddress(const std::string &ip, uint16_t port);
    /**
    * @brief Construct a new Inet Address object
    * @param port 
    * @param any  true:INADDR_ANY; false:INADDR_LOOPBACK
    */
    explicit InetAddress(uint16_t port = 0, bool any = false);
    explicit InetAddress(const sockaddr_in &addr)
        : addr_(addr)
    {
    }
    

    std::string toIp() const;
    std::string toIpPort() const;
    uint16_t toPort() const;

    void setSockAddr(const sockaddr_in &addr){ addr_ = addr;}
    const sockaddr* getSockAddr() const {return (sockaddr*)(&addr_);} //这里要返回通用的sockaddr指针类型
    socklen_t getSockLen() const { return sizeof(addr_);}

private:
    sockaddr_in addr_;
};

}
#endif