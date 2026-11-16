#!/usr/bin/env bash
# Benchmark runner for a Connect 4 solver, following Pascal Pons's protocol:
# http://blog.gamesolver.org/solving-connect-four/02-test-protocol/
#
# The solver must read one position per line on stdin and write, for each one,
#   <position> <score> <explored nodes> <time in microseconds>
# on stdout (space separated). This script feeds it the 6 test sets, compares
# the scores with the expected ones, prints a summary table and, on request,
# appends one line per set to a CSV file that opens directly in Excel / LibreOffice.
#
# Usage:
#   bench/bench.sh download                 fetch the 6 test sets into bench/data
#   bench/bench.sh [options] [SET ...]      run the benchmark
#
# Options:
#   -s SOLVER   solver executable             (default: build/release/c4solver)
#   -d DIR      directory of the test sets    (default: bench/data)
#   -n N        only the first N positions of each set
#   -t SECONDS  time limit per set, 0 = none  (default: 0)
#   -w          weak mode: pass -w to the solver and compare only the sign
#   -o FILE     append one summary line per set to this CSV file (created if needed)
#   -e STEP     blog part number being tested (3 to 12), written in the CSV
#   -l TEXT     free comment, written in the CSV
#   -p FILE     write one line per position to this CSV file (overwritten)
#   -h          this help
#
# SET is Test_L3_R1, L3_R1 or End-Easy (same for the five others); default: all.
# Exit status: 0 if every position of every set was solved with the right score.
#
# CSV files use ';' as separator and contain only integers and text, so that they
# open correctly with a French (or any) locale: the means are computed in the
# spreadsheet (see suivi_benchmark.xlsx).

set -u

SOLVER=build/release/c4solver
DATA_DIR=bench/data
LIMIT=0
TIME_LIMIT=0
WEAK=0
OUT_CSV=
PP_CSV=
STEP=
LABEL=
BASE_URL=http://blog.gamesolver.org/data
ALL_SETS=(Test_L3_R1 Test_L2_R1 Test_L2_R2 Test_L1_R1 Test_L1_R2 Test_L1_R3)
BOM=$'\xEF\xBB\xBF'

usage() { sed -n '2,/^$/p' "$0" | sed 's/^# \{0,1\}//'; }

resolve_set() {
    case "$(printf '%s' "$1" | tr '[:upper:]' '[:lower:]')" in
        test_l3_r1|l3_r1|end-easy)      echo Test_L3_R1 ;;
        test_l2_r1|l2_r1|middle-easy)   echo Test_L2_R1 ;;
        test_l2_r2|l2_r2|middle-medium) echo Test_L2_R2 ;;
        test_l1_r1|l1_r1|begin-easy)    echo Test_L1_R1 ;;
        test_l1_r2|l1_r2|begin-medium)  echo Test_L1_R2 ;;
        test_l1_r3|l1_r3|begin-hard)    echo Test_L1_R3 ;;
        *) return 1 ;;
    esac
}

label_of() {
    case "$1" in
        Test_L3_R1) echo End-Easy ;;      Test_L2_R1) echo Middle-Easy ;;
        Test_L2_R2) echo Middle-Medium ;; Test_L1_R1) echo Begin-Easy ;;
        Test_L1_R2) echo Begin-Medium ;;  Test_L1_R3) echo Begin-Hard ;;
    esac
}

download() {
    mkdir -p "$DATA_DIR" || exit 2
    local s
    for s in "${ALL_SETS[@]}"; do
        echo "downloading $s"
        curl -fsSL -o "$DATA_DIR/$s" "$BASE_URL/$s" || { echo "failed: $BASE_URL/$s" >&2; exit 2; }
        echo "  $(wc -l < "$DATA_DIR/$s") lines"
    done
}

# Reads the expected file then the solver output, prints one summary row.
# Extra settings come from the environment (ENVIRON) so that no character gets interpreted by awk.
read -r -d '' AWK_PROG <<'AWK'
function sgn(x) { return (x > 0) - (x < 0) }
function fmt_time(us) {
    if (us >= 1e6) return sprintf("%.2f s", us / 1e6)
    if (us >= 1e3) return sprintf("%.2f ms", us / 1e3)
    return sprintf("%.2f us", us)
}
BEGIN { csv = ENVIRON["BENCH_CSV"]; pp = ENVIRON["BENCH_PP"] }
FNR == NR { epos[FNR] = $1; escore[FNR] = $2; total = FNR; next }
{
    i++
    if (i > total) { extra++; next }
    if ($1 != epos[i]) {
        misaligned++
        if (shown++ < 5) printf("  line %d: expected position %s, got %s\n", i, epos[i], $1) > "/dev/stderr"
        next
    }
    ok = weak ? (sgn($2) == sgn(escore[i])) : ($2 == escore[i])
    if (!ok) {
        errors++
        if (shown++ < 5) printf("  line %d (%s): expected %s, got %s\n", i, $1, escore[i], $2) > "/dev/stderr"
    }
    if (pp != "") printf("%s;%d;%s;%d;%s;%s;%d;%s;%s\n", set, i, $1, length($1), escore[i], $2, ok ? 1 : 0, $3, $4) >> pp
    nodes += $3; time += $4; done++
}
END {
    mean_t = done ? time / done : 0
    mean_n = done ? nodes / done : 0
    kpos   = time > 0 ? nodes / time * 1000 : 0
    printf("%-11s %-14s %5d/%-5d %6d %11s %14.1f %9.0f", set, label, done, total, errors + misaligned, fmt_time(mean_t), mean_n, kpos)
    if (done < total) printf("   INCOMPLETE (solver exit status %d)", status)
    if (extra)        printf("   %d extra output lines", extra)
    printf("\n")
    if (csv != "")
        printf("%s;%s;%s;%s;%s;%s;%s;%d;%d;%d;%.0f;%.0f;%d\n", ENVIRON["BENCH_DATE"], ENVIRON["BENCH_COMMIT"],
               ENVIRON["BENCH_STEP"], ENVIRON["BENCH_LABEL"], set, label, weak ? "weak" : "strong",
               total, done, errors + misaligned, time, nodes, status) >> csv
    exit (errors + misaligned > 0 || done < total || extra > 0)
}
AWK

run_set() {
    local set=$1 label file expected out err status
    label=$(label_of "$set")
    file=$DATA_DIR/$set
    if [[ ! -f $file ]]; then
        echo "$set: $file not found (run: bench/bench.sh download)" >&2
        return 2
    fi
    expected=$TMP/$set.expected; out=$TMP/$set.out; err=$TMP/$set.err

    # expected file: "position score" per line (CR removed, optionally truncated)
    if (( LIMIT > 0 )); then tr -d '\r' < "$file" | head -n "$LIMIT" > "$expected"
    else tr -d '\r' < "$file" > "$expected"; fi
    cut -d' ' -f1 "$expected" > "$TMP/$set.in"

    local cmd=("$SOLVER")
    (( WEAK )) && cmd+=(-w)
    (( TIME_LIMIT > 0 )) && cmd=(timeout "$TIME_LIMIT" "${cmd[@]}")

    "${cmd[@]}" < "$TMP/$set.in" > "$out" 2> "$err"
    status=$?
    [[ -s $err ]] && head -n 3 "$err" >&2

    awk -v set="$set" -v label="$label" -v weak="$WEAK" -v status="$status" \
        "$AWK_PROG" "$expected" "$out"
}

main() {
    if [[ ${1:-} == download ]]; then download; exit 0; fi

    local opt
    while getopts "s:d:n:t:wo:e:l:p:h" opt; do
        case $opt in
            s) SOLVER=$OPTARG ;;
            d) DATA_DIR=$OPTARG ;;
            n) LIMIT=$OPTARG ;;
            t) TIME_LIMIT=$OPTARG ;;
            w) WEAK=1 ;;
            o) OUT_CSV=$OPTARG ;;
            e) STEP=$OPTARG ;;
            l) LABEL=$OPTARG ;;
            p) PP_CSV=$OPTARG ;;
            h) usage; exit 0 ;;
            *) usage >&2; exit 2 ;;
        esac
    done
    shift $((OPTIND - 1))

    [[ $LIMIT =~ ^[0-9]+$ && $TIME_LIMIT =~ ^[0-9]+$ ]] || { echo "-n and -t take integers" >&2; exit 2; }
    [[ -z $STEP || $STEP =~ ^[0-9]+$ ]] || { echo "-e takes the blog part number (an integer)" >&2; exit 2; }
    [[ -x $SOLVER ]] || { echo "solver not found or not executable: $SOLVER" >&2; exit 2; }

    local sets=() s
    if (( $# == 0 )); then sets=("${ALL_SETS[@]}")
    else
        for s in "$@"; do
            sets+=("$(resolve_set "$s")") || { echo "unknown set: $s" >&2; exit 2; }
        done
    fi

    TMP=$(mktemp -d) || exit 2
    trap 'rm -rf "$TMP"' EXIT

    # --- CSV files (';' separated, integers and text only) ---
    if [[ -n $OUT_CSV && ! -s $OUT_CSV ]]; then
        mkdir -p "$(dirname "$OUT_CSV")"
        printf '%s' "$BOM" > "$OUT_CSV"
        echo 'date;commit;etape;commentaire;jeu;nom;mode;positions_total;positions_traitees;erreurs;temps_total_us;noeuds_total;code_sortie' >> "$OUT_CSV"
    fi
    if [[ -n $PP_CSV ]]; then
        mkdir -p "$(dirname "$PP_CSV")"
        printf '%s' "$BOM" > "$PP_CSV"
        echo 'jeu;ligne;position;coups_joues;score_attendu;score_obtenu;ok;noeuds;temps_us' >> "$PP_CSV"
    fi
    local commit=
    commit=$(git rev-parse --short HEAD 2>/dev/null) || commit=
    if [[ -n $commit && -n $(git status --porcelain -- . ':!bench' 2>/dev/null) ]]; then commit+=-dirty; fi
    LABEL=${LABEL//[;$'\n\r']/,}
    export BENCH_CSV=$OUT_CSV BENCH_PP=$PP_CSV BENCH_STEP=$STEP BENCH_LABEL=$LABEL
    export BENCH_COMMIT=$commit BENCH_DATE
    BENCH_DATE=$(date '+%Y-%m-%d %H:%M:%S')

    printf '%-11s %-14s %-11s %6s %11s %14s %9s\n' Set Name Done Errors "Mean time" "Mean nb pos" "K pos/s"
    local rc=0 r
    for s in "${sets[@]}"; do
        run_set "$s"; r=$?
        (( r > rc )) && rc=$r
    done
    [[ -n $OUT_CSV ]] && echo "-> summary appended to $OUT_CSV" >&2
    [[ -n $PP_CSV ]] && echo "-> per-position results in $PP_CSV" >&2
    exit "$rc"
}

main "$@"
