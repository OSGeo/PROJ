# Hashbang deliberately missing because this file should be sourced, not executed

function_exists() {
    declare -f -F "$1" > /dev/null
    return $?
}

# Checks that bash-completion is recent enough
function_exists _get_comp_words_by_ref || return 0

_projinfo()
{
  local cur prev
  COMPREPLY=()
  _get_comp_words_by_ref cur prev
  choices=$(projinfo completion ${COMP_LINE})
  if [[ "$cur" == "=" ]]; then
    mapfile -t COMPREPLY < <(compgen -W "$choices" --)
  elif [[ "$cur" == ":" ]]; then
    mapfile -t COMPREPLY < <(compgen -W "$choices" --)
  elif [[ "${cur: -1}" == "/" ]]; then
    mapfile -t COMPREPLY < <(compgen -W "$choices" --)
  elif [[ "${cur: -2}" == "/ " ]]; then
    mapfile -t COMPREPLY < <(compgen -W "$choices" --)
  elif [[ "${cur: -1}" == "+" ]]; then
    mapfile -t COMPREPLY < <(compgen -W "$choices" --)
  elif [[ "${cur: -2}" == "+ " ]]; then
    mapfile -t COMPREPLY < <(compgen -W "$choices" --)
  elif [[ "$cur" == "!" ]]; then
    mapfile -t COMPREPLY < <(compgen -W "$choices" -P "! " --)
  else
    mapfile -t COMPREPLY < <(compgen -W "$choices" -- "$cur")
  fi
  for element in "${COMPREPLY[@]}"; do
    if [[ $element == */ ]]; then
      # Do not add a space if one of the suggestion ends with slash
      compopt -o nospace
      break
    elif [[ $element == *= ]]; then
      # Do not add a space if one of the suggestion ends with equal
      compopt -o nospace
      break
    elif [[ $element == *: ]]; then
      # Do not add a space if one of the suggestion ends with colon
      compopt -o nospace
      break
    fi
  done
}
complete -o default -F _projinfo projinfo

