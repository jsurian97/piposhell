#!/bin/bash
# Comprehensive functional test suite mirroring the 42 minishell evaluation grid.
# Compares minishell behaviour against bash for core mandatory features:
# builtins, parsing, redirections, pipelines, heredoc, error handling, exit codes, etc.

set -o pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}" )"/.. && pwd)"
MINISHELL_BIN="${MINISHELL_BIN:-$ROOT_DIR/minishell}"
BASH_BIN="${BASH_BIN:-/bin/bash}"
KEEP_FAILURE_ARTIFACTS=0
VERBOSE=0

usage() {
    cat <<'EOF'
Usage: tests/run_eval_suite.sh [options]

Options:
  -k, --keep-artifacts    Keep failing command scripts for inspection
  -v, --verbose           Dump bash/minishell outputs even on success
  -h, --help              Show this help and exit

Environment overrides:
  MINISHELL_BIN=/path/to/minishell  Use a custom minishell binary
  BASH_BIN=/path/to/bash            Use a custom bash binary
EOF
}

while [[ $# -gt 0 ]]; do
    case "$1" in
        -k|--keep-artifacts)
            KEEP_FAILURE_ARTIFACTS=1
            shift
            ;;
        -v|--verbose)
            VERBOSE=1
            shift
            ;;
        -h|--help)
            usage
            exit 0
            ;;
        *)
            echo "Unknown option: $1" >&2
            usage
            exit 1
            ;;
    esac
done

if [[ ! -x "$MINISHELL_BIN" ]]; then
    echo "Error: minishell binary not found at $MINISHELL_BIN" >&2
    echo "Compile the project (make) or set MINISHELL_BIN." >&2
    exit 1
fi

if ! command -v "$BASH_BIN" >/dev/null 2>&1; then
    echo "Error: bash binary not found (looked for $BASH_BIN)." >&2
    exit 1
fi

if [[ -t 1 ]]; then
    GREEN="$(tput setaf 2)"
    RED="$(tput setaf 1)"
    YELLOW="$(tput setaf 3)"
    RESET="$(tput sgr0)"
else
    GREEN=""
    RED=""
    YELLOW=""
    RESET=""
fi

TEST_CATEGORIES=()
TEST_DESCRIPTIONS=()
TEST_COMMANDS=()

add_test() {
    TEST_CATEGORIES+=("$1")
    TEST_DESCRIPTIONS+=("$2")
    TEST_COMMANDS+=("$3")
}

normalize_stderr() {
    sed -E 's@^(/[[:alnum:]/._-]*bash|minishell): line [0-9]+: @shell: @' | \
    sed -E 's@^(/[[:alnum:]/._-]*bash|minishell):@shell:@'
}

normalize_stdout() {
    sed -E '/^> /d'
}

print_verbose_output() {
    local title="$1"
    local stdout_file="$2"
    local stderr_file="$3"
    echo "$YELLOW$title stdout:$RESET"
    if [[ -s "$stdout_file" ]]; then
        cat "$stdout_file"
    else
        echo "(empty)"
    fi
    echo "$YELLOW$title stderr:$RESET"
    if [[ -s "$stderr_file" ]]; then
        cat "$stderr_file"
    else
        echo "(empty)"
    fi
}

run_shell() {
    local shell="$1" cmd_file="$2" out_file="$3" err_file="$4"
    if [[ "$shell" == "bash" ]]; then
        "$BASH_BIN" --noprofile --norc < "$cmd_file" >"$out_file" 2>"$err_file"
    else
        "$MINISHELL_BIN" < "$cmd_file" >"$out_file" 2>"$err_file"
    fi
    return $?
}

run_test() {
    local index="$1"
    local category="${TEST_CATEGORIES[$index]}"
    local description="${TEST_DESCRIPTIONS[$index]}"
    local commands="${TEST_COMMANDS[$index]}"

    local cmd_file
    cmd_file="$(mktemp)"
    printf "%s\n" "$commands" > "$cmd_file"
    echo "exit" >> "$cmd_file"

    local bash_out bash_err mini_out mini_err
    bash_out="$(mktemp)"
    bash_err="$(mktemp)"
    mini_out="$(mktemp)"
    mini_err="$(mktemp)"

    run_shell "bash" "$cmd_file" "$bash_out" "$bash_err"
    local bash_status=$?

    run_shell "minishell" "$cmd_file" "$mini_out" "$mini_err"
    local mini_status=$?

    local norm_bash_out norm_mini_out norm_bash_err norm_mini_err
    norm_bash_out="$(mktemp)"
    norm_mini_out="$(mktemp)"
    norm_bash_err="$(mktemp)"
    norm_mini_err="$(mktemp)"
    normalize_stdout < "$bash_out" > "$norm_bash_out"
    normalize_stdout < "$mini_out" > "$norm_mini_out"
    normalize_stderr < "$bash_err" > "$norm_bash_err"
    normalize_stderr < "$mini_err" > "$norm_mini_err"

    local stdout_match=0
    local stderr_match=0
    local status_match=0

    if cmp -s "$norm_bash_out" "$norm_mini_out"; then
        stdout_match=1
    fi
    if cmp -s "$norm_bash_err" "$norm_mini_err"; then
        stderr_match=1
    fi
    if [[ "$bash_status" -eq "$mini_status" ]]; then
        status_match=1
    fi

    local result="FAIL"
    local color="$RED"
    if [[ $stdout_match -eq 1 && $stderr_match -eq 1 && $status_match -eq 1 ]]; then
        result="OK"
        color="$GREEN"
    fi

    printf "[%02d/%02d] %-12s %-35s %s%s%s\n" \
        $((index + 1)) ${#TEST_COMMANDS[@]} "$category" "$description" "$color" "$result" "$RESET"

    if [[ "$result" != "OK" ]] || [[ $VERBOSE -eq 1 ]]; then
        print_verbose_output "Bash" "$bash_out" "$bash_err"
        print_verbose_output "Minishell" "$mini_out" "$mini_err"
        echo "$YELLOWExit statuses:$RESET bash=$bash_status minishell=$mini_status"
    fi

    if [[ "$result" != "OK" ]]; then
        echo "$YELLOWstdout diff (normalized):$RESET"
        diff -u "$norm_bash_out" "$norm_mini_out" || true
        echo "$YELLOWstderr diff (normalized):$RESET"
        diff -u "$norm_bash_err" "$norm_mini_err" || true
        if [[ $KEEP_FAILURE_ARTIFACTS -eq 1 ]]; then
            echo "$YELLOWCommand file kept at:$RESET $cmd_file"
        else
            rm -f "$cmd_file"
        fi
    else
        rm -f "$cmd_file"
    fi

    rm -f "$bash_out" "$bash_err" "$mini_out" "$mini_err" "$norm_bash_out" "$norm_mini_out" "$norm_bash_err" "$norm_mini_err"

    [[ "$result" == "OK" ]]
    return $?
}

# --- Test definitions (ordered to mirror the evaluation checklist) ---
add_test "Builtins" "echo basic output" "$(cat <<'EOS'
echo hello world
EOS
)"
add_test "Builtins" "echo -n chaining" "$(cat <<'EOS'
echo -n -n hello
EOS
)"
add_test "Parsing" "single quotes" "$(cat <<'EOS'
echo 'single quoted text'
EOS
)"
add_test "Parsing" "double quotes + expansion" "$(cat <<'EOS'
export MSG="hello world"
echo "$MSG"
EOS
)"
add_test "Parsing" "missing variable" "$(cat <<'EOS'
echo $THIS_VARIABLE_SHOULD_NOT_EXIST
EOS
)"
add_test "Status" "previous exit code" "$(cat <<'EOS'
false
echo $?
EOS
)"
add_test "Env" "export + echo" "$(cat <<'EOS'
export MINI_EVAL_VAR=testing
echo $MINI_EVAL_VAR
EOS
)"
add_test "Env" "unset removal" "$(cat <<'EOS'
export MINI_EVAL_VAR=testing
unset MINI_EVAL_VAR
echo $MINI_EVAL_VAR
EOS
)"
add_test "Env" "export listing" "$(cat <<'EOS'
export MINI_EVAL_VAR=testing
export | grep MINI_EVAL_VAR=
EOS
)"
add_test "Builtins" "cd success" "$(cat <<'EOS'
pwd
cd /tmp
pwd
EOS
)"
add_test "Builtins" "cd failure" "$(cat <<'EOS'
cd /definitely_not_a_real_directory
EOS
)"
add_test "Builtins" "pwd builtin" "$(cat <<'EOS'
pwd
EOS
)"
add_test "Execution" "absolute path" "$(cat <<'EOS'
/bin/echo absolute path ok
EOS
)"
add_test "Execution" "PATH search" "$(cat <<'EOS'
printf 'path lookup works\n'
EOS
)"
add_test "Execution" "command not found" "$(cat <<'EOS'
this_command_should_not_exist
EOS
)"
add_test "Execution" "permission denied" "$(cat <<'EOS'
rm -f /tmp/ms_eval_perm
printf 'secret' > /tmp/ms_eval_perm
chmod 000 /tmp/ms_eval_perm
cat /tmp/ms_eval_perm
chmod 600 /tmp/ms_eval_perm
rm -f /tmp/ms_eval_perm
EOS
)"
add_test "Pipes" "single pipe" "$(cat <<'EOS'
echo piped | cat
EOS
)"
add_test "Pipes" "multiple pipes" "$(cat <<'EOS'
echo cascade | tr a-z A-Z | tr A-Z a-z
EOS
)"
add_test "Redirect" "output truncation" "$(cat <<'EOS'
rm -f /tmp/ms_eval_out
echo first > /tmp/ms_eval_out
cat /tmp/ms_eval_out
rm -f /tmp/ms_eval_out
EOS
)"
add_test "Redirect" "output append" "$(cat <<'EOS'
rm -f /tmp/ms_eval_append
echo first > /tmp/ms_eval_append
echo second >> /tmp/ms_eval_append
cat /tmp/ms_eval_append
rm -f /tmp/ms_eval_append
EOS
)"
add_test "Redirect" "input redirection" "$(cat <<'EOS'
printf 'alpha\nbeta\n' > /tmp/ms_eval_input
cat < /tmp/ms_eval_input
rm -f /tmp/ms_eval_input
EOS
)"
add_test "Herodoc" "simple heredoc" "$(cat <<'EOS'
cat << EOF
hello heredoc
EOF
EOS
)"
add_test "Parsing" "syntax error" "$(cat <<'EOS'
echo | |
EOS
)"
add_test "Builtins" "exit numeric" "$(cat <<'EOS'
exit 42
EOS
)"
add_test "Builtins" "exit non-numeric" "$(cat <<'EOS'
exit notanumber
EOS
)"
add_test "Env" "export invalid identifier" "$(cat <<'EOS'
export 1INVALID=oops
EOS
)"
add_test "Env" "unset invalid identifier" "$(cat <<'EOS'
unset 1INVALID
EOS
)"

add_test "Parsing" "single-quoted dollar literal" "$(cat <<'EOS'
echo '$HOME should not expand'
EOS
)"
add_test "Parsing" "mixed nested quotes" "$(cat <<'EOS'
echo "outer 'single' and \"double\""
EOS
)"
add_test "Env" "brace expansion" "$(cat <<'EOS'
echo ${HOME}
EOS
)"
add_test "Builtins" "cd to HOME" "$(cat <<'EOS'
cd
pwd
EOS
)"
add_test "Builtins" "cd dash toggles" "$(cat <<'EOS'
pwd
cd /tmp
pwd
cd -
pwd
EOS
)"
add_test "Env" "export quoted value" "$(cat <<'EOS'
export MINI_QUOTED_VAR="value with spaces"
env | grep MINI_QUOTED_VAR=
unset MINI_QUOTED_VAR
EOS
)"
add_test "Status" "pipeline exit status (last success)" "$(cat <<'EOS'
false | true
echo $?
EOS
)"
add_test "Status" "pipeline exit status (last failure)" "$(cat <<'EOS'
true | false
echo $?
EOS
)"
add_test "Pipes" "builtin through pipeline" "$(cat <<'EOS'
export PIPE_VAR=works
echo $PIPE_VAR | tr a-z A-Z
unset PIPE_VAR
EOS
)"
add_test "Execution" "local executable" "$(cat <<'EOS'
rm -f /tmp/ms_eval_script.sh
printf '#!/bin/sh\necho local script\n' > /tmp/ms_eval_script.sh
chmod +x /tmp/ms_eval_script.sh
/tmp/ms_eval_script.sh
rm -f /tmp/ms_eval_script.sh
EOS
)"
add_test "Redirect" "mixed redirections" "$(cat <<'EOS'
printf 'alpha\nbeta\n' > /tmp/ms_eval_mixed
grep beta < /tmp/ms_eval_mixed > /tmp/ms_eval_mixed_out
cat /tmp/ms_eval_mixed_out
rm -f /tmp/ms_eval_mixed /tmp/ms_eval_mixed_out
EOS
)"
add_test "Herodoc" "heredoc with expansion" "$(cat <<'EOS'
export HD_VAR=expanded
cat << EOF
value $HD_VAR
EOF
unset HD_VAR
EOS
)"
add_test "Herodoc" "quoted heredoc (no expansion)" "$(cat <<'EOS'
export HD_VAR=should_not_expand
cat <<'EOF'
value $HD_VAR
EOF
unset HD_VAR
EOS
)"

TOTAL=${#TEST_COMMANDS[@]}
PASSED=0
FAILED=0

for i in "${!TEST_COMMANDS[@]}"; do
    if run_test "$i"; then
        PASSED=$((PASSED + 1))
    else
        FAILED=$((FAILED + 1))
    fi
    echo
done

if [[ $FAILED -eq 0 ]]; then
    echo "$GREEN>>> All $TOTAL evaluation tests passed.$RESET"
else
    echo "$RED>>> $FAILED/$TOTAL evaluation tests failed.$RESET"
fi

exit $FAILED
