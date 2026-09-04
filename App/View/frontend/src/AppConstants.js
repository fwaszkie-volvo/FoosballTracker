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
  PLAYERS: "/api/players",
  PLAYER: "/api/player",
  TEAMS: "/api/teams",
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
  CREATE_PLAYER: "Create Player",
  CREATE_PLAYER_TITLE: "Create player",
  NICKNAME: "Nickname",
  CANCEL: "Cancel",
  CREATE: "Create",
  PLAYER_CREATED: "Player created",
  PLAYER_CREATE_ERROR: "Unable to create player.",
  GENERATE_TEAMS: "Generate Teams",
  TEAM_GENERATION_ERROR: "Unable to generate teams.",
};

export const INITIAL_STATUS = {
  analyzing: false,
  error: null,
  videoUrl: null,
};

export const INITIAL_DISPLAY_POSITIONS = {
  red: { defence: "------", offence: "------" },
  blue: { defence: "------", offence: "------" },
};
