import threading

class SharedState:
    """스레드 안전한 상태 저장 (사람 감지 플래그 등)"""
    def __init__(self):
        self._lock = threading.Lock()
        self._person = False

    def set_person(self, val: bool):
        with self._lock:
            self._person = val

    def get_person(self) -> bool:
        with self._lock:
            return self._person
