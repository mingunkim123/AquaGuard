import os

# Picovoice
PV_ACCESS_KEY   = os.getenv("PV_ACCESS_KEY", "")
KEYWORD_PATH    = os.getenv("PV_KEYWORD_PATH", "./kw/창문파쇄.ppn")
MODEL_PATH      = os.getenv("PV_MODEL_PATH", "./model/porcupine_params_ko.pv")
AUDIO_INPUT_IDX = int(os.getenv("AG_AUDIO_INPUT_INDEX", "-1"))

# Serial (OpenCR)
SERIAL_PORT = os.getenv("AG_SERIAL", "/dev/ttyACM0")
BAUD_RATE   = int(os.getenv("AG_BAUD", "115200"))
HEARTBEAT_PERIOD = float(os.getenv("AG_HB_PERIOD", "1.0"))

# 동작
VU_PERIOD         = float(os.getenv("VU_PERIOD", "0.2"))
DEBOUNCE_SECONDS  = float(os.getenv("DEBOUNCE_SECONDS", "2.0"))
TTS_TEXT          = os.getenv("AG_TTS_TEXT", "창문을 파쇄합니다")

# Hailo
HAILO_DETECTION_SCRIPT = os.getenv(
    "HAILO_DETECTION_SCRIPT",
    os.path.expanduser("~/hailo-rpi5-examples/basic_pipelines/detection.py")
)
HEF_PATH     = os.getenv("HEF_PATH", "/usr/share/hailo-models/yolov8s_h8l.hef")
VIDEO_INPUT  = os.getenv("VIDEO_INPUT", "/dev/video0")
LABELS_JSON  = os.getenv("LABELS_JSON", "")

# Vision 디버그/테스트
DEBUG_VISION = bool(int(os.getenv("DEBUG_VISION", "1")))
ANY_CLASS    = bool(int(os.getenv("ANY_CLASS", "0")))
TARGET_LABEL = os.getenv("TARGET_LABEL", "person")

# 히스테리시스 & 타임아웃
MIN_CONF = float(os.getenv("PERSON_MIN_CONF", "0.6"))
SEEN_N   = int(os.getenv("PERSON_SEEN_N", "3"))
MISS_N   = int(os.getenv("PERSON_MISS_N", "8"))
HARD_TO  = float(os.getenv("PERSON_HARD_TIMEOUT", "2.0"))
