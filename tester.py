import XInputReader
import time
import pyautogui

path64 = "x64\\Debug\\xinputReader.dll"
path32 = "Debug\\xinputReader.dll"
path = "xinputReader.dll"

#time.sleep(10)

with XInputReader.XInputReader() as inputReader:
    while True:
        time.sleep(1.0 / 50)
        ret = inputReader.poll()
        if len(ret) > 0:
            print(ret[0].format())
            print(len(ret[0].buttons))


