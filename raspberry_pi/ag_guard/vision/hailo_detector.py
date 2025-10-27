import os
import re
import time
import subprocess
import threading

class HailoDetector(threading.Thread):
    """detection.py 서브프로세스 실행 + 로그 파싱 + 히스테리시스 제어"""
    def __init__(self, shutdown_evt: threading.Event, state,
                 detection_script: str, hef_path: str, video_input: str,
                 labels_json: str, debug: bool,
                 any_class: bool, target_label: str,
                 min_conf: float, seen_n: int, miss_n: int, hard_timeout: float):
        super().__init__(daemon=True)
        self.shutdown_evt = shutdown_evt
        self.state = state

        self.detection_script = detection_script
        self.hef_path = hef_path
        self.video_input = video_input
        self.labels_json = labels_json
        self.debug = debug

        self.any_class = any_class
        self.target_label = target_label
        self.min_conf = min_conf
        self.seen_n = seen_n
        self.miss_n = miss_n
        self.hard_timeout = hard_timeout

        self._proc = None

        # 정규식 준비
        self._re_detected = re.compile(r"Detected:\s*([A-Za-z0-9_\- ]+)\s*,\s*Confidence:\s*([0-9.]+)")
        self._re_label    = re.compile(r"Label:\s*([A-Za-z0-9_\- ]+).*?Confidence:\s*([0-9.]+)")

        # 히스테리시스 카운터
        self._seen = 0
        self._miss = 0
        self._last_det_ts = 0.0

    def _build_cmd(self):
        cmd = ["python3", self.detection_script, "--hef-path", self.hef_path, "--input", self.video_input]
        if self.debug:
            cmd.append("--show-fps")
        if self.labels_json:
            cmd += ["--labels-json", self.labels_json]
        return cmd

    def run(self):
        if not os.path.isfile(self.detection_script):
            print(f"[Vision] detection.py not found: {self.detection_script}"); return
        if not os.path.isfile(self.hef_path):
            print(f"[Vision] HEF not found: {self.hef_path}"); return

        print("[Vision] Starting HailoRT thread...")
        cmd = self._build_cmd()
        print("[Vision] Running pipeline:", " ".join(cmd))

        try:
            self._proc = subprocess.Popen(
                cmd,
                stdout=subprocess.PIPE,
                stderr=subprocess.STDOUT,
                text=True,
                bufsize=1
            )
            for line in iter(self._proc.stdout.readline, ''):
                if self.shutdown_evt.is_set():
                    break
                line = line.rstrip("\n")
                if not line:
                    continue
                if self.debug:
                    print(f"[Vision][pipe] {line}")

                label, conf = self._parse_line(line)
                if label is None or conf is None:
                    continue

                is_hit = (conf >= self.min_conf) if self.any_class else (label == self.target_label and conf >= self.min_conf)
                now = time.time()

                if is_hit:
                    self._seen += 1
                    self._miss = 0
                    self._last_det_ts = now
                    if self._seen >= self.seen_n:
                        if not self.state.get_person():
                            print(f"[Vision] Person DETECTED (conf={conf:.2f}, seen={self._seen})")
                        self.state.set_person(True)
                else:
                    self._miss += 1
                    self._seen = 0
                    need_clear_by_miss = (self._miss >= self.miss_n)
                    need_clear_by_timeout = (self.state.get_person() and (now - self._last_det_ts > self.hard_timeout))
                    if need_clear_by_miss or need_clear_by_timeout:
                        if self.state.get_person():
                            reason = "miss" if need_clear_by_miss else "timeout"
                            print(f"[Vision] Person LOST ({reason})")
                        self.state.set_person(False)

            print("[Vision] Pipeline ended.")
        except Exception as e:
            print(f"[Vision] ERROR: {e}")
        finally:
            if self._proc and self._proc.poll() is None:
                try:
                    self._proc.terminate()
                    self._proc.wait(timeout=2.0)
                except Exception:
                    self._proc.kill()
            if self.state.get_person():
                print("[Vision] Person LOST (thread-exit)")
            self.state.set_person(False)
            print("[Vision] Hailo pipeline thread finished.")

    def _parse_line(self, line: str):
        m1 = self._re_detected.search(line)
        if m1:
            label = m1.group(1).strip().lower()
            try: conf = float(m1.group(2))
            except ValueError: conf = None
            return label, conf
        m2 = self._re_label.search(line)
        if m2:
            label = m2.group(1).strip().lower()
            try: conf = float(m2.group(2))
            except ValueError: conf = None
            return label, conf
        return None, None
