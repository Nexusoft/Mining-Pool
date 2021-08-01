import json
import requests
from django.shortcuts import render, redirect
from django.contrib import messages
from django.views.generic import TemplateView
from .tables import OverviewTable, AccountWorksTable, AccountPayoutsTable
from .forms import WalletSearchForm
from .rpc_requests import get_latest_blocks, get_meta_info, socket_connect, socket_disconnect, get_account_header, \
    get_account_works, get_account_payouts


def block_overview_list(request):
    template_name = 'presenter/overview_list.html'

    # Todo Get from .env
    try:
        socket = socket_connect(_ip='127.0.0.1', _port=5000)

        # Get the Data for the Main Table
        latest_block_json = get_latest_blocks(_socket=socket)
        table_data = OverviewTable(latest_block_json['result'])

        print(latest_block_json)

        # Get the Meta Info
        meta_info_json = get_meta_info(_socket=socket)
        pool_hashrate = meta_info_json['result']['pool_hashrate']
        network_hashrate = meta_info_json['result']['network_hashrate']
        payout_threshold = meta_info_json['result']['payout_threshold']
        fee = meta_info_json['result']['fee']

        print(meta_info_json)

        socket_disconnect(_socket=socket)

        return render(request, template_name, {'table': table_data,
                                               'pool_hashrate': pool_hashrate,
                                               'network_hashrate': network_hashrate,
                                               'payout_threshold': payout_threshold,
                                               'fee': fee,
                                               })

    except Exception as ex:
        # Todo log Exception
        print(ex)
        messages.error(request, 'Could not establish connection to the Backend Server')
        return redirect('presenter:error')


class ErrorView(TemplateView):
    template_name = 'presenter/error.html'


def wallet_detail(request):
    template_name = 'presenter/wallet_detail.html'

    form = WalletSearchForm(request.POST)

    if not form.is_valid():
        print(f"Form is invalid")
        errors_json = json.loads(form.errors.as_json())
        error_message = errors_json['__all__'][0]['message']
        messages.error(request, error_message)
        return redirect('presenter:index')

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

    account_payouts_json = json.dumps({'id': 1, 'jsonrpc': '2.0', 'result': [
        {'time': 15, 'amount': 5, 'state': 10, 'txhash': 10},
        {'time': 15, 'amount': 5, 'state': 10, 'txhash': 10},
        {'time': 15, 'amount': 5, 'state': 10, 'txhash': 10},
    ]})

    account_payouts_json = json.loads(account_payouts_json)

    table_account_works = AccountWorksTable(account_works_json['result'])
    table_account_payouts = AccountPayoutsTable(account_payouts_json['result'])

    return render(request, template_name, {'wallet_id': wallet_id,
                                           'last_day_recv': last_day_recv,
                                           'unpaid_balance': unpaid_balance,
                                           'total_revenue': total_revenue,
                                           'table_account_works': table_account_works,
                                           'table_account_payouts': table_account_payouts
                                           })


def block_detail(request, hash):
    return redirect(f'https://explorer.nexus.io/search/{hash}')

