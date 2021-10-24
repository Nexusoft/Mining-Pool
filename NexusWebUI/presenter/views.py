import json
import logging

import requests
from django.shortcuts import render, redirect
from django.contrib import messages
from django.views.generic import TemplateView
from .tables import OverviewTable, AccountWorksTable, AccountPayoutsTable
from .forms import WalletSearchForm
from .rpc_requests import get_latest_blocks, get_meta_info, socket_connect, socket_disconnect, get_account_header, \
    get_account_works, get_account_payouts
from django.conf import settings
from django.core.cache import cache

logger = logging.getLogger('NexusWebUI')


def block_overview_list(request):
    template_name = 'presenter/overview_list.html'
    socket = None

    # Todo Test Cache when Backend not available

    try:
        # Try to retrieve from cache
        block_overview_latest_json = cache.get('block_overview_latest_json')
        block_overview_meta_json = cache.get('block_overview_meta_json')

        if not block_overview_latest_json or not block_overview_meta_json:

            socket = socket_connect(_ip=getattr(settings, "POOL_IP", None),
                                    _port=getattr(settings, "POOL_PORT", None))

            # Get the Data for the Main Table
            block_overview_latest_json = get_latest_blocks(_socket=socket)

            # Get the Meta Info
            block_overview_meta_json = get_meta_info(_socket=socket)

            # Save data in the cache
            cache.set('block_overview_latest_json', block_overview_latest_json, 30)
            cache.set('block_overview_meta_json', block_overview_meta_json, 30)

        # Main Table
        table_data = OverviewTable(block_overview_latest_json['result'])

        # Meta Table
        pool_hashrate = block_overview_meta_json['result']['pool_hashrate']
        network_hashrate = block_overview_meta_json['result']['network_hashrate']
        payout_threshold = block_overview_meta_json['result']['payout_threshold']
        fee = block_overview_meta_json['result']['fee']

        return render(request, template_name, {'table': table_data,
                                               'pool_hashrate': pool_hashrate,
                                               'network_hashrate': network_hashrate,
                                               'payout_threshold': payout_threshold,
                                               'fee': fee,
                                               })

    except Exception as ex:
        logger.error(ex)
        return redirect('presenter:error_pool')
    finally:
        if socket:
            socket_disconnect(_socket=socket)


class ErrorView(TemplateView):
    template_name = 'presenter/error.html'


class ErrorPoolView(TemplateView):
    template_name = 'presenter/errors/error_pool_not_available.html'


class ErrorInvalidWalletIdView(TemplateView):
    template_name = 'presenter/errors/error_invalid_wallet_id.html'


class ErrorUnknownWallet(TemplateView):
    template_name = 'presenter/errors/error_unknown_wallet.html'


def wallet_detail(request):
    # Todo implement Cache

    template_name = 'presenter/wallet_detail.html'
    socket = None

    try:
        form = WalletSearchForm(request.POST)

        if not form.is_valid():
            print(f"Form is invalid")
            errors_json = json.loads(form.errors.as_json())
            wallet_error = errors_json['__all__'][0]['message']
            if wallet_error == -11:
                return redirect('presenter:error_wallet_id')
            else:
                return redirect('presenter:error_wallet_id')

        wallet_id = request.POST.get('wallet_id')

        socket = socket_connect(_ip=getattr(settings, "POOL_IP", None),
                                _port=getattr(settings, "POOL_PORT", None))

        # Get the Account Detail Page Header Information
        account_header_json = get_account_header(_socket=socket, _account=wallet_id)

        if None in account_header_json['result']:
            print("Received no Result for Wallet Request")
            logger.error(f"Received no Result for Wallet Request with ID: {wallet_id}")
            raise Exception(f"Received no Result for Wallet Request with ID: {wallet_id}")

        last_day_recv = account_header_json['result']['last_day_recv']
        unpaid_balance = account_header_json['result']['unpaid_balance']
        total_revenue = account_header_json['result']['total_revenue']

        # Get the Account Works List
        account_works_json = get_account_works(_socket=socket, _account=wallet_id)
        account_works_table = AccountWorksTable(account_works_json['result'])

        # Get the Account Payouts List
        account_payouts_json = get_account_payouts(_socket=socket, _account=wallet_id)
        account_payouts_table = AccountPayoutsTable(account_payouts_json['result'])

        socket_disconnect(_socket=socket)

        # # Test Data
        # last_day_recv = 5
        # unpaid_balance = 10
        # total_revenue = 0.2341234
        #
        # account_works_json = json.dumps({'id': 1, 'jsonrpc': '2.0', 'result': [
        #     {'id': 1, 'status': 'bla', 'hslast10': 5, 'hslast1d': 10, 'lastshare': 1, 'rejectratio': 5},
        #     {'id': 2, 'status': 'bla', 'hslast10': 5, 'hslast1d': 10, 'lastshare': 1, 'rejectratio': 5},
        #     {'id': 3, 'status': 'bla', 'hslast10': 5, 'hslast1d': 10, 'lastshare': 1, 'rejectratio': 5},
        # ]})
        #
        # account_works_json = json.loads(account_works_json)
        #
        # account_payouts_json = json.dumps({'id': 1, 'jsonrpc': '2.0', 'result': [
        #     {'time': 15, 'amount': 5, 'state': 10, 'txhash': 10},
        #     {'time': 15, 'amount': 5, 'state': 10, 'txhash': 10},
        #     {'time': 15, 'amount': 5, 'state': 10, 'txhash': 10},
        # ]})
        # account_payouts_json = json.loads(account_payouts_json)
        #
        # table_account_works = AccountWorksTable(account_works_json['result'])
        # table_account_payouts = AccountPayoutsTable(account_payouts_json['result'])
        # return render(request, template_name, {'wallet_id': wallet_id,
        #                                        'last_day_recv': last_day_recv,
        #                                        'unpaid_balance': unpaid_balance,
        #                                        'total_revenue': total_revenue,
        #                                        'table_account_works': table_account_works,
        #                                        'table_account_payouts': table_account_payouts
        #                                        })

        return render(request, template_name, {'wallet_id': wallet_id,
                                               'last_day_recv': last_day_recv,
                                               'unpaid_balance': unpaid_balance,
                                               'total_revenue': total_revenue,
                                               'table_account_works': account_works_table,
                                               'table_account_payouts': account_payouts_table
                                               })

    except Exception as ex:
        print("Exception when trying to fetch Wallet Info: ", ex)
        logger.error(ex)
        return redirect('presenter:error_pool')
    finally:
        if socket:
            socket_disconnect(_socket=socket)


def block_detail(request, hash):
    return redirect(f'https://explorer.nexus.io/search/{hash}')

