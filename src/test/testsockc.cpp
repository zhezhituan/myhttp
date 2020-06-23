#include "socket.h"
#include <string>
#include <iostream>

int main(){
    sstream sclient;
    sclient.start();
    char buf[1000];
    while(true){
        if(sclient.connent("127.0.0.1",2222)){
            std::cout<<"hascon";
            while(true){
            std::string s;
            std::cin>>s;
            sclient.send((void*)s.c_str(),s.size(),0);
            sclient.recv(buf,1000,0);
            std::cout<<buf<<std::endl;
            }
        }
        else{
            std::cout<<strerror(errno)<<std::endl;
        }
    }
    return 0;
}