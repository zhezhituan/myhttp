#include "socket.h"
#include <string>
#include <iostream>

int main(){
    sstream sclient;
    sclient.start();
    std::cout<<"start"<<sclient.fd();
    char buf[1000];
    while(true){
        if(sclient.connent("127.0.0.1",2222)==0){
            std::cout<<"hascon"<<sclient.fd();
            while(true){
            std::string s;
            std::cin>>s;
            sclient.send((void*)s.c_str(),s.size(),0);
            sclient.recv(buf,1000,0);
            std::cout<<buf<<std::endl;
            }
        }
        else{
            std::cout<<sclient.fd()<<strerror(errno)<<std::endl;
            break;
        }
    }
    return 0;
}