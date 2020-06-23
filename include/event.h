#ifndef _EVENT_H_
#define _EVENT_H_

#include <sys/epoll.h>
#include <pthread.h>
#include <map>
#include "queue.h"
#include "handle.h"

typedef std::pair<int, uint32_t> task;

class sthreadpool{
    private:
        int m_pthreadnum;
        int m_tasknum;
        squeue<task>* m_taskqueue;
        std::map<int, Ihandle*>* m_handlemap;
        std::vector<pthread_t> ptv;
        int create();
    public:
        static void* process(void*);
        sthreadpool(){};
        sthreadpool(int,squeue<task>*,std::map<int, Ihandle*>*);
};

class event
{
private:
    
    std::map<int,Ihandle*> m_handlemap;
    squeue<task> m_taskqueue;
    sthreadpool m_threadpool;
    pthread_t m_detectionthread;
    int m_epollfd;
    int m_threadnum;
    int m_tasknum;
    event(int threadnum, int tasknum);
public:
    static event* eventproxy;
    void create();
    int reghandle(Ihandle*);
    int unreghandle(Ihandle*);
    int pushtask(task);
    struct epoll_event m_eventbuf[1000];
    static void *epollwait(void*);
    friend class sthreadpool;
    static event* getinstance(){
        if(event::eventproxy==NULL){
            eventproxy = new event(8,16);
            eventproxy->create();
        }
        return eventproxy;
    }
};

#endif