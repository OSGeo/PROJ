#!/bin/bash

openssl aes-256-cbc -K $encrypted_a32de4b30e70_key -iv $encrypted_a32de4b30e70_iv -in travis/projdocs-private.key.enc -out travis/projdocs-private.key -d
cp travis/projdocs-private.key ~/.ssh/id_rsa
chmod 600 ~/.ssh/id_rsa
echo -e "Host *\n\tStrictHostKeyChecking no\n" > ~/.ssh/config


