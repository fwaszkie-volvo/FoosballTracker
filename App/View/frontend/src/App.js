import "./App.css";

import { useCallback, useEffect, useRef, useState } from "react";

import {
  AnalysisModal,
  AppHeader,
  CreatePlayerModal,
  ErrorModal,
  GenerateTeamsModal,
  StatsPanel,
  VideoOverlayPanel,
} from "./AppViewParts";
import {
  API_ROUTE,
  DEFAULT_TEAM_NAMES,
  FILE_ACCEPT_VIDEO_TYPE,
  INITIAL_DISPLAY_POSITIONS,
  INITIAL_STATUS,
  LIVE_STREAM_STATE,
  LIVE_STREAM_TIMEOUT_MS,
  MEDIA_SRC,
  MODE,
  SETS_PER_MATCH,
  STATUS_POLL_INTERVAL_MS,
  UI_TEXT,
} from "./AppConstants";

function App() {
  const [status, setStatus] = useState(INITIAL_STATUS);
  const [mode, setMode] = useState(MODE.IDLE);
  const [videoVersion, setVideoVersion] = useState(0);
  const [currentFileName, setCurrentFileName] = useState(
    UI_TEXT.DEFAULT_FILE_NAME,
  );
  const [liveStreamState, setLiveStreamState] = useState(
    LIVE_STREAM_STATE.IDLE,
  );
  const [isCreatePlayerOpen, setIsCreatePlayerOpen] = useState(false);
  const [nickname, setNickname] = useState("");
  const [playerCreated, setPlayerCreated] = useState(false);
  const [isGenerateTeamsOpen, setIsGenerateTeamsOpen] = useState(false);
  const [teamNicknames, setTeamNicknames] = useState(["", "", "", ""]);
  const [playerStatuses, setPlayerStatuses] = useState([
    null,
    null,
    null,
    null,
  ]);
  const [teamNames, setTeamNames] = useState(DEFAULT_TEAM_NAMES);
  const [displayTeamNames, setDisplayTeamNames] = useState(DEFAULT_TEAM_NAMES);
  const [displayPositions, setDisplayPositions] = useState(
    INITIAL_DISPLAY_POSITIONS,
  );
  const [currentSetIndex, setCurrentSetIndex] = useState(0);
  const [generatedTeams, setGeneratedTeams] = useState(null);
  const [teamSchema, setTeamSchema] = useState("random");
  const [teamFormation, setTeamFormation] = useState("random");
  const fileInputRef = useRef(null);
  const wasAnalyzingRef = useRef(false);
  const liveProbeTimerRef = useRef(null);

  const refreshStatus = useCallback(async () => {
    try {
      const response = await fetch(API_ROUTE.STATUS);
      const data = await response.json();
      setStatus((current) => ({
        ...data,
        error: data.error ?? current.error,
      }));
      if (data.videoUrl) {
        setMode((current) => (current === MODE.LIVE ? current : MODE.VIDEO));
      }
      // The analysis result file only changes once the backend finishes the
      // asynchronous analysis job, so bump the video cache-buster then.
      if (wasAnalyzingRef.current && !data.analyzing) {
        setVideoVersion((current) => current + 1);
      }
      wasAnalyzingRef.current = data.analyzing;
    } catch {
      // Backend temporarily unreachable; keep last known status.
    }
  }, []);

  useEffect(() => {
    refreshStatus();
    const interval = setInterval(refreshStatus, STATUS_POLL_INTERVAL_MS);
    return () => clearInterval(interval);
  }, [refreshStatus]);

  useEffect(() => {
    if (mode !== MODE.LIVE) {
      setLiveStreamState(LIVE_STREAM_STATE.IDLE);
      if (liveProbeTimerRef.current) {
        clearTimeout(liveProbeTimerRef.current);
        liveProbeTimerRef.current = null;
      }
      return;
    }

    if (liveStreamState !== LIVE_STREAM_STATE.CHECKING) {
      return;
    }

    liveProbeTimerRef.current = setTimeout(() => {
      setLiveStreamState((current) =>
        current === LIVE_STREAM_STATE.CHECKING
          ? LIVE_STREAM_STATE.UNAVAILABLE
          : current,
      );
      liveProbeTimerRef.current = null;
    }, LIVE_STREAM_TIMEOUT_MS);

    return () => {
      if (liveProbeTimerRef.current) {
        clearTimeout(liveProbeTimerRef.current);
        liveProbeTimerRef.current = null;
      }
    };
  }, [mode, liveStreamState]);

  const handleLiveClick = async () => {
    setMode(MODE.LIVE);
    setLiveStreamState(LIVE_STREAM_STATE.CHECKING);
    try {
      const response = await fetch(API_ROUTE.LIVE, { method: "POST" });
      if (!response.ok) {
        setLiveStreamState(LIVE_STREAM_STATE.UNAVAILABLE);
      }
    } catch {
      setLiveStreamState(LIVE_STREAM_STATE.UNAVAILABLE);
    }
  };

  const handleLoadClick = () => {
    fileInputRef.current?.click();
  };

  const handleFileSelected = async (event) => {
    const file = event.target.files?.[0];
    event.target.value = "";
    if (!file) {
      return;
    }
    const formData = new FormData();
    formData.append("file", file);
    setMode(MODE.VIDEO);
    setCurrentFileName(file.name);
    await fetch(API_ROUTE.LOAD, { method: "POST", body: formData });
    setVideoVersion((current) => current + 1);
    refreshStatus();
  };

  const handleAnalClick = async () => {
    await fetch(API_ROUTE.ANALYSE, { method: "POST" });
    refreshStatus();
  };

  const handleSaveClick = () => {
    window.location.href = API_ROUTE.SAVE;
  };

  const handleCreatePlayer = async (event) => {
    event.preventDefault();
    setPlayerCreated(false);
    const response = await fetch(API_ROUTE.PLAYERS, {
      method: "POST",
      headers: { "Content-Type": "text/plain" },
      body: nickname.trim(),
    });
    if (response.ok) {
      setNickname("");
      setPlayerCreated(true);
      return;
    }
    const data = await response.json().catch(() => null);
    setStatus((current) => ({
      ...current,
      error: data?.error || UI_TEXT.PLAYER_CREATE_ERROR,
    }));
  };

  const openCreatePlayer = () => {
    setPlayerCreated(false);
    setNickname("");
    setIsCreatePlayerOpen(true);
  };

  const closeCreatePlayer = () => {
    setPlayerCreated(false);
    setNickname("");
    setIsCreatePlayerOpen(false);
  };

  const openGenerateTeams = () => {
    setGeneratedTeams(null);
    setTeamNicknames(["", "", "", ""]);
    setPlayerStatuses([null, null, null, null]);
    setTeamNames(DEFAULT_TEAM_NAMES);
    setTeamSchema("random");
    setTeamFormation("random");
    setIsGenerateTeamsOpen(true);
  };

  const closeGenerateTeams = () => {
    setGeneratedTeams(null);
    setTeamNicknames(["", "", "", ""]);
    setPlayerStatuses([null, null, null, null]);
    setTeamNames(DEFAULT_TEAM_NAMES);
    setTeamSchema("random");
    setTeamFormation("random");
    setIsGenerateTeamsOpen(false);
  };

  const handleSaveTeams = () => {
    setDisplayTeamNames(teamNames);
    if (generatedTeams?.formation) {
      setDisplayPositions(generatedTeams.formation);
    }
    setCurrentSetIndex(0);
    closeGenerateTeams();
  };

  const handlePrevSet = () =>
    setCurrentSetIndex((current) => Math.max(current - 1, 0));

  const handleNextSet = () =>
    setCurrentSetIndex((current) => Math.min(current + 1, SETS_PER_MATCH - 1));

  const handlePlayerBlur = async (index) => {
    const nicknameValue = teamNicknames[index].trim();
    if (!nicknameValue) {
      setPlayerStatuses((current) =>
        current.map((playerStatus, statusIndex) =>
          statusIndex === index ? null : playerStatus,
        ),
      );
      return;
    }

    const response = await fetch(
      `${API_ROUTE.PLAYER}?nickname=${encodeURIComponent(nicknameValue)}`,
    );
    if (!response.ok) {
      return;
    }
    const data = await response.json();
    setPlayerStatuses((current) =>
      current.map((playerStatus, statusIndex) =>
        statusIndex === index
          ? data.exists
            ? { exists: true, elo: data.elo }
            : { exists: false }
          : playerStatus,
      ),
    );
  };

  const handleGenerateTeams = async () => {
    const response = await fetch(
      `${API_ROUTE.TEAMS}?mode=${teamSchema}&formation=${teamFormation}`,
      {
        method: "POST",
        headers: { "Content-Type": "text/plain" },
        body: teamNicknames.map((value) => value.trim()).join("\n"),
      },
    );
    if (response.ok) {
      setGeneratedTeams(await response.json());
      return;
    }
    const data = await response.json().catch(() => null);
    setStatus((current) => ({
      ...current,
      error: data?.error || UI_TEXT.TEAM_GENERATION_ERROR,
    }));
  };

  const dismissError = () =>
    setStatus((current) => ({ ...current, error: null }));

  const modeLabel =
    mode === MODE.LIVE
      ? UI_TEXT.MODE_LABEL_LIVE
      : mode === MODE.VIDEO
        ? UI_TEXT.MODE_LABEL_VIDEO
        : UI_TEXT.MODE_LABEL_IDLE;

  const canAnalyse =
    mode === MODE.VIDEO && Boolean(status.videoUrl) && !status.analyzing;
  const canSave = mode === MODE.VIDEO && Boolean(status.videoUrl);

  return (
    <div className="app-shell">
      <AppHeader
        mode={mode}
        currentFileName={currentFileName}
        modeLabel={modeLabel}
        canAnalyse={canAnalyse}
        canSave={canSave}
        onLiveClick={handleLiveClick}
        onLoadClick={handleLoadClick}
        onAnalClick={handleAnalClick}
        onSaveClick={handleSaveClick}
        onCreatePlayerClick={openCreatePlayer}
        onGenerateTeamsClick={openGenerateTeams}
      />

      <main className="viewer-panel">
        <div className="video-stage">
          <VideoOverlayPanel
            teamNames={displayTeamNames}
            positions={displayPositions[currentSetIndex]}
            setIndex={currentSetIndex}
            setCount={SETS_PER_MATCH}
            onPrevSet={handlePrevSet}
            onNextSet={handleNextSet}
          />

          {mode === MODE.LIVE && (
            <>
              {liveStreamState !== LIVE_STREAM_STATE.UNAVAILABLE && (
                <img
                  className={`live-view ${liveStreamState === LIVE_STREAM_STATE.CHECKING ? "live-view-probing" : ""}`}
                  src={MEDIA_SRC.LIVE_STREAM}
                  alt="Live view"
                  onLoad={() => setLiveStreamState(LIVE_STREAM_STATE.READY)}
                  onError={() =>
                    setLiveStreamState(LIVE_STREAM_STATE.UNAVAILABLE)
                  }
                />
              )}
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

        <StatsPanel />
      </main>

      <input
        ref={fileInputRef}
        type="file"
        accept={FILE_ACCEPT_VIDEO_TYPE}
        className="hidden-file-input"
        onChange={handleFileSelected}
      />

      <AnalysisModal visible={status.analyzing} />
      <CreatePlayerModal
        visible={isCreatePlayerOpen}
        nickname={nickname}
        successMessage={playerCreated ? UI_TEXT.PLAYER_CREATED : null}
        onNicknameChange={setNickname}
        onSubmit={handleCreatePlayer}
        onCancel={closeCreatePlayer}
      />
      <GenerateTeamsModal
        visible={isGenerateTeamsOpen}
        nicknames={teamNicknames}
        playerStatuses={playerStatuses}
        teamNames={teamNames}
        teams={generatedTeams?.teams}
        schema={teamSchema}
        formation={teamFormation}
        onSchemaChange={setTeamSchema}
        onFormationChange={setTeamFormation}
        onNicknameChange={(index, value) => {
          setTeamNicknames((current) =>
            current.map((nicknameValue, nicknameIndex) =>
              nicknameIndex === index ? value : nicknameValue,
            ),
          );
          setPlayerStatuses((current) =>
            current.map((playerStatus, statusIndex) =>
              statusIndex === index ? null : playerStatus,
            ),
          );
        }}
        onPlayerBlur={handlePlayerBlur}
        onTeamNameChange={(index, value) =>
          setTeamNames((current) =>
            current.map((teamName, teamIndex) =>
              teamIndex === index ? value : teamName,
            ),
          )
        }
        onGenerate={handleGenerateTeams}
        onSave={handleSaveTeams}
        onCancel={closeGenerateTeams}
      />
      <ErrorModal error={status.error} onDismiss={dismissError} />
    </div>
  );
}

export default App;
