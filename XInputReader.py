import ctypes


class JoyStateStruct(ctypes.Structure):
    _fields_ = [("connected", ctypes.c_char),
                ("axisCount", ctypes.c_int),
                ("axis", ctypes.c_long * 12),
                ("buttonCount", ctypes.c_int),
                ("buttons", ctypes.c_uint8 * 128),
                ("name", ctypes.c_wchar * 260),
                ("guid", ctypes.c_char * 16)]


class JoyStatesStruct(ctypes.Structure):
    _fields_ = [("stateCount", ctypes.c_int),
                ("joyStates", ctypes.POINTER(JoyStateStruct))]

class JoyState:
    def __init__(self, jsStr):
        self.name = jsStr.name
        self.guid = jsStr.guid
        self.connected = ord(jsStr.connected) != 0
        self.axises = [0] * jsStr.axisCount

        for i in range(jsStr.axisCount):
            self.axises[i] = jsStr.axis[i]

        self.buttons = [False] * jsStr.buttonCount
        for i in range(jsStr.buttonCount):
            self.buttons[i] = jsStr.buttons[i] != 0

    def __str__(self):
        return "JoyState(name = {0})".format(self.name)

    def format(self):
        return "JoyState(name = {0}, axises = {1}, buttons = {2}".format(self.name, self.axises, ["T" if i else "F" for i in self.buttons])


class XInputReader(object):

    def __init__(self, path = None):
        self.dll = self._tryToLoadDll(path)

        if self.dll is None:
            raise Exception("Could not find xinputreader.dll, xinputreader_32.dll or xinputreader_64.dll")
        
        self._versionFn = self.dll[1]
        self._versionFn.restype = ctypes.c_char_p

        self._startFn = self.dll[2]

        self._stopFn = self.dll[3]

        self._pollFn = self.dll[4]
        self._pollFn.restype = ctypes.POINTER(JoyStatesStruct)
		
        self._rescanFn = self.dll[5]
        self._setFreq = self.dll[6]

    def _tryToLoadDll(self, path):
        if path is None:
            try:
                return ctypes.WinDLL("xinputReader_32.dll")
            except OSError as e:
                pass

            try:
                return ctypes.WinDLL("xinputReader_64.dll")
            except OSError as e:
                pass
            
        else:
            return ctypes.WinDLL(path)

        return None
        
    def version(self):
        return self._versionFn().decode("LATIN-1")

    def poll(self):
        callVals =  self._pollFn()
        ret = []
        for i in range(callVals.contents.stateCount):
            ret.append(JoyState(callVals.contents.joyStates[i]))

        return ret

    def start(self):
        self._startFn()

    def stop(self):
        self._stopFn()
		
    def rescan(self):
        self._rescanFn()

    def setFreq(self, freq):
        self._setFreq(ctypes.c_int(freq))
		
    def __enter__(self):
        self.start()
        return self

    def __exit__(self, exc_type, exc_value, traceback):
        self.stop()
