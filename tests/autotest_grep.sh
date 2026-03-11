cd "$(dirname "$0")" || exit

TEST_DIR="test_files"
GREP_EXE="../grep/s21_grep"
LOG_FILE="fails.log"

rm -f "$LOG_FILE"

SUCCESS=0
FAIL=0
COUNTER=0

echo "GREP TESTING STARTED"

FLAGS="v c l n i"

for f1 in $FLAGS; do
    for f2 in $FLAGS; do
        for f3 in $FLAGS; do
            for f4 in $FLAGS; do
                T_FLAGS=""
                [ "$f1" != "none" ] && T_FLAGS="$T_FLAGS -$f1"
                [ "$f2" != "none" ] && T_FLAGS="$T_FLAGS -$f2"
                [ "$f3" != "none" ] && T_FLAGS="$T_FLAGS -$f3"
                [ "$f4" != "none" ] && T_FLAGS="$T_FLAGS -$f4"
                T_FLAGS=$(echo "$T_FLAGS" | sed 's/^ //')
                [ -z "$T_FLAGS" ] && continue
                
                COUNTER=$((COUNTER + 1))
                
                grep $T_FLAGS "hello" "$TEST_DIR/grep_text1.txt" 2>/dev/null > orig.log
                $GREP_EXE $T_FLAGS "hello" "$TEST_DIR/grep_text1.txt" 2>/dev/null > s21.log
                
                if diff s21.log orig.log >/dev/null; then
                    SUCCESS=$((SUCCESS + 1))
                else
                    FAIL=$((FAIL + 1))
                    echo "FAIL: grep $T_FLAGS hello" >> "$LOG_FILE"
                    diff s21.log orig.log >> "$LOG_FILE" 2>/dev/null
                    echo "---" >> "$LOG_FILE"
                fi
            done
        done
    done
done

COUNTER=$((COUNTER + 1))
grep hello nonexistent.txt 2> orig_err.log
$GREP_EXE hello nonexistent.txt 2> s21_err.log
sed -i 's/s21_grep/grep/g' s21_err.log

if diff s21_err.log orig_err.log >/dev/null; then
    SUCCESS=$((SUCCESS + 1))
else
    FAIL=$((FAIL + 1))
    echo "FAIL: grep nonexistent.txt" >> "$LOG_FILE"
    diff orig_err.log s21_err.log >> "$LOG_FILE"
    echo "---" >> "$LOG_FILE"
fi

COUNTER=$((COUNTER + 1))
echo "hello world" | grep hello 2>/dev/null > orig.log
echo "hello world" | $GREP_EXE hello 2>/dev/null > s21.log

if diff s21.log orig.log >/dev/null; then
    SUCCESS=$((SUCCESS + 1))
else
    FAIL=$((FAIL + 1))
    echo "FAIL: grep stdin" >> "$LOG_FILE"
    diff s21.log orig.log >> "$LOG_FILE"
    echo "---" >> "$LOG_FILE"
fi

echo "----------------------------------"
echo "TOTAL: $COUNTER | SUCCESS: $SUCCESS | FAIL: $FAIL"
echo "Log file: $LOG_FILE"

rm -f orig.log s21.log orig_err.log s21_err.log

[ $FAIL -ne 0 ] && exit 1 || exit 0