#include <string>

class Code
{
public:

/** 
 * �ж��ַ����Ƿ�utf-8��ʽ,
 * 1 bytes 0xxxxxxx 
 * 2 bytes 110xxxxx 10xxxxxx 
 * 3 bytes 1110xxxx 10xxxxxx 10xxxxxx 
 * 4 bytes 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx 
 * ע�⣺
 *   ������5,6�ֽڵ�utf-8����
 *   ����ַ�������Ϊ0,������ascii����,����true
 */
    static bool IsUtf8(const std::string& str);
    static bool IsUtf8(const char* str);

private:
    static bool IsUtf8Internal(const char* pos, size_t len);

};
