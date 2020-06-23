#include "event.h"
#include "log.h"
event* event::eventproxy;
event::event(int threadnum, int tasknum)
{
    m_threadnum=threadnum;
    m_tasknum=tasknum;
}
void event::create(){
    m_taskqueue = squeue<task>(m_tasknum);
    m_threadpool = sthreadpool(m_threadnum, &m_taskqueue, &m_handlemap);
    m_epollfd=epoll_create(1000);
    pthread_create(&m_detectionthread, NULL, epollwait, this);
}
int event::reghandle(Ihandle *handle)
{
    int fd = handle->fd();
    if (m_handlemap.find(fd) == m_handlemap.end())
    {
        struct epoll_event ev;
        ev.data.fd=fd;
        ev.events=EPOLLIN|EPOLLET;
        epoll_ctl(m_epollfd,EPOLL_CTL_ADD,fd,&ev);
        m_handlemap[fd] = handle;
        LOG(INFO) << "reghandle" << fd;
    }
    else
    {
        LOG(INFO) << "reghandle" << fd;
    }
    return 0;
}
int event::unreghandle(Ihandle *handle)
{
    int fd = handle->fd();
    if (m_handlemap.find(fd) == m_handlemap.end())
    {
        return 0;
    }
    else
    {
        m_handlemap.erase(fd);
        LOG(INFO) << "unreghandle" << fd;
    }
    return 0;
}
int event::pushtask(task task)
{
    m_taskqueue.push(task);
}

void *event::epollwait(void *arg)
{
    event* ev=event::getinstance();
    int epollfd = ev->m_epollfd;
    epoll_event *eventbuff = &(ev->m_eventbuf[0]);
    int nevent;
    while (true)
    {
        nevent = epoll_wait(epollfd, eventbuff, 1000, 60);
        for (int i = 0; i < nevent; i++)
        {
            epoll_event t_ev = *(eventbuff + i);
            LOG(INFO) << t_ev.data.fd << t_ev.events;
            task t = {(int)t_ev.data.fd, (uint32_t)t_ev.events};
            ev->m_taskqueue.push(t);
        }
    }
}

int sthreadpool::create(){
    for(int i=0;i<m_pthreadnum;i++){
        pthread_t pt;
        pthread_create(&pt,NULL,process,(void*)this);
        ptv.push_back(pt);
    }
    return 1;
}

sthreadpool::sthreadpool(int _pthreadnum,squeue<task>* _taskqueue,std::map<int, Ihandle*>* _handlemap){
    m_pthreadnum=_pthreadnum;
    m_taskqueue=_taskqueue;
    m_handlemap=_handlemap;
    create();
    LOG(INFO)<<"threadpool create";
}

void* sthreadpool::process(void* arg){
    task task;
    std::map<int,Ihandle*>::iterator itor;
    event* ev=event::getinstance();
    while(ev->m_taskqueue.pop(task)){
        itor=ev->m_handlemap.find(task.first);
        if(itor==ev->m_handlemap.end()){
            LOG(INFO)<<task.first<<"has no handle";
        }
        else (*itor).second->handle(task.second);
    }
}

