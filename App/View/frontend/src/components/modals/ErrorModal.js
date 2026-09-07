import { UI_TEXT } from "../../AppConstants";

export function ErrorModal({ error, onDismiss }) {
  if (!error) return null;
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
