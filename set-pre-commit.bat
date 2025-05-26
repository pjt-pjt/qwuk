@echo off
if not exist .git\hooks\pre-commit (
    echo Copy pre-commit to git
    copy git-hooks\pre-commit .git\hooks
) else (
    echo Update pre-commit to git
    copy git-hooks\pre-commit .git\hooks
)

