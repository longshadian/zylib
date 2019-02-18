#ifndef _DATABASE_QUERYRESULT_H
#define _DATABASE_QUERYRESULT_H

#include <memory>

class GameUser;
class ObjUser;

enum class DB_RESULT
{
    SUCCESS   = 0,    //�ɹ�
    ERROR     = 1,    //����
};

template <typename T>
struct QueryResult
{
    DB_RESULT   m_result;
    T           m_data;
};

using DBLoadUserResult      = QueryResult<std::shared_ptr<ObjUser>>;  //���ݿ�����û�����
using DBRegisterUserResult  = QueryResult<std::shared_ptr<ObjUser>>;  //���ݿ�ע���û�����

using GameLoadUserResult        = QueryResult<std::shared_ptr<GameUser>>;
using GameRegisterUserResult    = QueryResult<std::shared_ptr<GameUser>>;

#endif
