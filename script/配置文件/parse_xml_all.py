import parse_xml
import os
import fnmatch

def main():
    for f in os.listdir("."):
        if fnmatch.fnmatch(f, "*.xml"):
            print("读取xml:"+str(f))
            parse_xml.startParse(str(f))
            print("\n")

if __name__ == '__main__':
    main()
    
