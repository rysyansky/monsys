import type {
  BackendMetricsPayload,
  MetricsPayload,
  ProcessTuple
} from "./types";

export let latestPayload: MetricsPayload | null = null;

const wsUrl = import.meta.env.PROD
  ? new URL("/ws", window.location.origin).toString()
  : "ws://localhost:8080/ws";

const ws = new WebSocket(wsUrl);

ws.onopen = () => {
  console.log("ws connected");
};

ws.onmessage = (event) => {
  const raw = JSON.parse(event.data) as
    | MetricsPayload
    | BackendMetricsPayload;

  latestPayload = normalizePayload(raw);

  window.dispatchEvent(
    new CustomEvent("metrics-update", {
      detail: latestPayload
    })
  );
};

ws.onerror = console.error;

function normalizePayload(
  raw: MetricsPayload | BackendMetricsPayload
): MetricsPayload {
  const incomingProcesses = (raw as { processes_data?: unknown }).processes_data;

  if (Array.isArray(incomingProcesses)) {
    const typed = raw as MetricsPayload;

    return {
      ...typed,
      processes_meta:
        typed.processes_meta ??
        typed.processes_data.map(([pid, proc]) => ({
          pid,
          name: proc.name ?? "unknown"
        }))
    };
  }

  const processMap =
    ((raw as BackendMetricsPayload).processes_data ?? {}) as Record<string, MetricsPayload["processes_data"][number][1]>;

  const processes_data: ProcessTuple[] = Object.entries(
    processMap
  ).map(([pid, proc]) => [Number(pid), proc]);

  const processes_meta = processes_data.map(([pid, proc]) => ({
    pid,
    name: proc.name ?? "unknown"
  }));

  return {
    timestamp_ms: (raw as BackendMetricsPayload).timestamp_ms,
    all_cpu_percent: raw.all_cpu_percent,
    cpu_cores: raw.cpu_cores,
    disks: raw.disks,
    memory: raw.memory,
    network: raw.network,
    uptime_seconds: raw.uptime_seconds,
    processes_data,
    processes_meta
  };
}