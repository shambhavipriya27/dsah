# Campus Cache — LFU Visualizer

An interactive browser dashboard connected to the actual C LFU cache implementation. The C program performs GET/PUT/eviction; the browser only renders the returned state and animations.

## Requirements
- Windows 10/11
- Python 3.9+
- GCC (MinGW-w64 / WinLibs) available on PATH as `gcc`

## Run
1. Keep the `CampusFileCache` folder together.
2. Ensure `python` and `gcc` work in Command Prompt (`python --version`, `gcc --version`).
3. Double-click `run.bat`.
4. Open **http://127.0.0.1:8000** in your browser.
5. Keep the black terminal window open while using the dashboard. Press Ctrl+C in it to stop.

If GCC is installed but not on PATH, open Command Prompt and set it for this launch, for example:

```bat
set CC=C:\\path\\to\\mingw64\\bin\\gcc.exe
python bridge\\server.py
```

## Features
- Actual C implementation handles GET, PUT, update, LFU eviction and LRU tie-breaking.
- Live frequency buckets, MRU/LRU ordering and hash bucket indices.
- Cache capacity controls (0–200), metrics, operation trace and eviction explanation.
- Manual commands and queued sequences with play/pause/step and adjustable speed.
- Original terminal interface is retained: compile the backend sources and run `CampusCache.exe` without `--server`.

## Build manually
From `backend/`:

```bash
gcc -std=c11 -Wall -Wextra -g main.c ServerMode.c DoublyLinkedList.c HashTable.c FrequencyTable.c LFUCache.c -o CampusCache.exe
```

## Input protocol note
The browser-to-C bridge uses tab-delimited commands, so keys and values must not contain tabs or newlines. The normal assignment terminal interface remains available for its original whitespace-delimited input.
