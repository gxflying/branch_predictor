import os
import sys
import datetime
import time

def execCmd(cmd):  
    r = os.popen(cmd)  
    text = r.read()  
    r.close()  
    return text 

log_file = ["no_loop", "matchCounter", "reverse_matchCounter", "boom"]
log_file = ["boom"]
#log_file = ["matchCounter", "reverse_matchCounter"]

now = time.strftime("%Y-%m-%d %H:%M:%S")
print (now)
for idx in range(len(log_file)):
    log = "log_" + str(log_file[idx]) + "_2.txt"
    exe = "predictor_" + str(log_file[idx]) + ".exe"
    print (exe)
    with open(log,"w") as f_w:
        for root,dirs,files in os.walk(r"../../../log"):
                for file in files:
                    trace = os.path.join(root,file)
                    cmd = exe + " " + trace + " " + log
                    print(str("\t\t-> ") + file)
                    res = execCmd(cmd)
                    f_w.write(res.split("\n")[2].strip() + "\n")

now = time.strftime("%Y-%m-%d %H:%M:%S")
print (now)                    
