"use strict";
// Lab14: BME280 (MQTT, Flow 2) vs Chennai OpenWeather (Flow 1) comparison dashboard.
// Builds everything inside Flow 2. Run while Node-RED is running:  node lab14-dashboard.cjs
// Safe to re-run: it removes its own previous nodes before adding new ones.

const endpoint = "http://127.0.0.1:1880/flows";
const dashboard = "http://localhost:1880/dashboard/lab14";
const headers = { "Node-RED-API-Version": "v2", "Content-Type": "application/json" };

const FLOW1 = "7893c5f92ffa2983", FLOW2 = "ad2085d72e1ae047";
const HTTP = "78ea8984f1eab47e", MQTT = "9e533464425ece2e";

// Runs inside the Node-RED function node. Outputs: 1 summary, 2 temperature, 3 humidity, 4 pressure.
function combine(msg, context) {
  const now = Date.now();
  let sensor = context.get("sensor"), weather = context.get("weather");
  let fresh = null; // which source produced new chart points
  let p = msg.payload;

  // Plant watering rule (humidity from BME280): dry air dries soil faster, so it needs more water.
  const waterFor = h => {
    if (h > 70) return { level: "NO WATER", reason: "Humidity " + h + " % is above 70 %: air is moist" };
    if (h >= 65) return { level: "LITTLE WATER", reason: "Humidity " + h + " % is between 65 and 70 %" };
    return { level: "MORE WATER", reason: "Humidity " + h + " % is below 65 %: air is dry" };
  };

  if (msg.topic === "stm32/leela" || msg.topic === "stm32/gk") {
    if (msg.retain) return null; // ignore old retained message from broker
    if (Buffer.isBuffer(p)) p = p.toString();
    if (msg.topic === "stm32/gk") {
      // Friend's board sends text: "Temperature: 24.17 C, Pressure: 1012.53 hPa, Humidity: 58.04 %"
      const m = String(p).match(/Temperature:\s*(-?[\d.]+).*Pressure:\s*([\d.]+).*Humidity:\s*([\d.]+)/);
      if (!m) return null;
      p = { status: "success", temperature: +m[1], pressure: +m[2], humidity: +m[3] };
    }
    if (typeof p === "string") { try { p = JSON.parse(p); } catch (e) { return null; } } // greeting text
    if (!p || p.status !== "success" || ![p.temperature, p.humidity, p.pressure].every(Number.isFinite)) return null;
    sensor = { t: p.temperature, h: p.humidity, p: p.pressure, at: now, from: msg.topic };
    context.set("sensor", sensor);
    // BME280 publishes every 2 s; plot one point per minute to keep 12 h charts light.
    if (now - (context.get("lastPlot") || 0) >= 60000) { context.set("lastPlot", now); fresh = "sensor"; }
  } else if (msg.statusCode !== undefined) {
    const m = p && p.main;
    if (Number(msg.statusCode) === 200 && m && [m.temp, m.humidity, m.pressure].every(Number.isFinite)) {
      weather = { t: m.temp, h: m.humidity, p: m.pressure, obs: p.dt * 1000, at: now };
      context.set("weather", weather);
      context.set("error", "");
      fresh = "weather";
    } else {
      context.set("error", "Weather request failed (HTTP " + msg.statusCode + "). Check Flow 1.");
    }
  }

  const age = t => { const s = Math.round((now - t) / 1000); return s < 120 ? s + " s ago" : Math.round(s / 60) + " min ago"; };
  const fmt = (v, u) => v == null ? "—" : (Math.round(v * 10) / 10) + u;
  const diff = (a, b, u) => (a == null || b == null) ? "—" : ((a - b) >= 0 ? "+" : "") + (Math.round((a - b) * 10) / 10) + u;
  const s = sensor || {}, w = weather || {};
  const summary = { payload: {
    sensor: sensor ? "Received " + age(sensor.at) + " from " + sensor.from + (now - sensor.at > 120000 ? " (no new data)" : "") : "Waiting for MQTT data on stm32/leela or stm32/gk",
    weather: weather ? "Fetched " + age(weather.at) + ", observed " + age(weather.obs) : "Waiting for Flow 1 weather",
    error: context.get("error") || "",
    water: sensor ? waterFor(Math.round(s.h * 10) / 10) : null,
    rows: [
      ["Temperature", fmt(s.t, " °C"), fmt(w.t, " °C"), diff(s.t, w.t, " °C")],
      ["Humidity", fmt(s.h, " %"), fmt(w.h, " %"), diff(s.h, w.h, " %")],
      ["Pressure", fmt(s.p, " hPa"), fmt(w.p, " hPa"), diff(s.p, w.p, " hPa")]
    ]
  } };

  if (!fresh) return [summary, null, null, null];
  // Each chart gets both series; the latest known value of the other source is repeated so lines stay aligned.
  const pts = k => [sensor && { topic: "BME280", payload: sensor[k] }, weather && { topic: "Chennai", payload: weather[k] }].filter(Boolean);
  return [summary, pts("t"), pts("h"), pts("p")];
}

const summaryTemplate = `<template><div class="l14">
<p><b>BME280:</b> {{msg.payload?.sensor}}<br><b>Chennai:</b> {{msg.payload?.weather}}</p>
<p v-if="msg.payload?.error" class="err">{{msg.payload.error}}</p>
<table><thead><tr><th></th><th>BME280</th><th>Chennai</th><th>Difference</th></tr></thead>
<tbody><tr v-for="r in msg.payload?.rows || []" :key="r[0]"><th>{{r[0]}}</th><td>{{r[1]}}</td><td>{{r[2]}}</td><td>{{r[3]}}</td></tr></tbody></table>
<p class="note">Difference = BME280 minus Chennai. Chennai pressure is sea-level pressure from OpenWeather.</p>
<div class="fan"><b>Plant watering (from BME280 humidity):</b>
<span v-if="msg.payload?.water"> <span class="spd">{{msg.payload.water.level}}</span>: {{msg.payload.water.reason}}</span>
<span v-else> waiting for sensor data</span></div>
</div></template>
<style>.l14 .fan{margin-top:12px;padding:10px;border:1px solid #ccc}.l14 .fan.test{border-style:dashed;color:#555}.l14 .spd{font-weight:bold;font-size:18px}
.l14 table{width:100%;border-collapse:collapse}.l14 th,.l14 td{text-align:left;padding:8px;border-bottom:1px solid #ddd}
.l14 .note{font-size:12px;color:#777}.l14 .err{color:#b00}</style>`;

function integrate(flows) {
  const get = id => flows.find(n => n.id === id);
  const http = get(HTTP), mqtt = get(MQTT);
  if (http?.type !== "http request" || mqtt?.type !== "mqtt in" || mqtt.topic !== "stm32/leela" || !get(FLOW2))
    throw Error("Expected Flow 1 http request / Flow 2 mqtt in nodes not found.");

  // Remove earlier Lab14 nodes (old separate tab, page, groups) but keep Flow 1 weather link out + 10 min poll.
  const keep = new Set(["lab14_weather_out", "lab14_poll"]);
  const old = new Set(flows.filter(n => !keep.has(n.id) && (n.id.startsWith("lab14_") || n.z === "lab14_compare_tab")).map(n => n.id));
  for (let i = flows.length - 1; i >= 0; i--) if (old.has(flows[i].id)) flows.splice(i, 1);
  for (const n of flows) if (n.wires) n.wires = n.wires.map(w => w.filter(id => !old.has(id)));

  const add = n => flows.push(n);
  const z = FLOW2, base = get("c6040a73c538f5ff"), group = get("0688409456d45fc4");

  // Flow 1 -> Flow 2 link for weather data.
  let out = get("lab14_weather_out");
  if (!out) { out = { id: "lab14_weather_out", type: "link out", z: FLOW1, name: "Weather to Flow 2", mode: "link", x: http.x + 160, y: http.y + 60, wires: [] }; add(out); }
  out.links = ["lab14_weather_in"];
  if (!http.wires[0].includes(out.id)) http.wires[0].push(out.id);
  if (!get("lab14_poll")) {
    const inj = get("06df65e09ccc1e2a");
    add({ ...JSON.parse(JSON.stringify(inj)), id: "lab14_poll", name: "Refresh Chennai every 10 min", once: true, onceDelay: 2, repeat: "600", y: inj.y + 100 });
  }

  // Flow 2 nodes.
  add({ id: "lab14_weather_in", type: "link in", z, name: "Weather from Flow 1", links: ["lab14_weather_out"], x: 230, y: 460, wires: [["lab14_combine"]] });
  mqtt.wires[0].push("lab14_combine");
  // Friend's board (text payload) on the same broker.
  add({ id: "lab14_mqtt_gk", type: "mqtt in", z, name: "Friend BME280 (stm32/gk)", topic: "stm32/gk", qos: "0", datatype: "utf8",
    broker: mqtt.broker, nl: false, rap: true, rh: 0, inputs: 0, x: 230, y: 320, wires: [["lab14_combine", "lab14_gk_debug"]] });
  add({ id: "lab14_gk_debug", type: "debug", z, name: "debug stm32/gk", active: true, tosidebar: true, console: false,
    tostatus: false, complete: "payload", targetType: "msg", x: 470, y: 280, wires: [] });
  add({ id: "lab14_tick", type: "inject", z, name: "Update ages 30 s", props: [{ p: "topic", vt: "str" }], topic: "tick", repeat: "30", once: true, onceDelay: 1, x: 230, y: 520, wires: [["lab14_combine"]] });
  add({ id: "lab14_combine", type: "function", z, name: "Combine BME280 + Chennai", func: "return (" + combine.toString() + ")(msg, context);",
    outputs: 4, outputLabels: ["summary", "temperature", "humidity", "pressure"], x: 520, y: 380,
    wires: [["lab14_summary"], ["lab14_t1", "lab14_t12"], ["lab14_h1", "lab14_h12"], ["lab14_p1", "lab14_p12"]] });

  // Dashboard page and groups.
  add({ ...base, id: "lab14_page", name: "Lab14 BME280 vs Chennai", path: "/lab14", order: 1 });
  // Lab14 first so /dashboard opens it by default; push other pages after it.
  for (const n of flows) if (n.type === "ui-page" && n.id !== "lab14_page" && n.order <= 1) n.order = 2;
  if (get("c6040a73c538f5ff")) get("c6040a73c538f5ff").order = 3;
  const grp = (id, name, order) => add({ ...group, id, page: "lab14_page", name, width: 12, order, showTitle: true });
  grp("lab14_g_now", "Latest values", 1);
  grp("lab14_g_1h", "Last 1 hour", 2);
  grp("lab14_g_12h", "Last 12 hours", 3);

  add({ id: "lab14_summary", type: "ui-template", z, group: "lab14_g_now", name: "Comparison table", order: 1, width: 12, height: 0,
    templateScope: "local", resendOnRefresh: true, passthru: false, storeOutMessages: true, format: summaryTemplate, x: 800, y: 300, wires: [[]] });

  const chart = (id, g, label, unit, hours, order, y) => add({ id, type: "ui-chart", z, group: g, name: label + " " + hours + "h",
    label: label + " (" + unit + ")", order, chartType: "line", category: "topic", categoryType: "msg",
    xAxisLabel: "", xAxisProperty: "", xAxisPropertyType: "timestamp", xAxisType: "time", xAxisFormat: "", xAxisFormatType: "auto",
    xmin: "", xmax: "", yAxisLabel: unit, yAxisProperty: "payload", yAxisPropertyType: "msg", ymin: "", ymax: "", bins: 10,
    action: "append", stackSeries: false, pointShape: "circle", pointRadius: 2, showLegend: true,
    removeOlder: hours, removeOlderUnit: "3600", removeOlderPoints: "",
    colors: ["#333333", "#1f77b4", "#999999", "#bbbbbb", "#cccccc", "#dddddd", "#eeeeee", "#aaaaaa", "#888888"],
    textColor: ["#666666"], textColorDefault: true, gridColor: ["#e5e5e5"], gridColorDefault: true,
    width: 4, height: 6, className: "", interpolation: "linear", x: 820, y, wires: [[]] });
  chart("lab14_t1", "lab14_g_1h", "Temperature", "°C", 1, 1, 360);
  chart("lab14_h1", "lab14_g_1h", "Humidity", "%", 1, 2, 420);
  chart("lab14_p1", "lab14_g_1h", "Pressure", "hPa", 1, 3, 480);
  chart("lab14_t12", "lab14_g_12h", "Temperature", "°C", 12, 1, 540);
  chart("lab14_h12", "lab14_g_12h", "Humidity", "%", 12, 2, 600);
  chart("lab14_p12", "lab14_g_12h", "Pressure", "hPa", 12, 3, 660);
  return flows;
}

async function install() {
  const res = await fetch(endpoint, { headers });
  if (!res.ok) throw Error("Cannot read Node-RED flows: HTTP " + res.status);
  const current = await res.json();
  integrate(current.flows);
  const dep = await fetch(endpoint, { method: "POST", headers: { ...headers, "Node-RED-Deployment-Type": "flows" }, body: JSON.stringify(current) });
  if (!dep.ok) throw Error("Deployment failed: HTTP " + dep.status + " " + await dep.text());
  console.log("Deployed. Open " + dashboard);
}
if (require.main === module) install().catch(e => { console.error(e.message); process.exitCode = 1; });
module.exports = { integrate, combine };
