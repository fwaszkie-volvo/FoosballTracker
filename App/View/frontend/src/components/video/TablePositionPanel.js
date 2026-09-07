import {
  DEFAULT_TEAM_COLORS,
  UI_TEXT,
  getTeamColorRgb,
} from "../../AppConstants";

export function TablePositionPanel({
  positions,
  teamColors = DEFAULT_TEAM_COLORS,
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
          className={setIndex % 2 === 1 ? "table-image-flipped" : ""}
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
          <div
            className="table-marker team-block"
            style={{ "--team-color": getTeamColorRgb(teamColors[1]) }}
          >
            <strong>{positions.blue.offence}</strong>
          </div>
          <div
            className="table-marker team-block"
            style={{ "--team-color": getTeamColorRgb(teamColors[1]) }}
          >
            <strong>{positions.blue.defence}</strong>
          </div>
        </div>
        <div
          className="table-marker-column table-marker-column-left"
          aria-hidden="true"
        >
          <div
            className="table-marker team-block"
            style={{ "--team-color": getTeamColorRgb(teamColors[0]) }}
          >
            <strong>{positions.red.defence}</strong>
          </div>
          <div
            className="table-marker team-block"
            style={{ "--team-color": getTeamColorRgb(teamColors[0]) }}
          >
            <strong>{positions.red.offence}</strong>
          </div>
        </div>
      </div>
      <div className="set-nav-col set-nav-col-right">
        <button
          type="button"
          className="set-nav-arrow"
          onClick={onNextSet}
          aria-label={UI_TEXT.NEXT_SET_ARIA}
        >
          &#8250;
        </button>
        <span className="set-nav-score">{UI_TEXT.SCORE_PLACEHOLDER}</span>
      </div>
    </article>
  );
}
