import type { MetricsPayload } from "./types";

function formatPercent(value: number): string {
  if (value > 0 && value < 1) {
    return "<1%";
  }

  return `${value.toFixed(2)}%`;
}

function formatBytes(v: number): string {
  if (v >= 1024 * 1024 * 1024) {
    return (v / 1024 / 1024 / 1024).toFixed(2) + " GB";
  }

  if (v >= 1024 * 1024) {
    return (v / 1024 / 1024).toFixed(2) + " MB";
  }

  if (v >= 1024) {
    return (v / 1024).toFixed(2) + " KB";
  }

  return `${v} B`;
}

function formatUptime(rawUptime: number): string {
  const seconds = Math.floor(
    rawUptime > 10_000_000_000 ? rawUptime / 1000 : rawUptime
  );
  const h = Math.floor(seconds / 3600);
  const m = Math.floor((seconds % 3600) / 60);
  const s = seconds % 60;

  return [
    h.toString().padStart(2, "0"),
    m.toString().padStart(2, "0"),
    s.toString().padStart(2, "0")
  ].join(":");
}

export function renderMetrics(data: MetricsPayload) {
  renderCpu(data);
  renderDisk(data);
  renderMemory(data);
  renderNetwork(data);

  const uptime = document.getElementById("uptime")!;
  uptime.textContent = formatUptime(data.uptime_seconds);
}

function renderCpu(data: MetricsPayload) {
  const modeSelect = document.getElementById("cpu-mode") as HTMLSelectElement;
  const coreSelect = document.getElementById("cpu-core") as HTMLSelectElement;
  const valueLabel = document.getElementById("cpu-value") as HTMLElement;

  if (coreSelect.children.length === 0) {
    coreSelect.innerHTML = `
      <option value="-1">All CPU</option>
      ${data.cpu_cores.map((_, i) =>
        `<option value="${i}">Core ${i}</option>`
      ).join("")}
    `;
  }

  const mode = modeSelect.value || "total_usage";
  const core = Number(coreSelect.value || -1);

  const source =
    core === -1
      ? data.all_cpu_percent
      : data.cpu_cores[core];

  const value = source[mode as keyof typeof source];

  valueLabel.textContent = formatPercent(Number(value));
}

function renderDisk(data: MetricsPayload) {
  const select = document.getElementById("disk-select") as HTMLSelectElement;

  if (select.children.length === 0) {
    select.innerHTML = data.disks
      .map((d, i) =>
        `<option value="${i}">${d.device}</option>`
      )
      .join("");
  }

  const disk = data.disks[Number(select.value || 0)];

  document.getElementById("disk-util")!.textContent =
    `Util: ${formatPercent(disk.utilization_percent)}`;

  document.getElementById("disk-read")!.textContent =
    `Read: ${formatBytes(disk.read_bytes_per_sec)}/s`;

  document.getElementById("disk-write")!.textContent =
    `Write: ${formatBytes(disk.write_bytes_per_sec)}/s`;

  const readLamp = document.querySelector(".read")!;
  const writeLamp = document.querySelector(".write")!;

  readLamp.classList.toggle(
    "active",
    disk.read_bytes_per_sec > 0
  );

  writeLamp.classList.toggle(
    "active",
    disk.write_bytes_per_sec > 0
  );
}

function renderMemory(data: MetricsPayload) {
  const used =
    data.memory.total_bytes -
    data.memory.available_bytes;

  document.getElementById("memory-used")!.textContent =
    `${formatBytes(used)} / ${formatBytes(data.memory.total_bytes)}`;

  document.getElementById("swap-used")!.textContent =
    `${formatBytes(data.memory.swap_used_bytes)} / ${formatBytes(data.memory.swap_total_bytes)}`;
}

function renderNetwork(data: MetricsPayload) {
  const select = document.getElementById("net-select") as HTMLSelectElement;

  if (select.children.length === 0) {
    select.innerHTML = data.network
      .map((n, i) =>
        `<option value="${i}">${n.interface}</option>`
      )
      .join("");
  }

  const net = data.network[Number(select.value || 0)];

  document.getElementById("net-rx")!.textContent =
    `RX: ${formatBytes(net.rx_bytes_per_sec)}/s`;

  document.getElementById("net-tx")!.textContent =
    `TX: ${formatBytes(net.tx_bytes_per_sec)}/s`;
}