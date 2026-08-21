export function AppHeader({
  mode,
  currentFileName,
  modeLabel,
  analyzing,
  canAnalyse,
  canSave,
  onLiveClick,
  onLoadClick,
  onAnalClick,
  onSaveClick,
}) {
  return (
    <header className="topbar">
      <div className="top-left-cluster">
        <p className="eyebrow">Foosball Tracker</p>
        <h1>Match Analysis Console</h1>
        <div className="button-grid">
          <button type="button" onClick={onLiveClick}>
            Go Live
          </button>
          <button type="button" onClick={onLoadClick}>
            Load Video
          </button>
          <button type="button" onClick={onAnalClick} disabled={!canAnalyse}>
            Anal
          </button>
          <button type="button" onClick={onSaveClick} disabled={!canSave}>
            Save Result
          </button>
        </div>
      </div>
      <div className="status-row">
        <span className="status-pill">{modeLabel}</span>
        {analyzing && <span className="status-pill warm">Analyzing</span>}
      </div>
      {mode !== "live" && (
        <p className="file-note header-file-note">Source: {currentFileName}</p>
      )}
    </header>
  );
}

export function VideoOverlayPanel() {
  return (
    <div className="video-overlay">
      <div className="scoreboard-row">
        <div className="clock-pill">00:00</div>
        <div className="team-block red-team">
          <strong>Red Team</strong>
        </div>
        <div className="score-pill">0 : 0</div>
        <div className="team-block blue-team">
          <strong>Blue Team</strong>
        </div>
      </div>

      <div className="diagram-stack">
        <article className="diagram-card">
          <p className="diagram-title">Ball Possession</p>
          <div className="diagram-bars">
            <span className="bar red" style={{ width: "52%" }} />
            <span className="bar blue" style={{ width: "48%" }} />
          </div>
        </article>

        <article className="diagram-card">
          <p className="diagram-title">Pressure Index</p>
          <svg
            className="pressure-chart"
            viewBox="0 0 160 56"
            role="img"
            aria-label="Pressure index line chart placeholder"
          >
            <polyline
              className="pressure-fill"
              points="0,56 8,42 28,40 48,28 68,31 88,22 108,24 128,16 148,18 160,56"
            />
            <polyline
              className="pressure-line"
              points="8,42 28,40 48,28 68,31 88,22 108,24 128,16 148,18"
            />
          </svg>
        </article>

        <article className="diagram-card">
          <p className="diagram-title">Passing Flow</p>
          <div className="flow-grid">
            <span className="node red" />
            <span className="node neutral" />
            <span className="node blue" />
          </div>
        </article>
      </div>
    </div>
  );
}

export function StatsPanel() {
  return (
    <section className="stats-panel">
      <article className="stat-card">
        <p className="stat-label">Ball Possession</p>
        <h3>52% - 48%</h3>
      </article>
      <article className="stat-card">
        <p className="stat-label">Shots On Goal</p>
        <h3>12 - 10</h3>
      </article>
      <article className="stat-card">
        <p className="stat-label">Pass Accuracy</p>
        <h3>87% - 84%</h3>
      </article>
      <article className="stat-card">
        <p className="stat-label">Fastest Shot</p>
        <h3>31.8 km/h</h3>
      </article>
    </section>
  );
}

export function AnalysisModal({ visible }) {
  if (!visible) {
    return null;
  }
  return (
    <div className="modal-overlay">
      <div className="modal">Analysis in progress</div>
    </div>
  );
}

export function ErrorModal({ error, onDismiss }) {
  if (!error) {
    return null;
  }
  return (
    <div className="modal-overlay">
      <div className="modal error">
        <p>{error}</p>
        <button type="button" onClick={onDismiss}>
          Dismiss
        </button>
      </div>
    </div>
  );
}
