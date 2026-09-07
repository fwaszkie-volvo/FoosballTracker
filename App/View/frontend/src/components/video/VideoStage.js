import {
  LIVE_STREAM_STATE,
  MEDIA_SRC,
  MODE,
  UI_TEXT,
} from "../../AppConstants";
import { ScoreboardOverlay } from "./ScoreboardOverlay";

export function VideoStage({
  mode,
  status,
  videoVersion,
  liveStreamState,
  teamNames,
  teamColors,
  positions,
  setIndex,
  setCount,
  onPrevSet,
  onNextSet,
  onLiveReady,
  onLiveError,
}) {
  return (
    <div className="video-stage">
      <ScoreboardOverlay
        teamNames={teamNames}
        teamColors={teamColors}
        positions={positions}
        setIndex={setIndex}
        setCount={setCount}
        onPrevSet={onPrevSet}
        onNextSet={onNextSet}
      />
      {mode === MODE.LIVE && (
        <>
          <>
            {liveStreamState !== LIVE_STREAM_STATE.UNAVAILABLE && (
              <img
                className={`live-view ${liveStreamState === LIVE_STREAM_STATE.CHECKING ? "live-view-probing" : ""}`}
                src={MEDIA_SRC.LIVE_STREAM}
                alt="Live view"
                onLoad={onLiveReady}
                onError={onLiveError}
              />
            )}
          </>
          {liveStreamState === LIVE_STREAM_STATE.CHECKING && (
            <p className="placeholder">{UI_TEXT.CONNECTING_STREAM}</p>
          )}
          {liveStreamState === LIVE_STREAM_STATE.UNAVAILABLE && (
            <p className="placeholder">{UI_TEXT.STREAM_UNAVAILABLE}</p>
          )}
        </>
      )}
      {mode === MODE.VIDEO && status.videoUrl && (
        <video
          className="video-view"
          src={`${status.videoUrl}?v=${videoVersion}`}
          controls
          autoPlay
        />
      )}
      {mode === MODE.IDLE && (
        <p className="placeholder">{UI_TEXT.IDLE_PLACEHOLDER}</p>
      )}
    </div>
  );
}
