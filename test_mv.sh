#!/bin/bash

# 🛠️ Thư mục và file test
TEST_DIR="test_move"
DEST_DIR="$TEST_DIR/dest"
LOG_FILE="test_move.log"

# 🏗️ Tạo thư mục test
mkdir -p "$TEST_DIR"
mkdir -p "$DEST_DIR"

# 📄 Tạo các file test
touch "$TEST_DIR/file1.txt"
touch "$TEST_DIR/file2.txt"
touch "$TEST_DIR/my file.txt"
touch "$TEST_DIR/.hiddenfile"
mkdir "$TEST_DIR/folder1"

# ✍️ Ghi log kết quả
echo "----- Test move command -----" > $LOG_FILE

# 🚀 Hàm thực thi test case
run_test() {
    echo -e "\n[TEST] $1" | tee -a $LOG_FILE
    eval "$2"
    status=$?

    if [ $status -eq 0 ]; then
        echo "✅ Success" | tee -a $LOG_FILE
    else
        echo "❌ Failed (status=$status)" | tee -a $LOG_FILE
    fi

    echo "-----------------------------" | tee -a $LOG_FILE
}

# 🧪 Test 1: Di chuyển một file đơn giản
run_test "Di chuyển file1.txt vào folder1/" \
    "move $TEST_DIR/file1.txt $TEST_DIR/folder1/ && ls $TEST_DIR/folder1/"

# 🧪 Test 2: Đổi tên file
run_test "Đổi tên file2.txt thành file_renamed.txt" \
    "move $TEST_DIR/file2.txt $TEST_DIR/file_renamed.txt && ls $TEST_DIR/"

# 🧪 Test 3: Di chuyển nhiều file
run_test "Di chuyển tất cả file txt vào folder1/" \
    "move $TEST_DIR/*.txt $TEST_DIR/folder1/ && ls $TEST_DIR/folder1/"

# 🧪 Test 4: Di chuyển thư mục
run_test "Di chuyển folder1 vào dest/" \
    "move $TEST_DIR/folder1 $DEST_DIR/ && ls $DEST_DIR/"

# 🧪 Test 5: Di chuyển file ẩn
run_test "Di chuyển file ẩn .hiddenfile vào dest/" \
    "move $TEST_DIR/.hiddenfile $DEST_DIR/ && ls -a $DEST_DIR/"

# 🧪 Test 6: Di chuyển file không tồn tại
run_test "Thử di chuyển file không tồn tại" \
    "move $TEST_DIR/not_exist.txt $DEST_DIR/"

# 🧪 Test 7: Di chuyển file có dấu cách
run_test "Di chuyển file có dấu cách trong tên" \
    "move \"$TEST_DIR/my file.txt\" $DEST_DIR/ && ls \"$DEST_DIR/\""

# 🧹 Dọn dẹp sau khi test
echo -e "\n[Cleaning up test files...]" | tee -a $LOG_FILE
rm -rf "$TEST_DIR"
rm -rf "$DEST_DIR"

echo -e "\n✅ Test hoàn tất. Kiểm tra log tại $LOG_FILE"


# #!/bin/bash

# # Thư mục và file test
# TEST_DIR="test_move"
# DEST_DIR="$TEST_DIR/dest"
# LOG_FILE="test_move.log"

# # Tạo thư mục test
# mkdir -p "$TEST_DIR"
# mkdir -p "$DEST_DIR"

# # Tạo các file test
# touch "$TEST_DIR/file1.txt"
# touch "$TEST_DIR/file2.txt"
# touch "$TEST_DIR/my file.txt"
# touch "$TEST_DIR/.hiddenfile"
# mkdir "$TEST_DIR/folder1"

# # Ghi log kết quả
# echo "----- Test move command -----" > $LOG_FILE

# run_test() {
#     echo -e "\n[TEST] $1" | tee -a $LOG_FILE
#     eval "$2"
#     echo "=> Status: $?" | tee -a $LOG_FILE
#     echo "-----------------------------" | tee -a $LOG_FILE
# }

# # Test 1: Di chuyển một file đơn giản
# run_test "Di chuyển file1.txt vào folder1/" \
#     "move $TEST_DIR/file1.txt $TEST_DIR/folder1/ && ls $TEST_DIR/folder1/"

# # Test 2: Đổi tên file
# run_test "Đổi tên file2.txt thành file_renamed.txt" \
#     "move $TEST_DIR/file2.txt $TEST_DIR/file_renamed.txt && ls $TEST_DIR/"

# # Test 3: Di chuyển nhiều file
# run_test "Di chuyển tất cả file txt vào folder1/" \
#     "move $TEST_DIR/*.txt $TEST_DIR/folder1/ && ls $TEST_DIR/folder1/"

# # Test 4: Di chuyển thư mục
# run_test "Di chuyển folder1 vào dest/" \
#     "move $TEST_DIR/folder1 $DEST_DIR/ && ls $DEST_DIR/"

# # Test 5: Di chuyển file ẩn
# run_test "Di chuyển file ẩn .hiddenfile vào dest/" \
#     "move $TEST_DIR/.hiddenfile $DEST_DIR/ && ls -a $DEST_DIR/"

# # Test 6: Ghi đè file có sẵn
# touch "$DEST_DIR/file1.txt"
# run_test "Ghi đè file1.txt trong dest/" \
#     "move $DEST_DIR/file1.txt $DEST_DIR/file_renamed.txt"

# # Test 7: Không ghi đè file có sẵn
# run_test "Không ghi đè file1.txt trong dest/" \
#     "move -n $DEST_DIR/file_renamed.txt $DEST_DIR/file1.txt"

# # Test 8: Hiển thị thông tin khi di chuyển
# run_test "Di chuyển file với -v để hiển thị thông tin" \
#     "move -v $DEST_DIR/file_renamed.txt $DEST_DIR/file_final.txt"

# # Test 9: Di chuyển file không tồn tại
# run_test "Thử di chuyển file không tồn tại" \
#     "move $TEST_DIR/not_exist.txt $DEST_DIR/"

# # Test 10: Di chuyển file có dấu cách
# run_test "Di chuyển file có dấu cách trong tên" \
#     "move \"$TEST_DIR/my file.txt\" $DEST_DIR/ && ls \"$DEST_DIR/\""

# # Xóa dữ liệu sau khi test
# echo -e "\n[Cleaning up test files...]" | tee -a $LOG_FILE
# rm -rf "$TEST_DIR"
# rm -rf "$DEST_DIR"

# echo -e "\n✅ Test hoàn tất. Kiểm tra log tại $LOG_FILE"

