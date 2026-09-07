import { UI_TEXT } from "../../AppConstants";

const STATS = [
  [UI_TEXT.BALL_POSSESSION, "52% - 48%"],
  [UI_TEXT.SHOTS_ON_GOAL, "12 - 10"],
  [UI_TEXT.PASS_ACCURACY, "87% - 84%"],
  [UI_TEXT.FASTEST_SHOT, "31.8 km/h"],
];

export function StatsPanel() {
  return (
    <section className="stats-panel">
      {STATS.map(([label, value]) => (
        <article className="stat-card" key={label}>
          <p className="stat-label">{label}</p>
          <h3>{value}</h3>
        </article>
      ))}
    </section>
  );
}
