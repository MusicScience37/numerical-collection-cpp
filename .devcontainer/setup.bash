#!/bin/bash

sudo chmod 0777 /cache_volume/
mkdir -p $CCACHE_DIR
mkdir -p $VCPKG_DEFAULT_BINARY_CACHE
mkdir -p $POETRY_CACHE_DIR

poetry config virtualenvs.in-project true
poetry env use 3.13
poetry install

poetry run pre-commit install

poetry run playwright install --only-shell --with-deps chromium

poetry run plotly_get_chrome -y

git config commit.template .gitmessage

git config gpg.program gpg2
git config commit.gpgsign true
git config tag.gpgsign true

echo "source /usr/share/bash-completion/completions/git" >>~/.bashrc

echo "source /workspaces/numerical-collection-cpp/.devcontainer/set_num_threads.bash" >>~/.bashrc
