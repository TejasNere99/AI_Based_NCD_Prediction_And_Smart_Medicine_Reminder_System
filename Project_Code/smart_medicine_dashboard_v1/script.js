const SLOT_COUNT = 6;
const schedule = Array.from({ length: SLOT_COUNT }, (_, i) => ({ slot: i + 1, medicine: "", time: "" }));
let history = [];

const slotRows = document.getElementById("slotRows");
const testSlot = document.getElementById("testSlot");
const historyBody = document.getElementById("historyBody");
const historyCount = document.getElementById("historyCount");
const saveMessage = document.getElementById("saveMessage");
const connectionMessage = document.getElementById("connectionMessage");

function renderSlots() {
  slotRows.innerHTML = "";
  testSlot.innerHTML = "";
  schedule.forEach(item => {
    const row = document.createElement("div");
    row.className = "slot-row";

    const slotCell = document.createElement("div");
    slotCell.className = "slot-cell slot-number";
    slotCell.textContent = `Slot ${item.slot}`;

    const medicineCell = document.createElement("div");
    medicineCell.className = "slot-cell";
    const medicineInput = document.createElement("input");
    medicineInput.type = "text";
    medicineInput.placeholder = "Medicine name";
    medicineInput.value = item.medicine;
    medicineInput.addEventListener("input", e => { item.medicine = e.target.value.trim(); });
    medicineCell.appendChild(medicineInput);

    const timeCell = document.createElement("div");
    timeCell.className = "slot-cell";
    const timeInput = document.createElement("input");
    timeInput.type = "time";
    timeInput.value = item.time;
    timeInput.addEventListener("input", e => { item.time = e.target.value; });
    timeCell.appendChild(timeInput);

    const sendCell = document.createElement("div");
    sendCell.className = "slot-cell send-cell";
    const sendButton = document.createElement("button");
    sendButton.className = "primary-btn send-btn";
    sendButton.textContent = "Send to ESP32";
    sendButton.addEventListener("click", () => sendSlotToESP32(item.slot));
    sendCell.appendChild(sendButton);

    row.append(slotCell, medicineCell, timeCell, sendCell);
    slotRows.appendChild(row);

    const option = document.createElement("option");
    option.value = item.slot;
    option.textContent = `Slot ${item.slot}`;
    testSlot.appendChild(option);
  });
}

function pad(v) { return String(v).padStart(2, "0") }
function nowParts() {
  const n = new Date();
  return {
    date: `${pad(n.getDate())}/${pad(n.getMonth() + 1)}/${n.getFullYear()}`,
    time: `${pad(n.getHours())}:${pad(n.getMinutes())}:${pad(n.getSeconds())}`
  };
}
function updateClock() {
  const p = nowParts();
  document.getElementById("currentTime").textContent = p.time;
  document.getElementById("currentDate").textContent = p.date;
}

async function testConnection() {

  const ip =
    document.getElementById("esp32IP").value.trim();

  if (!ip) {

    connectionMessage.textContent =
      "Enter the ESP32 IP address first.";

    connectionMessage.style.color =
      "#9d2424";

    return;
  }

  connectionMessage.textContent =
    "Testing...";

  connectionMessage.style.color =
    "#66717d";

  try {

    const response = await fetch(
      `http://${ip}/`,
      {
        method: "GET"
      }
    );

    const result =
      await response.text();

    if (!response.ok) {
      throw new Error(result);
    }

    connectionMessage.textContent =
      `Connected: ${result}`;

    connectionMessage.style.color =
      "#176b39";

  }

  catch (error) {

    console.error(error);

    connectionMessage.textContent =
      "Connection failed. Check IP and make sure laptop + ESP32 are on the same Wi-Fi.";

    connectionMessage.style.color =
      "#9d2424";
  }
}

async function sendSlotToESP32(slotNumber) {
  const ip = document.getElementById("esp32IP").value.trim();
  if (!ip) { alert("Enter ESP32 IP address first."); return; }
  const item = schedule[slotNumber - 1];
  if (!item.medicine || !item.time) { alert(`Enter medicine name and time for Slot ${slotNumber} first.`); return; }

  const data = new URLSearchParams();
  data.append("slot", item.slot);
  data.append("medicine", item.medicine);
  data.append("time", item.time);

  const button = [...document.querySelectorAll(".send-btn")][slotNumber - 1];
  if (button) button.textContent = "Sending...";

  try {
    const response = await fetch(`http://${ip}/api/schedule`, {
      method: "POST",
      headers: { "Content-Type": "application/x-www-form-urlencoded" },
      body: data
    });
    const result = await response.text();
    if (!response.ok) throw new Error(result);
    if (button) { button.textContent = "Sent ✓"; button.className = "secondary-btn send-btn"; }
    connectionMessage.textContent = `Slot ${slotNumber} sent successfully.`;
    connectionMessage.style.color = "#176b39";
  } catch (error) {
    if (button) button.textContent = "Send to ESP32";
    alert(`ESP32 connection failed: ${error.message}`);
  }
}

document.getElementById("testConnectionBtn").addEventListener("click", testConnection);

document.getElementById("saveAllBtn").addEventListener("click", () => {
  const configured = schedule.filter(x => x.medicine && x.time).length;
  localStorage.setItem("medicineSchedule", JSON.stringify(schedule));
  saveMessage.textContent = `${configured} of 6 slots saved locally.`;
  setTimeout(() => saveMessage.textContent = "", 3000);
});

function addHistory(status) {
  const item = schedule[Number(testSlot.value) - 1];
  const p = nowParts();
  history.unshift({
    date: p.date, scheduled: item.time || "--:--", slot: item.slot,
    medicine: item.medicine || "Unnamed Medicine", status, takenAt: status === "TAKEN" ? p.time : "--"
  });
  renderHistory();
}
function renderHistory() {
  historyCount.textContent = `${history.length} record${history.length === 1 ? "" : "s"}`;
  if (!history.length) {
    historyBody.innerHTML = '<tr><td colspan="6" class="empty">No medicine records yet.</td></tr>'; return;
  }
  historyBody.innerHTML = history.map(r => `<tr>
    <td>${r.date}</td><td>${r.scheduled}</td><td>${r.slot}</td><td>${escapeHtml(r.medicine)}</td>
    <td class="${r.status === "TAKEN" ? "status-taken" : "status-missed"}">${r.status === "TAKEN" ? "TAKEN" : "NOT TAKEN"}</td>
    <td>${r.takenAt}</td></tr>`).join("");
}
function escapeHtml(v) { return String(v).replaceAll("&", "&amp;").replaceAll("<", "&lt;").replaceAll(">", "&gt;").replaceAll('"', "&quot;").replaceAll("'", "&#039;") }
async function sendAllSlotsToESP32() {

  const esp32IP =
    document.getElementById("esp32IP").value.trim();

  if (!esp32IP) {
    alert("Enter ESP32 IP address first.");
    return;
  }

  // ============================================
  // ONLY FILLED SLOTS
  // ============================================

  const configuredSlots = schedule.filter(
    slot =>
      slot.medicine.trim() !== "" &&
      slot.time.trim() !== ""
  );

  // ============================================
  // AT LEAST ONE SLOT REQUIRED
  // ============================================

  if (configuredSlots.length === 0) {
    alert(
      "Please configure at least one medicine slot."
    );
    return;
  }

  // ============================================
  // PREPARE DATA
  // ============================================

  const data = new URLSearchParams();

  configuredSlots.forEach(slot => {

    data.append(
      `slot${slot.slot}`,
      slot.slot
    );

    data.append(
      `medicine${slot.slot}`,
      slot.medicine
    );

    data.append(
      `time${slot.slot}`,
      slot.time
    );

  });

  // ============================================
  // BUTTON
  // ============================================

  const sendAllButton =
    document.getElementById("sendAllBtn");

  sendAllButton.textContent =
    "Sending...";

  // ============================================
  // SEND TO ESP32
  // ============================================

  try {

    const response = await fetch(
      `http://${esp32IP}/api/schedule/all`,
      {
        method: "POST",

        headers: {
          "Content-Type":
            "application/x-www-form-urlencoded"
        },

        body: data
      }
    );

    const result =
      await response.text();

    if (!response.ok) {
      throw new Error(result);
    }

    // ==========================================
    // SUCCESS
    // ==========================================

    sendAllButton.textContent =
      "Sent Successfully ✓";

    document.getElementById(
      "connectionMessage"
    ).textContent =
      `${configuredSlots.length} medicine slot(s) sent to ESP32.`;

    document.getElementById(
      "connectionMessage"
    ).style.color =
      "#176b39";

    console.log(
      "Configured slots sent:",
      configuredSlots
    );

  }

  catch (error) {

    console.error(
      "Schedule send failed:",
      error
    );

    sendAllButton.textContent =
      "Send All Slots to ESP32";

    alert(
      `Failed to send schedule to ESP32.\n\n${error.message}`
    );
  }
}
async function syncTimeToESP32() {

  const esp32IP =
    document.getElementById("esp32IP").value.trim();

  if (!esp32IP) {
    console.log("ESP32 IP not entered.");
    return;
  }

  const now = new Date();

  const data = new URLSearchParams();

  data.append("hour", now.getHours());
  data.append("minute", now.getMinutes());
  data.append("second", now.getSeconds());

  data.append("day", now.getDate());
  data.append("month", now.getMonth() + 1);
  data.append("year", now.getFullYear());

  try {

    const response = await fetch(
      `http://${esp32IP}/api/time`,
      {
        method: "POST",

        headers: {
          "Content-Type":
            "application/x-www-form-urlencoded"
        },

        body: data
      }
    );

    if (!response.ok) {
      throw new Error("Time sync failed");
    }

    const result = await response.text();

    console.log("Time Sync:", result);

  }

  catch (error) {

    console.error(
      "ESP32 time synchronization failed:",
      error
    );
  }
}
let lastESP32EventKey = "";
async function getESP32Status() {

  const ip =
    document.getElementById("esp32IP").value.trim();

  if (!ip) return;

  try {

    const response =
      await fetch(
        `http://${ip}/api/status`
      );

    if (!response.ok) return;

    const data =
      await response.json();

    if (
      data.status !== "TAKEN" &&
      data.status !== "NOT_TAKEN"
    ) {
      return;
    }

    const eventKey =
      `${data.eventId ?? ""}-${data.slot}-${data.status}-${data.scheduledTime}-${data.takenTime}-${nowParts().date}`;

    if (eventKey === lastESP32EventKey) {
      return;
    }

    lastESP32EventKey = eventKey;

    addESP32History(data);

  } catch (error) {

    console.log(
      "ESP32 status unavailable"
    );
  }
}
function addESP32History(data) {

  const now = new Date();

  history.unshift({

    date:
      `${pad(now.getDate())}/` +
      `${pad(now.getMonth() + 1)}/` +
      `${now.getFullYear()}`,

    scheduled:
      data.scheduledTime || "--:--",

    slot:
      data.slot,

    medicine:
      data.medicine || "Unknown",

    status:
      data.status === "TAKEN"
        ? "TAKEN"
        : "NOT TAKEN",

    takenAt:
      data.takenTime || "--"

  });

  renderHistory();

  document.getElementById(
    "dispenserStatus"
  ).textContent =
    data.status === "TAKEN"
      ? "MEDICINE TAKEN"
      : "MEDICINE NOT TAKEN";
}
let timeSyncInterval = null;
if (!timeSyncInterval) {

  syncTimeToESP32();

  timeSyncInterval =
    setInterval(
      syncTimeToESP32,
      60000
    );
}

document.getElementById("simulateTaken").addEventListener("click", () => { addHistory("TAKEN"); document.getElementById("dispenserStatus").textContent = "MEDICINE TAKEN"; });
document.getElementById("simulateMissed").addEventListener("click", () => { addHistory("NOT TAKEN"); document.getElementById("dispenserStatus").textContent = "MEDICINE NOT TAKEN"; });
document.getElementById("clearHistory").addEventListener("click", () => { history = []; renderHistory(); document.getElementById("dispenserStatus").textContent = "WAITING"; });
document
  .getElementById("sendAllBtn")
  .addEventListener(
    "click",
    sendAllSlotsToESP32
  );
document
  .getElementById("syncTimeBtn")
  .addEventListener(
    "click",
    syncTimeToESP32
  );

const saved = localStorage.getItem("medicineSchedule");
if (saved) { try { JSON.parse(saved).forEach((x, i) => { if (schedule[i]) { schedule[i].medicine = x.medicine || ""; schedule[i].time = x.time || ""; } }) } catch (_) { } }

renderSlots();
renderHistory();
updateClock();

setInterval(updateClock, 1000);

// ESP32 status polling
setInterval(
  getESP32Status,
  1000
);