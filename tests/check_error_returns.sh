#!/bin/bash
# Extensive regression test to ensure minishell matches bash on failure exit codes.

set -o pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}" )"/.. && pwd)"
MINISHELL_BIN="${MINISHELL_BIN:-$ROOT_DIR/minishell}"
BASH_BIN="${BASH_BIN:-/bin/bash}"

if [[ ! -x "$MINISHELL_BIN" ]]; then
    echo "Error: minishell binary not found at $MINISHELL_BIN" >&2
    exit 1
fi

if ! command -v "$BASH_BIN" >/dev/null 2>&1; then
    echo "Error: bash binary not found (looked for $BASH_BIN)." >&2
    exit 1
fi

TMP_ERR_PREFIX="/tmp/ms_errtest_${BASHPID}"
trap 'rm -f "${TMP_ERR_PREFIX}"* 2>/dev/null' EXIT

TEST_DESCRIPTIONS=()
TEST_COMMANDS=()

add_test() {
    local description="$1"
    local command_block
    command_block="$(cat)"
    TEST_DESCRIPTIONS+=("$description")
    TEST_COMMANDS+=("$command_block")
}

# --- Command resolution errors ---
add_test "Command not found" <<'CMDS'
command_should_not_exist
CMDS

add_test "Directory execution" <<'CMDS'
/tmp
CMDS

add_test "Pipeline missing command" <<'CMDS'
echo hi | command_should_not_exist
CMDS

# --- Execution permission / format issues ---
add_test "File without execute bit" <<'CMDS'
rm -f "${TMP_ERR_PREFIX}_noexec"
printf 'echo hi\n' > "${TMP_ERR_PREFIX}_noexec"
chmod 644 "${TMP_ERR_PREFIX}_noexec"
"${TMP_ERR_PREFIX}_noexec"
CMDS

add_test "Script missing interpreter" <<'CMDS'
rm -f "${TMP_ERR_PREFIX}_badinterp"
printf '#!/bin/doesnotexist\n' > "${TMP_ERR_PREFIX}_badinterp"
chmod +x "${TMP_ERR_PREFIX}_badinterp"
"${TMP_ERR_PREFIX}_badinterp"
CMDS

add_test "Read permission denied" <<'CMDS'
rm -f "${TMP_ERR_PREFIX}_noread"
printf 'secret' > "${TMP_ERR_PREFIX}_noread"
chmod 000 "${TMP_ERR_PREFIX}_noread"
cat "${TMP_ERR_PREFIX}_noread"
CMDS

# --- Filesystem / redirections ---
add_test "Missing file" <<'CMDS'
cat /file/does/not/exist
CMDS

add_test "Missing file via redirection" <<'CMDS'
cat < /file/does/not/exist
CMDS

add_test "Output redirection to directory" <<'CMDS'
echo hi > /
CMDS

add_test "Permission denied redirection" <<'CMDS'
rm -f "${TMP_ERR_PREFIX}_lock"
printf 'content' > "${TMP_ERR_PREFIX}_lock"
chmod 000 "${TMP_ERR_PREFIX}_lock"
echo hi > "${TMP_ERR_PREFIX}_lock"
CMDS

# --- Syntax parsing errors ---
add_test "Syntax pipe error" <<'CMDS'
echo | |
CMDS

add_test "Syntax redirection error" <<'CMDS'
echo > > file
CMDS

add_test "Unmatched double quote" <<'CMDS'
echo "unterminated
CMDS

add_test "Unmatched single quote" <<'CMDS'
echo 'unterminated
CMDS

# --- Builtins ---
add_test "Builtin cd nonexistent" <<'CMDS'
cd /nonexistent
CMDS

add_test "Builtin cd on regular file" <<'CMDS'
rm -f "${TMP_ERR_PREFIX}_file"
printf 'data' > "${TMP_ERR_PREFIX}_file"
cd "${TMP_ERR_PREFIX}_file"
CMDS

add_test "Exit non numeric" <<'CMDS'
exit notanumber
CMDS

add_test "Exit too many args" <<'CMDS'
exit 1 2
CMDS

add_test "Export invalid identifier" <<'CMDS'
export 1INVALID=oops
CMDS

add_test "Export empty name" <<'CMDS'
export ""
CMDS

add_test "Unset invalid identifier" <<'CMDS'
unset 1INVALID
CMDS

# --- External commands ---
add_test "Permission denied (system file)" <<'CMDS'
cat /root/.bashrc
CMDS

add_test "ls missing path" <<'CMDS'
ls /nonexistent
CMDS

printf "Checking minishell error exit codes vs bash...\n\n"

pass=0
fail=0
for idx in "${!TEST_DESCRIPTIONS[@]}"; do
    description="${TEST_DESCRIPTIONS[$idx]}"
    commands="${TEST_COMMANDS[$idx]}"

    cmd_file=$(mktemp)
    {
        printf 'export TMP_ERR_PREFIX=%q\n' "$TMP_ERR_PREFIX"
        printf '%s\n' "$commands"
        echo "exit"
    } > "$cmd_file"

    bash_out=$(mktemp)
    bash_err=$(mktemp)
    mini_out=$(mktemp)
    mini_err=$(mktemp)

    "$BASH_BIN" --noprofile --norc < "$cmd_file" >"$bash_out" 2>"$bash_err"
    bash_status=$?

    "$MINISHELL_BIN" < "$cmd_file" >"$mini_out" 2>"$mini_err"
    mini_status=$?

    result="FAIL"
    if [[ $bash_status -eq $mini_status ]]; then
        result="OK"
        pass=$((pass + 1))
    else
        fail=$((fail + 1))
    fi

    printf "[%02d] %-32s bash=%3d minishell=%3d -> %s\n" \
        "$((idx + 1))" "$description" "$bash_status" "$mini_status" "$result"

    if [[ "$result" == "FAIL" ]]; then
        echo "    bash stderr:"
        sed 's/^/        /' "$bash_err"
        echo "    minishell stderr:"
        sed 's/^/        /' "$mini_err"
    fi

    rm -f "$cmd_file" "$bash_out" "$bash_err" "$mini_out" "$mini_err"
    echo
done

printf "Summary: %d OK, %d FAIL\n" "$pass" "$fail"
exit $fail
