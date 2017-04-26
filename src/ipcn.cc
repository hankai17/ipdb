#include "ipcn.hh"
#include<iconv.h>
#include <chrono>
#include <thread>
#include"log.hh"

int ipcn_spider_ip::write_data(char* page_data,size_t size, size_t nmemb,std::string *data)
{
    if(data == NULL)
        return 0;
    data->append(page_data);
    return size * nmemb;
}

ipcn_spider_ip::ipcn_spider_ip(std::string ip_b, std::string ip_e)
{
    step = 0;
    ip_back = 0;
    current_result = "";
    next_result = "";
    ip_begin = ip_b; 
    ip_end = ip_e; 
    current_ip = ip_begin;
    once_flag = 0;
    once1_flag = 0;
    pre_ip = "";
    pre_data = "";

    write_current_data = "";
    write_next_data = "";
    write_last_ip = "";
    memcpy(User_Agent,"User-Agent: Mozilla/5.0 (Windows NT 6.1; \
        WOW64; rv:52.0) Gecko/20100101 Firefox/52.0", 
            strlen("User-Agent: Mozilla/5.0 (Windows NT 6.1; \
        WOW64; rv:52.0) Gecko/20100101 Firefox/52.0"));
    memcpy(url, "http://www.ip.cn/ip-", strlen("http://www.ip.cn/ip-")); 
    char file_name[64] = {0};
    sprintf(file_name,"%s~~%s_ipcn.txt",(char*)ip_b.data(),(char*)ip_e.data());
    fd = open(file_name, O_RDWR|O_TRUNC|O_CREAT, 0664);
}

ipcn_spider_ip::~ipcn_spider_ip()
{
    close(fd);
}

std::string ipcn_spider_ip::confirm_url(std::string ip)
{
    if(once_flag == 0)
    {
        unsigned long ip_long = inet_network(ip.data());
        char tmp[32];
        int_2_str(ip_long,tmp); 
        std::string current_ip1 = {tmp};
        std::string new_url = url + current_ip1 + ".html"; 
        once_flag = 1;
        pre_ip = {ip};
        return new_url;
    }
    else
    {
        if( inet_network(ip.data()) < inet_network(ip_end.data()) )
        {
            unsigned long ip_long = inet_network(ip.data());
            if(ip_back)
            {
                ip_long = ip_long - step + 1;
                step = 1;
                ip_back = 0;
            }
            else
            {
                ip_long += step;
            }
            char tmp[32];
            int_2_str(ip_long,tmp); 
            current_ip = {tmp};
            std::string new_url = url + current_ip + ".html"; 
            return new_url;
        }
        else if( inet_network(ip.data()) > inet_network(ip_end.data()) )
        {
            unsigned long ip_long = inet_network(ip.data());
            ip_long = ip_long - step + 1;
            step = 1;
            ip_back = 0;
            char tmp[32];
            int_2_str(ip_long,tmp); 
            current_ip = {tmp};
            std::string new_url = url + current_ip + ".html"; 
            return new_url;
        }
        else 
        {
            std::string err_url = {"NULL"};
            return err_url;
        }
    }
    std::string err_url = {"NULL"};
    return err_url;
}

void ipcn_spider_ip::curl_http()
{
    struct curl_slist *slist = NULL;
    slist = curl_slist_append(slist, User_Agent);
    while(1)
    {
        std::string&& current_url = confirm_url(current_ip);
        std::cout<<"current_url="<<current_url<<std::endl;
        if(current_url == "NULL")
        {
            if(write_last_ip != ip_end)
            {
                Log(__FILE__, __LINE__,Level[4],0,"%s~~%s",(char*)(ip_begin.data()),(char*)(ip_end.data()));
            }
            break;
        }
        CURL* curl = curl_easy_init(); 
        std::chrono::milliseconds sleepDuration(300);
        if (curl != NULL)
        {
            curl_easy_setopt(curl, CURLOPT_URL, current_url.data());
            curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
            curl_easy_setopt(curl, CURLOPT_HEADER, 0); 
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, slist);
            curl_easy_setopt(curl, CURLOPT_HTTPGET, 0);
            curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10);
            curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 10);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data); 
            curl_easy_setopt(curl, CURLOPT_WRITEDATA,&buffer);

            unRet = curl_easy_perform(curl);
            if (CURLE_OK == unRet)
            {
                curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &code);
                if (code != 200)
                {
                    Log(__FILE__, __LINE__,Level[2],0,"%s~~%s %s curl_perform ok,but reponse:%d",(char*)(ip_begin.data()),(char*)(ip_end.data()),(char*)(current_url.data()),(int)code);
                    buffer = "";
                }
            }
            else 
            {
                Log(__FILE__, __LINE__,Level[2],0,"%s~~%s %s curl_perform err",(char*)(ip_begin.data()),(char*)(ip_end.data()),(char*)(current_url.data()));
                    buffer = "";
            }
            curl_easy_cleanup(curl);
        }
        else 
        {
            Log(__FILE__, __LINE__,Level[2],0,"%s~~%s %s curl err",(char*)(ip_begin.data()),(char*)(ip_end.data()),(char*)(current_url.data()));
                    buffer = "";
        }

        parse_data();
        buffer = "";
        std::this_thread::sleep_for(sleepDuration);
    }
}

int ipcn_spider_ip::cidr(std::string ip_s, std::string ip_e, std::string des) 
{
    uint32_t ip_start, ip_end;
    struct in_addr addr;
    ip_start = ntohl(inet_addr((char*)ip_s.data()));
    ip_end = ntohl(inet_addr((char*)ip_e.data()));

    uint32_t net = ip_start;
    uint32_t mask;
    int i;
    while ( net <= ip_end ) 
    {
        for(i = 32; i >=0; i--) 
        {
            mask = (i == 0) ? 0 : 0xffffffff << (32 - i);
            if ( (net & mask) != net || (net | ~mask) > ip_end )
            {
                addr.s_addr = htonl(net);
                char tmp[128] = {0};
                sprintf(tmp,"%s/%d ",inet_ntoa(addr),i+1);
                std::string d = {tmp + des + "\n"};
                write(fd, (char*)d.data(), strlen(d.data())); 
                mask = 0xffffffff << (32 - i - 1);
                net = (net | ~mask) + 1;
                break;
            }
        }

        if( i == -1 ) 
        {
            //std::cout<<"0.0.0.0/0\n";
            break;
        }
        if( net == 0 ) 
        {
            break;
        }
    }
    return 0;
}

void ipcn_spider_ip::parse_data()
{
    if(buffer.size() == 0)
    {
        Log(__FILE__, __LINE__,Level[2],0,"%s~~%s %s no data, again",(char*)(ip_begin.data()),(char*)(ip_end.data()),(char*)(current_ip.data()));
        sleep(10);
        unsigned long ip_long = inet_network((char*)(current_ip.data()));
        ip_long = ip_long - step;
        char tmp[32];
        int_2_str(ip_long,tmp); 
        current_ip = {tmp};
        return;
    }
    std::string detail1 = buffer.substr(buffer.find("所在地理位置：<code>")+strlen("所在地理位置：<code>"), buffer.find("<",buffer.find("所在地理位置：<code>")+strlen("所在地理位置：<code>"))-(buffer.find("所在地理位置：<code>")+strlen("所在地理位置：<code>")));

    //std::string city = buffer.substr(buffer.find("GeoIP: ")+strlen("GeoIP: "), buffer.find(",",buffer.find("GeoIP: ")+strlen("GeoIP: "))-(buffer.find("GeoIP: ")+strlen("GeoIP: ")));
    std::string detail = revise_result(detail1);
    if(once1_flag == 0)
    {
        pre_data = detail;
        once1_flag = 1;
    }
    std::string des = current_ip + " " + detail + "\n";


    if(current_result == "" )
    {
        current_result = detail;
        write_current_data = detail;

        if(write_next_data != write_current_data)
        {
            if(pre_ip != current_ip)
            {
                unsigned long ip_foo = inet_network(current_ip.data());
                ip_foo --;
                char tmp[32];
                int_2_str(ip_foo,tmp); 
                std::string current_ip_foo = {tmp};

                cidr(pre_ip,current_ip_foo,pre_data);
                pre_data = detail;
            } 

            pre_ip = current_ip;
            write_last_ip = current_ip;
        }
        write_current_data = write_next_data;
        write_next_data = "";
        step = 1;
    }
    else 
    {
        next_result = detail; 
    }

    if( current_result != "" && next_result != "" )
    {
        if( !strcmp(current_result.data(),next_result.data())) 
        {
            if(inet_network(current_ip.data()) <= inet_network(ip_end.data()))
            {
                step *= 2;
                write_next_data = detail;
                current_result = next_result;
                next_result = "";
            }
            else
            {
                current_ip = ip_end;
            }
        }
        else
        {
            current_result = ""; 
            next_result = "";
            ip_back = 1; 
        }
    }
}

void ipcn_spider_ip::insert_db()
{
    //std::cout << "insert_db" << std::endl;
}

