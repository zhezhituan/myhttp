#include "handle.h"
#include "event.h"
#include "log.h"
sserverhandle::sserverhandle(sserver* _server){
    m_server=_server;
    reg();
}
int sserverhandle::reg(){
    event::getinstance()->reghandle((Ihandle*)this);
}
int sserverhandle::unreg(){
    event::getinstance()->unreghandle((Ihandle*)this);
}
int sserverhandle::handle(uint32_t ev){
    if(ev&EPOLLIN){
        sstream* newcon;
        newcon=m_server->accept();
        sstreamhandle* newconhandle=new sstreamhandle(newcon);
    }
}
int sserverhandle::fd(){
    return m_server->fd();
}

sstreamhandle::sstreamhandle(sstream* _sstream){
    m_stream=_sstream;
    reg();
}
int sstreamhandle::reg(){
    event::getinstance()->reghandle((Ihandle*)this);
}
int sstreamhandle::unreg(){
    event::getinstance()->unreghandle((Ihandle*)this);
}
int sstreamhandle::handle(uint32_t ev){
    if(ev&EPOLLIN){ 
        char buf[100];
        int nrecv=m_stream->recv(buf,100,0);
        if(nrecv==0){
            unreg();
            LOG(INFO)<<this->fd()<<"close"<<std::endl;
            m_stream->close();
            return 0;
        }
        m_stream->send(buf,nrecv,0);
    }
}

int sstreamhandle::fd(){
    return m_stream->fd();
}