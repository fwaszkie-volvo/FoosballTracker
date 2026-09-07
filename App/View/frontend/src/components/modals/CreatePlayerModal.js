import { UI_TEXT } from "../../AppConstants";

export function CreatePlayerModal({
  visible,
  nickname,
  successMessage,
  onNicknameChange,
  onSubmit,
  onCancel,
}) {
  if (!visible) return null;
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
