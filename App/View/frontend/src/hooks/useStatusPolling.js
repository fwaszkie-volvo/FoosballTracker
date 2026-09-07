import { useCallback, useEffect, useRef, useState } from "react";

import {
  API_ROUTE,
  INITIAL_STATUS,
  MODE,
  STATUS_POLL_INTERVAL_MS,
} from "../AppConstants";

export function useStatusPolling(setMode) {
  const [status, setStatus] = useState(INITIAL_STATUS);
  const [videoVersion, setVideoVersion] = useState(0);
  const wasAnalyzingRef = useRef(false);

  const refreshStatus = useCallback(async () => {
    try {
      const response = await fetch(API_ROUTE.STATUS);
      const data = await response.json();
      setStatus((current) => ({ ...data, error: data.error ?? current.error }));
      if (data.videoUrl)
        setMode((current) => (current === MODE.LIVE ? current : MODE.VIDEO));
      if (wasAnalyzingRef.current && !data.analyzing)
        setVideoVersion((current) => current + 1);
      wasAnalyzingRef.current = data.analyzing;
    } catch {
      // Keep the last known status while the backend is temporarily unavailable.
    }
  }, [setMode]);

  useEffect(() => {
    refreshStatus();
    const interval = setInterval(refreshStatus, STATUS_POLL_INTERVAL_MS);
    return () => clearInterval(interval);
  }, [refreshStatus]);

  return { status, setStatus, videoVersion, setVideoVersion, refreshStatus };
}
