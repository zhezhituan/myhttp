#include "socket.h"
#include <sys/epoll.h>
#include <map>
#include <string>
#include <iostream>
#include "handle.h"
#include "event.h"
int main(){
    event* evproxy=event::getinstance();
    sserver server("0.0.0.0",1234);
    server.start(10,true);
    Ihandle* serverhandle = new sserverhandle(&server);
    while(true){
        std::string cmd;
        std::cin>>cmd;
        //dong something
    }
    return 0;
}