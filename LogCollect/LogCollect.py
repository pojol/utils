import time
import threading
import os
import signal
import traceback

hstr = '\033[1;33m' + '-h' + '\033[0m'
wfstr = '\033[1;33m' + '-wf' + '\033[0m'
lstr = '\033[1;33m' + '-l' + '\033[0m'
rfstr = '\033[1;33m' + '-r' + '\033[0m'
addstr = '\033[1;33m' + '-add' + '\033[0m'
wstr = '\033[1;33m' + '-w' + '\033[0m'
cstr = '\033[1;33m' + '-c' + '\033[0m'

help_str = '[' + hstr + '] ' + '[' + wfstr +' dir ext]' + '[' + lstr + '] [' + rfstr + ' posFlag] [' + addstr + ' lv word] [' + wstr + ' ][' + cstr + ']\n' \
           "--heap]\n" \
           "--watch] file directory [dir] extension [ext]\n" \
           "--look] matching words\n" \
           "--reload] file [posFlag] 0=begin else=end\n" \
           "--add] matching words [word]\n" \
           "--write] file\n" \
           "--clear matching words\n"\

INTERVAL = 0.01 

normal_list = []
high_list = []

WATCHING = False
PATH = ''
EXT = ''

# fileName {"status":"run", "thread": th}
file_thread_list = {}

mutex = threading.Lock()

def print_matching_words():
    print("normal matching words {}".format(normal_list))
    print("high matching words {}".format(high_list))

def watch_file(fileName, file, begin = False):
    pos = 0

    if not begin:
        file.seek(0, 2)

    while True:
        try :
            pos = file.tell()
            _line = file.readline()
        except:
            print('\033[1;31m' + traceback.print_exc() + '\033[0m')
            file.seek(pos)
            time.sleep(INTERVAL)
            continue

        if _line:
            for normal_str in normal_list:
                if _line.find(normal_str) != -1:
                    print(_line.strip('\n'))
            for high_str in high_list:
                if _line.find(high_str) != -1:
                    print('\033[1;31m' + _line.strip('\n') + '\033[0m')
        else:
            file.seek(pos)
            status = ''
            mutex.acquire()
            status = file_thread_list[fileName]['status']
            mutex.release()

            if status == 'run':
                time.sleep(INTERVAL)
            else:
                break
    pass

def watch_files(path, ext, posFlag = False):
    print("watching {} extension = {}".format(path, ext))
    global WATCHING
    global PATH
    global EXT

    for fileName in os.listdir(path):
        if ext != '':
            if fileName.find(ext) == -1:
                continue
        filePath = path + '/' + fileName
        if os.path.isfile(filePath):
            print(filePath)

            file = open(filePath, 'r', encoding='utf-8')
            fileThread = threading.Thread(target=watch_file, args=(fileName, file, posFlag))
            file_thread_list[fileName] = {"status":"run", "thread":fileThread}
            fileThread.start()

    WATCHING = True
    PATH = path
    EXT = ext
    pass

def close_watch():
    print("closed")
    if not WATCHING and PATH == '':
        print("not watching")
        return

    mutex.acquire()
    for key in file_thread_list:
        file_thread_list[key]['status'] = 'stop'
    mutex.release()

    while True:
        time.sleep(1)
        alive = False
        for key in file_thread_list:
            if file_thread_list[key]['thread'].is_alive():
                alive = True
                break
        if not alive:
            break

    for key in file_thread_list:
        file_thread_list[key]['thread'] = None

    print('closed is complete')

def accept_cmd():
    global normal_list
    global high_list
    global WATCHING
    global PATH
    global EXT

    while True:
        try :
            _input = input()

            if _input == '-h':
                print(help_str)
            elif _input.find('-add') != -1:
                args = _input[5:]
                if args.count(' ') != 1:
                    print("input err")
                    break
                _args = args.split(' ')
                if int(_args[0]) == 0:
                    normal_list.append(_args[1])
                elif int(_args[0]) == 1:
                    high_list.append(_args[1])
                else:
                    print("only 0 or 1 level")
                    break
                print_matching_words()
            elif _input == '-l':
                print_matching_words()
            elif _input.find('-wf') != -1:
                args = _input[4:]
                _args = args.split(' ')

                _path = _args[0]
                _ext = ''
                if len(_args) != 1:
                    _ext = _args[1]

                if not os.path.exists(_path):
                    print("file path err")
                    break
                elif WATCHING:
                    print("repeat watching!")
                    break
                else:
                    watch_files(_path, _ext)
            elif _input == '-c':
                normal_list = []
                high_list = []
                print_matching_words()
            elif _input.find('-r') != -1:
                args = _input[3:]

                close_watch()
                watch_files(PATH, EXT, int(args) == 0)
                pass

        except EOFError:
            return None
        except Exception as e:
            print(e)
            break
        time.sleep(0.1)

    pass

if __name__ == '__main__':

    cmd_thread = threading.Thread(target=accept_cmd)
    cmd_thread.start()

    try:
        while True:
            time.sleep(1)
            pass
    except KeyboardInterrupt:
        os.kill(os.getpid(), signal.SIGTERM)
        pass
