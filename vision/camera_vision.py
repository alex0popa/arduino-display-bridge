"""Capture camera snapshots and analyze them with a local Ollama vision model.

Contents:
- `_parse_args`: read CLI options and environment-based configuration.
- `_get_snapshot`: fetch and validate one JPEG from the camera.
- `_analyze_snapshot`: send one image to Ollama and return its response.
- `_similarity`: compare consecutive descriptions for textual stability.
- `_print_metrics`: report latency and stability for the recent analysis window.
- `main`: run the sequential capture and analysis loop.
"""

from __future__ import annotations

import argparse
import base64
import json
import math
import os
import time
from collections import deque
from datetime import datetime
from typing import Any
from urllib.error import HTTPError, URLError
from urllib.request import Request, urlopen

DEFAULT_PROMPT = (
    "Briefly describe the scene captured by the robot. Name visible objects and "
    "their positions in the image; clearly state any uncertainty."
)
JPEG_SIGNATURE = b"\xff\xd8\xff"
METRICS_WINDOW_SIZE = 10


def _parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Send periodic camera snapshots to a local Ollama vision model."
    )
    parser.add_argument("--camera-url", default=os.environ.get("CAMERA_URL"))
    parser.add_argument("--ollama-url", default=os.environ.get("OLLAMA_URL"))
    parser.add_argument("--model", default=os.environ.get("OLLAMA_MODEL"))
    parser.add_argument("--interval", type=float, default=2.0, help="Seconds between analyses")
    parser.add_argument("--prompt", default=DEFAULT_PROMPT)
    return parser.parse_args()


def _get_snapshot(camera_url: str) -> bytes:
    request = Request(camera_url, headers={"Accept": "image/jpeg"})
    with urlopen(request, timeout=10) as response:
        image_bytes = response.read()

    has_jpeg_signature = image_bytes.startswith(JPEG_SIGNATURE)
    if not has_jpeg_signature:
        raise ValueError(f"Camera endpoint did not return a JPEG image: {camera_url}")
    return image_bytes


def _analyze_snapshot(ollama_url: str, model: str, prompt: str, image_bytes: bytes) -> str:
    payload = {
        "model": model,
        "messages": [
            {
                "role": "user",
                "content": prompt,
                "images": [base64.b64encode(image_bytes).decode("ascii")],
            }
        ],
        "stream": False,
        "options": {"num_ctx": 4096, "temperature": 0.2},
    }
    request = Request(
        ollama_url,
        data=json.dumps(payload).encode("utf-8"),
        headers={"Content-Type": "application/json"},
        method="POST",
    )
    with urlopen(request, timeout=180) as response:
        result: Any = json.loads(response.read())

    response_has_text_message = (
        isinstance(result, dict)
        and isinstance(result.get("message"), dict)
        and isinstance(result["message"].get("content"), str)
    )
    if not response_has_text_message:
        raise ValueError("Ollama response did not contain a message string")
    return result["message"]["content"]


def _similarity(previous: str, current: str) -> float:
    previous_words = set(previous.lower().split())
    current_words = set(current.lower().split())
    combined_words = previous_words | current_words
    if not combined_words:
        return 1.0
    return len(previous_words & current_words) / len(combined_words)


def _print_metrics(latencies: deque[float], stabilities: deque[float]) -> None:
    average_latency = sum(latencies) / len(latencies)
    average_stability = sum(stabilities) / len(stabilities) if stabilities else None
    stability_text = (
        f"{average_stability:.0%}" if average_stability is not None else "n/a (first frame)"
    )
    print(
        f"Metrics (last {len(latencies)}): latency avg/min/max "
        f"{average_latency:.1f}/{min(latencies):.1f}/{max(latencies):.1f}s; "
        f"description similarity avg {stability_text}",
        flush=True,
    )


def main() -> None:
    args = _parse_args()
    missing_settings = [
        name
        for name, value in (
            ("CAMERA_URL / --camera-url", args.camera_url),
            ("OLLAMA_URL / --ollama-url", args.ollama_url),
            ("OLLAMA_MODEL / --model", args.model),
        )
        if not value
    ]
    if missing_settings:
        raise SystemExit(f"Set required configuration: {', '.join(missing_settings)}")
    interval_is_valid = math.isfinite(args.interval) and args.interval > 0
    if not interval_is_valid:
        raise SystemExit("--interval must be a finite number greater than zero")

    print(f"Camera: {args.camera_url}")
    print(f"Ollama model: {args.model}")
    print(f"Analysis interval: {args.interval:g}s after each response")
    print("Press Ctrl-C to stop.")

    latencies: deque[float] = deque(maxlen=METRICS_WINDOW_SIZE)
    stabilities: deque[float] = deque(maxlen=METRICS_WINDOW_SIZE)
    previous_analysis: str | None = None

    try:
        while True:
            started_at = time.monotonic()
            image_bytes = _get_snapshot(args.camera_url)
            analysis = _analyze_snapshot(args.ollama_url, args.model, args.prompt, image_bytes)
            elapsed = time.monotonic() - started_at
            latencies.append(elapsed)
            if previous_analysis is not None:
                stabilities.append(_similarity(previous_analysis, analysis))
            previous_analysis = analysis
            timestamp = datetime.now().astimezone().isoformat(timespec="seconds")
            print(f"\n[{timestamp}] Analysis took {elapsed:.1f}s")
            print(analysis, flush=True)
            _print_metrics(latencies, stabilities)
            time.sleep(args.interval)
    except KeyboardInterrupt:
        print("\nStopped.")
    except (HTTPError, URLError, TimeoutError, ValueError, KeyError, json.JSONDecodeError) as error:
        raise SystemExit(f"Camera/Ollama request failed: {error}") from error


if __name__ == "__main__":
    main()
