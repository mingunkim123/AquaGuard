import struct
import time
import pvporcupine
import pyaudio

class KeywordListener:
    def __init__(self, access_key: str, keyword_path: str, model_path: str, input_index: int, vu_period: float):
        self.access_key = access_key
        self.keyword_path = keyword_path
        self.model_path = model_path
        self.input_index = input_index
        self.vu_period = vu_period

        self.porcupine = None
        self.pa = None
        self.stream = None

    def init(self):
        if not self.access_key:
            raise RuntimeError("PV_ACCESS_KEY is not set.")
        self.porcupine = pvporcupine.create(
            access_key=self.access_key,
            keyword_paths=[self.keyword_path],
            model_path=self.model_path
        )
        self.pa = pyaudio.PyAudio()
        kwargs = dict(rate=self.porcupine.sample_rate,
                      channels=1,
                      format=pyaudio.paInt16,
                      input=True,
                      frames_per_buffer=self.porcupine.frame_length)
        if self.input_index >= 0:
            kwargs["input_device_index"] = self.input_index
            print(f"[Audio] Using device index {self.input_index}")
        self.stream = self.pa.open(**kwargs)
        print(f"[PV] OK. SR={self.porcupine.sample_rate}, FL={self.porcupine.frame_length}")
        print("[Audio] OK")

    def read_frame(self):
        pcm = self.stream.read(self.porcupine.frame_length, exception_on_overflow=False)
        frame = struct.unpack_from("h" * self.porcupine.frame_length, pcm)
        return frame

    def process(self, frame):
        return self.porcupine.process(frame)

    def vu_meter_line(self, frame, last_vu_ts):
        max_int16 = 32768.0
        now = time.time()
        if now - last_vu_ts < self.vu_period:
            return None, last_vu_ts
        ssum = sum(s * s for s in frame)
        rms = (ssum / len(frame)) ** 0.5 if len(frame) > 0 else 0
        pct = int(min(100, max(0, rms / max_int16 * 100)))
        bar = "#" * (pct // 5)
        return f"Mic: [{bar:<20}] {pct:3d}%", now

    def close(self):
        try:
            if self.stream:
                self.stream.stop_stream()
                self.stream.close()
        except Exception:
            pass
        try:
            if self.pa:
                self.pa.terminate()
        except Exception:
            pass
        try:
            if self.porcupine:
                self.porcupine.delete()
        except Exception:
            pass
