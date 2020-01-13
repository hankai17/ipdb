# ipdb
spider ip database. use libcurl boost c11

## 注意事项
1. makefile管理两个目录:inc和src
2. inc和src目录下分别放了.hh文件和.cc文件
3. spider_ip.hh文件里有纯虚函数类spider_ip, 其它类(ip138/ipcn/tb/ipip/qip)均继承于它
4. 运行make命令 即生成spider可运行程序
5. 注意:程序依赖于boost库 需根据运行环境修改makefile文件

## 工具
1. tools目录下放置了2个工具和一个脚本
2. qip工具用于实时查询ip
3. mystop工具用于停止程序运行
4. sh脚本用于合并处理 爬得的.txt文件

## 日志 
1. log日志文件生成的默认位置在 /root 目录下, 可通过log.cc文件修改位置
