
import logging.config
import os
import sys
import time
from logging.handlers import TimedRotatingFileHandler


LOG_MODULE_NAME = "zhwkupdate"

def GetLog():
    return logging.getLogger(LOG_MODULE_NAME)


def InitLogToFile():
    LOGFILE_DIR = os.path.join("./log/")
    if not os.path.exists(LOGFILE_DIR):
        os.mkdir(LOGFILE_DIR)
    LOGFILE = os.path.join(LOGFILE_DIR, "update_cms.log")

    STANDARD_FORMAT = '[%(asctime)s] [%(threadName)s:%(thread)d] [%(filename)s:%(lineno)d] ' \
                      '[%(levelname)s] %(message)s'
    fmt = logging.Formatter(STANDARD_FORMAT)
    hdl = TimedRotatingFileHandler(filename=LOGFILE, when='H')
    hdl.setFormatter(fmt)
    logger = logging.getLogger(LOG_MODULE_NAME)
    logger.addHandler(hdl)


def InitLogToConsole():
    STANDARD_FORMAT = '[%(asctime)s] [%(threadName)s:%(thread)d] [%(filename)s:%(lineno)d] ' \
                      '[%(levelname)s] %(message)s'
    fmt = logging.Formatter(STANDARD_FORMAT)
    hdl = logging.StreamHandler(stream=sys.stdout)
    hdl.setFormatter(fmt)
    logger = logging.getLogger(LOG_MODULE_NAME)
    logger.addHandler(hdl)


def InitLog():
    InitLogToFile()
    InitLogToConsole()
    GetLog().setLevel(logging.INFO)


if __name__ == '__main__':
    InitLog()
    while True:
        GetLog().info("aaaaaa ")
        time.sleep(1)
