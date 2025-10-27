import time
import threading
from ag_guard import config
from ag_guard.state import SharedState
from ag_guard.audio.keyword_listener import KeywordListener
from ag_guard.audio.tts import speak_korean
from ag_guard.comm.serial_comm import SerialComm, PacketType
from ag_guard.vision.hailo_detector import HailoDetector

class Controller:
    def __init__(self):
        self.shutdown_evt = threading.Event()
        self.state = SharedState()
        self.kw = None
        self.serial = None
        self.vision = None
        self.last_trigger_ts = 0.0

    def start(self):
        # Keyword
        self.kw = KeywordListener(
            access_key=config.PV_ACCESS_KEY,
            keyword_path=config.KEYWORD_PATH,
            model_path=config.MODEL_PATH,
            input_index=config.AUDIO_INPUT_IDX,
            vu_period=config.VU_PERIOD
        )
        self.kw.init()

        # Serial
        self.serial = SerialComm(
            port=config.SERIAL_PORT,
            baud=config.BAUD_RATE,
            heartbeat_period=config.HEARTBEAT_PERIOD,
            shutdown_evt=self.shutdown_evt
        )
        print("[Serial] Opening...")
        self.serial.open()

        # Vision
        self.vision = HailoDetector(
            shutdown_evt=self.shutdown_evt,
            state=self.state,
            detection_script=config.HAILO_DETECTION_SCRIPT,
            hef_path=config.HEF_PATH,
            video_input=config.VIDEO_INPUT,
            labels_json=config.LABELS_JSON,
            debug=config.DEBUG_VISION,
            any_class=config.ANY_CLASS,
            target_label=config.TARGET_LABEL,
            min_conf=config.MIN_CONF,
            seen_n=config.SEEN_N,
            miss_n=config.MISS_N,
            hard_timeout=config.HARD_TO
        )
        self.vision.start()

        print("\n[SYSTEM READY] Listening... (Ctrl+C to exit)")
        self.loop()

    def loop(self):
        last_vu_ts = 0.0
        while not self.shutdown_evt.is_set():
            try:
                frame = self.kw.read_frame()
            except OSError:
                time.sleep(0.05)
                continue

            line, last_vu_ts = self.kw.vu_meter_line(frame, last_vu_ts)
            if line is not None:
                p = self.state.get_person()
                print(f"{line} | Person: {'YES' if p else 'NO'}   ", end="\r", flush=True)

            ret = self.kw.process(frame)
            if ret >= 0:
                now = time.time()
                if now - self.last_trigger_ts >= config.DEBOUNCE_SECONDS:
                    print("\n[KEYWORD] Detected.")
                    self.last_trigger_ts = now
                    if self.state.get_person():
                        print("  -> Person present. Action!")
                        self.serial.send(PacketType.SMASH_NOW)
                        speak_korean(config.TTS_TEXT)
                        print("  -> Done.")
                    else:
                        print("  -> No person. Ignored.")
                else:
                    print("\n[INFO] Debouncing...")

    def stop(self):
        print("\n[SYS] Cleaning up...")
        self.shutdown_evt.set()
        try:
            if self.serial:
                self.serial.close()
        except Exception:
            pass
        try:
            if self.vision and self.vision.is_alive():
                self.vision.join(timeout=3.0)
        except Exception:
            pass
        try:
            if self.kw:
                self.kw.close()
        except Exception:
            pass
        print("[SYS] Exit.")
