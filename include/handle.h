#ifndef _HANDLE_H_
#define _HANDLE_H_

#include <sys/epoll.h>
#include "socket.h"

class Ihandle{
    public:
    virtual int fd()=0;
    virtual int handle(uint32_t)=0;
    virtual int reg()=0;
    virtual int unreg()=0;
};


class sstreamhandle:public Ihandle{
    public:
    sstream* m_stream;
    sstreamhandle(){};
    ~sstreamhandle();
    sstreamhandle(sstream*);
    int handle(uint32_t);
    int reg();
    int unreg();
    int fd();
};
class sserverhandle:public Ihandle{
    public:
    sserver* m_server;
    sserverhandle(){};
    ~sserverhandle();
    sserverhandle(sserver*);
    int handle(uint32_t);
    int reg();
    int unreg();
    int fd();
};



#endif