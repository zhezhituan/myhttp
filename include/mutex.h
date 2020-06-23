#ifndef _MUTEX_H_
#define _MUTEX_H_
#include <pthread.h>

class smutex{
    private:
        pthread_mutex_t m_mutex;
    public:
        void lock();
        void unlock();
        smutex();
        ~smutex();
    friend class sguard;
    friend class scond;
};
class sguard{
    private:
        smutex* m_smutex;
    public:
        sguard(smutex*);
        ~sguard();
};
class scond{
    private:
        pthread_cond_t m_cond;
    public:
        scond();
        ~scond();
        int wait(smutex* smutex);
        int notify();
        int notifyall();};


#define GUARD(_smutex) sguard __guard(&_smutex)



#endif