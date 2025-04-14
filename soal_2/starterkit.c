import os
import sys
import time
import base64
from datetime import datetime

STARTERKIT_DIR = "starterkit"
QUARANTINE_DIR = "quarantine"
LOG_FILE = "activity.log"
PID_FILE = "decryptor.pid"

def log_activity(message):
    timestamp = datetime.now().strftime("[%d-%m-%Y][%H:%M:%S]")
    with open(LOG_FILE, "a") as log:
        log.write(f"{timestamp} - {message}\n")

def decrypt_filename(filename):
    try:
        decoded = base64.b64decode(filename).decode("utf-8")
        return decoded
    except Exception:
        return None

def decrypt_daemon():
    pid = os.getpid()
    log_activity(f"Successfully started decryption process with PID {pid}.")
    with open(PID_FILE, "w") as f:
        f.write(str(pid))
    while True:
        for filename in os.listdir(QUARANTINE_DIR):
            filepath = os.path.join(QUARANTINE_DIR, filename)
            if os.path.isfile(filepath):
                decoded = decrypt_filename(filename)
                if decoded and decoded != filename:
                    os.rename(filepath, os.path.join(QUARANTINE_DIR, decoded))
        time.sleep(5)

def move_files(src, dst, action):
    for filename in os.listdir(src):
        src_path = os.path.join(src, filename)
        dst_path = os.path.join(dst, filename)
        if os.path.isfile(src_path):
            os.rename(src_path, dst_path)
            log_activity(f"{filename} - Successfully {action}.")

def eradicate_quarantine():
    for filename in os.listdir(QUARANTINE_DIR):
        filepath = os.path.join(QUARANTINE_DIR, filename)
        if os.path.isfile(filepath):
            os.remove(filepath)
            log_activity(f"{filename} - Successfully deleted.")

def shutdown_decryptor():
    if os.path.exists(PID_FILE):
        with open(PID_FILE, "r") as f:
            pid = int(f.read())
        try:
            os.kill(pid, 9)
            log_activity(f"Successfully shut off decryption process with PID {pid}.")
            os.remove(PID_FILE)
        except ProcessLookupError:
            log_activity(f"Failed to shut down decryptor. PID {pid} not found.")
    else:
        log_activity("Shutdown failed. No PID file found.")

def show_usage():
    print("Usage:")
    print("  python3 starterkit_main.py --decrypt")
    print("  python3 starterkit_main.py --quarantine")
    print("  python3 starterkit_main.py --return")
    print("  python3 starterkit_main.py --eradicate")
    print("  python3 starterkit_main.py --shutdown")

if __name__ == "__main__":
    if len(sys.argv) != 2:
        show_usage()
        sys.exit(1)

    command = sys.argv[1]

    if command == "--decrypt":
        decrypt_daemon()
    elif command == "--quarantine":
        move_files(STARTERKIT_DIR, QUARANTINE_DIR, "moved to quarantine directory")
    elif command == "--return":
        move_files(QUARANTINE_DIR, STARTERKIT_DIR, "returned to starter kit directory")
    elif command == "--eradicate":
        eradicate_quarantine()
    elif command == "--shutdown":
        shutdown_decryptor()
    else:
        show_usage()
        sys.exit(1)
