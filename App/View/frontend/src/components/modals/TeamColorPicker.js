import { TEAM_COLOR_PALETTE, UI_TEXT } from "../../AppConstants";

export function TeamColorPicker({ selectedColor, disabledColor, onSelect }) {
  return (
    <div className="team-color-picker">
      {TEAM_COLOR_PALETTE.map((color) => {
        const isTaken = color.id === disabledColor;
        return (
          <button
            key={color.id}
            type="button"
            className={`team-color-swatch ${color.id === selectedColor ? "active" : ""}`}
            style={{ "--swatch-color": color.rgb }}
            onClick={() => onSelect(color.id)}
            disabled={isTaken}
            aria-label={`${color.label}${isTaken ? ` (${UI_TEXT.COLOR_TAKEN})` : ""}`}
            title={isTaken ? UI_TEXT.COLOR_TAKEN : color.label}
          />
        );
      })}
    </div>
  );
}
