import json
import requests
from django.shortcuts import render
from django.views.generic import TemplateView

from .tables import OverviewTable, AccountWorksTable, AccountPayoutsTable
from .rpc_requests import get_latest_blocks, get_meta_info, socket_connect, socket_disconnect, get_account_header, \
    get_account_works, get_account_payouts


# INDEX
class IndexView(TemplateView):
    template_name = 'presenter/presenter_index.html'
    extra_context = {"NonVoiceIndex": 'active'}


def block_overview_list(request):
    template_name = 'presenter/overview_list.html'
    # Todo Get from .env
    url = "http://127.0.0.1:5000/"

    socket = socket_connect(_ip='127.0.0.1', _port=5000)

    # Todo if Socket = None send Message to User

    # Get the Data for the Main Table
    latest_block_json = get_latest_blocks(_socket=socket)
    print(latest_block_json)
    print(type(latest_block_json))
    table_data = OverviewTable(latest_block_json['result'])

    # Get the Meta Info
    meta_info_json = get_meta_info(_socket=socket)
    pool_hashrate = meta_info_json['result']['pool_hashrate']
    network_hashrate = meta_info_json['result']['network_hashrate']
    payout_threshold = meta_info_json['result']['payout_threshold']
    fee = meta_info_json['result']['fee']

    socket_disconnect(_socket=socket)

    return render(request, template_name, {'table': table_data,
                                           'pool_hashrate': pool_hashrate,
                                           'network_hashrate': network_hashrate,
                                           'payout_threshold': payout_threshold,
                                           'fee': fee,
                                           })


def block_detail(request, block_id):
    template_name = 'presenter/block_detail.html'
    # Todo Get from .env
    url = "http://127.0.0.1:5000/"

    # Todo get Infos from Method
    # block = '12345'
    # hash = '23421342353434gjhgjk34kg51345f345jkf341j5jf435f435hfjhff'
    # proof_hash = 'aergearger34t3ghtjg34i8t6guz4t34tk787g43kq3tuk347t8uig43'
    # reward = 2.33
    # time = '2021-01-01 00:00:00 UTC'

    json_object = block_id

    return render(request, template_name, {'json_object': json_object})

    # return render(request, template_name, {'block': block,
    #                                        'hash': hash,
    #                                        'proof_hash': proof_hash,
    #                                        'reward': reward,
    #                                        'time': time,
    #                                        'channel',
    #                                        'difficulty',
    #                                        'none',
    #                                        'bits',
    #                                        'size',
    #                                        'version',
    #                                        'previous',
    #                                        'next',
    #                                        'merkle',
    #                                        })


def wallet_detail(request):
    template_name = 'presenter/wallet_detail.html'

    # Todo Get from .env
    url = "http://127.0.0.1:5000/"

    wallet_id = request.POST.get('wallet_id')
    # Todo if wallet ID is not valid (from get_account)
    #  --> return message to user and redirect to overview

    # Todo Get Infos for Wallet (params)

    # socket = socket_connect(_ip='127.0.0.1', _port=5000)

    # Get the Account Detail Page Header Information
    # account_header_json = get_account_header(_socket=socket)
    # last_day_recv = account_header_json['result']['last_day_recv']
    # unpaid_balance = account_header_json['result']['unpaid_balance']
    # total_revenue = account_header_json['result']['total_revenue']

    # Get the Account Works List
    # account_works_json = get_account_works(_socket=socket)
    # account_works_table = AccountWorksTable(account_works_json['result'])

    # Get the Account Payouts List
    # account_payouts_json = get_account_payouts(_socket=socket)
    # account_payouts_table = AccountPayoutsTable(account_payouts_json['result'])


    # Test Data
    last_day_recv = 5
    unpaid_balance = 10
    total_revenue = 0.2341234

    account_works_json = json.dumps({'id': 1, 'jsonrpc': '2.0', 'result': [
        {'id': 1, 'status': 'bla', 'hslast10': 5, 'hslast1d': 10, 'lastshare': 1, 'rejectratio': 5},
        {'id': 2, 'status': 'bla', 'hslast10': 5, 'hslast1d': 10, 'lastshare': 1, 'rejectratio': 5},
        {'id': 3, 'status': 'bla', 'hslast10': 5, 'hslast1d': 10, 'lastshare': 1, 'rejectratio': 5},
    ]})

    account_works_json = json.loads(account_works_json)

    print(account_works_json['result'])
    print(type(account_works_json))
    table_account_works = AccountWorksTable(account_works_json['result'])
    print(type(table_account_works))

    return render(request, template_name, {'wallet_id': wallet_id,
                                           'last_day_recv': last_day_recv,
                                           'unpaid_balance': unpaid_balance,
                                           'total_revenue': total_revenue,
                                           'table_account_works': table_account_works
                                           })

