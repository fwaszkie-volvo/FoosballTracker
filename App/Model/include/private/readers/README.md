# Readers Usage

## 1) Build and run

```bash
cd FoosballTracker
./run build
./run app
```

## 2) Online mode local test (RTSP)

Start local RTSP server (Terminal A):

```bash
docker rm -f foosball-rtsp >/dev/null 2>&1 || true
docker run --name foosball-rtsp -p 8554:8554 -d bluenviron/mediamtx:latest
```

Publish test video to RTSP (Terminal B):

```bash
cd FoosballTracker
ffmpeg -re -i Tests/test_files/test_video.mp4 -map 0:v:0 -an -c:v copy -rtsp_transport tcp -f rtsp rtsp://127.0.0.1:8554/stream
```

Run app in online mode (Terminal C):

```bash
cd FoosballTracker
./run app
```
