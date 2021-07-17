import json

import requests


def get_latest_blocks(_url):
    payload = {"jsonrpc": "2.0", "method": "get_latest_blocks", "id": 1}

    response = requests.post(url=_url, json=payload)
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
