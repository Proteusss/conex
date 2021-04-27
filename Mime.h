/**
 * @file Mime.h
 * @author your name (you@domain.com)
 * @brief tool for analyse mime.type
 * @date 2021-04-19
 */
#ifndef CONEX_MIME_H
#define CONEX_MIME_H
#include <unistd.h>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
namespace conex
{
std::string constructPath(const std::string& path)
{
    //获取代码所在的工作目录
    std::string homePath(::get_current_dir_name());
    if(path == "/")
    {
        return homePath+"/index.html";
    }
    else
    {
        return homePath + path;
    }
}
std::string getExtent(const std::string& path)
{   
    //FIXME: 边界情况会危险
    int i;
    for(i = path.size()-1; i >= 0; --i)
    {
        if( path[i] == '.')
            break;
    }
    std::string res(path.begin()+i+1,path.end());
    return res;

}
void getContentType(const std::string& extension, std::string& contentType)
{
    std::ifstream mimeTypes("mime.types");
    std::string line;
    while( getline(mimeTypes,line) )
    {
        //开始匹配
        if(line[0] != '#')
        {
            std::vector<std::string> exts;
            std::stringstream lineStream(line);
            //将contentype从流中输出到字符串中
            contentType.clear();
            lineStream>>contentType;
            //遍历可能的拓展名
            std::string tmpExtent;
            while(lineStream>>tmpExtent)
            {
                exts.push_back(tmpExtent);
            }
            for(auto ext : exts)
            {
                if(ext == extension)
                {
                    mimeTypes.close();
                    return;
                }
            }
        }
    }
    //无匹配项时设置为
    contentType = "text/plain";
    mimeTypes.close();
}
std::string getContent(const std::string& filename)
{
    std::ifstream fin(filename, std::ios::in |std::ios::binary);
    if(fin.fail())
    {
        return "";
    }
    std::ostringstream oss;
    oss<<fin.rdbuf();
    return oss.str();
}
}

#endif 