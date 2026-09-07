import { UI_TEXT } from "../../AppConstants";

export function AnalysisModal({ visible }) {
  if (!visible) return null;
  return (
    <div className="modal-overlay">
      <div className="modal">{UI_TEXT.ANALYSIS_IN_PROGRESS}</div>
    </div>
  );
}
