import json
import socket
import requests
import pybson
import bson


def socket_connect(_ip, _port):

    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect((_ip, _port))
    return s



def socket_disconnect(_socket):
    _socket.close()


def get_latest_blocks(_socket):
    # Todo Documentation
    # Todo Error Handling

    payload = {"jsonrpc": "2.0", "method": "get_latest_blocks", "id": 1}

    # s = socket.socket()
    # s.connect(('127.0.0.1', 5000))
    # s.send(pybson.dumps(payload))
    _socket.send(pybson.dumps(payload))

    # response = s.recv(20000)
    response = _socket.recv(20000)
    json_data = bson.loads(response)

    return json_data


def get_meta_info(_socket):
    payload = {"jsonrpc": "2.0", "method": "get_meta_info", "id": 1}

    _socket.send(pybson.dumps(payload))
    response = _socket.recv(1024)
    json_data = bson.loads(response)

    return json_data


def get_block_details(_url):
    payload = {"jsonrpc": "2.0", "method": "get_block_details", "id": 1}

    response = requests.post(url=_url, json=payload)
    json_data = json.loads(response.text)

    return json_data
