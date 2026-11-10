# Camera Vision

This optional experiment periodically captures one JPEG from the ELEGOO camera and sends it to a local Ollama vision model. It does not change the robot firmware or send movement commands. It analyzes still images in sequence; it is not a real-time object tracker.

Purpose: Configure and run the local camera vision experiment.
Status: Current.
Owner: Project maintainers.

## Files

| File | Purpose |
| --- | --- |
| `camera_vision.py` | Captures camera snapshots and sends them to the configured local Ollama vision model. |
| `.env.example` | Tracked template for the required camera URL, Ollama URL, and model identifier. |
| `.env` | Local environment values; ignored by Git. |

## Configuration

The local configuration file is [`vision/.env`](.env). It is excluded from version control. Create it from the template if it does not exist:

```sh
cp vision/.env.example vision/.env
```

The file defines three required environment variables:

- `CAMERA_URL`: camera snapshot endpoint.
- `OLLAMA_URL`: Ollama chat API endpoint.
- `OLLAMA_MODEL`: vision model identifier.

The checked-in [`.env.example`](.env.example) has the current robot and Ollama values as a starting point. Edit `vision/.env` for your environment.

## Run

While the Mac has Internet access, load the configuration into the shell and download the configured model:

```sh
set -a
source vision/.env
set +a
ollama pull "$OLLAMA_MODEL"
```

Then connect the Mac to the robot's `ELEGOO-...` Wi-Fi network, confirm the camera selector is set to **Cam**, and start the script from the repository root:

```sh
uv run --env-file vision/.env python vision/camera_vision.py
```

The script captures from `CAMERA_URL`, calls Ollama using `OLLAMA_URL` and `OLLAMA_MODEL`, prints each response and end-to-end request latency, then waits the configured interval (two seconds by default). After each response it also reports average, minimum, and maximum latency across the last ten analyses, plus average Jaccard similarity between the word sets of consecutive descriptions. Similarity is a rough text stability signal, not a measure of visual accuracy. Requests are sequential, so slow analyses do not build up a backlog. Press `Ctrl-C` to stop.

The request uses a 4,096-token context to keep memory use more bounded on Macs with 16 GB of unified memory. The model prompt and interval can be overridden with `--prompt` and `--interval`.
