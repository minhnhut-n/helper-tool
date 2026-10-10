cơ chế hoạt động của logger như sau

application code include logger lib vào sử dụng các api của logger

tại logger lib (application code), logger sẽ check init trước tất cả các hàm, với
likely(init) để điều hướng nhanh code

nhiệm vụ logger lib
- mở socket và ghi thông tin từ nhiều application

logger daemon, work background, thực hiện load các log từ file descriptor
===
app-code.c: Chứa mã nguồn ứng dụng mẫu (client) gọi các hàm API ghi log.

logger-lib.c: Hiện thực các hàm của thư viện client, thực hiện việc mở kết nối socket và truyền thông điệp log từ ứng dụng.
logger-lib.h: File header định nghĩa các public API (như hàm logger) cung cấp cho application code sử dụng.

logger-main.c: Điểm khởi chạy (entry point) chính cho tiến trình Daemon logger.

logger-srv.c: Hiện thực logic máy chủ của daemon, quản lý kết nối socket, vòng đệm trung tâm và các background worker.
logger-srv.h: File header định nghĩa cấu trúc dữ liệu, ring buffer và các hàm nội bộ dành riêng cho tầng daemon/server.

logger.service: File cấu hình systemd service để quản lý tiến trình daemon chạy ngầm tự động trên Linux.

Makefile: Script tự động hóa quá trình biên dịch (build) toàn bộ các module trong package.
README.md: Tài liệu hướng dẫn sử dụng, mô tả kiến trúc và cách tích hợp thư viện logger vào dự án.