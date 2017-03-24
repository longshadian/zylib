import xml.etree.ElementTree as ET
import openpyxl
import json
import sys
import os
import types

ATTR_KEYS = ("name", "column", "type", "to", "default_value")
ATTR_TYPES = {"int32":int, "uint32":int, "int64":int, "uint64":int, "string":str}

##描述文件
class DescData:
    m_from = ""
    m_to = ""
    m_to_dir = "."
    m_field = []
    def __init__(self):
        self.m_field = ""
        self.m_to = ""
        self.m_to_dir = ""
        self.m_field = []

##xlsx文件内容
class XlsxData:
    m_column_name = []
    m_text = []
    def __init__(self):
        self.m_column_name = []
        self.m_text = []

##sql结构
class SqlData:
    m_table_name = ""
    m_filed_name = []
    m_rows = []
    def __init__(self):
        self.m_table_name = ""
        self.m_filed_name = []
        self.m_rows = []

def isClient(s=""):
    if s == 'c' or s == 'cs' or s == 'sc':
        return True
    return False

def isServer(s=""):
    if s == 's' or s == 'cs' or s == 'sc':
        return True
    return False

def readDescXml(file_path, desc_data = DescData()):
    xml_tree = ET.parse(file_path)
    xml_root = xml_tree.getroot()
    if xml_root.tag != "root":
        print("ERROR:unknown tag:{tag}".format(tag=xml_root.tag))
        return False
    if "from" in xml_root.attrib:
        desc_data.m_from = xml_root.attrib["from"]
    else:
        print("ERROR: missing key:from")
        return False
    if "to" in xml_root.attrib:
        desc_data.m_to = xml_root.attrib["to"]
    else:
        print("ERROR: missing key:from")
        return False
    if "to_dir" in xml_root.attrib:
        desc_data.m_to_dir = xml_root.attrib["to_dir"]

    for child in xml_root:
        if child.tag != "field":
            print("ERROR:unknown tag:{tag}".format(tag=child.tag))
            return False
        #print("root tag:{} attr:{}".format(child.tag, child.attrib))
        attr = dict(child.attrib)
        for key in attr.keys():
            if key not in ATTR_KEYS:
                print("ERROR:unknown key:{key}".format(key=key))
                return False
            if key == "name":
                field = attr["name"]
                if field in desc_data.m_field:
                    print("ERROR:repeat name:{value}".format(value=field))
                    return False
                desc_data.m_field.append(attr)
    return True

def readXlsx(file_path, xlsx_data=XlsxData()):
    wb = openpyxl.load_workbook(file_path, read_only=True)
    ws = wb["Sheet1"]

    ##获取列名
    for num, row in enumerate(ws.rows):
        if num == 0:
            for cell in row:
                column_name = cell.value
                ##xlsx顶格开始，不能有空格
                if not column_name:
                    print("ERROR:null column")
                    return False
                xlsx_data.m_column_name.append(column_name)
            break

    ##获取列数据
    for num, row in enumerate(ws.rows):
        if num > 0:
            text = {}
            for idx, cell in enumerate(row):
                column_name = xlsx_data.m_column_name[idx]
                #if cell.value is None:
                #    text[column_name] = str()
                #else:
                text[column_name] = str(cell.value)
            xlsx_data.m_text.append(text)
    return True

##把xlsx数据转成xml
def writeXlsxToXml(desc_data=DescData(), xlsx_data=XlsxData()):
    xml_root = ET.Element("root")
    for line, row in enumerate(xlsx_data.m_text):
        xml_record = ET.Element("record")
        for field in desc_data.m_field:
            field_name = field["name"]
            column = field["column"]

            xml_field = ET.Element(field_name)
            if column not in row:
                print("ERROR xlsx line:{line} missing column: {name} {column}".format(line=line+1, name=field_name, column=column))
                return False
            if row[column] == "None":
                if "default_value" not in field:
                    print("ERROR xlsx line:{line} column:{column} NULL".format(line=line + 1, column=column))
                    return False
                xml_field.text = field["default_value"]
            else:
                xml_field.text = row[column]
            xml_field.set("to", field["to"])
            xml_field.set("type", field["type"])
            xml_record.append(xml_field)
        xml_root.append(xml_record)
    tree = ET.ElementTree(xml_root)

    out_name = os.path.join(desc_data.m_to_dir, desc_data.m_to) + ".xml"
    tree.write(out_name, encoding="UTF-8")
    print("生成中间文件:{}".format(out_name))
    return True

##由描述文件生成proto3文件
def genProto3(desc_data=DescData()):
    head ='''syntax = "proto3";
package pt;
option optimize_for = LITE_RUNTIME;
'''
    file_name =  os.path.join(desc_data.m_to_dir, desc_data.m_to) + ".proto"
    fw = open(file_name, "w")
    fw.write(head)
    fw.write("\n")

    ##生成客户端结构
    client_field = []
    for field in desc_data.m_field:
        if field["to"] == "c" or field["to"] == "cs":
            client_field.append(field)
    if len(client_field) > 0:
        data_name = desc_data.m_to + "_client"
        fw.write("message {}\n".format(data_name))
        fw.write("{\n")
        for pos, field in enumerate(client_field):
            type = field["type"]
            name = field["name"]
            fw.write("\t{type} {name} = {pos};\n".format(type=type, name=name, pos=pos+1))
        fw.write("};\n")
        fw.write("message {}\n".format(data_name+"_conf"))
        fw.write("{\n")
        fw.write("\trepeated {} data = 1;\n".format(data_name))
        fw.write("}\n\n")

    ##生成服务端结构
    server_field = []
    for field in desc_data.m_field:
        if field["to"] == "s" or field["to"] == "cs":
            server_field.append(field)
    if len(server_field) > 0:
        data_name = desc_data.m_to + "_server"
        fw.write("message {}\n".format(data_name))
        fw.write("{\n")
        for pos, field in enumerate(server_field):
            type = field["type"]
            name = field["name"]
            fw.write("\t{type} {name} = {pos};\n".format(type=type, name=name, pos=pos+1))
        fw.write("};\n")
        fw.write("message {}\n".format(data_name+"_conf"))
        fw.write("{\n")
        fw.write("\trepeated {} data = 1;\n".format(data_name))
        fw.write("}\n\n")
    print("生成协议文件:{}".format(file_name))
    fw.close()

##生成json数据
def genJson(desc_data=DescData(), xml_path=""):
    ##生成客户端json字符串
    client = []
    server = []

    xml_tree = ET.parse(xml_path)
    xml_root = xml_tree.getroot()
    for record in xml_root:
        json_client = {}
        json_server = {}
        for field in record:
            to = field.attrib["to"]
            type = field.attrib["type"]
            value = field.text
            if value is None:
                continue
            if isClient(to) and isServer(to):
                json_client[field.tag] = ATTR_TYPES[type](value)
                json_server[field.tag] = ATTR_TYPES[type](value)
            elif isClient(to):
                json_client[field.tag] = ATTR_TYPES[type](value)
            elif to == "s":
                json_server[field.tag] = ATTR_TYPES[type](value)
        if len(json_client) > 0:
            client.append(json_client)
        if len(json_server) > 0:
            server.append(json_server)

    if len(client) > 0:
        ##client_str = json.dumps(client, sort_keys=True,indent=4)
        client_str = json.dumps(client, sort_keys=True)
        file_name = os.path.join(desc_data.m_to_dir, desc_data.m_to) + "_client.json"
        fw = open(file_name, "w")
        fw.write(client_str)
        fw.close()
        print("生成客户端配置文件:{}".format(file_name))

    if len(server) > 0:
        server_str = json.dumps({"data":server})
        file_name = os.path.join(desc_data.m_to_dir, desc_data.m_to) + "_server.json"
        fw = open(file_name, "w")
        fw.write(server_str)
        fw.close()
        print("生成服务端配置文件:{}".format(file_name))

##生成sql文件
def parseXmlToSql(desc_data=DescData(), xml_path=""):
    xml_tree = ET.parse(xml_path)
    xml_root = xml_tree.getroot()

    sql_data = SqlData()
    sql_data.m_table_name = desc_data.m_to

    for record in xml_root:
        field_name = []
        row = []
        for field in record:
            to = field.attrib["to"]
            type = field.attrib["type"]
            value = field.text
            if not isServer(to):
                continue
            if value is None:
                value = ""
            field_name.append(field.tag)
            row.append(ATTR_TYPES[type](value))
        sql_data.m_filed_name = field_name
        sql_data.m_rows.append(row)
        if len(field_name) == 0:
            return
    if len(sql_data.m_rows) == 0:
        return
    sql = genInsertSql(sql_data)
    sql_name = os.path.join(desc_data.m_to_dir, desc_data.m_to) + ".sql"
    print("生成sql文件:{}".format(sql_name))
    fw = open(sql_name, "w", encoding="utf-8")
    fw.write(sql)
    fw.close()

def genInsertSql(sql_data = SqlData()):
    s = "insert into {} (".format(sql_data.m_table_name)
    for num, field in enumerate(sql_data.m_filed_name):
        s += "{},".format(field)
    s = s[:-1]
    s += ") values "
    for row in sql_data.m_rows:
        for num, column in enumerate(row):
            if num == 0:
                s += "("
            if isinstance(column, int):
                s += "{},".format(column)
            else:
                val = str(column)
                val = val.replace("'", "\\'")
                val = val.replace('"', '\\"')
                s += "'{}',".format(val)
        s = s[:-1]
        s += "),"
    s = s[:-1]
    s += ";"
    return s

    
def startParse(desc_file):
    ##读取描述文件
    desc_data = DescData()
    if not readDescXml(desc_file, desc_data):
        return

    ##读取excel数据
    xlsx_data = XlsxData()
    if not readXlsx(desc_data.m_from, xlsx_data=xlsx_data):
        return

    ##生成中间配置xml
    if not writeXlsxToXml(desc_data=desc_data, xlsx_data=xlsx_data):
        return

    ##生成proto
    genProto3(desc_data=desc_data)

    temp_xml = os.path.join(desc_data.m_to_dir, desc_data.m_to) + ".xml"
    genJson(desc_data=desc_data, xml_path=temp_xml)
    parseXmlToSql(desc_data=desc_data, xml_path=temp_xml)
    
def main():
    if len(sys.argv) == 2:
        startParse(str(sys.argv[1]))
    else:
        print("请输入xml:")

if __name__ == '__main__':
    main()
    
        
    
