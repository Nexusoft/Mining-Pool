import json
import socket
import requests
import pybson
import bson


def get_latest_blocks(_url):
    payload = {"jsonrpc": "2.0", "method": "get_latest_blocks", "id": 1}

    s = socket.socket()
    s.connect(('127.0.0.1', 5000))
    s.send(pybson.dumps(payload))

    response = s.recv(20000)

    json_data = bson.loads(response)

    print(f"Result: {json_data['result']}")

    return json_data


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
