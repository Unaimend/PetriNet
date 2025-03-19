#!/bin/bash --login

# >>> mamba initialize >>>
# !! Contents within this block are managed by 'mamba init' !!
export MAMBA_EXE="/home/me/.local/bin/micromamba";
export MAMBA_ROOT_PREFIX="/home/me/micromamba";
__mamba_setup="$('/home/me/.local/bin/micromamba' shell hook --shell bash --prefix '/home/me/micromamba' 2> /dev/null)"
if [ $? -eq 0 ]; then
    eval "$__mamba_setup"
else
    if [ -f "/home/me/micromamba/etc/profile.d/micromamba.sh" ]; then
        . "/home/me/micromamba/etc/profile.d/micromamba.sh"
    else
        export  PATH="/home/me/micromamba/bin:$PATH"  # extra space after export prevents interference from conda init
    fi
fi
unset __mamba_setup
# <<< mamba initialize <<<

micromamba activate simulator

export CMAKE_EXEC=$(type -p cmake)
