import type { MetricsPayload, ProcessTuple } from "./types";

const enabledFields = new Set<string>();

let initialized = false;

const PROCESS_FIELDS = [
  "cpu_percent",
  "memory_percent",
  "state"
];

const SORT_FIELDS = [
  "pid",
  "cpu_percent",
  "memory_percent"
];

const PROCESS_FIELD_LABELS: Record<string, string> = {
  cpu_percent: "CPU",
  memory_percent: "Memory",
  state: "State"
};

let latestData: MetricsPayload | null = null;

export function renderTable(data: MetricsPayload) {
  latestData = data;

  if (!initialized) {
    initTable();
    initialized = true;
  }

  const sortSelect = document.getElementById(
    "sort-select"
  ) as HTMLSelectElement;

  const sortField = sortSelect.value;
  const filterInput = document.getElementById(
    "table-filter"
  ) as HTMLInputElement;
  const filterText = filterInput.value.trim().toLowerCase();

  let rows = normalizeProcessRows(data.processes_data as unknown);

  if (filterText) {
    rows = rows.filter(([pid, proc]) => {
      const rowText = [
        pid.toString(),
        proc.name ?? ""
      ].join(" ").toLowerCase();

      return rowText.includes(filterText);
    });
  }

  if (sortField === "pid") {
    rows.sort((a, b) => a[0] - b[0]);
  } else if (SORT_FIELDS.includes(sortField)) {
    rows.sort((a, b) => {
      return (
        b[1][sortField as keyof typeof b[1]] as number
      ) - (
        a[1][sortField as keyof typeof a[1]] as number
      );
    });
  }

  const tbody = document.getElementById("table-body")!;

  const processNames = new Map<number, string>();

  if (Array.isArray(data.processes_meta)) {
    for (const meta of data.processes_meta) {
      processNames.set(meta.pid, meta.name);
    }
  }

  tbody.innerHTML = rows
    .map(([pid, proc]) => {
      return `
        <tr>
          <td class="cell cell--numeric">${pid}</td>
          <td class="cell cell--string">${processNames.get(pid) ?? proc.name ?? "unknown"}</td>

          ${PROCESS_FIELDS.map((field) => {

            if (!enabledFields.has(field)) {
              return "";
            }

            const value = proc[field as keyof typeof proc];
            const cellClass = field === "state" ? "cell--string" : "cell--numeric";

            return `
              <td class="cell ${cellClass}">${formatProcessValue(field, value)}</td>
            `;
          }).join("")}

        </tr>
      `;
    })
    .join("");
}

function normalizeProcessRows(processes: unknown): ProcessTuple[] {
  if (Array.isArray(processes)) {
    return processes as ProcessTuple[];
  }

  if (!processes || typeof processes !== "object") {
    return [];
  }

  return Object.entries(processes as Record<string, ProcessTuple[1]>).map(
    ([pid, proc]) => [Number(pid), proc]
  );
}

function initTable() {
  const toggles = document.getElementById("toggles")!;
  const sortSelect = document.getElementById(
    "sort-select"
  ) as HTMLSelectElement;
  const filterInput = document.getElementById(
    "table-filter"
  ) as HTMLInputElement;

  enabledFields.add("cpu_percent");
  enabledFields.add("memory_percent");

  PROCESS_FIELDS.forEach((field) => {
    const btn = document.createElement("button");

    btn.className = "toggle-btn";
    btn.textContent = PROCESS_FIELD_LABELS[field] ?? field;

    if (enabledFields.has(field)) {
      btn.classList.add("active");
    }

    btn.onclick = () => {
      if (enabledFields.has(field)) {
        enabledFields.delete(field);
        btn.classList.remove("active");
      } else {
        enabledFields.add(field);
        btn.classList.add("active");
      }

      rebuildHead();
      rebuildSortSelect();
    };

    toggles.appendChild(btn);
  });

  rebuildHead();
  rebuildSortSelect();

  filterInput.addEventListener("input", () => {
    if (latestData) {
      renderTable(latestData);
    }
  });

  sortSelect.addEventListener("change", () => {
    if (latestData) {
      renderTable(latestData);
    }
  });
}

function rebuildHead() {
  const head = document.getElementById("table-head")!;

  head.innerHTML = "<th>PID</th><th>Name</th>";

  PROCESS_FIELDS.forEach((field) => {
    if (!enabledFields.has(field)) {
      return;
    }

    head.innerHTML += `<th>${PROCESS_FIELD_LABELS[field] ?? field}</th>`;
  });
}

function rebuildSortSelect() {
  const sortSelect = document.getElementById(
    "sort-select"
  ) as HTMLSelectElement;

  const previousValue = sortSelect.value;
  sortSelect.innerHTML = "";

  const availableSortFields = [
    "pid",
    ...SORT_FIELDS.filter((field) => enabledFields.has(field))
  ];

  availableSortFields.forEach((field) => {

    const opt = document.createElement("option");

    opt.value = field;
    opt.textContent = field === "pid" ? "PID" : PROCESS_FIELD_LABELS[field] ?? field;

    sortSelect.appendChild(opt);
  });

  if (availableSortFields.includes(previousValue)) {
    sortSelect.value = previousValue;
  } else {
    sortSelect.value = "pid";
  }
}

function formatProcessValue(field: string, value: unknown): string {
  if (field === "cpu_percent" || field === "memory_percent") {
    const numericValue = typeof value === "number" ? value : Number(value);

    if (Number.isFinite(numericValue)) {
      return formatPercent(numericValue);
    }
  }

  return String(value ?? "");
}

function formatPercent(value: number): string {
  if (value > 0 && value < 1) {
    return "<1%";
  }

  return `${value.toFixed(2)}%`;
}