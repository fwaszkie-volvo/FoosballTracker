import {
  DEFAULT_TEAM_COLORS,
  UI_TEXT,
  getTeamColorRgb,
} from "../../AppConstants";
import { TablePositionPanel } from "./TablePositionPanel";

export function ScoreboardOverlay({
  teamNames,
  teamColors = DEFAULT_TEAM_COLORS,
  positions,
  setIndex,
  setCount,
  onPrevSet,
  onNextSet,
}) {
  return (
    <div className="video-overlay">
      <div className="scoreboard-row">
        <div className="clock-pill">00:00</div>
        <div
          className="team-block"
          style={{ "--team-color": getTeamColorRgb(teamColors[0]) }}
        >
          <strong>{teamNames[0]}</strong>
        </div>
        <div className="score-pill">0 : 0</div>
        <div
          className="team-block"
          style={{ "--team-color": getTeamColorRgb(teamColors[1]) }}
        >
          <strong>{teamNames[1]}</strong>
        </div>
      </div>
      <TablePositionPanel
        positions={positions}
        teamColors={teamColors}
        setIndex={setIndex}
        setCount={setCount}
        onPrevSet={onPrevSet}
        onNextSet={onNextSet}
      />
      <div className="diagram-stack">
        <article className="diagram-card">
          <p className="diagram-title">{UI_TEXT.BALL_POSSESSION}</p>
          <div className="diagram-bars">
            <span
              className="bar"
              style={{
                width: "52%",
                "--team-color": getTeamColorRgb(teamColors[0]),
              }}
            />
            <span
              className="bar"
              style={{
                width: "48%",
                "--team-color": getTeamColorRgb(teamColors[1]),
              }}
            />
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
            <span
              className="node"
              style={{ "--team-color": getTeamColorRgb(teamColors[0]) }}
            />
            <span className="node neutral" />
            <span
              className="node"
              style={{ "--team-color": getTeamColorRgb(teamColors[1]) }}
            />
          </div>
        </article>
      </div>
    </div>
  );
}
