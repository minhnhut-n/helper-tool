
#cho phép spawn một process và tương tác với IO của device, nhận giá trị trả về từ debugger
#kiểm soát luồng nhập xuất
import subprocess
#phục vụ cho các thao tác với các dòng lệnh, terminal, argc, argv,...
import sys
#lấy ngày giờ từ hệ thống
from datetime import datetime
#cho phép set/get các path trong directory, phục vụ tốt cho resource
from pathlib import Path

"""Run adb and return its output."""
def run_adb(args):
    result = subprocess.run(
        ["adb", *args],
        capture_output=True,
        text=True,
        encoding="utf-8", #decode type
        errors="replace",
        timeout=60,
    )

    if result.returncode != 0:
        message = result.stderr.strip() or result.stdout.strip()
        raise RuntimeError(message or "ADB command failed")

    return result.stdout.strip()

"""check an android device is connected or not"""
def check_device():
    try:
        output = run_adb(["devices"]) #tham số của adb tool
    except (FileNotFoundError, RuntimeError) as exc:
        print(f"Cannot run ADB: {exc}")
        print("Install Platform-Tools and add adb to PATH.")
        sys.exit(1)

    devices = [
        #truy xuất phần tử đầu tiên của line
        line.split()[0]

        #các phần tử từ thứ 2 trở đi
        for line in output.splitlines()[1:]

        #EQR4NBPFZ9ZLAUUC        device
        #Xác định số lượng cột của phần tử và autority status
        if len(line.split()) >= 2 and line.split()[1] == "device"
    ]
    ##expected result: [<device name> <arg list> <true/false>]

    if len(devices) != 1:
        print(output)
        print(
            "\nPlease connect exactly one authorized Android "
            "device. Check USB debugging if needed."
        )
        sys.exit(1)

    print(f"Connected device: {devices[0]}")

""""fcn that help to list all available to uninstall"""
def get_user_apps():
    output = run_adb(["shell", "pm", "list", "packages", "-3"])
    packages = []

    for line in output.splitlines():
        line = line.strip()
        if line.startswith("package:"):
            packages.append(line[len("package:"):])

    return sorted(set(packages))

"""helper fcn to remove a specific app"""
def uninstall_package(package):
    return run_adb([
        "shell", "pm", "uninstall", "--user", "0", package
    ])


def main():
    check_device()

    try:
        packages = get_user_apps()
    except (RuntimeError, FileNotFoundError) as exc:
        print(f"Cannot list applications: {exc}")
        sys.exit(1)

    if not packages:
        print("No third-party user apps found.")
        return

    print(f"\nFound {len(packages)} user-installed packages:\n")

    for index, package in enumerate(packages, start=1):
        print(f"{index:3}. {package}") #quy định độ rộng tối thiểu là 3 kí tự sau dấu :

    print("\nOptions:")
    print("  all  - Remove ALL listed user-installed apps")
    print("  q    - Quit without changes")
    print("  Or enter package numbers separated by commas.")

    choice = input("Selection: ").strip() #remove the leading and trailing whitespace (back and fore)

    if choice.lower() == "q":
        print("Cancelled.")
        return

    if choice.lower() == "all":
        selected = packages.copy()
    else:
        try:
            indexes = sorted({
                # for 2 cases: => [1,2,3]
                #   - 1,2,3
                #   - 1,   2, 3
                int(part.strip()) for part in choice.split(",")
            })

            if not indexes or any(
                index < 1 or index > len(packages)
                for index in indexes
            ):
                raise ValueError

        except ValueError:
            print("Invalid selection. Nothing was removed.")
            return

        #make a list selected
        selected = [packages[index - 1] for index in indexes]

    print(f"\nSelected {len(selected)} package(s) for removal:")
    for package in selected:
        print(f"  - {package}")

    print(
        "\nWARNING: Removing apps may delete their local data. "
        "System apps are not included in this list."
    )

    confirm = input(
        f"Type REMOVE to uninstall these {len(selected)} app(s): "
    ).strip()

    if confirm != "REMOVE":
        print("Cancelled. Nothing was removed.")
        return

    report = [
        f"Android App Cleaner report - {datetime.now().isoformat()}",
        "",
    ]

    #loop in remove packages
    for package in selected:
        try:
            result = uninstall_package(package)
            status = result or "No output"
        except (RuntimeError, FileNotFoundError) as exc:
            status = f"ERROR: {exc}"

        print(f"{package}: {status}")
        report.append(f"{package}: {status}")

    report_path = Path(__file__).with_name("uninstall_report.txt")
    report_path.write_text("\n".join(report) + "\n", encoding="utf-8")

    print(f"\nReport saved to: {report_path}")


if __name__ == "__main__":
    main()
