# 📁 UDP File Transfer Application

This project was developed for the course **Redes de Computadores (Computer Networks)** during the academic year **2022/2023**,  
**Bachelor in Electrical and Computer Engineering – University of Coimbra**.

---

## 🎯 Objective

Implement a simple **file transfer application over UDP**, assuming local network communication with no reliability requirements. The client reads a text file and sends it to the server, which receives the packets and writes the data to a new file.

---

## 📦 Key Specifications

- Uses **UDP sockets**
- Maximum datagram size: **500 bytes**
- Port: **9876**
- Ensures **ordered delivery** using a sequence number per packet
- Implements **basic acknowledgment mechanism**
- Ignores out-of-order packets and resends last ACK if needed

---

## ⚙️ How it works

### 👨‍💻 Client:
- Opens a file (`file1.txt`)
- Splits content into 500-byte chunks
- Sends each chunk via `sendto()`, including a sequence number
- Waits for ACK before sending next
- Retransmits if no ACK is received

### 🖥️ Server:
- Listens on port 9876
- Receives packets via `recvfrom()`
- Checks sequence number
- If expected, writes data and sends ACK
- If not, resends previous ACK

---

## 📁 Files

- `client_udp.c` – Client code
- `server_udp.c` – Server code
- `file1.txt` – Sample input file (Shakespeare poem)
- `RC_Project.pdf` – Official assignment description
- `Relatorio_RC_Proj.pdf` – Project report with design, diagrams and results

---

## 👨‍🎓 Author

- **Gonçalo Tavares Bastos** – nº 2020238997  
📅 Submitted: May 2023
