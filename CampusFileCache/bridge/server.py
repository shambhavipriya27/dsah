from http.server import ThreadingHTTPServer, SimpleHTTPRequestHandler
from pathlib import Path
import json, os, subprocess, threading, shutil, sys

ROOT = Path(__file__).resolve().parents[1]
BACKEND = ROOT / "backend"
FRONTEND = ROOT / "frontend"
EXE = BACKEND / ("CampusCache.exe" if os.name == "nt" else "CampusCache")
lock = threading.Lock()
process = None

class Handler(SimpleHTTPRequestHandler):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, directory=str(FRONTEND), **kwargs)
    def log_message(self, fmt, *args):
        print("[web] " + fmt % args)
    def _send(self, code, obj):
        data = json.dumps(obj).encode()
        self.send_response(code); self.send_header("Content-Type", "application/json; charset=utf-8")
        self.send_header("Content-Length", str(len(data))); self.send_header("Access-Control-Allow-Origin", "*")
        self.end_headers(); self.wfile.write(data)
    def do_OPTIONS(self):
        self.send_response(204); self.send_header("Access-Control-Allow-Origin", "*")
        self.send_header("Access-Control-Allow-Headers", "Content-Type"); self.send_header("Access-Control-Allow-Methods", "POST, OPTIONS")
        self.end_headers()
    def do_POST(self):
        if self.path != "/api/command": return self._send(404, {"error":"Not found"})
        try: payload = json.loads(self.rfile.read(int(self.headers.get("Content-Length", "0"))))
        except Exception: return self._send(400, {"error":"Invalid JSON request"})
        command = str(payload.get("command", "")).upper()
        if command not in ("INIT", "RESET", "GET", "PUT", "STATE"): return self._send(400, {"error":"Unsupported command"})
        fields = [command]
        if command in ("INIT", "RESET"):
            fields.append(str(int(payload.get("capacity", 5))))
        elif command == "GET": fields.append(str(payload.get("key", "")))
        elif command == "PUT": fields.extend([str(payload.get("key", "")), str(payload.get("value", ""))])
        if any("\t" in x or "\n" in x or "\r" in x for x in fields): return self._send(400, {"error":"Tabs/newlines are not allowed"})
        with lock:
            try:
                if process is None or process.poll() is not None: return self._send(503, {"error":"C backend is not running"})
                process.stdin.write("\t".join(fields) + "\n"); process.stdin.flush()
                line = process.stdout.readline()
                if not line: return self._send(503, {"error":"C backend stopped unexpectedly"})
                result = json.loads(line)
                return self._send(200, result)
            except Exception as exc: return self._send(500, {"error":str(exc)})

def compile_backend():
    compiler = os.environ.get("CC") or shutil.which("gcc") or shutil.which("clang")
    if not compiler:
        raise RuntimeError("GCC was not found. Install MinGW-w64/GCC and add its bin folder to PATH, or set the CC environment variable.")
    sources = [BACKEND / name for name in ("main.c", "ServerMode.c", "DoublyLinkedList.c", "HashTable.c", "FrequencyTable.c", "LFUCache.c")]
    cmd = [compiler, "-std=c11", "-Wall", "-Wextra", "-g", *map(str, sources), "-o", str(EXE)]
    print("[build]", " ".join(cmd))
    run = subprocess.run(cmd, cwd=BACKEND, capture_output=True, text=True)
    if run.returncode:
        raise RuntimeError("C compilation failed:\n" + run.stdout + run.stderr)
    if run.stderr.strip(): print(run.stderr)

def main():
    global process
    compile_backend()
    process = subprocess.Popen([str(EXE), "--server"], stdin=subprocess.PIPE, stdout=subprocess.PIPE,
                               stderr=subprocess.PIPE, text=True, bufsize=1, cwd=BACKEND)
    process.stdin.write("INIT\t5\n"); process.stdin.flush()
    ready = process.stdout.readline()
    if not ready: raise RuntimeError("C backend did not start. " + (process.stderr.readline() or ""))
    print("[backend]", ready.strip())
    print("\nCampus Cache is ready: http://127.0.0.1:8000")
    print("Keep this terminal open while using the visualiser. Press Ctrl+C to stop.")
    try: ThreadingHTTPServer(("127.0.0.1", 8000), Handler).serve_forever()
    except KeyboardInterrupt: print("\nShutting down...")
    finally:
        if process and process.poll() is None:
            try: process.stdin.write("QUIT\n"); process.stdin.flush(); process.wait(timeout=2)
            except Exception: process.terminate()

if __name__ == "__main__":
    try: main()
    except Exception as exc:
        print("\nSTARTUP ERROR:\n" + str(exc)); input("Press Enter to close...")
