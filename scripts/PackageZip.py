import shutil
import os
import datetime
import subprocess
import zipfile

###打包源代码
def packageSrc(version="", dest=""):
    base_src_path = ".."
    base_dst_path = "src"
    copy_dir = ["proto", "share", "src", "utils", "src_charge"]
    ignore_pattern = shutil.ignore_patterns("*.o", "*.cc", "*.a", "zjhserver", "zjh_charge")

    if os.path.exists(base_dst_path):
        shutil.rmtree(base_dst_path)
    os.mkdir(base_dst_path)

    for entry in copy_dir:
        src = os.path.join(base_src_path, entry)
        dst = os.path.join(base_dst_path, entry)
        print("复制文件:from[{}] to[{}]".format(src, dst))
        shutil.copytree(src, dst, ignore=ignore_pattern)

    '''
    t = datetime.datetime.now()
    tar_name = t.strftime("src-%m-%d-%H-%M.tar.gz")
    cmd = "tar -cf {0} {1}/*".format(tar_name, base_dst_path)
    subprocess.call(cmd, shell=True)
    '''
    t = datetime.datetime.now()
    tar_name = t.strftime("src-%m-%d-%H-%M")
    print("压缩文件:{}.zip".format(tar_name))
    shutil.make_archive(tar_name, 'zip', base_dir=base_dst_path)
    shutil.rmtree(base_dst_path)
    print("打包完成:{}.zip".format(tar_name))

def main(version="", src_path=""):
    src_path = r"C:\Users\admin\Desktop\0.41"

    t = datetime.datetime.now()
    dst_folder = t.strftime("{0}-%Y-%m-%d-%H-%M-%S".format(version))
    print(dst_folder)

    ## 1.创建临时目录
    dst_root_path = os.path.join("./", dst_folder)
    if os.path.exists(dst_root_path):
        print("目录:{0} 已经存在，请稍后再试".format(dst_folder))
        return

    ## 2.复制整个文件夹
    dst_src_path = os.path.join(dst_root_path, version)
    print("复制文件:from[{0}] to[{1}]".format(src_path, dst_src_path))
    ## 创建一个嵌套的目录,方便接下来压缩
    shutil.copytree(src_path, os.path.join(dst_src_path, version))

    ## 3.把dst_src_path打成zip包后，删除文件夹
    print("开始打包：{0}.zip".format(version))
    shutil.make_archive(dst_src_path, 'zip', root_dir=dst_src_path)
    shutil.rmtree(dst_src_path)

    ## 4.


if __name__ == '__main__':
    main()
