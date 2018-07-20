#!/bin/bash
openssl aes-256-cbc -K $encrypted_e3bb6b75f43b_key -iv $encrypted_e3bb6b75f43b_iv -in travis/projdocs-private.key.enc -out travis/projdocs-private.key -d
cp travis/projdocs-private.key ~/.ssh/id_rsa
chmod 600 ~/.ssh/id_rsa
echo -e "Host *\n\tStrictHostKeyChecking no\n" > ~/.ssh/config


