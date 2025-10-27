import signal
from ag_guard.controller import Controller

def run():
    ctl = Controller()

    def _sig_handler(sig, frame):
        ctl.stop()

    signal.signal(signal.SIGINT, _sig_handler)
    signal.signal(signal.SIGTERM, _sig_handler)

    try:
        ctl.start()
    except KeyboardInterrupt:
        ctl.stop()
    except Exception as e:
        print(f"[SYS] Unexpected error: {e}")
        ctl.stop()

if __name__ == "__main__":
    run()
