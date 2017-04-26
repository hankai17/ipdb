#pragma once
#include"spider_ip.hh"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <boost/regex.hpp>

class ipip_spider_ip: public spider_ip
{
    public:
        ipip_spider_ip(std::string ip_b, std::string ip_e);
        static int write_data(char* page_data,size_t size, size_t nmemb,std::string *data);
        virtual std::string confirm_url(std::string ip);
        virtual void curl_http();
        virtual void parse_data();
        virtual void insert_db();
        ~ipip_spider_ip();

        int g2u(char *inbuf,size_t inlen,char *outbuf,size_t outlen); 
        int code_convert(char *from_charset,char *to_charset,char *inbuf,size_t inlen,char *outbuf,size_t outlen);
        std::string revise_result(std::string&);
        int cidr(std::string ip_s, std::string ip_e, std::string des); 
    public:
        std::string ip_begin; 
        std::string ip_end;
        char url[1024];
        char User_Agent[500];
        std::string buffer;
        char unRet;
        long code;
        std::string current_ip;

        std::string country;
        std::string province;
        std::string city;
        std::string isp;
        std::string ip;
        int fd;
        int step;

        std::string current_result;
        std::string next_result;
        int ip_back;

        int once_flag;

        std::string write_current_data;
        std::string write_next_data;

        std::string write_last_ip;
        int once1_flag;

        std::string pre_ip;
        std::string pre_data;

        boost::regex::flag_type flag;
        std::string regex;
        boost::smatch result;

};

