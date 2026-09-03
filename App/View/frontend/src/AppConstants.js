export const STATUS_POLL_INTERVAL_MS = 1000;
export const LIVE_STREAM_TIMEOUT_MS = 3000;
export const FILE_ACCEPT_VIDEO_TYPE = "video/mp4";

export const MODE = {
  IDLE: "idle",
  VIDEO: "video",
  LIVE: "live",
};

export const LIVE_STREAM_STATE = {
  IDLE: "idle",
  CHECKING: "checking",
  READY: "ready",
  UNAVAILABLE: "unavailable",
};

export const API_ROUTE = {
  STATUS: "/api/status",
  LIVE: "/api/live",
  LOAD: "/api/load",
  ANALYSE: "/api/analyse",
  SAVE: "/api/save",
};

export const MEDIA_SRC = {
  LIVE_STREAM: "/media/live.mjpg",
};

export const UI_TEXT = {
  DEFAULT_FILE_NAME: "No file loaded",
  CONNECTING_STREAM: "Connecting to live stream...",
  STREAM_UNAVAILABLE: "Live stream unavailable.",
  IDLE_PLACEHOLDER: "Start live mode or load a match recording.",
  MODE_LABEL_LIVE: "Live capture",
  MODE_LABEL_VIDEO: "Video playback",
  MODE_LABEL_IDLE: "Idle",
};

export const INITIAL_STATUS = {
  analyzing: false,
  error: null,
  videoUrl: null,
};
