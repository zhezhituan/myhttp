#include "mutex.h"
smutex::smutex(){
    pthread_mutex_init(&m_mutex,NULL);
}
smutex::~smutex(){
    pthread_mutex_destroy(&m_mutex);
}
void smutex::lock(){
    pthread_mutex_lock(&m_mutex);
}

void smutex::unlock(){
    pthread_mutex_unlock(&m_mutex);
}

sguard::sguard(smutex* _smutex){
    m_smutex=_smutex;
    m_smutex->lock();
}

sguard::~sguard(){
    m_smutex->unlock();
}

scond::scond(){
    pthread_cond_init(&m_cond,NULL);
}

scond::~scond(){
    pthread_cond_destroy(&m_cond);
}

int scond::wait(smutex* _smutex){
    return pthread_cond_wait(&m_cond,&(_smutex->m_mutex));
}
int scond::notify(){
    return pthread_cond_signal(&m_cond);
}
int scond::notifyall(){
    return pthread_cond_broadcast(&m_cond);
}