# TCP File Transfer (Client-Server) in C

This project implements a basic client-server file transfer system over TCP sockets using the C programming language. The client requests a file from the server, and the server responds by sending the file line-by-line using a simple custom packet structure.

> Developed as part of a Computer Networks course project.

---

## 🔧 Features

- Custom TCP-based communication protocol  
- Graceful handling of missing files  
- Packet-based transfer with headers (count + sequence number)  
- Server supports multiple client requests (sequentially)  
- Sample test file included (`sample.txt`)

---

## 📁 Project Structure

```
tcp-file-transfer/
├── client/
│   ├── TCPEchoClient.c        # TCP client source
│   └── TCPEchoClient          # Compiled binary (via Makefile)
├── server/
│   ├── TCPEchoServer.c        # TCP server source
│   ├── HandleTCPClient.c      # File transfer logic
│   ├── DieWithError.c         # Error utility
│   ├── sample.txt             # Sample file for testing
│   └── TCPEchoServer          # Compiled binary (via Makefile)
├── Makefile                   # Build system
├── .gitignore
├── out.txt                    # Client output (gitignored)
└── README.md
```

---

## ⚙️ Compilation

Use the included `Makefile` to build both client and server:

```bash
make
```

- Client binary: `client/TCPEchoClient`
- Server binary: `server/TCPEchoServer`

To clean up compiled binaries:

```bash
make clean
```

---

## 🚀 Usage

### Start the Server

```bash
./server/TCPEchoServer
```

### Run the Client (in a separate terminal)

```bash
./client/TCPEchoClient
```

You’ll be prompted to enter a file name. Try:

```
sample.txt
```

If the file exists on the server, it will be sent and saved to `out.txt` in the client directory. The client will print the number of packets and total bytes received.

---

## 📦 Protocol Format

Each packet from the server to the client contains:
- 2 bytes: `count` (number of bytes of data)
- 2 bytes: `seqNum` (sequence number)
- N bytes: data (up to 80 bytes)

An End-of-Transmission (EOT) packet is indicated when `count == 0`.

---

## 🧪 Example Output

```
Enter file name: sample.txt
sample.txt exists.
sample.txt received.
Number of packets received: 4
Total bytes received: 124
```

---

## 📄 License

This project is open source and available under the [MIT License](LICENSE).

---

## 👨‍💻 Author

Developed by [Amena2026](https://github.com/Amena2026) — Computer Science student @ University of Delaware.
