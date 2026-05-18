import "./style.css";

import "./ws";

import { renderMetrics } from "./metrics";
import { renderTable } from "./table";

window.addEventListener("metrics-update", (e: any) => {
  renderMetrics(e.detail);
  renderTable(e.detail);
});