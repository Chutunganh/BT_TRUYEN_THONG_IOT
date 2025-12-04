// ==== DOM ELEMENTS ====
const connectBtn = document.getElementById("connectBtn");
const disconnectBtn = document.getElementById("disconnectBtn");
const onBtn = document.getElementById("onBtn");
const offBtn = document.getElementById("offBtn");

const statusLabel = document.getElementById("status");
const connectionInfo = document.getElementById("connectionInfo");

let isConnected = false;

// ==== UPDATE UI ====
function updateUI(connected) {
  isConnected = connected;

  if (connected) {
    statusLabel.textContent = "Status: Connected";
    statusLabel.classList.remove("hidden");

    onBtn.disabled = false;
    offBtn.disabled = false;

    disconnectBtn.classList.remove("hidden");
    connectionInfo.classList.remove("hidden");

    connectBtn.disabled = true;
  } else {
    statusLabel.textContent = "Status: Disconnected";

    onBtn.disabled = true;
    offBtn.disabled = true;

    disconnectBtn.classList.add("hidden");
    connectionInfo.classList.add("hidden");

    connectBtn.disabled = false;
  }
}

// ==== CONNECT BUTTON ====
connectBtn.addEventListener("click", async () => {
  try {
    // Gửi yêu cầu kết nối đến backend Node.js
    const res = await fetch("http://localhost:3000/connect", {
      method: "POST",
    });

    const data = await res.json();

    if (data.status === "OK") {
      updateUI(true);
    }
  } catch (err) {
    alert("Không thể kết nối với Arduino!");
    console.error(err);
  }
});

// ==== SEND LED COMMAND ====
async function sendCommand(cmd) {
  if (!isConnected) return;

  try {
    await fetch("http://localhost:3000/led", {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify({ command: cmd }),
    });
  } catch (err) {
    console.error("Lỗi gửi lệnh:", err);
  }
}

onBtn.addEventListener("click", () => {
  sendCommand("on");
  showToast("Bạn vừa nhấn: ON");
});

offBtn.addEventListener("click", () => {
  sendCommand("off");
  showToast("Bạn vừa nhấn: OFF");
});

// ==== DISCONNECT BUTTON ====
disconnectBtn.addEventListener("click", async () => {
  try {
    await fetch("http://localhost:3000/disconnect", {
      method: "POST",
    });

    updateUI(false);
  } catch (err) {
    console.error("Lỗi disconnect:", err);
  }
});

// ===== Toast Notification =====
function showToast(message) {
  const toast = document.getElementById("toast");
  toast.textContent = message;
  toast.classList.add("show");

  setTimeout(() => {
    toast.classList.remove("show");
  }, 2500); // toast tự ẩn sau 2.5 giây
}
