@echo off
if not exist .git\hooks\pre-commit (
    echo Copy pre-commit to git
    copy git-hooks\pre-commit .git\hooks
) else (
    echo Update pre-commit to git
    copy git-hooks\pre-commit .git\hooks
)

if not exist .git\hooks\post-checkout (
    echo Copy post-checkout to git
    copy git-hooks\post-checkout .git\hooks
) else (
    echo Update post-checkout to git
    copy git-hooks\post-checkout .git\hooks
)
