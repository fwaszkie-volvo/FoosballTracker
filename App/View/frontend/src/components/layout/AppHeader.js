import { UI_TEXT } from "../../AppConstants";

export function AppHeader({
  mode,
  currentFileName,
  modeLabel,
  canAnalyse,
  canSave,
  onLiveClick,
  onLoadClick,
  onAnalClick,
  onSaveClick,
  onCreatePlayerClick,
  onGenerateTeamsClick,
}) {
  return (
    <header className="topbar">
      <div className="top-left-cluster">
        <p className="eyebrow">{UI_TEXT.APP_EYEBROW}</p>
        <h1>{UI_TEXT.APP_TITLE}</h1>
        <div className="button-grid">
          <button type="button" onClick={onLiveClick}>
            {UI_TEXT.GO_LIVE}
          </button>
          <button type="button" onClick={onLoadClick}>
            {UI_TEXT.LOAD_VIDEO}
          </button>
          <button type="button" onClick={onAnalClick} disabled={!canAnalyse}>
            {UI_TEXT.ANALYSE}
          </button>
          <button type="button" onClick={onSaveClick} disabled={!canSave}>
            {UI_TEXT.SAVE_RESULT}
          </button>
          <button type="button" onClick={onCreatePlayerClick}>
            {UI_TEXT.CREATE_PLAYER}
          </button>
          <button type="button" onClick={onGenerateTeamsClick}>
            {UI_TEXT.GENERATE_TEAMS}
          </button>
        </div>
      </div>
      <div className="status-row">
        <span className="status-pill">{modeLabel}</span>
      </div>
      {mode !== "live" && (
        <p className="file-note header-file-note">
          {UI_TEXT.SOURCE_LABEL} {currentFileName}
        </p>
      )}
    </header>
  );
}
