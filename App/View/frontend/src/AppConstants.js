export const STATUS_POLL_INTERVAL_MS = 1000;
export const LIVE_STREAM_TIMEOUT_MS = 3000;
export const FILE_ACCEPT_VIDEO_TYPE = "video/mp4";
export const SETS_PER_MATCH = 4;

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
  NICKNAME: "Nickname",
  CANCEL: "Cancel",
  CREATE: "Create",
  PLAYER_CREATED: "Player created",
  PLAYER_CREATE_ERROR: "Unable to create player.",
  GENERATE_TEAMS: "Generate Teams",
  TEAM_GENERATION_ERROR: "Unable to generate teams.",
  APP_EYEBROW: "Foosball Tracker",
  APP_TITLE: "Match Analysis Console",
  GO_LIVE: "Go Live",
  LOAD_VIDEO: "Load Video",
  ANALYSE: "Anal",
  SAVE_RESULT: "Save Result",
  SOURCE_LABEL: "Source:",
  BALL_POSSESSION: "Ball Possession",
  PRESSURE_INDEX: "Pressure Index",
  PRESSURE_CHART_ARIA: "Pressure index line chart placeholder",
  PASSING_FLOW: "Passing Flow",
  SHOTS_ON_GOAL: "Shots On Goal",
  PASS_ACCURACY: "Pass Accuracy",
  FASTEST_SHOT: "Fastest Shot",
  TABLE_POSITIONS_ARIA: "Foosball table positions",
  TABLE_IMAGE_ARIA: "Foosball table image placement",
  ANALYSIS_IN_PROGRESS: "Analysis in progress",
  DISMISS: "Dismiss",
  PLAYER_PLACEHOLDER_PREFIX: "Player",
  PLAYER_ELO_PREFIX: "Elo:",
  PLAYER_NOT_EXISTS: "Not exists",
  NICKNAMES_LEGEND: "Nicknames",
  SCHEMA_LEGEND: "Schema",
  RANDOM: "Random",
  BY_ELO: "By Elo",
  FORMATION_LEGEND: "Formation",
  FORMATION_STANDARD: "9th Standard",
  GENERATION_RESULTS: "Generation results",
  RED_TEAM: "Red Team",
  BLUE_TEAM: "Blue Team",
  PLACEHOLDER_DASH: "------",
  SAVE: "Save",
  GENERATE: "Generate",
  SET_LABEL_PREFIX: "Set",
  SCORE_PLACEHOLDER: "- : -",
  PREVIOUS_SET_ARIA: "Previous set",
  NEXT_SET_ARIA: "Next set",
};

export const INITIAL_STATUS = {
  analyzing: false,
  error: null,
  videoUrl: null,
};

export const INITIAL_DISPLAY_POSITIONS = Array.from(
  { length: SETS_PER_MATCH },
  () => ({
    red: {
      defence: UI_TEXT.PLACEHOLDER_DASH,
      offence: UI_TEXT.PLACEHOLDER_DASH,
    },
    blue: {
      defence: UI_TEXT.PLACEHOLDER_DASH,
      offence: UI_TEXT.PLACEHOLDER_DASH,
    },
  }),
);

export const DEFAULT_TEAM_NAMES = [UI_TEXT.RED_TEAM, UI_TEXT.BLUE_TEAM];
