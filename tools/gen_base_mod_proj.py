#!/usr/bin/python3

import sys, getopt
import os, os.path
import re
import shutil

def dumpUsage():
    print("Usage: python3 /opt/SoftwareFactory/tools/gen_base_mod_proj.py --name=MOD_NAME --dir=MOD_DIR")
    print("Options:")
    print("  --name   MOD_NAME            Base module name, for example: usermod")
    print("  --dir    MOD_DIR             Base module project dirtory, for example: /home/yourname/")
    print("")
    print("Sample: python3 /opt/SoftwareFactory/tools/gen_base_mod_proj.py --name=\"usermod\" --dir=\"/home/yourname/\"")
    print("")

def checkParams(opts):
    """
    检查模块名是否符合命名规则
    检查目录是否存在
    """
    res = {}
    for opt, arg in opts:
        if opt in ('--name'):
            if re.match('^[a-zA-Z_][a-zA-Z0-9_]*$', arg):
                res['name'] = arg
            else:
                return res
        elif opt in ('--dir'):
            res['dir'] = arg;
    res['dir'] = res['dir'] + res['name'] + '/'
    return res

def replaceParams(params, line):
    return re.sub("@template_mod@", params["name"], line)

def replaceFile(params, file):
    file_content = ""
    rf = open(file, 'r')
    while True:
        line = rf.readline()
        if line == '' or line is None:
            break
        # replaceParams(params, line)
        file_content = file_content + replaceParams(params, line)
    # print(file_content)
    rf.close()

    wf = open(file, 'w')
    wf.write(file_content)
    wf.close()

if __name__ == "__main__":
    try:
        opts, args = getopt.getopt(sys.argv[1:],
            "h",
            ["name=", "dir="])
    except getopt.GetOptError:
        dumpUsage()
        sys.exit(1)
    
    opt_cnt = 2
    if len(opts) != opt_cnt:
        dumpUsage()
        sys.exit(1)
    
    print("opts %s" % opts)
    params_dict = checkParams(opts)
    if len(params_dict) != opt_cnt:
        dumpUsage()
        sys.exit(1)
    
    # 拷贝到指定目录
    shutil.copytree("/opt/SoftwareFactory/templates/base_mod", params_dict["dir"], True)

    # 重命名模板工程文件
    rename_cpp = params_dict["dir"] + "com/" + params_dict["name"] + ".cpp"
    rename_json = params_dict["dir"] + "conf/" + params_dict["name"] + ".json"
    os.rename(params_dict["dir"] + "com/template_mod.cpp", rename_cpp)
    os.rename(params_dict["dir"] + "conf/template_mod.json", rename_json)

    # 替换文件中变量
    rename_cmakelist = params_dict["dir"] + "CMakeLists.txt"
    print("replace content: " + rename_cmakelist)
    replaceFile(params_dict, rename_cmakelist)
    print("replace content: " + rename_json)
    replaceFile(params_dict, rename_json)

    print("Success!!!")
    