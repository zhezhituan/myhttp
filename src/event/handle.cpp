#include "handle.h"
#include "event.h"
#include "log.h"
#include "http.h"

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
    LOG(INFO)<<this->fd()<<"close"<<std::endl;
    m_stream->close();
    delete m_stream;
    return 0;
}
int sstreamhandle::handle(uint32_t ev){
    if(ev&EPOLLIN){ 
        char buf[1024];
        int nrecv=m_stream->recv(buf,1024,0);
        if(nrecv==0){
            unreg();
        }
        buf[nrecv]=0;
        SHttpRequest httprequest;
        if(httprequest.load_packet(buf,nrecv)<0){
            LOG(INFO)<<"parse request error"<<std::endl;
            return -1;
        }
        LOG(INFO)<<"fd"<<m_stream->fd()<<httprequest.url()<<std::endl;
        IHttpRespose* httprepose = handle_request(httprequest);
        LOG(INFO)<<httprepose->serialize()<<std::endl;
        if(httprepose!=NULL){
            if(m_stream->send((void*)httprepose->serialize(),httprepose->size(),0)<0){
                LOG(INFO)<<"sstream send error"<<std::endl;
            }
            delete httprepose; 
        }
    }
}

int sstreamhandle::fd(){
    return m_stream->fd();
}