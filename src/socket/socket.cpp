#include "socket.h"
#include "log.h"
sserver::sserver(const std::string _addr, const uint16_t _port)
{
    GUARD(m_smutex);
    m_addr = _addr;
    m_port = _port;
    m_fd=-1;
}
int sserver::fd()
{
    GUARD(m_smutex);
    return m_fd;
}
int sserver::start(size_t backlog, bool enblock)
{
    GUARD(m_smutex);
    if (m_fd > 0)
        return 1;
    int t_fd;
    if ((t_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        LOG(INFO) << "sserver socket error" << strerror(errno)<<std::endl;
        return -1;
    }
    setnoblock(t_fd, enblock);
    struct sockaddr_in serv_addr;
    //#include <netinet/in.h>
    bzero((void *)&serv_addr, sizeof(sockaddr));
    serv_addr.sin_family = AF_INET;
    //#include <arpa/inet.h>
    serv_addr.sin_port = htons(m_port);
    if (inet_aton(m_addr.c_str(), &serv_addr.sin_addr) < 0)
        return -1;
    int ret = 0;
    if ((ret = bind(t_fd, (sockaddr *)&serv_addr, sizeof(serv_addr))) < 0)
    {
        //<unistd.h>
        LOG(INFO) << "sserver start error" << strerror(errno)<<std::endl;
        ::close(t_fd);
        return -1;
    }
    if (listen(t_fd, backlog) < 0)
    {
        LOG(INFO) << "sserver listen error" << strerror(errno)<<std::endl;
        ::close(t_fd);
        return -1;
    }
    m_fd = t_fd;
    return 0;
}
int sserver::close()
{
    GUARD(m_smutex);
    if (m_fd < 0)
        return -1;
    int ret;
    if ((ret=::close(m_fd)) < 0){
        LOG(INFO) << "sserver close error" << strerror(errno)<<std::endl;
        return -1;
    }
    m_fd = -1;
    return 0;
}
sstream *sserver::accept()
{
    GUARD(m_smutex);
    if (m_fd < 0)
        return NULL;
    struct sockaddr_in clientaddr;
    socklen_t len = sizeof(clientaddr);
    int clientfd = ::accept(m_fd, (sockaddr *)&clientaddr, &len);
    if (clientfd < 0){
        LOG(INFO) << "sserver accept error" << strerror(errno)<<std::endl;
        return NULL;
    }
    sstream *client = new sstream(clientfd);
    return client;
}

int sstream::fd()
{
    GUARD(m_smutex);
    return m_fd;
}
sstream::sstream()
{
    m_fd=-1;
}
sstream::sstream(int _fd)
{
    GUARD(m_smutex);
    m_fd = _fd;
}
sstream::sstream(const sstream *_client)
{
    GUARD(m_smutex);
    m_fd = _client->m_fd;
}
int sstream::start()
{
    GUARD(m_smutex);
    if (m_fd > 0)
        return 0;
    m_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (m_fd < 0){
        LOG(INFO) << "sstream start error" << strerror(errno)<<std::endl;
        return -1;
    }
        
    return 0;
}
int sstream::close()
{
    GUARD(m_smutex);
    if (m_fd < 0)
        return 0;
    if (::close(m_fd) < 0){
        LOG(INFO) << "sstream close error" << strerror(errno)<<std::endl;
        return -1;
    }
    else
    {   
        m_fd = -1;
        return 0;
    }
}
int sstream::connent(const std::string _addr, const uint16_t _port)
{
    GUARD(m_smutex);
    if (m_fd < 0)
        return -1;
    struct sockaddr_in clientaddr;
    bzero(&clientaddr, sizeof(clientaddr));
    clientaddr.sin_family = AF_INET;
    clientaddr.sin_port = htons(_port);
    if (inet_aton(_addr.c_str(), &clientaddr.sin_addr) < 0){
        LOG(INFO) << "sstream inet_aton error" << strerror(errno)<<std::endl;
        return -1;
    }
    if (::connect(m_fd, (sockaddr *)&clientaddr, sizeof(clientaddr)) < 0){
        LOG(INFO) << "sstream conent error" << strerror(errno)<<std::endl;
        return -1;
    }
        
    return 0;
}
ssize_t sstream::recv(void *buf, size_t len, int flags)
{
    GUARD(m_smutex);
    if (m_fd < 0)
        return -1;
    return ::recv(m_fd, buf, len, flags);
}
ssize_t sstream::send(void *buf, size_t len, int flags)
{
    GUARD(m_smutex);
    int nsend = 0;
    int ret = 0;
    while (nsend < len)
    {
        ret = ::send(m_fd, (char *)buf + nsend, len - nsend, 0);
        if (ret < 0)
        {
            if (errno == EINTR)
            {
                continue;
            }
            else
                return ret;
        }
        nsend += ret;
    }
    return nsend;
}
int setnoblock(int fd,bool en){
    int flags = fcntl(fd, F_GETFL, 0);
	if(en)
		flags |= O_NONBLOCK;
	else
		flags &=~ O_NONBLOCK;
	return fcntl(fd, F_SETFL, flags);
}