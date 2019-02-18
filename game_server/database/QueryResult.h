#ifndef _DATABASE_QUERYRESULT_H
#define _DATABASE_QUERYRESULT_H

#include <memory>

class GameUser;
class ObjUser;

enum class DB_RESULT
{
    SUCCESS   = 0,    //成功
    ERROR     = 1,    //出错
};

template <typename T>
struct QueryResult
{
    DB_RESULT   m_result;
    T           m_data;
};

using DBLoadUserResult      = QueryResult<std::shared_ptr<ObjUser>>;  //数据库加载用户数据
using DBRegisterUserResult  = QueryResult<std::shared_ptr<ObjUser>>;  //数据库注册用户数据

using GameLoadUserResult        = QueryResult<std::shared_ptr<GameUser>>;
using GameRegisterUserResult    = QueryResult<std::shared_ptr<GameUser>>;

#endif
