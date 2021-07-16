import json
import requests
from django.shortcuts import render
from presenter.tables import OverviewTable
from presenter.rpc_requests import get_latest_blocks, get_meta_info


# INDEX
def index(request):
    template_name = 'presenter/presenter_index.html'

    url = "http://127.0.0.1:5000/"

    # Get the Data for the Main Table
    latest_block_json = get_latest_blocks(_url=url)
    table_data = OverviewTable(latest_block_json['result'])

    # Get the Meta Info
    meta_info_json = get_meta_info(_url=url)
    pool_hashrate = meta_info_json['result'][0]['pool_hashrate']
    network_hashrate = meta_info_json['result'][0]['network_hashrate']
    payout_threshold = meta_info_json['result'][0]['payout_threshold']
    fee = meta_info_json['result'][0]['fee']

    return render(request, template_name, {'table': table_data,
                                           'pool_hashrate': pool_hashrate,
                                           'network_hashrate': network_hashrate,
                                           'payout_threshold': payout_threshold,
                                           'fee': fee,
                                           })
