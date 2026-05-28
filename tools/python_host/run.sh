#!/usr/bin/bash

if [ -d .venv ]
then
    source .venv/bin/activate
else
    python -m venv .venv
    source .venv/bin/activate
    python -m pip install -r requirements.txt
fi

python3 main.py
