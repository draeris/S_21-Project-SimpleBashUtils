cd "$(dirname "$0")" || exit

TEST_DIR="test_files"
CAT_EXE="../cat/s21_cat"
LOG_FILE="fails.log"

rm -f "$LOG_FILE"

SUCCESS=0
FAIL=0
COUNTER=0

echo "CAT TESTING STARTED"

FLAGS="b e n s t v E T"

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
                
                
                if [[ "$UNIQUE_FLAGS" == *"-s"* ]]; then
                    TEST_FILE="$TEST_DIR/cat_empty_lines.txt"
                else
                    TEST_FILE="$TEST_DIR/cat_basic.txt"
                fi
                
                COUNTER=$((COUNTER + 1))
                
                cat $UNIQUE_FLAGS "$TEST_FILE" 2>/dev/null > orig.log
                $CAT_EXE $UNIQUE_FLAGS "$TEST_FILE" 2>/dev/null > s21.log
                
                if diff s21.log orig.log >/dev/null; then
                    SUCCESS=$((SUCCESS + 1))
                else
                    FAIL=$((FAIL + 1))
                    echo "FAIL: cat $UNIQUE_FLAGS ($TEST_FILE)" >> "$LOG_FILE"
                    diff s21.log orig.log >> "$LOG_FILE" 2>/dev/null
                    echo "---" >> "$LOG_FILE"
                fi
            done
        done
    done
done

COUNTER=$((COUNTER + 1))
cat nonexistent.txt 2> orig_err.log
$CAT_EXE nonexistent.txt 2> s21_err.log
sed -i 's/s21_cat/cat/g' s21_err.log

if diff s21_err.log orig_err.log >/dev/null; then
    SUCCESS=$((SUCCESS + 1))
else
    FAIL=$((FAIL + 1))
    echo "FAIL: cat nonexistent.txt" >> "$LOG_FILE"
    diff orig_err.log s21_err.log >> "$LOG_FILE"
    echo "---" >> "$LOG_FILE"
fi

COUNTER=$((COUNTER + 1))
echo "test line" | cat -n 2>/dev/null > orig.log
echo "test line" | $CAT_EXE -n 2>/dev/null > s21.log

if diff s21.log orig.log >/dev/null; then
    SUCCESS=$((SUCCESS + 1))
else
    FAIL=$((FAIL + 1))
    echo "FAIL: cat stdin" >> "$LOG_FILE"
    diff s21.log orig.log >> "$LOG_FILE"
    echo "---" >> "$LOG_FILE"
fi

echo "----------------------------------"
echo "TOTAL: $COUNTER | SUCCESS: $SUCCESS | FAIL: $FAIL"
echo "Log file: $LOG_FILE"

rm -f orig.log s21.log orig_err.log s21_err.log

[ $FAIL -ne 0 ] && exit 1 || exit 0