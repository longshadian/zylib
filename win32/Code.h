#include <string>

class Code
{
public:

/** 
 * 判断字符串是否utf-8格式,
 * 1 bytes 0xxxxxxx 
 * 2 bytes 110xxxxx 10xxxxxx 
 * 3 bytes 1110xxxx 10xxxxxx 10xxxxxx 
 * 4 bytes 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx 
 * 注意：
 *   不考虑5,6字节的utf-8编码
 *   如果字符串长度为0,或者是ascii编码,返回true
 */
    static bool IsUtf8(const std::string& str);
    static bool IsUtf8(const char* str);

private:
    static bool IsUtf8Internal(const char* pos, size_t len);

};
