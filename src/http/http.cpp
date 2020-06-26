#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <libgen.h>
#include <unistd.h>
#include <sys/stat.h>

#include "log.h"
#include "http.h"
size_t addrlen(const char *start, const char *end)
{
    return (size_t)(end - start);
}
std::string extention_name(const std::string &basename)
{
    std::string ex_name("");
    size_t pos = basename.find_last_of('.');
    if (pos == std::string::npos)
        return ex_name;
    else
        return basename.substr(pos + 1);
}
const char *find_content(const char *start, const char *end, char endc, size_t &contlen, size_t &sumlen)
{
    size_t contentlen = 0;
    const char *contstart = NULL;
    for (const char *mstart = start; mstart < end; mstart++)
    {
        if (contstart == NULL)
        {
            //直到mstart不为空，确定start
            if (*mstart != ' ')
            {
                contstart = mstart;
                contentlen = 1;
            }
        }
        else
        {
            if (*mstart == endc)
            {
                contlen = contentlen;
                sumlen = addrlen(start, mstart);
                return contstart;
            }
            contentlen++;
        }
    }
    return NULL;
}
const char *find_line(const char *start, const char *end)
{
    //找到\r\n组成的空行
    for (const char *i = start; i < end - 1; i++)
    {
        if (i[0] == '\r' && i[1] == '\n')
        {
            return &i[2];
        }
    }
    return NULL;
}
const char *find_headline(const char *start, const char *end)
{
    //在头部前面有两个\r\n
    for (const char *hstart = start; hstart < (end - 3); hstart++)
    {
        if (hstart[0] == '\r' && hstart[1] == '\n' &&
            hstart[2] == '\r' && hstart[3] == '\n')
        {
            return &hstart[4];
        }
    }
    return NULL;
}
const char *http_content_type(const std::string &extension)
{
    if (extension.compare("html") == 0)
        return HTTP_HEAD_HTML_TYPE;
    else if (extension.compare("css") == 0)
        return HTTP_HEAD_CSS_TYPE;
    else if (extension.compare("gif") == 0)
        return HTTP_HEAD_GIF_TYPE;
    else if (extension.compare("jpg") == 0)
        return HTTP_HEAD_JPG_TYPE;
    else if (extension.compare("png") == 0)
        return HTTP_HEAD_PNG_TYPE;
    return NULL;
}
SHttpRequest::SHttpRequest() : m_strerr("success"), m_body(NULL), m_bodylen(0) {}
SHttpRequest::~SHttpRequest()
{
    if (m_body)
        delete m_body;
}
int SHttpRequest::load_packet(const char *msg, size_t msglen)
{
    const char *remainmsg = msg;
    const char *endmsg = msg + msglen;
    //find first endline
    const char *endline = find_line(remainmsg, endmsg);
    if (endline == NULL)
    {
        set_strerror("startLine no find");
        return -1;
    }
    if (parse_startline(remainmsg, endline) < 0)
    {
        set_strerror("invalid startline");
        return -1;
    }
    remainmsg = endline;
    const char *headline_end = find_headline(remainmsg, endmsg);
    if (headline_end == NULL)
    {
        set_strerror("header no find");
        return -1;
    }
    if (parse_headers(remainmsg, headline_end) < 0)
    {
        set_strerror("parse header error");
        return -1;
    }
    remainmsg = headline_end;
    if (parse_body(remainmsg, endmsg) < 0)
    {
        set_strerror("parse body error");
        return -1;
    }
    return 0;
}
const std::string &SHttpRequest::start_line()
{
    return m_startline;
}
const std::string &SHttpRequest::method()
{
    return m_method;
}
const std::string &SHttpRequest::url()
{
    return m_url;
}
const std::string &SHttpRequest::version()
{
    return m_version;
}
const HttpHead_t &SHttpRequest::headers()
{
    return m_headers;
}
bool SHttpRequest::has_head(const std::string &name)
{
    HttpHead_t::iterator itor = m_headers.find(name);
    if (itor == m_headers.end())
        return false;
    return true;
}
const std::string &SHttpRequest::head_content(const std::string &name)
{
    if (m_headers.find(name) != m_headers.end())
        return m_headers[name];
    return "";
}
const size_t SHttpRequest::boby_len()
{
    return m_bodylen;
}
const char *SHttpRequest::body()
{
    return m_body;
}
const char *SHttpRequest::strerror()
{
    return m_strerr.c_str();
}
inline void SHttpRequest::set_strerror(const char *str)
{
    m_strerr = str;
}
int SHttpRequest::parse_startline(const char *start, const char *end)
{
    size_t contlen = 0, sumlen = 0;
    const char *cont = NULL, *remainbuff = start;
    cont = find_content(remainbuff, end, '\r', contlen, sumlen);
    if (cont == NULL)
        return -1;
    m_startline = std::string(cont, contlen);
    cont = find_content(remainbuff, end, ' ', contlen, sumlen);
    if (cont == NULL)
        return -1;
    m_method = std::string(cont, contlen);
    remainbuff += sumlen;

    cont = find_content(remainbuff, end, ' ', contlen, sumlen);
    if (cont == NULL)
        return -1;
    m_url = std::string(cont, contlen);
    remainbuff += sumlen;

    cont = find_content(remainbuff, end, '\r', contlen, sumlen);
    if (cont == NULL)
        return -1;
    m_version = std::string(cont, contlen);
    return 0;
}
int SHttpRequest::parse_headers(const char *start, const char *end)
{
    size_t contlen = 0, sumlen = 0;
    const char *line_start = start;
    std::string head, attr;
    m_headers.clear();
    for (;;)
    {
        const char *line_end = find_line(line_start, end);
        if (line_end == NULL)
            return -1;
        else if (line_end == end) // end
            break;

        const char *headstart = find_content(line_start, line_end, ':', contlen, sumlen);
        if (headstart == NULL)
            return -1;
        head = std::string(headstart, contlen);

        const char *attrstart = line_start + sumlen + 0x01;
        attrstart = find_content(attrstart, line_end, '\r', contlen, sumlen);
        if (attrstart == NULL)
            return -1;
        attr = std::string(attrstart, contlen);

        line_start = line_end;
        m_headers[head] = attr;
    }
    return 0;
}

int SHttpRequest::parse_body(const char *start, const char *end)
{
    size_t bodylen = addrlen(start, end);
    if (bodylen == 0x00)
        return 0;

    char *buff = new char[bodylen];
    if (buff == NULL)
        return -1;
    memcpy(buff, start, bodylen);

    if (m_body != NULL)
        delete m_body;
    m_body = buff;
    m_bodylen = bodylen;
    return 0;
}
SHttpRespose::SHttpRespose()
{
    m_package.body = NULL;
    m_package.bodylen = 0x00;
    m_package.data = NULL;
    m_package.datalen = 0x00;
    m_package.dirty = true;
}
SHttpRespose::~SHttpRespose()
{
    if (m_package.body != NULL)
        delete[] m_package.body;
    if (m_package.data != NULL)
        delete[] m_package.data;
}
int SHttpRespose::set_version(const std::string &version)
{
    m_package.version = version;
    m_package.dirty = true;
    return 0;
}
int SHttpRespose::set_status(const std::string &status, const std::string &reason)
{
    m_package.status = status;
    m_package.reason = reason;
    m_package.dirty = true;
    return 0;
}
int SHttpRespose::add_head(const std::string &name, const std::string &attr)
{
    if (name.empty() || attr.empty())
        return -1;

    m_package.headers[name] = attr;
    m_package.dirty = true;
    return 0;
}
int SHttpRespose::del_head(const std::string &name)
{
    HttpHead_t::iterator itor = m_package.headers.find(name);
    if (itor == m_package.headers.end())
        return -1;
    m_package.headers.erase(itor);
    m_package.dirty = true;
    return 0;
}
int SHttpRespose::set_body(const char *body, size_t bodylen)
{
    if (body == NULL || bodylen == 0x00 || bodylen > BODY_MAXSIZE)
        return -1;

    char *buff = new char[bodylen];
    assert(buff != NULL);

    memcpy(buff, body, bodylen);

    if (m_package.body != NULL)
        delete[] m_package.body;
    m_package.body = buff;
    m_package.bodylen = bodylen;
    m_package.dirty = true;
    return 0;
}
size_t SHttpRespose::size()
{
    if (m_package.dirty)
    {
        m_package.totalsize = startline_stringsize() + headers_stringsize();
        m_package.totalsize += m_package.bodylen;
    }
    return m_package.totalsize;
}
char *SHttpRespose::serialize()
{
    if (!m_package.dirty)
        return m_package.data;

    size_t totalsize = size();
    char *buffreserver = new char[totalsize];
    assert(buffreserver != NULL);

    char *buff = buffreserver;
    int nprint = snprintf(buff, totalsize, "%s %s %s\r\n", m_package.version.c_str(),
                          m_package.status.c_str(), m_package.reason.c_str());
    if (nprint < 0)
        goto ErrorHandle;

    totalsize -= nprint;
    buff += nprint;

    for (HttpHead_t::iterator itor = m_package.headers.begin(); itor != m_package.headers.end(); itor++)
    {
        const std::string &name = itor->first;
        const std::string &attr = itor->second;

        nprint = snprintf(buff, totalsize, "%s: %s\r\n", name.c_str(), attr.c_str());
        if (nprint < 0)
            goto ErrorHandle;
        totalsize -= nprint;
        buff += nprint;
    }

    nprint = snprintf(buff, totalsize, "\r\n");
    if (nprint < 0)
        goto ErrorHandle;
    totalsize -= nprint;
    buff += nprint;

    memcpy(buff, m_package.body, totalsize);
    if (totalsize != m_package.bodylen)
    {
        LOG(ERROR) << "body copy error, target %ld, actually %ld\n"
                   << m_package.bodylen << totalsize;
    }
    if (m_package.data != NULL)
        delete m_package.data;
    m_package.data = buffreserver;

    m_package.dirty = false;
    return m_package.data;
ErrorHandle:
    delete buffreserver;
    return NULL;
}
size_t SHttpRespose::startline_stringsize()
{
    const size_t otherchar_size = 1 * 2 + 2; // black * 2 + CRLF
    size_t total_size = otherchar_size + m_package.version.size();
    total_size += m_package.status.size() + m_package.reason.size();
    return total_size;
}
size_t SHttpRespose::headers_stringsize()
{
    const size_t otherchar_size = 2 + 2;  // ': ' + CRLF
    const size_t head_terminatorsize = 2; // CRLF

    size_t stringsize = 0;
    HttpHead_t::iterator itor = m_package.headers.begin();
    for (; itor != m_package.headers.end(); itor++)
    {
        const std::string &name = itor->first;
        const std::string &attr = itor->second;

        stringsize += name.size() + attr.size() + otherchar_size;
    }

    stringsize += head_terminatorsize;
    return stringsize;
}
    #define HTTP_ROOT "html"
    #define HTTP_DEFAULTFILE 	"index.html"
    #define HTTP_SLASH 			"/"
    #define HTTP_CURRENT_DIR	"."
    #define HTTP_ABOVE_DIR 		".."
std::string http_path_handle(const std::string& dname,const std::string& bname){
        std::string filepath(HTTP_ROOT);
	    if(bname == HTTP_SLASH || bname == HTTP_CURRENT_DIR || \
	    	bname == HTTP_ABOVE_DIR){	// limit in root dir
	    	filepath += HTTP_SLASH;
	    	filepath += HTTP_DEFAULTFILE;
	    }else if(dname == HTTP_CURRENT_DIR){
	    	filepath += HTTP_SLASH;
	    	filepath += bname;
	    }else if(dname == HTTP_SLASH){
	    	filepath += dname;
	    	filepath += bname;
	    }else{
	    	filepath += dname;
	    	filepath += HTTP_SLASH;
	    	filepath += bname;
	    }
	    return filepath;
    }

    void split_url(const std::string url,std::string& dir,std::string& base){
        char *dirc = strdup(url.c_str());
	    dir = dirname(dirc);//截取路径
	    delete dirc;
	    char *basec = strdup(url.c_str());
	    base = basename(basec);	//截取文件
	    delete basec;	
    }
IHttpRespose* handle_request(IHttpRequest &request){
	    const std::string &method = request.method();
	    const std::string &url = request.url();

	    std::string dname, bname;
	    split_url(url, dname, bname);

	    SHttpRespose *respose = new SHttpRespose;	
	    std::string filepath = http_path_handle(dname, bname);
	    if(method == "GET"){

	    	std::string extention = extention_name(filepath);
	    	if(extention.empty() || access(filepath.c_str(), R_OK) < 0){
	    		respose->set_version(HTTP_VERSION);
	    		respose->set_status("404", "Not Found");
	    		respose->add_head(HTTP_HEAD_CONNECTION, "close");
	    		return respose;
	    	}

    
	    	struct stat filestat;
	    	stat(filepath.c_str(), &filestat);
	    	const size_t filesize = filestat.st_size;

	    	char *fbuff = new char[filesize];
	    	assert(fbuff != NULL);

	    	FILE *fp = fopen(filepath.c_str(), "rb");
	    	if(fp == NULL || fread(fbuff, filesize, 1, fp) != 0x01){
            
	    		delete fbuff;

	    		respose->set_version(HTTP_VERSION);
	    		respose->set_status("500", "Internal Server Error");
	    		respose->add_head(HTTP_HEAD_CONNECTION, "close");
	    		return respose;
	    	}
    
	    	fclose(fp);

	    	char sfilesize[16] = {0x00};
	    	snprintf(sfilesize, sizeof(sfilesize), "%ld", filesize);

	    	respose->set_version(HTTP_VERSION);
	    	respose->set_status("200", "OK");
	    	respose->add_head(HTTP_HEAD_CONTENT_TYPE, http_content_type(extention));
	    	respose->add_head(HTTP_HEAD_CONTENT_LEN, sfilesize);
	    	respose->add_head(HTTP_HEAD_CONNECTION, "keep-alive");
	    	respose->set_body(fbuff, filesize);
	    	delete fbuff;
	    }

	    return respose;
    }   