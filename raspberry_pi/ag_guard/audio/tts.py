import os
import shutil
import subprocess
import tempfile

HAS_GTTS = True
try:
    from gtts import gTTS
    import playsound
except Exception:
    HAS_GTTS = False

def _speak_piper(text: str) -> bool:
    p = os.getenv("PIPER_MODEL", "")
    if not p or not shutil.which("piper"):
        if not p:
            print("[TTS] PIPER_MODEL env var not set.")
        return False
    try:
        with tempfile.NamedTemporaryFile(delete=False, suffix=".wav") as f:
            wav = f.name
        cmd = ["piper", "--model", p, "--output_file", wav]
        subprocess.run(cmd, input=text.encode("utf-8"), check=True, capture_output=True)
        player = shutil.which("aplay") or shutil.which("mpg123")
        if not player:
            print(f"[TTS] Player not found. Saved to {wav}")
            return True
        subprocess.run([player, "-q", wav], check=True)
        os.unlink(wav)
        return True
    except Exception as e:
        print(f"[TTS] Piper failed: {e}")
        try:
            if 'wav' in locals() and os.path.exists(wav): os.unlink(wav)
        except Exception:
            pass
        return False

def _speak_gtts(text: str) -> bool:
    if not HAS_GTTS:
        return False
    tmp = None
    try:
        tts = gTTS(text, lang="ko")
        tmp = tempfile.NamedTemporaryFile(delete=False, suffix=".mp3")
        tmp_path = tmp.name; tmp.close()
        tts.save(tmp_path)
        try:
            playsound.playsound(tmp_path)
            return True
        except Exception:
            if shutil.which("mpg123"):
                subprocess.run(["mpg123", "-q", tmp_path], check=True)
                return True
    except Exception as e:
        print(f"[TTS] gTTS failed: {e}")
    finally:
        try:
            if tmp is not None and os.path.exists(tmp.name):
                os.unlink(tmp.name)
        except Exception:
            pass
    return False

def speak_korean(text: str):
    print(f"[TTS] Speak: '{text}'")
    if _speak_piper(text):
        print("[TTS] Piper OK.")
        return
    print("[TTS] Piper unavailable, fallback to gTTS...")
    if _speak_gtts(text):
        print("[TTS] gTTS OK.")
        return
    print("[TTS] All TTS methods failed.")
