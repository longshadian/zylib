
import logging.config
import os
import sys


LOGFILE_DIR=os.path.join("./log/")

'''
STANDARD_FORMAT = '[%(asctime)s][%(threadName)s:%(thread)d][task_id:%(name)s][%(filename)s:%(lineno)d]' \
                  '[%(levelname)s]%(message)s' #其中name为getlogger指定的名字
                  
'''

STANDARD_FORMAT = '[%(asctime)s] [%(threadName)s:%(thread)d] [%(filename)s:%(lineno)d] ' \
                  '[%(levelname)s] %(message)s'


SIMPLE_FORMAT = '[%(levelname)s][%(asctime)s][%(filename)s:%(lineno)d]%(message)s'

ID_SIMPLE_FORMAT = '[%(levelname)s][%(asctime)s] %(message)s'

LOGFILE_PATH = os.path.join(LOGFILE_DIR, "a.log")

LOGGING_DIC = {
    'version': 1,
    'disable_existing_loggers': False,
    'formatters': {
        'standard': {
            'format': STANDARD_FORMAT
        },
        'simple': {
            'format': SIMPLE_FORMAT
        },
    },
    'filters': {},
    'handlers': {
        #打印到终端的日志
        'console': {
            'level': 'INFO',
            'class': 'logging.StreamHandler',  # 打印到屏幕
            'formatter': 'standard'
        },
        #打印到文件的日志,收集info及以上的日志
        'default': {
            'level': 'DEBUG',
            'class': 'logging.handlers.RotatingFileHandler',  # 保存到文件
            'formatter': 'standard',
            'filename': LOGFILE_PATH,  # 日志文件
            'maxBytes': 1024*100,  # 日志大小 5M
            'backupCount': 5,
            'encoding': 'utf-8',  # 日志文件的编码，再也不用担心中文log乱码了
        },
    },
    'loggers': {
        'zhwkupdate': {
            'handlers': ['console'],  # 这里把上面定义的两个handler都加上，即log数据既写入文件又打印到屏幕
            'level': 'INFO',
            'propagate': True,  # 向上（更高level的logger）传递
        },
    },
}


def InitLog():
    if not os.path.exists(LOGFILE_DIR):
        os.mkdir(LOGFILE_DIR)

    logging.config.dictConfig(LOGGING_DIC)
    logger = logging.getLogger("zhwkupdate")

    GetLog()

def GetLog():
    return logging.getLogger("zhwkupdate")
