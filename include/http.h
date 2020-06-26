#ifndef _HTTP_H_
#define _HTTP_H_

#include <string>
#include <map>
#include "sys/types.h"

    #define HTTP_VERSION "HTTP/1.1"
    #define HTTP_HEAD_CONTENT_TYPE "Content-type"
    #define HTTP_HEAD_CONTENT_LEN "Content-length"
    #define HTTP_HEAD_CONNECTION "Connection"
    #define HTTP_HEAD_KEEP_ALIVE "Keep-Alive"

    #define HTTP_ATTR_KEEP_ALIVE "keep-alive"
    #define HTTP_HEAD_HTML_TYPE "text/html"
    #define HTTP_HEAD_CSS_TYPE "text/css"
    #define HTTP_HEAD_GIF_TYPE "text/gif"
    #define HTTP_HEAD_JPG_TYPE "text/jpeg"
    #define HTTP_HEAD_PNG_TYPE "text/png"
    inline size_t addrlen(const char* start,const char* end);
    //文件拓展名
    std::string extention_name(const std::string& basename);
    const char* find_content(const char* start,const char*end,char endc,size_t& contlen,size_t& sumlen);
    const char* find_line(const char* start,const char*end);
    const char* find_headline(const char* start,const char*end);
    const char* http_content_type(const std::string& extension);
    void split_url(const std::string url,std::string& dir,std::string& base);
    std::string http_path_handle(const std::string& dname,const std::string& bname);
    

    typedef std::map<std::string,std::string> HttpHead_t;
    typedef std::pair<std::string,std::string> HttpHeadPair_t;
    class IHttpRequest{
        public:
            ~IHttpRequest(){};
            virtual const std::string& start_line()=0;
            virtual const std::string& method()=0;
            virtual const std::string& url()=0;
            virtual const std::string& version()=0;
            virtual const HttpHead_t& headers()=0;
            virtual bool has_head(const std::string&)=0;
            virtual const std::string& head_content(const std::string&)=0;
            virtual const size_t boby_len()=0;
            virtual const char* body()=0;
    };
    class SHttpRequest:public IHttpRequest{
        public:
            SHttpRequest();
            ~SHttpRequest();
            int load_packet(const char* msg,size_t msglen);
            const std::string& start_line();
            const std::string& method();
            const std::string& url();
            const std::string& version();
            const HttpHead_t& headers();
            bool has_head(const std::string& name);
            const std::string& head_content(const std::string& name);
            const size_t boby_len();
            const char* body();
            const char* strerror();
        private:
            inline void set_strerror(const char* str);
            int parse_startline(const char* start,const char* end);
            int parse_headers(const char* start,const char* end);
            int parse_body(const char* start,const char* end);

            std::string m_strerr;
            std::string m_method;
            std::string m_startline;
            std::string m_url;
            std::string m_version;
            HttpHead_t m_headers;
            char* m_body;
            size_t m_bodylen;
    };
    class IHttpRespose
    {
	public:
		virtual ~IHttpRespose(){};
		virtual int set_version(const std::string &) = 0;
		virtual int set_status(const std::string &, const std::string &) = 0;
		virtual int add_head(const std::string &, const std::string &) = 0;
		virtual int del_head(const std::string &) = 0;
		virtual int set_body(const char *, size_t) = 0;
		virtual size_t size() = 0;
		virtual const char *serialize() = 0;
    };
    class SHttpRespose: public IHttpRespose{
	public:
		SHttpRespose();
		~SHttpRespose();
		int set_version(const std::string &version);
		int set_status(const std::string &status, const std::string &reason);
		int add_head(const std::string &name, const std::string &attr);
		int del_head(const std::string &name);
		int set_body(const char *body, size_t bodylen);
		size_t size();
		char *serialize();
		
	private:
		size_t startline_stringsize();
		size_t headers_stringsize();

		enum Config{
			MAXLINE = 1024,
			BODY_MAXSIZE = 64 * 1024,
		};
		typedef struct {
			std::string version;
			std::string status;
			std::string reason;
			HttpHead_t headers;
			char *body;
			size_t bodylen;
			char *data;
			size_t datalen;
			size_t totalsize;
			bool dirty;
		}respose_package_t;
		
	private:
		respose_package_t m_package;
};

IHttpRespose* handle_request(IHttpRequest &request);

#endif