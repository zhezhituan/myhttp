#ifndef _SOCKET_H_
#define _SOCKET_H_
#include <sys/types.h>
#include <string>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <memory.h>
#include "mutex.h"

int setnoblock(int fd,bool en);

class Isocket{
    virtual int fd()=0;
};
class sstream:public Isocket{
    private:
        int m_fd;
        smutex m_smutex;
    public:
        int fd();
        sstream();
        sstream(int fd);
        sstream(const sstream*);
        int start();
        int close();
        int connent(const std::string _addr,const uint16_t _port);
        ssize_t recv(void* buf,size_t len,int flags);
        ssize_t send(void* buf,size_t len,int flags);
};
class sserver:public Isocket{
    private:
        int m_fd;
        std::string m_addr;
        uint16_t m_port;
        smutex m_smutex;

    public:
        sserver();
        sserver(const std::string _addr,const uint16_t _port);
        int fd();
        int start(size_t backlog,bool);
        int close();
        sstream* accept();
};



#endif