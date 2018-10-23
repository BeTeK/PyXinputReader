import XInputReader
import time
import pyautogui

path64 = "x64\\Debug\\xinputReader.dll"
path32 = "Debug\\xinputReader.dll"
path = "xinputReader.dll"

with XInputReader.XInputReader(path64) as inputReader:
    while True:
        time.sleep(1.0 / 50)
        ret = inputReader.poll()
        if len(ret) > 0:
            print(ret[0].format())


