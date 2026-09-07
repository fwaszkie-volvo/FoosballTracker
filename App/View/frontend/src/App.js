import "./App.css";

import { useEffect, useRef, useState } from "react";

import { AppHeader } from "./components/layout/AppHeader";
import { AnalysisModal } from "./components/modals/AnalysisModal";
import { CreatePlayerModal } from "./components/modals/CreatePlayerModal";
import { ErrorModal } from "./components/modals/ErrorModal";
import { GenerateTeamsModal } from "./components/modals/GenerateTeamsModal";
import { StatsPanel } from "./components/video/StatsPanel";
import { VideoStage } from "./components/video/VideoStage";
import { useStatusPolling } from "./hooks/useStatusPolling";
import {
  API_ROUTE,
  DEFAULT_TEAM_COLORS,
  DEFAULT_TEAM_NAMES,
  FILE_ACCEPT_VIDEO_TYPE,
  INITIAL_DISPLAY_POSITIONS,
  LIVE_STREAM_STATE,
  LIVE_STREAM_TIMEOUT_MS,
  MODE,
  SETS_PER_MATCH,
  UI_TEXT,
} from "./AppConstants";

function App() {
  const [mode, setMode] = useState(MODE.IDLE);
  const { status, setStatus, videoVersion, setVideoVersion, refreshStatus } =
    useStatusPolling(setMode);
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
  const [teamColors, setTeamColors] = useState(DEFAULT_TEAM_COLORS);
  const [displayTeamColors, setDisplayTeamColors] =
    useState(DEFAULT_TEAM_COLORS);
  const [displayPositions, setDisplayPositions] = useState(
    INITIAL_DISPLAY_POSITIONS,
  );
  const [currentSetIndex, setCurrentSetIndex] = useState(0);
  const [generatedTeams, setGeneratedTeams] = useState(null);
  const [teamSchema, setTeamSchema] = useState("random");
  const [teamFormation, setTeamFormation] = useState("random");
  const fileInputRef = useRef(null);
  const liveProbeTimerRef = useRef(null);

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
    setTeamColors(DEFAULT_TEAM_COLORS);
    setTeamSchema("random");
    setTeamFormation("random");
    setIsGenerateTeamsOpen(true);
  };

  const closeGenerateTeams = () => {
    setGeneratedTeams(null);
    setTeamNicknames(["", "", "", ""]);
    setPlayerStatuses([null, null, null, null]);
    setTeamNames(DEFAULT_TEAM_NAMES);
    setTeamColors(DEFAULT_TEAM_COLORS);
    setTeamSchema("random");
    setTeamFormation("random");
    setIsGenerateTeamsOpen(false);
  };

  const handleSaveTeams = () => {
    setDisplayTeamNames(teamNames);
    setDisplayTeamColors(teamColors);
    if (generatedTeams?.formation) {
      setDisplayPositions(generatedTeams.formation);
    }
    setCurrentSetIndex(0);
    closeGenerateTeams();
  };

  const handlePrevSet = () =>
    setCurrentSetIndex(
      (current) => (current - 1 + SETS_PER_MATCH) % SETS_PER_MATCH,
    );

  const handleNextSet = () =>
    setCurrentSetIndex((current) => (current + 1) % SETS_PER_MATCH);

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
    const trimmedNicknames = teamNicknames.map((value) => value.trim());
    const duplicateNickname = trimmedNicknames.find(
      (nickname, index) =>
        nickname &&
        trimmedNicknames.some(
          (otherNickname, otherIndex) =>
            otherIndex < index &&
            otherNickname.toLowerCase() === nickname.toLowerCase(),
        ),
    );
    if (duplicateNickname) {
      setStatus((current) => ({
        ...current,
        error: `${UI_TEXT.PLAYER_DUPLICATE_ERROR_PREFIX}${duplicateNickname}${UI_TEXT.PLAYER_DUPLICATE_ERROR_SUFFIX}`,
      }));
      return;
    }
    const response = await fetch(
      `${API_ROUTE.TEAMS}?mode=${teamSchema}&formation=${teamFormation}`,
      {
        method: "POST",
        headers: { "Content-Type": "text/plain" },
        body: trimmedNicknames.join("\n"),
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
        <VideoStage
          mode={mode}
          status={status}
          videoVersion={videoVersion}
          liveStreamState={liveStreamState}
          teamNames={displayTeamNames}
          teamColors={displayTeamColors}
          positions={displayPositions[currentSetIndex]}
          setIndex={currentSetIndex}
          setCount={SETS_PER_MATCH}
          onPrevSet={handlePrevSet}
          onNextSet={handleNextSet}
          onLiveReady={() => setLiveStreamState(LIVE_STREAM_STATE.READY)}
          onLiveError={() => setLiveStreamState(LIVE_STREAM_STATE.UNAVAILABLE)}
        />

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
        teamColors={teamColors}
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
        onTeamColorChange={(index, colorId) =>
          setTeamColors((current) =>
            current[index === 0 ? 1 : 0] === colorId
              ? current
              : current.map((color, colorIndex) =>
                  colorIndex === index ? colorId : color,
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
