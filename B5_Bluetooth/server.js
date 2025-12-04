const express = require("express");
const SerialPort = require("serialport").SerialPort;
const cors = require("cors");

const app = express();
app.use(express.json());
app.use(cors());

let port;
let isOpen = false;

// ==== CONNECT ARDUINO ====
app.post("/connect", (req, res) => {
  if (isOpen) return res.json({ status: "OK" });

  port = new SerialPort({
    path: "COM2",
    baudRate: 9600,
  });

  port.on("open", () => {
    isOpen = true;
    console.log("COM2 opened!");
  });

  return res.json({ status: "OK" });
});

// ==== SEND LED COMMAND ====
app.post("/led", (req, res) => {
  const { command } = req.body;
  if (!isOpen) return res.json({ status: "ERROR", msg: "Port not open" });

  port.write(command);
  console.log("Sent:", command);
  res.json({ status: "OK" });
});

// ==== DISCONNECT ====
app.post("/disconnect", (req, res) => {
  if (port && isOpen) {
    port.close();
    isOpen = false;
    console.log("COM2 closed!");
  }
  res.json({ status: "OK" });
});

app.use(express.static("public"));

app.listen(3000, () => console.log("Server running at http://localhost:3000"));
