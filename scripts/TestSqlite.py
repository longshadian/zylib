
import sqlite3
import datetime


CREATE_TABLE_NETBAR = '''
CREATE TABLE netbar ( 
        id                    INT            PRIMARY KEY
                                             UNIQUE,
        netbar_name           VARCHAR( 64 )  NOT NULL
                                             DEFAULT ( '' ),
        netbar_server_version VARCHAR( 64 )  NOT NULL
                                             DEFAULT ( '' ),
        update_server_version VARCHAR( 64 )  NOT NULL
                                             DEFAULT ( '' ),
        update_plugin_version VARCHAR( 64 )  NOT NULL
                                             DEFAULT ( '' ),
        client_version        VARCHAR( 64 )  NOT NULL
                                             DEFAULT ( '' ),
        netbar_db_version     VARCHAR( 64 )  NOT NULL
                                             DEFAULT ( '' ),
        can_update            INT( 1 )       NOT NULL
                                             DEFAULT ( 0 ),
        updatetime            DATETIME       NOT NULL
                                             DEFAULT ( '1970-01-01 08:00:00' ) 
    );
'''

UPDATE_TABLE_NETBAR = '''update netbar set netbar_server_version = '{0}',
update_server_version = '{1}',
update_plugin_version = '{2}',
client_version = '{3}',
netbar_db_version = '{4}',
updatetime = '{5}'
where id = {6}
'''

def GetValue(data, key, default_value):
    if key in data:
        return data[key]
    else:
        return default_value


class DBManager:
    m_conn = sqlite3.Connection

    def Init(self, db_name=""):
        self.m_conn = sqlite3.connect(db_name)

    def Close(self):
        if self.m_conn is True:
            self.m_conn.close()

    def CreateDB(self, db_name=""):
        c = self.m_conn.cursor()
        c.execute(CREATE_TABLE_NETBAR)
        self.m_conn.commit()

    def UpdateTableNetbar(self, json_data):
        t = datetime.datetime.now()
        current_tm = t.strftime("%Y-%m-%d %H:%M:%S")
        a = '''
        sql = UPDATE_TABLE_NETBAR.format(json_data["netbar_server_version"],
                                         json_data["update_server_version"],
                                         json_data["update_plugin_version"],
                                         json_data["client_version"],
                                         json_data["netbar_db_version"],
                                         current_tm,
                                         json_data["shopid"])
                                         '''
        sql = UPDATE_TABLE_NETBAR.format(GetValue(json_data, "netbar_server_version", ""),
                                         GetValue(json_data, "update_server_version", ""),
                                         GetValue(json_data, "update_plugin_version", ""),
                                         GetValue(json_data, "client_version", ""),
                                         GetValue(json_data, "netbar_db_version", ""),
                                         current_tm,
                                         GetValue(json_data, "shopid", 0)
                                         )
        c = self.m_conn.cursor()
        c.execute(sql)
        c.close()
        self.m_conn.commit()


    def SelectNetbar(self, shopid=0):
        sql = "select can_update from netbar where id = {}".format(shopid)
        c = self.m_conn.cursor()
        c.execute(sql)
        can_update = 0
        for row in c:
            can_update = int(row[0])
            break
        c.close()
        return can_update


def Main():
    db_name = "zzz.db"
    dbmgr = DBManager()
    dbmgr.Init(db_name)

    data = {"shopid":1}
    dbmgr.UpdateTableNetbar(json_data=data)

    can_update = dbmgr.SelectNetbar(shopid=2)
    print("can_update {}".format(can_update))
    dbmgr.Close()


