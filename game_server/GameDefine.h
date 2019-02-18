#ifndef _GAME_DEFINE_H_
#define _GAME_DEFINE_H_

#include <cstdint>
#include <limits>
#include <string>

namespace gamedef {

//����������
enum { USER_INIT_DIAMOND  = 1000 };            //��ҳ�ʼ��ʯ

//ÿ�����ͬʱ���ڷ�����
enum { MAX_USER_ROOM_COUNT = 10 };

//��Ҵ�������δʹ�ú�ر�
enum { ROOM_TIMEOUT_SECONDS = 60 * 1 };


extern const std::string DATABASE_ID_GAME;
extern const std::string DATABASE_ID_STATIS;

extern const std::string REDIS_ID_GAME;

extern const std::string GUID_USERID;

extern const std::string GAME_VERSION;

}

#endif