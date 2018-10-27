import ctypes
import os.path

class JoyStateStruct(ctypes.Structure):
    _fields_ = [("connected", ctypes.c_char),
                ("axisCount", ctypes.c_int),
                ("axis", ctypes.c_long * 12),
                ("buttonCount", ctypes.c_int),
                ("buttons", ctypes.c_uint8 * int(128 / 8)),
                ("name", ctypes.c_wchar * 260),
                ("guid", ctypes.c_char * 16)]


class JoyStatesStruct(ctypes.Structure):
    _fields_ = [("stateCount", ctypes.c_int),
                ("joyStates", ctypes.POINTER(JoyStateStruct))]

class JoyState:
    def __init__(self, jsStr):
        self.name = str(jsStr.name)
        self.guid = jsStr.guid
        self.connected = ord(jsStr.connected) != 0
        self.axises = [0] * jsStr.axisCount

        for i in range(jsStr.axisCount):
            self.axises[i] = jsStr.axis[i]

        self.buttons = [False] * jsStr.buttonCount
        for i in range(jsStr.buttonCount):
            self.buttons[i] = jsStr.buttons[i // 8] & (1 << (i % 8)) != 0

    def __str__(self):
        return "JoyState(name = {0})".format(self.name)

    def format(self):
        return "JoyState(name = {0}, axises = {1}, buttons = {2}".format(self.name, self.axises, ["T" if i else "F" for i in self.buttons])


class XInputReader(object):

    def __init__(self, path = None):
        self.dll = self._tryToLoadDll(path)

        if self.dll is None:
            raise Exception("Could not find xinputreader.dll, xinputreader_32.dll or xinputreader_64.dll")

        self.inputReaderHandle = None

        self._versionFn = self.dll[1]
        self._versionFn.restype = ctypes.c_char_p

        self._startFn = self.dll[2]
        self._startFn.restype = ctypes.c_void_p

        self._stopFn = self.dll[3]
        self._stopFn.restype = None

        self._pollFn = self.dll[4]
        self._pollFn.restype = ctypes.POINTER(JoyStatesStruct)

        self._rescanFn = self.dll[5]
        self._rescanFn.restype = None

        self._setFreq = self.dll[6]
        self._setFreq.restype = None

        self.inputReaderHandle = self._startFn()

    def _tryToLoadDll(self, path):
        if path is None:
            try:
                return ctypes.CDLL("xinputReader_32.dll")
            except OSError as e:
                pass

            try:
                return ctypes.WinDLL("xinputReader_64.dll")
            except OSError as e:
                pass

            try:
                return ctypes.CDLL(os.path.join(os.path.dirname(__file__), "xinputReader_32.dll"))
            except OSError as e:
                pass

            try:
                return ctypes.WinDLL(os.path.join(os.path.dirname(__file__), "xinputReader_64.dll"))
            except OSError as e:
                pass

        else:
            try:
                return ctypes.CDLL(path)
            except OSError as e:
                pass

            try:
                return ctypes.WinDLL(path)
            except OSError as e:
                raise e

        return None
        
    def version(self):
        return self._versionFn().decode("LATIN-1")

    def _throwIfNotStarted(self):
        if self.inputReaderHandle is None:
            raise Exception("XInputReader is not started yet. Call start() or use with .. as ..")

    def poll(self):
        self._throwIfNotStarted()

        callVals =  self._pollFn(ctypes.c_void_p(self.inputReaderHandle))
        ret = []
        for i in range(callVals.contents.stateCount):
            ret.append(JoyState(callVals.contents.joyStates[i]))

        return ret

    def stop(self):
        self._throwIfNotStarted()
        self._stopFn(ctypes.c_void_p(self.inputReaderHandle))
        self.inputReaderHandle = None

    def rescan(self):
        self._throwIfNotStarted()
        self._rescanFn(ctypes.c_void_p(self.inputReaderHandle))

    def setFreq(self, freq):
        self._throwIfNotStarted()
        self._setFreq(ctypes.c_void_p(self.inputReaderHandle), ctypes.c_int(freq))

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_value, traceback):
        self.stop()
