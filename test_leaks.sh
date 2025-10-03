#!/bin/bash
# Script de test complet pour les fuites mémoire du minishell
# Tests exhaustifs de tous les composants critiques

LOG_FILE="leak_test_$(date +%Y%m%d_%H%M%S).log"
SUMMARY_FILE="leak_summary.txt"

# Compteurs pour le résumé
TOTAL_TESTS=0
PASSED_TESTS=0
FAILED_TESTS=0

# Initialiser les fichiers de log
echo "=========================================" > "$LOG_FILE"
echo "   TESTS DE FUITES MEMOIRE - MINISHELL" >> "$LOG_FILE"
echo "   Date: $(date)" >> "$LOG_FILE"
echo "=========================================" >> "$LOG_FILE"
echo "" >> "$LOG_FILE"

echo "MINISHELL LEAK TESTS - $(date)" | tee -a "$LOG_FILE"
echo "Log file: $LOG_FILE"
echo ""

# Fonction pour tester une commande
test_leak() {
    local test_name="$1"
    local command="$2"
    
    TOTAL_TESTS=$((TOTAL_TESTS + 1))
    
    echo "Test #$TOTAL_TESTS: $test_name" | tee -a "$LOG_FILE"
    echo "Command: $command" >> "$LOG_FILE"
    
    # Créer un fichier temporaire pour la commande
    echo -e "$command\nexit" > /tmp/minishell_test.cmd
    
    # Exécuter avec valgrind - ignorer les "still reachable"
    valgrind --leak-check=full --show-leak-kinds=definite,indirect,possible \
             --error-exitcode=1 --quiet --track-origins=yes \
             ./minishell < /tmp/minishell_test.cmd > /dev/null 2> /tmp/valgrind_out
    
    local exit_code=$?
    
    # Vérifier aussi manuellement s'il y a des vraies fuites
    local real_leaks=$(grep -E "(definitely lost|indirectly lost|possibly lost)" /tmp/valgrind_out | grep -v "0 bytes" | wc -l)
    
    if [ $exit_code -eq 0 ] && [ $real_leaks -eq 0 ]; then
        echo "✓ PASS" | tee -a "$LOG_FILE"
        PASSED_TESTS=$((PASSED_TESTS + 1))
    else
        echo "✗ FAIL - Memory leak detected" | tee -a "$LOG_FILE"
        echo "--- Valgrind Output ---" >> "$LOG_FILE"
        cat /tmp/valgrind_out >> "$LOG_FILE"
        echo "--- Real leaks found: $real_leaks ---" >> "$LOG_FILE"
        echo "----------------------" >> "$LOG_FILE"
        FAILED_TESTS=$((FAILED_TESTS + 1))
    fi
    
    echo "" >> "$LOG_FILE"
    rm -f /tmp/minishell_test.cmd /tmp/valgrind_out
}

# Vérifier que le binaire existe
if [ ! -f "./minishell" ]; then
    echo "Error: ./minishell not found. Please compile first." | tee -a "$LOG_FILE"
    exit 1
fi

echo "Starting comprehensive leak tests..." | tee -a "$LOG_FILE"
echo "Note: 'Still reachable' blocks are ignored (not real leaks)" | tee -a "$LOG_FILE"
echo ""

# ===== TESTS BASIQUES =====
echo "=== TESTS BASIQUES ===" | tee -a "$LOG_FILE"
test_leak "Commande simple" "echo hello"
test_leak "Commande avec arguments" "echo hello world test"
test_leak "Commande inexistante" "command_not_found"
test_leak "Commande vide" ""
test_leak "Espaces seulement" "   "
test_leak "Tabulations" $'\t\t\t'

# ===== TESTS VARIABLES D'ENVIRONNEMENT =====
echo "=== TESTS VARIABLES ENVIRONNEMENT ===" | tee -a "$LOG_FILE"
test_leak "Export simple" "export TEST=value"
test_leak "Export sans valeur" "export TEST"
test_leak "Export écrasement" "export TEST=old; export TEST=new"
test_leak "Multiple exports" "export A=1; export B=2; export C=3"
test_leak "Unset variable" "export TEST=value; unset TEST"
test_leak "Unset inexistante" "unset NONEXISTENT"
test_leak "Export puis unset multiple" "export A=1; export B=2; unset A; unset B"
test_leak "Affichage env" "env"
test_leak "Export sans args" "export"
test_leak "Variable avec espaces" "export TEST='hello world'"
test_leak "Variable vide" "export TEST="

# ===== TESTS EXPANSION DE VARIABLES =====
echo "=== TESTS EXPANSION VARIABLES ===" | tee -a "$LOG_FILE"
test_leak "Variable simple" "echo \$USER"
test_leak "Variable inexistante" "echo \$NONEXISTENT"
test_leak "Exit status" "echo \$?"
test_leak "Variable dans quotes" 'echo "Hello $USER"'
test_leak "Variables multiples" "echo \$USER \$HOME \$PATH"
test_leak "Variable avec caractères spéciaux" "export SPECIAL='test@#\$%'; echo \$SPECIAL"

# ===== TESTS GUILLEMETS ET PARSING =====
echo "=== TESTS GUILLEMETS ET PARSING ===" | tee -a "$LOG_FILE"
test_leak "Double quotes" 'echo "hello world"'
test_leak "Single quotes" "echo 'hello world'"
test_leak "Quotes vides doubles" 'echo ""'
test_leak "Quotes vides simples" "echo ''"
test_leak "Mixed quotes" 'echo "hello '\''world'\''"'
test_leak "Guillemets non fermés doubles" 'echo "hello'
test_leak "Guillemets non fermés simples" "echo 'hello"
test_leak "Quotes imbriquées" 'echo "outer '\''inner'\'' outer"'

# ===== TESTS PIPES ET REDIRECTIONS =====
echo "=== TESTS PIPES ET REDIRECTIONS ===" | tee -a "$LOG_FILE"
test_leak "Pipe simple" "echo hello | cat"
test_leak "Pipe multiple" "echo hello | cat | grep hello"
test_leak "Pipe avec builtin" "export TEST=value | grep TEST"
test_leak "Redirection sortie" "echo hello > /tmp/minishell_test_out"
test_leak "Redirection append" "echo hello >> /tmp/minishell_test_out"
test_leak "Redirection entrée" "cat < /etc/passwd | head -1"
test_leak "Here document" 'cat << EOF\nhello\nworld\nEOF'
test_leak "Pipe vers fichier inexistant" "echo test | cat > /root/impossible"

# ===== TESTS BUILTINS =====
echo "=== TESTS BUILTINS ===" | tee -a "$LOG_FILE"
test_leak "cd valide" "cd /tmp"
test_leak "cd inexistant" "cd /nonexistent"
test_leak "cd sans args" "cd"
test_leak "cd avec tilde" "cd ~"
test_leak "pwd" "pwd"
test_leak "echo builtin" "echo test"
test_leak "echo avec options" "echo -n test"

# ===== TESTS EXECUTION COMMANDES =====
echo "=== TESTS EXECUTION COMMANDES ===" | tee -a "$LOG_FILE"
test_leak "Commande absolue" "/bin/echo hello"
test_leak "Commande dans PATH" "ls /tmp"
test_leak "Commande avec args multiples" "ls -la /tmp"
test_leak "Commande script" "sh -c 'echo test'"

# ===== TESTS GESTION ERREURS =====
echo "=== TESTS GESTION ERREURS ===" | tee -a "$LOG_FILE"
test_leak "Permission denied" "cat /root/.bashrc"
test_leak "Directory as command" "/tmp"
test_leak "Syntaxe pipe invalide" "echo | |"
test_leak "Redirection invalide" "echo > > file"
test_leak "Fichier inexistant" "cat /file/does/not/exist"

# ===== TESTS SIGNAUX =====
echo "=== TESTS SIGNAUX ===" | tee -a "$LOG_FILE"
test_leak "Commande longue" "sleep 0.1"
test_leak "Pipe avec sleep" "sleep 0.1 | cat"

# ===== TESTS STRESS ET LIMITES =====
echo "=== TESTS STRESS ===" | tee -a "$LOG_FILE"

# Test stress variables
stress_vars=""
for i in {1..50}; do
    stress_vars="$stress_vars export VAR$i=value$i;"
done
test_leak "Stress export" "$stress_vars"

# Test stress unset
stress_unset=""
for i in {1..50}; do
    stress_unset="$stress_unset unset VAR$i;"
done
test_leak "Stress unset" "$stress_vars $stress_unset"

# Test longue ligne
long_command="echo "
for i in {1..200}; do
    long_command="$long_command word$i"
done
test_leak "Commande très longue" "$long_command"

# Test pipes multiples
pipe_command="echo test"
for i in {1..10}; do
    pipe_command="$pipe_command | cat"
done
test_leak "Pipes multiples" "$pipe_command"

# ===== TESTS SCENARIOS COMPLEXES =====
echo "=== TESTS SCENARIOS COMPLEXES ===" | tee -a "$LOG_FILE"
test_leak "Scenario complet" "export TEST=hello; echo \$TEST; export TEST=world; echo \$TEST; unset TEST; echo \$TEST"
test_leak "PATH modification" "export PATH=/bin:/usr/bin; ls >/dev/null; export PATH=; command_not_found 2>/dev/null || echo 'no path'"
test_leak "Combinaison pipes+vars" "export MSG=hello; echo \$MSG | cat | grep hello"
test_leak "Redirections multiples" "echo test > /tmp/test1; cat < /tmp/test1 > /tmp/test2; cat /tmp/test2"

# ===== TESTS EDGE CASES =====
echo "=== TESTS EDGE CASES ===" | tee -a "$LOG_FILE"
test_leak "Dollar seul" "echo \$"
test_leak "Question mark seule" "echo ?"
test_leak "Backslash" "echo \\"
test_leak "Caractères spéciaux" "echo @#%^&*()"
test_leak "Unicode basic" "echo héllo"
test_leak "Très long nom variable" "export $(printf 'A%.0s' {1..100})=test"

# ===== NETTOYAGE =====
rm -f /tmp/minishell_test_out /tmp/test1 /tmp/test2

# ===== GENERER LE RESUME =====
echo "=========================================" | tee -a "$LOG_FILE"
echo "              RESUME FINAL" | tee -a "$LOG_FILE"
echo "=========================================" | tee -a "$LOG_FILE"

{
    echo "RESUME DES TESTS DE FUITES MEMOIRE"
    echo "=================================="
    echo "Date: $(date)"
    echo "Total des tests: $TOTAL_TESTS"
    echo "Tests réussis: $PASSED_TESTS"
    echo "Tests échoués: $FAILED_TESTS"
    echo ""
    echo "Note: Les blocs 'still reachable' sont ignorés (pas de vraies fuites)"
    echo ""
    
    if [ $FAILED_TESTS -eq 0 ]; then
        echo "🎉 TOUS LES TESTS SONT PASSES! Aucune fuite détectée."
    else
        echo "⚠️  $FAILED_TESTS test(s) ont échoué avec des fuites mémoire."
        echo "Consultez le fichier $LOG_FILE pour les détails."
    fi
    
    echo ""
    echo "Taux de réussite: $(( PASSED_TESTS * 100 / TOTAL_TESTS ))%"
    echo ""
    echo "Fichiers générés:"
    echo "- Log détaillé: $LOG_FILE"
    echo "- Résumé: $SUMMARY_FILE"
    
} | tee "$SUMMARY_FILE" | tee -a "$LOG_FILE"

echo ""
echo "Tests terminés. Consultez '$SUMMARY_FILE' pour le résumé."