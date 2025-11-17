# Implementation details — HTTP Client/Server demo

Tài liệu này mô tả chi tiết cách chương trình hoạt động, cấu trúc dự án, những chỉnh sửa đã thực hiện và cách build/run trên Windows.

## Mục tiêu
- Triển khai server HTTP đơn giản phục vụ file ảnh từ một thư mục `images/`.
- Triển khai client đọc danh sách file từ `filelist.txt` và tải từng file bằng HTTP GET.
- Giao diện CLI để điều khiển server (`start [port]`, `stop`) và client (`load <list_file> <server_ip> <port>`).

## Cấu trúc chính
- `HTTPv1/` — server và core network classes
  - `HttpServer.*` — lớp server kế thừa `TCPServer`, chấp nhận kết nối và bắt đầu session mới.
  - `HttpHandler.*` — xử lý các yêu cầu HTTP đơn giản (chỉ GET) và trả về nội dung file (binary) hoặc mã 404/403/400.
  - `tcpserver.*`, `tcpserversocket.*`, `tcpsocket.*` — lớp socket/connection cơ bản.
  - `server_cli.*` — giao diện dòng lệnh để start/stop server.

- `HTTPv1_client/` — client để gửi GET và lưu file
  - `ResourceDownload.*` — đọc danh sách file (mỗi dòng 1 tài nguyên), thực hiện GET và lưu file.
  - `client_cli.*` — giao diện dòng lệnh client: `load` command.

- `images/` — thư mục nơi đặt các file ảnh trên server (đặt file trong `HTTPv1/images/` để server có thể phục vụ).

## Key behaviors và note
- Server lắng nghe port (mặc định 8888). Dùng `Server> start 8888` để bật server. Có thể dùng `Server> start` (nếu mặc định là 8888) — CLI có hỗ trợ port tuỳ chọn.
- HTTP xử lý theo kiểu "HTTP/1.1-like":
  - Nhanh: server đọc dòng request (GET /file.jpg HTTP/1.1), đọc header tới CRLF CRLF và sau đó trả về header response + nội dung file (binary) nếu có.
  - Server trả về `Content-Type` dựa trên phần mở rộng: `.jpg`, `.png`, `.gif` (mặc định `application/octet-stream`).
  - Nếu file không tồn tại: trả 404 Not Found; nếu yêu cầu có `..` (traversal) thì trả 403 Forbidden.

- Client behavior:
  - `ResourceDownload::loadFileList` nhận `filelist.txt` (mỗi dòng tên file hoặc comment bắt đầu `#`).
  - Client xây dựng request `GET /<filename> HTTP/1.1` và gửi đến server.
  - Khi nhận response, client đọc status line, headers, xác định `Content-Length` nếu có, hoặc đọc tới khi server đóng kết nối.
  - Dữ liệu được lưu theo `basename` của file (ví dụ danh sách có `images/anh1.png` thì client sẽ lưu file thành `anh1.png` ở thư mục hiện tại trên client).

## Thử nghiệm & các lỗi thường gặp
- Connect failed (connect()): Kiểm tra server đã start chưa, kiểm tra port có đúng không (8888 mặc định), kiểm tra firewall.
  - PowerShell test:
  ```powershell
  Test-NetConnection -ComputerName 192.168.0.103 -Port 8888
  ```
  - Nếu port bị chặn, mở firewall:
  ```powershell
  netsh advfirewall firewall add rule name="HTTPv1 8888" dir=in action=allow protocol=TCP localport=8888
  ```

- Server trả 404: Do server không tìm thấy file — kiểm tra file `HTTPv1/images/<filename>` hoặc các candidate path. Người dùng nên đặt file trong `HTTPv1/images` hoặc chạy server từ thư mục gốc nơi `images/` có thể truy cập được.

## Build / Run (Windows - PowerShell)
- Build bằng g++ (MinGW) — một ví dụ đơn giản:
```powershell
cd e:\HTTP-main\HTTPv1
g++ -std=c++11 -o server.exe *.cpp -lws2_32
```
- Chạy server (CLI):
```powershell
.\server.exe
Server> start 8888
```

- Chuẩn bị client: trong `HTTPv1_client/filelist.txt` có các dòng như:
```
anh1.png
anh2.png
anh3.png
```
- Build client:
```powershell
cd D:\HTTP\HTTPv1_client
```
- Chạy client (CLI):
```powershell
.\client.exe

```

## Hạn chế và hướng mở rộng (gợi ý)
- Chỉ hỗ trợ `GET`, không hỗ trợ `Range` requests hoặc chunked transfer encoding.
- HTTP header parsing cơ bản — có thể bị lỗi với header phức tạp.
- File serving dùng `ifstream` đọc toàn bộ file vào bộ nhớ; với file lớn nên stream chunk-by-chunk.
- Cần cấu hình `images` path qua file cấu hình hoặc CLI để tránh phụ thuộc vào working dir.
- Hiện không có HTTPS — nên tránh dùng môi trường thực tế nếu cần bảo mật.
- Thêm logging server để xuất đường dẫn nào bị 404 — hữu ích khi server chạy từ `bin/Release` hoặc `bin/Debug`.

## Các file tôi đã chỉnh sửa trong dự án
- `HTTPv1/HttpServer.cpp` — đọc request cho HTTP và forward tới `HttpHandler`.
- `HTTPv1/HttpHandler.cpp` — xử lý GET và phục vụ file (200, 404, 400, 403); xác định `Content-Type`.
- `HTTPv1/server_cli.cpp` — cho phép start server với port tùy chọn (giảm lỗi mismatch 8080 vs 8888).
- `HTTPv1_client/ResourceDownload.cpp` — parse response headers, đọc nội dung đầy đủ, lưu file cục bộ (basename), xử lý comment/empty lines.

---

Nếu bạn muốn tôi thêm: các ví dụ thực thi (một script test), sửa lỗi `images` path injection, hoặc tạo một `README` tiếng Anh song song, hãy báo tôi biết.