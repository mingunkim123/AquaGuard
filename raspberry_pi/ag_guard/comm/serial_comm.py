import struct
import threading
import serial
from ag_guard.utils.crc import checksum_ccitt

class PacketType:
    SENSOR_REPORT   = 0xA1
    EVENT_DETECTED  = 0xA2
    HEARTBEAT       = 0xB1
    SMASH_NOW       = 0xC1
    CANCEL          = 0xC2
    ACTION_DONE     = 0xD1

class SerialComm:
    def __init__(self, port: str, baud: int, heartbeat_period: float, shutdown_evt: threading.Event):
        self.port = port
        self.baud = baud
        self.heartbeat_period = heartbeat_period
        self.shutdown_evt = shutdown_evt
        self.ser = None
        self.hb_thread = None
        self.lock = threading.Lock()

    def open(self):
        print(f"[Serial] Open {self.port} @ {self.baud}")
        self.ser = serial.Serial(self.port, self.baud, timeout=0.05)
        self.hb_thread = threading.Thread(target=self._hb_loop, daemon=True)
        self.hb_thread.start()
        print("[Serial] OK")

    def close(self):
        try:
            if self.ser:
                self.ser.close()
        except Exception as e:
            print(f"[Serial] Close error: {e}")

    def send(self, ptype: int, payload: bytes = b''):
        if not self.ser or not self.ser.is_open:
            print("[Serial] Port not open")
            return
        length = len(payload)
        hdr = struct.pack('<BBB', 0x02, ptype, length)
        crc = checksum_ccitt(hdr + payload)
        pkt = hdr + payload + struct.pack('<H', crc)
        with self.lock:
            try:
                self.ser.write(pkt)
                if ptype == PacketType.SMASH_NOW:
                    print("[Serial] >>> SMASH_NOW")
            except Exception as e:
                print(f"[Serial] Write error: {e}")

    def _hb_loop(self):
        print("[Serial] Heartbeat thread running.")
        while not self.shutdown_evt.is_set():
            self.send(PacketType.HEARTBEAT)
            self.shutdown_evt.wait(self.heartbeat_period)
        print("[Serial] Heartbeat thread exit.")
