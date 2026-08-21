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
  onCreatePlayerClick,
  onGenerateTeamsClick,
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
          <button type="button" onClick={onCreatePlayerClick}>
            Create Player
          </button>
          <button type="button" onClick={onGenerateTeamsClick}>
            Generate Teams
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

export function CreatePlayerModal({
  visible,
  nickname,
  successMessage,
  onNicknameChange,
  onSubmit,
  onCancel,
}) {
  if (!visible) {
    return null;
  }

  return (
    <div className="modal-overlay">
      <form className="modal player-modal" onSubmit={onSubmit}>
        <h2>Create player</h2>
        <div className="player-label-row">
          <label htmlFor="player-nickname">Nickname</label>
          {successMessage && <p className="player-success">{successMessage}</p>}
        </div>
        <input
          id="player-nickname"
          value={nickname}
          onChange={(event) => onNicknameChange(event.target.value)}
          autoFocus
          required
        />
        <div className="modal-actions">
          <button type="button" onClick={onCancel}>
            Cancel
          </button>
          <button type="submit">Create</button>
        </div>
      </form>
    </div>
  );
}

export function GenerateTeamsModal({
  visible,
  nicknames,
  playerStatuses,
  teamNames,
  teams,
  onNicknameChange,
  onPlayerBlur,
  onTeamNameChange,
  onGenerate,
  onCancel,
}) {
  if (!visible) {
    return null;
  }

  return (
    <div className="modal-overlay">
      <div className="modal teams-modal">
        <h2>Generate teams</h2>
        <div className="nickname-list">
          {nicknames.map((nickname, index) => (
            <div className="nickname-row" key={index}>
              <input
                value={nickname}
                placeholder={`Player ${index + 1}`}
                onChange={(event) =>
                  onNicknameChange(index, event.target.value)
                }
                onBlur={() => onPlayerBlur(index)}
              />
              {playerStatuses[index]?.exists && (
                <span className="player-elo-status">
                  Elo: {playerStatuses[index].elo}
                </span>
              )}
              {playerStatuses[index] && !playerStatuses[index].exists && (
                <span className="player-missing-status">Not exists</span>
              )}
            </div>
          ))}
        </div>
        <div className="modal-actions">
          <button type="button" onClick={onCancel}>
            Close
          </button>
          <button type="button" onClick={() => onGenerate(false)}>
            Random
          </button>
          <button type="button" onClick={() => onGenerate(true)}>
            By Elo
          </button>
        </div>
        {teams && (
          <div className="team-results">
            <h3>Generation results</h3>
            {teams.map((team, index) => (
              <article
                className={`generated-team team-block ${index === 0 ? "red-team" : "blue-team"}`}
                key={index}
              >
                <input
                  className="generated-team-name"
                  value={teamNames[index]}
                  onChange={(event) =>
                    onTeamNameChange(index, event.target.value)
                  }
                  aria-label={`${index === 0 ? "Red" : "Blue"} team name`}
                />
                <div className="generated-team-players">
                  <span className="generated-player generated-player-left">
                    <span>{team.players[0].nickname}</span>
                    <span>({team.players[0].elo})</span>
                  </span>
                  <span className="generated-player generated-player-right">
                    <span>({team.players[1].elo})</span>
                    <span>{team.players[1].nickname}</span>
                  </span>
                </div>
              </article>
            ))}
          </div>
        )}
      </div>
    </div>
  );
}
