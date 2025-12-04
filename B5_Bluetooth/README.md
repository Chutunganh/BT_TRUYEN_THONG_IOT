# Bluetooth-like LED Controller

### Arduino + Proteus + Node.js + WebApp

---

## Giới thiệu

Bài thực hành mô phỏng hệ thống điều khiển LED qua "Bluetooth" bằng cách kết hợp:

- **Arduino UNO** (mô phỏng trong Proteus)
- **Module HC-05** (UART Bluetooth Classic)
- **Cổng COM ảo (Virtual Serial Port Driver)**
- **Backend Node.js** để giao tiếp Serial
- **WebApp** để gửi lệnh ON/OFF
- **Toast notification** và **hiệu ứng nút nhấn**

Hệ thống cho phép sinh viên hiểu chuỗi truyền thông trong IoT:

WebApp → Node.js Backend → Serial COM2 → COM1 → HC-05 → Arduino → LED

---

## Hướng dẫn cài đặt & chạy

### **Bước 1 — Cài dependencies Node.js**

```bash
npm install
npm install express serialport cors
```

### **Bước 2 — Chạy Backend**

```bash
node server.js
```

### **Bước 3 — Truy cập WebApp**

Mở trình duyệt: http://localhost:3000
