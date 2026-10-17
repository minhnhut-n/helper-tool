cmake_minimum_required(VERSION 3.10)
project(LoggerDaemon C)

# Cấu hình chuẩn C (C11, cần cho stdatomic)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Thêm các cờ biên dịch tương đương Makefile cũ
add_compile_options(-Wall -Wextra)

# Thêm thư viện luồng (pthread)
find_package(Threads REQUIRED)

# Khai báo các file nguồn cho Daemon
set(DAEMON_SRCS
    logger-daemon.c
    ring_buffer.c
)

# Tạo file thực thi từ các file nguồn
add_executable(logger_daemon ${DAEMON_SRCS})

# Liên kết thư viện pthread vào file thực thi
target_link_libraries(logger_daemon PRIVATE Threads::Threads)