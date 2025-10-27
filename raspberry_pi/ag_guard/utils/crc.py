import crcmod.predefined

_crc_ccitt_false = crcmod.predefined.Crc('ccitt-false')

def checksum_ccitt(data: bytes) -> int:
    c = _crc_ccitt_false.copy()
    c.update(data)
    return c.crcValue
