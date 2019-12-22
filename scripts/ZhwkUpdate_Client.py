'''
测试python版本3.7, 3.7之前版本不确定是否能正常使用
此脚本用来生成智慧网咖更新程序需要的各种配置文件
'''

import os
import hashlib
import json
import sys
import shutil
import datetime
import argparse

class Cell:
    md5 = ''            #### md5
    file_name = ''      #### 文件名称
    dir_name = []       #### 文件所处目录名称

    def DirNameToString(self):
        s = ""
        for d in self.dir_name:
            if d == "":
                pass
            else:
                s += d
                s += "/"
        return s


class Md5jsonParams:
    m_root_path = ""
    m_type = ""
    m_version = ""
    m_input = ""


#### 计算文件md5
def MD5Sum(file):
    with open(file, 'rb') as fp:
        m = hashlib.md5()
        while True:
            blk = fp.read(4096)
            if not blk:
                break
            m.update(blk)
    return m.hexdigest()


#### 获取路径下的文件夹
def GetFolders(path):
    dir_list = []
    files = os.listdir(path)
    for f in files:
        if os.path.isdir(os.path.join(path, f)):
            if f == '.' or f == '..':
                pass
            else:
                dir_list.append(f)
    return dir_list

#### 获取路径下的文件
def GetFiles(path):
    file_list = []
    files = os.listdir(path)
    for f in files:
        if os.path.isfile(os.path.join(path, f)):
            file_list.append(f)
    return file_list

#### 是否是路径
def ISPath(path):
    if not os.path.isdir(path):
        print("error:{} is not dir!".format(path))
        return False
    return True

#### 生成cell
def GenCell(current_path, root_dir = [], cell_list = []):
    current_root_dir = root_dir.copy()
    file_list = GetFiles(current_path)
    for f in file_list:
        cell = Cell()
        cell.file_name = str(f)
        cell.md5 = MD5Sum(os.path.join(current_path, f))
        cell.md5.lower()
        cell.dir_name = current_root_dir
        cell_list.append(cell)

    folder_list = GetFolders(current_path)
    for d in folder_list:
        subpath = os.path.join(current_path, d)
        sub_root_dir = current_root_dir.copy()
        sub_root_dir.append(d)
        GenCell(subpath, sub_root_dir, cell_list)

#### cell转json
def ParseCellListToJson(cell_list=[]):
    js_list = []
    for cell in cell_list:
        js = {}
        js["name"] = cell.DirNameToString() + cell.file_name
        js["md5"] = cell.md5
        js_list.append(js)

    json_root = {}
    json_root["all"] = js_list
    content = json.dumps(json_root, indent=4)
    return content

#### 写json文件
def WriteJson(str, filename):
    with open(filename, "wb+") as f:
        f.write(str.encode("utf-8"))
        f.flush()

########################################################################################################################
########################################################################################################################
########################################################################################################################

def PrintPath(current_path):
    folder_list = GetFolders(current_path)
    for d in folder_list:
        print("folder: {}".format(d))

    file_list = GetFiles(current_path)
    for f in file_list:
        print("file: {}".format(f))

def PrintCellList(cell_list=[]):
    for cell in cell_list:
        print("{0:<30}\t\t{1:<30}\t\t{2:<30}".format(cell.DirNameToString(), cell.file_name, cell.md5))
    print("total size: {}".format(len(cell_list)))

#### 生成客户端/收银端需要的md5文件
def StartMd5Json(name, current_path):
    if not os.path.isdir(current_path):
        print("path: {} invalid!!".format(current_path))
        return

    print("正在扫描目录，请稍等...")
    cell_list = []
    root_dir = [""]
    GenCell(current_path, root_dir, cell_list)
    #PrintCellList(cell_list)
    js_content = ParseCellListToJson(cell_list)
    version_name = name
    WriteJson(js_content, os.path.join("./",  version_name))
    print("已生成文件:{}".format(version_name))

####生成固定下载格式的json文件
'''
{
	"version":"0",
	"download":
	[
		{
			"name":"zhwkMainServer.exe",
			"download_dir":"",
			"md5":"E3FA0C97760F6BA6862C54CDAAF37AA2",
			"url":"http://upgradepkg.aida58.com:9091/zhwkUpdatePkg"
		}
	]
}
'''
def GenFixedJson(current_path, url, version):
    download = []
    file_list = GetFiles(current_path)
    for fname in file_list:
        fpath = os.path.join(current_path, fname)
        fmd5 = MD5Sum(fpath)
        slot = {}
        slot["name"] = fname
        slot["download_dir"] = ""
        slot["md5"] = str(fmd5).upper()
        slot["url"] = url
        pp = {'type':'', 'version':''}
        slot['property'] = pp
        download.append(slot)
    root = {}
    root["download"] = download
    root["version"] = version
    return root


#### 生成固定格式json
def StartFixedJson(name, current_path_str, url, version):
    if not ISPath(current_path_str):
        print("path: {} invalid!!".format(current_path_str))
        return

    print("正在扫描目录，请稍等...")
    current_path = os.path.join(current_path_str)
    root = GenFixedJson(current_path, url, version)
    js = json.dumps(root, indent=4)
    #print(js)
    #output_name = os.path.basename(current_path)
    output_name = name
    WriteJson(js, os.path.join("./", output_name))
    print("已生成文件:{}".format(output_name))


def MainFixedjson(argv=[], url=""):
    desc = '''
    '''
    parser = argparse.ArgumentParser(description=desc, prog=argv[0])

    help = r'''更新类型:localserver(本地服务器)
    client(客户端)
    updateserver(更新器)
    updateplugin(更新器插件)
    cashier(收银端)'''
    parser.add_argument('--type', help=help, choices=['localserver', 'client', 'updateserver', 'updateplugin'], required=True)
    parser.add_argument('--version', help='--type 生成版本号', required=True)
    parser.add_argument('--input', help='输入目录', required=True)
    args = parser.parse_args(argv[1:])
    #parser.print_usage()
    args_dict = {"type": args.type, "version": args.version, "input": args.input}
    if not os.path.isdir(args_dict["input"]):
        print("--input {} 必须是目录!".format(args_dict["input"]))
        return
    #print(args_dict)
    name = {"localserver":"localserverupdate", "client":"clientupdate", "updateserver": "updateserverupdate", "updateplugin":"updatepluginupdate"}
    url = "http://upgradepkg.aida58.com:9091/zhwkUpdatePkg" + "/" + args_dict["type"]
    StartFixedJson(name=name[args_dict["type"]] + ".json", current_path_str=args_dict["input"], url=url, version= args_dict["version"])


def MainMd5json(argv=[]):
    desc = '''
            '''
    parser = argparse.ArgumentParser(description=desc, prog=argv[0])
    parser.add_argument('--type', help='类型', choices=['client', 'cashier'], required=True)
    parser.add_argument('--version', help='生成版本号', required=True)
    parser.add_argument('--input', help='输入目录', required=True)
    args = parser.parse_args(argv[1:])
    # parser.print_usage()
    args_dict = {"type": args.type, "version": args.version, "input": args.input}
    #args_dict = {"version": args.version, "input": args.input}
    if not os.path.isdir(args_dict["input"]):
        print("--input {} 必须是目录!".format(args_dict["input"]))
        return
    #print(args_dict)
    StartMd5Json(name=args_dict["version"] + ".json", current_path=args_dict["input"])

    js ={}
    js["zipname"] = args_dict["version"] + ".zip"
    js["jsonname"] = args_dict["version"] + ".json"
    js["version"] = args_dict["version"]
    js_content = json.dumps(js, indent=4)
    if args_dict["type"] == "client":
        WriteJson(js_content, os.path.join("./zhwkclientupdate.json"))
    elif args_dict["type"] == "cashier":
        WriteJson(js_content, os.path.join("./zhwkcashierupdate.json"))
    else:
        pass


def MainMd5json(argv=[]):
    desc = '''
            '''
    parser = argparse.ArgumentParser(description=desc, prog=argv[0])
    parser.add_argument('--type', help='类型', choices=['client', 'cashier'], required=True)
    parser.add_argument('--version', help='生成版本号', required=True)
    parser.add_argument('--input', help='输入目录', required=True)
    args = parser.parse_args(argv[1:])
    # parser.print_usage()
    args_dict = {"type": args.type, "version": args.version, "input": args.input}
    #args_dict = {"version": args.version, "input": args.input}
    if not os.path.isdir(args_dict["input"]):
        print("--input {} 必须是目录!".format(args_dict["input"]))
        return
    #print(args_dict)
    StartMd5Json(name=args_dict["version"] + ".json", current_path=args_dict["input"])

    js ={}
    js["zipname"] = args_dict["version"] + ".zip"
    js["jsonname"] = args_dict["version"] + ".json"
    js["version"] = args_dict["version"]
    js_content = json.dumps(js, indent=4)
    if args_dict["type"] == "client":
        WriteJson(js_content, os.path.join("./zhwkclientupdate.json"))
    elif args_dict["type"] == "cashier":
        WriteJson(js_content, os.path.join("./zhwkcashierupdate.json"))
    else:
        pass


#### 生成客户端/收银端需要的md5文件
def StartMd5Json(params  params, name, current_path):
    if not os.path.isdir(current_path):
        print("path: {} invalid!!".format(current_path))
        return

    print("正在扫描目录，请稍等...")
    cell_list = []
    root_dir = [""]
    GenCell(current_path, root_dir, cell_list)
    #PrintCellList(cell_list)
    js_content = ParseCellListToJson(cell_list)
    version_name = name
    WriteJson(js_content, os.path.join("./",  version_name))
    print("已生成文件:{}".format(version_name))




def Main(argv):
    Usage = '''Usage:
            生成固定文件 fixedjson --type client --version client200 --input ./client200
            生成md5文件 md5json --type client --version client200 --input ./client200
            生成客户端文件 client --type client --version client200 --input ./client200 
        '''
    if len(argv) < 2:
        print(Usage)
        return
    cmd = argv[1]
    if cmd == 'fixedjson':
        MainFixedjson(argv=argv[1:], url="")
    elif cmd == 'md5json':
        MainMd5json(argv[1:])
    else:
        print(Usage)


def TestWriteJson():
    WriteJson("aaaaaaa", "./b/b.txt")


if __name__ == '__main__':
    #Main(sys.argv)
    TestWriteJson()

