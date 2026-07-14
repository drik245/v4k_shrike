"""
Minimal DS3231 RTC driver for MicroPython
"""

class DS3231:
    def __init__(self, i2c, addr=0x68):
        self.i2c = i2c
        self.addr = addr

    def _dec2bcd(self, value):
        return (value // 10) << 4 | (value % 10)

    def _bcd2dec(self, value):
        return ((value >> 4) * 10) + (value & 0x0F)

    def datetime(self, datetime=None):
        if datetime is None:
            data = self.i2c.readfrom_mem(self.addr, 0x00, 7)
            ss = self._bcd2dec(data[0])
            mm = self._bcd2dec(data[1])
            hh = self._bcd2dec(data[2])
            wday = data[3]
            DD = self._bcd2dec(data[4])
            MM = self._bcd2dec(data[5] & 0x7F)
            YY = self._bcd2dec(data[6]) + 2000
            return (YY, MM, DD, wday, hh, mm, ss)
        else:
            YY, MM, DD, wday, hh, mm, ss = datetime
            data = bytearray(7)
            data[0] = self._dec2bcd(ss)
            data[1] = self._dec2bcd(mm)
            data[2] = self._dec2bcd(hh)
            data[3] = self._dec2bcd(wday)
            data[4] = self._dec2bcd(DD)
            data[5] = self._dec2bcd(MM)
            data[6] = self._dec2bcd(YY - 2000)
            self.i2c.writeto_mem(self.addr, 0x00, data)
