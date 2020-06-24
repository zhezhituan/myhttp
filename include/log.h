#ifndef _LOG_H_
#define _LOG_H_

#include<iostream>
#include<string>
#include<string.h>
#include<fstream>

typedef enum logrank{
INFO,
ERROR
} log_rank;   
class slog{
    public:
        slog(const std::string&,const std::string&);
        ~slog();
        std::ostream& logout(log_rank lr,const std::string& file,const std::string& function,const int line);
        static slog* getslog();
        static std::ofstream info_log_file;
        static std::ofstream error_log_file;
        static slog* m_log;
};
#ifdef _LOG_COUT_
    #define LOG(log_rank) std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<"  "
#else
    #define LOG(log_rank) slog::getslog()->logout(log_rank,__FILE__,__FUNCTION__,__LINE__)
#endif 
   

#endif
