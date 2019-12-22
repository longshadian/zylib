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

class FileMd5:
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

#### 生成FileMd5
def GenFileMD5(current_path, root_dir = [], cell_list = []):
    current_root_dir = root_dir.copy()
    file_list = GetFiles(current_path)
    for f in file_list:
        cell = FileMd5()
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
        GenFileMD5(subpath, sub_root_dir, cell_list)

#### cell转json
def FileMD5ListToJson(cell_list=[]):
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


def Main1(argv):
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



########################################################################################################################
########################################################################################################################
########################################################################################################################

#### 写json文件
def WriteJson(str, filename):
    with open(filename, "wb+") as f:
        f.write(str.encode("utf-8"))
        f.flush()


#### 生成客户端/收银端需要的md5文件
def StartMd5Json(input_path, md5_json, output_json, version):
    print("正在扫描目录，请稍等...")
    cell_list = []
    root_dir = [""]
    GenFileMD5(input_path, root_dir, cell_list)
    #PrintCellList(cell_list)
    js_content = FileMD5ListToJson(cell_list)
    WriteJson(js_content, md5_json)
    print("已生成文件:{}".format(md5_json))

    js = {}
    js["zipname"] = version + ".zip"
    js["jsonname"] = version + ".json"
    js["version"] = version
    js_content = json.dumps(js, indent=4)
    WriteJson(js_content, output_json)


def MainMd5json(argv=[]):
    desc = '''
            '''
    parser = argparse.ArgumentParser(description=desc, prog=argv[0])
    parser.add_argument('--type', help='类型', choices=['client'], required=True)
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

    version = args_dict["version"]
    StartMd5Json(input_path=args_dict["input"], md5_json="./"+ version + ".json",
                 output_json="./zhwkclientupdate.json", version=version)


def GenFixedJson(input_path, url, version):
    download = []
    file_list = GetFiles(input_path)
    for fname in file_list:
        fpath = os.path.join(input_path, fname)
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
def StartFixedJson(input_path, output_json, url, version):
    print("正在扫描目录，请稍等...")
    root = GenFixedJson(input_path, url, version)
    js = json.dumps(root, indent=4)
    #print(js)
    WriteJson(js, output_json)
    print("已生成文件:{}".format(output_json))


def MainFixedjson(argv=[], url=""):
    desc = '''
    '''
    parser = argparse.ArgumentParser(description=desc, prog=argv[0])

    help = r'''更新类型:localserver(本地服务器)
    client(客户端)
    updateserver(更新器)
    updateplugin(更新器插件)'''
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
    StartFixedJson(input_path=args_dict["input"], url=url, output_json=name[args_dict["type"]] + ".json", version= args_dict["version"])


#### 一键生成客户端所需要的.zip .json文件
def MainClient(argv=[]):
    desc = '''
            '''
    parser = argparse.ArgumentParser(description=desc, prog=argv[0])
    parser.add_argument('--type', help='类型', choices=['client'], required=True)
    parser.add_argument('--version', help='生成版本号', required=True)
    parser.add_argument('--input', help='输入目录', required=True)
    args = parser.parse_args(argv[1:])
    # parser.print_usage()
    args_dict = {"type": args.type, "version": args.version, "input": args.input}
    if not os.path.isdir(args_dict["input"]):
        print("--input {} 必须是目录!".format(args_dict["input"]))
        return

    version = args_dict["version"]
    input_src = args_dict["input"]

    ## 1.创建临时目录
    t = datetime.datetime.now()
    dst_root_folder = t.strftime("{0}-%Y-%m-%d-%H-%M-%S".format(version))
    dst_root_path = os.path.join("./", dst_root_folder)
    if os.path.exists(dst_root_path):
        print("目录:{0} 已经存在，请稍后再试".format(dst_root_folder))
        return
    print("创建临时目录:{0}".format(dst_root_folder))

    ## 2.复制整个文件夹
    dst_src_path = os.path.join(dst_root_path, version)
    print("复制文件:from[{0}] to[{1}]".format(input_src, dst_src_path))
    ## 创建一个嵌套的目录,方便接下来压缩
    shutil.copytree(input_src, os.path.join(dst_src_path, version),
                    ignore=shutil.ignore_patterns("cef.log", "ZhProtect.dll", "zhwkCfg.ini"))

    #### 3.创建md5文件
    md5_json_file = os.path.join(dst_root_path, version + ".json")
    print("开始生成md5文件:{0}".format(md5_json_file))
    StartMd5Json(input_path=os.path.join(dst_src_path, version), md5_json=md5_json_file,
                 output_json=os.path.join(dst_root_path, "zhwkclientupdate.json"), version=version)

    #### 4.把dst_src_path打成zip包后，删除文件夹
    print("开始打包：{0}.zip".format(version))
    shutil.make_archive(dst_src_path, 'zip', root_dir=dst_src_path)
    shutil.rmtree(dst_src_path)

    #### 5.生成fixedjson文件
    url = "http://upgradepkg.aida58.com:9091/zhwkUpdatePkg" + "/" + args_dict["type"]
    StartFixedJson(input_path=dst_root_path, url=url, output_json=os.path.join(dst_root_path, "clientupdate.json"),
                   version=version)


def Main(argv):
    Usage = '''Usage:
            生成固定文件              fixedjson --type client --version client200 --input ./client200
            生成客户端所需md5文件     md5json --type client --version client200 --input ./client200
            一键生成客户端更新文件    client --type client --version client200 --input ./client200 
        '''
    if len(argv) < 2:
        print(Usage)
        return
    cmd = argv[1]
    if cmd == 'fixedjson':
        MainFixedjson(argv=argv[1:], url="")
    elif cmd == 'md5json':
        MainMd5json(argv[1:])
    elif cmd == 'client':
        MainClient(argv[1:])
    else:
        print(Usage)

if __name__ == '__main__':
    Main(sys.argv)

