#ifndef _QUEUE_H_
#define _QUEUE_H_

#include "mutex.h"
#include <queue>
template <typename T> 
class squeue{
    private:
        std::queue<T> m_queue;
        smutex m_smutex;
        scond m_canpush;
        scond m_canpop;
        int m_waitcanpush;
        int m_waitcanpop;
        int maxsize;
    public:
        squeue(int);
        squeue();
        int pop(T&);
        int push(T&);
        int pop_noblock(T&);
        int push_noblock(T&);
};
template <typename T>
squeue<T>::squeue(){
}
template <typename T>
squeue<T>::squeue(int _size){
    maxsize=_size;
    m_waitcanpop=0;
    m_waitcanpush=0;
}
template <typename T>
int squeue<T>::pop(T& out){
    GUARD(m_smutex);
    while(m_queue.empty()){
        m_waitcanpop++;
        m_canpop.wait(&m_smutex);
    }
    out=m_queue.front();
    m_queue.pop();
    if(m_waitcanpush){
        m_waitcanpush--;
        m_canpush.notify();
    }
    return 1;
}

template <typename T>
int squeue<T>::push(T& in){
    GUARD(m_smutex);
    while(m_queue.size()==maxsize){
        m_waitcanpush++;
        m_canpush.wait(&m_smutex);
    }
    m_queue.push(in);
    if(m_waitcanpop){
        m_waitcanpop--;
        m_canpop.notify();
    }
    return 1;
}
template <typename T>
int squeue<T>::pop_noblock(T& out){
    GUARD(m_smutex);
    if(m_queue.empty())return -1;
    else{
        out=m_queue.front();
        m_queue.pop();
        if(m_waitcanpush){
            m_waitcanpush--;
            m_canpush.notify();
        }
        return 1;
    }
}
template <typename T>
int squeue<T>::push_noblock(T& in){
    GUARD(m_smutex);
    if(m_queue.size()==maxsize){
        return -1;
    }
    else{
        m_queue.push(in);
        if(m_waitcanpop){
            m_waitcanpop--;
            m_canpop.notify();
        }
        return 1;
    }
}
#endif