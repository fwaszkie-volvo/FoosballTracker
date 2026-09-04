import { UI_TEXT } from "./AppConstants";

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

export function VideoOverlayPanel({
  teamNames = ["Red Team", "Blue Team"],
  positions,
  setIndex = 0,
  setCount = 1,
  onPrevSet,
  onNextSet,
}) {
  return (
    <div className="video-overlay">
      <div className="scoreboard-row">
        <div className="clock-pill">00:00</div>
        <div className="team-block red-team">
          <strong>{teamNames[0]}</strong>
        </div>
        <div className="score-pill">0 : 0</div>
        <div className="team-block blue-team">
          <strong>{teamNames[1]}</strong>
        </div>
      </div>

      <TablePositionPanel
        positions={positions}
        setIndex={setIndex}
        setCount={setCount}
        onPrevSet={onPrevSet}
        onNextSet={onNextSet}
      />

      <div className="diagram-stack">
        <article className="diagram-card">
          <p className="diagram-title">{UI_TEXT.BALL_POSSESSION}</p>
          <div className="diagram-bars">
            <span className="bar red" style={{ width: "52%" }} />
            <span className="bar blue" style={{ width: "48%" }} />
          </div>
        </article>

        <article className="diagram-card">
          <p className="diagram-title">{UI_TEXT.PRESSURE_INDEX}</p>
          <svg
            className="pressure-chart"
            viewBox="0 0 160 56"
            role="img"
            aria-label={UI_TEXT.PRESSURE_CHART_ARIA}
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
          <p className="diagram-title">{UI_TEXT.PASSING_FLOW}</p>
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
        <p className="stat-label">{UI_TEXT.BALL_POSSESSION}</p>
        <h3>52% - 48%</h3>
      </article>
      <article className="stat-card">
        <p className="stat-label">{UI_TEXT.SHOTS_ON_GOAL}</p>
        <h3>12 - 10</h3>
      </article>
      <article className="stat-card">
        <p className="stat-label">{UI_TEXT.PASS_ACCURACY}</p>
        <h3>87% - 84%</h3>
      </article>
      <article className="stat-card">
        <p className="stat-label">{UI_TEXT.FASTEST_SHOT}</p>
        <h3>31.8 km/h</h3>
      </article>
    </section>
  );
}

export function TablePositionPanel({
  positions,
  setIndex = 0,
  setCount = 1,
  onPrevSet,
  onNextSet,
}) {
  return (
    <article
      className="table-position-panel diagram-card"
      aria-label={UI_TEXT.TABLE_POSITIONS_ARIA}
    >
      <div className="set-nav-col set-nav-col-left">
        <button
          type="button"
          className="set-nav-arrow"
          onClick={onPrevSet}
          disabled={setIndex === 0}
          aria-label={UI_TEXT.PREVIOUS_SET_ARIA}
        >
          &#8249;
        </button>
        <span className="set-nav-label">
          {UI_TEXT.SET_LABEL_PREFIX} {setIndex + 1}/{setCount}
        </span>
      </div>
      <div className="table-image-frame" aria-label={UI_TEXT.TABLE_IMAGE_ARIA}>
        <img
          src={`${process.env.PUBLIC_URL}/images/foosball-table.png`}
          alt=""
          onError={(event) => {
            event.currentTarget.hidden = true;
          }}
        />
        <div
          className="table-marker-column table-marker-column-right"
          aria-hidden="true"
        >
          <div className="table-marker team-block blue-team">
            <strong>{positions.blue.offence}</strong>
          </div>
          <div className="table-marker team-block blue-team">
            <strong>{positions.blue.defence}</strong>
          </div>
        </div>
        <div
          className="table-marker-column table-marker-column-left"
          aria-hidden="true"
        >
          <div className="table-marker team-block red-team">
            <strong>{positions.red.defence}</strong>
          </div>
          <div className="table-marker team-block red-team">
            <strong>{positions.red.offence}</strong>
          </div>
        </div>
      </div>
      <div className="set-nav-col set-nav-col-right">
        <button
          type="button"
          className="set-nav-arrow"
          onClick={onNextSet}
          disabled={setIndex === setCount - 1}
          aria-label={UI_TEXT.NEXT_SET_ARIA}
        >
          &#8250;
        </button>
        <span className="set-nav-score">{UI_TEXT.SCORE_PLACEHOLDER}</span>
      </div>
    </article>
  );
}

export function AnalysisModal({ visible }) {
  if (!visible) {
    return null;
  }
  return (
    <div className="modal-overlay">
      <div className="modal">{UI_TEXT.ANALYSIS_IN_PROGRESS}</div>
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
          {UI_TEXT.DISMISS}
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
        <h2>{UI_TEXT.CREATE_PLAYER}</h2>
        <div className="player-label-row">
          <label htmlFor="player-nickname">{UI_TEXT.NICKNAME}</label>
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
            {UI_TEXT.CANCEL}
          </button>
          <button type="submit">{UI_TEXT.CREATE}</button>
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
  schema,
  formation,
  onNicknameChange,
  onPlayerBlur,
  onTeamNameChange,
  onSchemaChange,
  onFormationChange,
  onGenerate,
  onSave,
  onCancel,
}) {
  if (!visible) {
    return null;
  }

  return (
    <div className="modal-overlay">
      <div className="modal teams-modal">
        <h2>{UI_TEXT.GENERATE_TEAMS}</h2>
        <fieldset className="nickname-group">
          <legend>{UI_TEXT.NICKNAMES_LEGEND}</legend>
          <div className="nickname-list">
            {nicknames.map((nickname, index) => (
              <div className="nickname-row" key={index}>
                <input
                  value={nickname}
                  placeholder={`${UI_TEXT.PLAYER_PLACEHOLDER_PREFIX} ${index + 1}`}
                  onChange={(event) =>
                    onNicknameChange(index, event.target.value)
                  }
                  onBlur={() => onPlayerBlur(index)}
                />
                {playerStatuses[index]?.exists && (
                  <span className="player-elo-status">
                    {UI_TEXT.PLAYER_ELO_PREFIX} {playerStatuses[index].elo}
                  </span>
                )}
                {playerStatuses[index] && !playerStatuses[index].exists && (
                  <span className="player-missing-status">
                    {UI_TEXT.PLAYER_NOT_EXISTS}
                  </span>
                )}
              </div>
            ))}
          </div>
        </fieldset>
        <div className="team-options">
          <fieldset className="option-group">
            <legend>{UI_TEXT.SCHEMA_LEGEND}</legend>
            <label>
              <input
                type="checkbox"
                checked={schema === "random"}
                onChange={() => onSchemaChange("random")}
              />
              {UI_TEXT.RANDOM}
            </label>
            <label>
              <input
                type="checkbox"
                checked={schema === "elo"}
                onChange={() =>
                  onSchemaChange(schema === "elo" ? "random" : "elo")
                }
              />
              {UI_TEXT.BY_ELO}
            </label>
          </fieldset>
          <fieldset className="option-group">
            <legend>{UI_TEXT.FORMATION_LEGEND}</legend>
            {[
              ["random", UI_TEXT.RANDOM],
              ["standard", UI_TEXT.FORMATION_STANDARD],
            ].map(([value, label]) => (
              <label key={value}>
                <input
                  type="checkbox"
                  checked={formation === value}
                  onChange={() => onFormationChange(value)}
                />
                {label}
              </label>
            ))}
          </fieldset>
        </div>
        <div className="team-results">
          <h3>{UI_TEXT.GENERATION_RESULTS}</h3>
          {teams
            ? teams.map((team, index) => (
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
                    aria-label={`${index === 0 ? UI_TEXT.RED_TEAM : UI_TEXT.BLUE_TEAM} name`}
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
              ))
            : [0, 1].map((index) => (
                <article
                  className={`generated-team team-block result-placeholder-team ${index === 0 ? "red-team" : "blue-team"}`}
                  key={index}
                >
                  <span className="generated-team-name">
                    {index === 0 ? UI_TEXT.RED_TEAM : UI_TEXT.BLUE_TEAM}
                  </span>
                  <div className="generated-team-players">
                    <span className="generated-player generated-player-left">
                      <span>{UI_TEXT.PLACEHOLDER_DASH}</span>
                      <span></span>
                    </span>
                    <span className="generated-player generated-player-right">
                      <span></span>
                      <span>{UI_TEXT.PLACEHOLDER_DASH}</span>
                    </span>
                  </div>
                </article>
              ))}
        </div>
        <div className="modal-actions">
          <button type="button" onClick={onCancel}>
            {UI_TEXT.CANCEL}
          </button>
          <button type="button" onClick={onGenerate}>
            {UI_TEXT.GENERATE}
          </button>
          <button type="button" onClick={onSave} disabled={!teams}>
            {UI_TEXT.SAVE}
          </button>
        </div>
      </div>
    </div>
  );
}
