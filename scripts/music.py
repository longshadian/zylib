from mutagen.mp3 import MP3
from mutagen.easyid3 import EasyID3

import os
import shutil

def ListFile(p):
    ls = []
    names = os.listdir(p)
    for name in names:
        if os.path.isfile(os.path.join(p, name)):
            ls.append(name)
    return ls

def Mp3ID(p):
    mp3 = MP3(p, ID3=EasyID3)
    for v in mp3.items():
        if v[0] == 'artist':
            return v[1][0]
    return None
    #return (mp3.items()['title'], mp3.items()['artist'])
    

def Main():
    p = r'D:\CloudMusic\黄妃 - 追追追.mp3'
    Mp3ID(p)
    #k,v = Mp3ID(p)
    #print(k,v)
    #ListFile(r'C:\Python\Python37-32')

if __name__ == '__main__':
    #Main()
    if True:
        dst_dir = r'D:\CloudMusic'
        root_dir = r'D:\CloudMusic'
        files = ListFile(root_dir)
        for f in files:
            src_fp = os.path.join(root_dir, f)
            ext = os.path.splitext(src_fp)[1]
            #print(f, ext, sep=" ========> ")
            if ext == '.mp3':
                artist = Mp3ID(src_fp)
                if artist is None:
                    print("file ext is null {}".format(f))
                    continue
                #print(artist)
                dst_fp = os.path.join(dst_dir, artist)
                if not os.path.exists(dst_fp):
                    os.mkdir(dst_fp)
                shutil.move(src_fp, dst_fp)


        

