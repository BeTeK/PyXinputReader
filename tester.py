import XInputReader
import time
import pyautogui

path64 = "x64\\Debug\\xinputReader.dll"
path32 = "Debug\\xinputReader.dll"
path = "xinputReader.dll"

time.sleep(10)


def findStick(states, namePart):
    for i in states:
        if i.name.find(namePart) >= 0:
            return i

    return None


with XInputReader.XInputReader(path64) as inputReader:
    shouldQuit = False
    while not shouldQuit:
        time.sleep(1.0 / 50)
        ret = inputReader.poll()
        if len(ret) > 0:
            print(ret[0].format())
            print(len(ret[0].buttons))

        quitStick = findStick(ret, "VKB")
        if quitStick is not None:
            shouldQuit = quitStick.buttons[0]


