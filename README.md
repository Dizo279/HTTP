# HTTP — Client / Server Demo (Image Download over HTTP)

Project demo cho phép client tải xuống một danh sách tài nguyên (các file ảnh) từ server bằng giao thức HTTP đơn giản.

Tính năng chính:
- Server: lắng nghe kết nối TCP, xử lý câu lệnh HTTP GET, trả về file ảnh nếu tồn tại (trong thư mục `images/`), trả về 404 nếu không tìm thấy, 403 nếu truy cập thư mục bị cấm.
- Client: đọc danh sách file từ file text (mỗi dòng một tên file), gửi yêu cầu GET đến server, lưu file nhận được vào ổ cứng.
- CLI đơn giản cho server và client: `Server>` và `Client>`.

Xem thêm chi tiết triển khai và cách chạy trong `IMPLEMENTATION_DETAILS.md`.
