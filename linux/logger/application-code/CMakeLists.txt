cmake_minimum_required(VERSION 3.10)
project(LoggerApp C)

# Cấu hình chuẩn C (C99)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Thêm các cờ biên dịch tương đương Makefile cũ
add_compile_options(-Wall -Wextra)

# Thêm thư viện luồng (pthread)
find_package(Threads REQUIRED)

# Khai báo các file nguồn cho ứng dụng
set(APP_SRCS
    app-code.c
    logger-srv.c
)

# Tạo file thực thi từ các file nguồn
add_executable(app ${APP_SRCS})

# Liên kết thư viện pthread vào file thực thi
target_link_libraries(app PRIVATE Threads::Threads)