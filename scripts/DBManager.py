
import sqlite3
import datetime
import os

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
    #m_conn = None


    def Init(self, db_name=""):
        db_path = os.path.join(db_name)
        if not os.path.exists(db_path):
            return False
        self.m_conn = sqlite3.connect(db_name)
        return True

    def Close(self):
        if self.m_conn is True:
            self.m_conn.close()

    def UpdateTableNetbar(self, json_data):
        t = datetime.datetime.now()
        current_tm = t.strftime("%Y-%m-%d %H:%M:%S")
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


def TestMain():
    db_name = "zzz.db"
    dbmgr = DBManager()
    dbmgr.Init(db_name)

    data = {"shopid":1}
    dbmgr.UpdateTableNetbar(json_data=data)

    can_update = dbmgr.SelectNetbar(shopid=2)
    print("can_update {}".format(can_update))
    dbmgr.Close()


