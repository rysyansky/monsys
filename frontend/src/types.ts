export interface CpuUsage {
  idle_usage: number;
  system_usage: number;
  total_usage: number;
  user_usage: number;
}

export interface DiskData {
  device: string;
  read_bytes_per_sec: number;
  write_bytes_per_sec: number;
  utilization_percent: number;
}

export interface MemoryData {
  available_bytes: number;
  total_bytes: number;
  swap_total_bytes: number;
  swap_used_bytes: number;
}

export interface NetworkData {
  interface: string;
  rx_bytes_per_sec: number;
  tx_bytes_per_sec: number;
  rx_packets_per_sec: number;
  tx_packets_per_sec: number;
}

export interface ProcessData {
  cpu_percent: number;
  memory_percent: number;
  state: string;

  name?: string;
}

export type ProcessTuple = [number, ProcessData];

export interface ProcessMeta {
  pid: number;
  name: string;
}

export interface MetricsPayload {
  timestamp_ms?: number;

  all_cpu_percent: CpuUsage;
  cpu_cores: CpuUsage[];

  disks: DiskData[];
  memory: MemoryData;
  network: NetworkData[];

  uptime_seconds: number;

  processes_meta: ProcessMeta[];
  processes_data: ProcessTuple[];
}

export interface BackendMetricsPayload {
  timestamp_ms: number;

  all_cpu_percent: CpuUsage;
  cpu_cores: CpuUsage[];

  disks: DiskData[];
  memory: MemoryData;
  network: NetworkData[];

  uptime_seconds: number;

  processes_data: Record<string, ProcessData>;
}