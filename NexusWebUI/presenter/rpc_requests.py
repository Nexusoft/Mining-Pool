import json
import logging
import socket
import requests
import pybson
import bson

logger = logging.getLogger('NexusWebUI')

def socket_connect(_ip, _port):
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect((_ip, _port))

    # Todo Log Message
    print("Opened connection")
    logger.info("Opened Connection to Backend")

    return s


def socket_disconnect(_socket):
    # Todo Log Message
    print("Closed connection")

    _socket.close()


def get_latest_blocks(_socket):
    # Todo Documentation
    # Todo Error Handling

    payload = {"jsonrpc": "2.0", "method": "get_latest_blocks", "id": 1}

    _socket.send(pybson.dumps(payload))
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
    """
    Todo Link to external block detail page: https://explorer.nexus.io/search/3946243
    :param _url:
    :return:
    """
    payload = {"jsonrpc": "2.0", "method": "get_block_details", "id": 1}

    response = requests.post(url=_url, json=payload)
    json_data = json.loads(response.text)

    return json_data


def get_account(_socket, _account):
    """
    Checks if the Account actually exists
    :param _url:
    :return:
    """
    payload = {"jsonrpc": "2.0", "method": "get_account", "params": [_account], "id": 1}

    _socket.send(pybson.dumps(payload))
    response = _socket.recv(1024)
    json_data = bson.loads(response)

    return json_data


def get_account_header(_socket, _account):
    """
    :param _url:
    :return:
    """
    payload = {"jsonrpc": "2.0", "method": "get_account_header", "params": [_account], "id": 1}

    _socket.send(pybson.dumps(payload))
    response = _socket.recv(1024)
    json_data = bson.loads(response)

    return json_data


def get_account_works(_socket):
    # Todo Documentation
    # Todo Error Handling

    payload = {"jsonrpc": "2.0", "method": "get_account_works", "params": [_account], "id": 1}

    _socket.send(pybson.dumps(payload))
    response = _socket.recv(20000)
    json_data = bson.loads(response)

    return json_data


def get_account_payouts(_socket, _account):
    # Todo Documentation
    # Todo Error Handling

    payload = {"jsonrpc": "2.0", "method": "get_account_payouts", "params": [_account], "id": 1}

    _socket.send(pybson.dumps(payload))
    response = _socket.recv(20000)
    json_data = bson.loads(response)

    return json_data