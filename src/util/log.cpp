#include "log.h"
std::ofstream slog::info_log_file;
std::ofstream slog::error_log_file;
slog* slog::m_log;
slog::slog(const std::string& info_file,const std::string& error_file){
    slog::info_log_file.open(info_file.c_str(),std::ios::app);
    slog::error_log_file.open(error_file.c_str(),std::ios::app);
}
std::ostream&  slog::logout(log_rank lr,const std::string& file,const std::string& function,const int line){
    time_t tm;
    time(&tm);
    char* dt = ctime(&tm);
    dt[strlen(dt)-1]=0;
    if(lr==INFO){
        return slog::info_log_file<<dt<<file<<":"<<function<<":"<<line<<"  ";
    }
    else return slog::error_log_file<<dt<<file<<":"<<function<<":"<<line<<"  ";
}
slog* slog::getslog(){
    if(slog::m_log==NULL){
        slog::m_log=new slog("/home/leon/myhttp/log/info.log","/home/leon/myhttp/log/error.log");
    }
    return slog::m_log;
}