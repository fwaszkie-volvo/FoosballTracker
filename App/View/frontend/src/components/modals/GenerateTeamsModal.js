import { UI_TEXT, getTeamColorRgb } from "../../AppConstants";
import { TeamColorPicker } from "./TeamColorPicker";

function TeamOptions({ schema, formation, onSchemaChange, onFormationChange }) {
  return (
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
            onChange={() => onSchemaChange(schema === "elo" ? "random" : "elo")}
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
  );
}

function TeamResult({
  team,
  index,
  teamNames,
  teamColors,
  onTeamNameChange,
  onTeamColorChange,
}) {
  return (
    <article
      className="generated-team team-block"
      style={{ "--team-color": getTeamColorRgb(teamColors[index]) }}
    >
      <div className="generated-team-header">
        <input
          className="generated-team-name"
          value={teamNames[index]}
          onChange={(event) => onTeamNameChange(index, event.target.value)}
          aria-label={`${index === 0 ? UI_TEXT.FIRST_TEAM : UI_TEXT.SECOND_TEAM} name`}
        />
        <TeamColorPicker
          selectedColor={teamColors[index]}
          disabledColor={teamColors[index === 0 ? 1 : 0]}
          onSelect={(colorId) => onTeamColorChange(index, colorId)}
        />
      </div>
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
  );
}

export function GenerateTeamsModal({
  visible,
  nicknames,
  playerStatuses,
  teamNames,
  teamColors,
  teams,
  schema,
  formation,
  onNicknameChange,
  onPlayerBlur,
  onTeamNameChange,
  onTeamColorChange,
  onSchemaChange,
  onFormationChange,
  onGenerate,
  onSave,
  onCancel,
}) {
  if (!visible) return null;
  const duplicateIndexes = nicknames.map((nickname, index) => {
    const normalized = nickname.trim().toLowerCase();
    return Boolean(
      normalized &&
      nicknames.some(
        (other, otherIndex) =>
          otherIndex < index && other.trim().toLowerCase() === normalized,
      ),
    );
  });
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
                {!duplicateIndexes[index] && playerStatuses[index]?.exists && (
                  <span className="player-elo-status">
                    {UI_TEXT.PLAYER_ELO_PREFIX} {playerStatuses[index].elo}
                  </span>
                )}
                {!duplicateIndexes[index] &&
                  playerStatuses[index] &&
                  !playerStatuses[index].exists && (
                    <span className="player-missing-status">
                      {UI_TEXT.PLAYER_NOT_EXISTS}
                    </span>
                  )}
                {duplicateIndexes[index] && (
                  <span className="player-duplicate-status">
                    {UI_TEXT.NICKNAME_DUPLICATE}
                  </span>
                )}
              </div>
            ))}
          </div>
        </fieldset>
        <TeamOptions
          schema={schema}
          formation={formation}
          onSchemaChange={onSchemaChange}
          onFormationChange={onFormationChange}
        />
        <div className="team-results">
          <h3>{UI_TEXT.GENERATION_RESULTS}</h3>
          {teams
            ? teams.map((team, index) => (
                <TeamResult
                  key={index}
                  team={team}
                  index={index}
                  teamNames={teamNames}
                  teamColors={teamColors}
                  onTeamNameChange={onTeamNameChange}
                  onTeamColorChange={onTeamColorChange}
                />
              ))
            : [0, 1].map((index) => (
                <article
                  className="generated-team team-block result-placeholder-team"
                  style={{ "--team-color": getTeamColorRgb(teamColors[index]) }}
                  key={index}
                >
                  <div className="generated-team-header">
                    <span className="generated-team-name">
                      {index === 0 ? UI_TEXT.FIRST_TEAM : UI_TEXT.SECOND_TEAM}
                    </span>
                    <TeamColorPicker
                      selectedColor={teamColors[index]}
                      disabledColor={teamColors[index === 0 ? 1 : 0]}
                      onSelect={(colorId) => onTeamColorChange(index, colorId)}
                    />
                  </div>
                  <div className="generated-team-players">
                    <span className="generated-player generated-player-left">
                      <span>{UI_TEXT.PLACEHOLDER_DASH}</span>
                      <span />
                    </span>
                    <span className="generated-player generated-player-right">
                      <span />
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
