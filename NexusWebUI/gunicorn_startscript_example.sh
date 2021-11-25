#!/bin/sh
cd /home/pool/NexusWebUI

/home/pool/NexusWebUI/venv/bin/gunicorn --workers 1 --timeout 5000 --bind unix:/home/pool/NexusWebUI/nexuswebui.sock NexusWebUI.wsgi