import json
import socket
import requests
import pjrpc
from pjrpc.client.backend import requests as pjrpc_client


def get_latest_blocks(_url):
    payload = {"jsonrpc": "2.0", "method": "get_latest_blocks", "id": 1}

    # response = requests.request(method='get_latest_blocks', url=_url)

    s = socket.socket()
    s.connect(('127.0.0.1', 5000))
    s.send(json.dumps(payload).encode('utf-8'))

    response = s.recv(1024)

    json_data = json.loads(response.text)
    sorted_obj = dict(json_data)
    sorted_obj['result'] = sorted(json_data['result'], key=lambda x: x['height'], reverse=True)

    return sorted_obj


def get_meta_info(_url):
    payload = {"jsonrpc": "2.0", "method": "get_meta_info", "id": 1}

    response = requests.post(url=_url, json=payload)
    json_data = json.loads(response.text)

    return json_data


def get_block_details(_url):
    payload = {"jsonrpc": "2.0", "method": "get_block_details", "id": 1}

    response = requests.post(url=_url, json=payload)
    json_data = json.loads(response.text)

    return json_data
