#!/usr/bin/python3

import sys, getopt
import os, os.path
import re
import shutil

def dumpUsage():
    print("Usage: python3 /opt/SoftwareFactory/tools/gen_base_mod_proj.py --mod_name=MOD_NAME --mod_dir=MOD_DIR")
    print("Options:")
    print("  --mod_name   MOD_NAME            Base module name, for example: usermod")
    print("  --mod_dir    MOD_DIR             Base module project dirtory, for example: /home/yourname/")
    print("")
    print("Sample: python3 /opt/SoftwareFactory/tools/gen_base_mod_proj.py --mod_name=\"usermod\" --mod_dir=\"/home/yourname/\"")
    print("")

def checkParams(opts):
    """
    检查模块名是否符合命名规则
    检查目录是否存在
    """
    res = {}
    for opt, arg in opts:
        if opt in ('--mod_name'):
            if re.match('^[a-zA-Z_][a-zA-Z0-9_]*$', arg):
                res['mod_name'] = arg
        elif opt in ('--mod_dir'):
            res['mod_dir'] = arg;
    return res

if __name__ == "__main__":
    try:
        opts, args = getopt.getopt(sys.argv[1:],
            "h",
            ["mod_name=", "mod_dir="])
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

    # 重命名模板工程文件
    # TODO
    
    # 拷贝到指定目录
    shutil.copytree("/opt/SoftwareFactory/templates/base_mod", params_dict["mod_dir"], True)

    print("Success!!!")
    