import collections
import json
import logging
import requests
from collections import OrderedDict
from django.shortcuts import render, redirect
from django.contrib import messages
from django.views.generic import TemplateView
from .tables import OverviewTable, AccountWorksTable, AccountPayoutsTable
from .forms import WalletSearchForm
from .rpc_requests import get_latest_blocks, get_meta_info, socket_connect, socket_disconnect, get_account, \
    get_account_works, get_account_payouts, get_account_header
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
            logger.info("Rebuilding Cache")
            print("Rebuilding Cache")

            socket = socket_connect(_ip=getattr(settings, "POOL_IP", None),
                                    _port=getattr(settings, "POOL_PORT", None))

            # Get the Data for the Main Table
            block_overview_latest_json = get_latest_blocks(_socket=socket)

            # Extract the necessary data as list of dicts
            block_overview_latest_list = block_overview_latest_json['result']

            # Add the new Hash column to show only a part of the hash
            for item in block_overview_latest_list:
                item.update({"hash_short": item['hash'][:10] + '...' + item['hash'][-10:]})

            # Sort the Block Data
            block_overview_latest_list_dict = sorted(block_overview_latest_list,
                                                     key=lambda i: i['height'], reverse=True)

            print("Blocks Received: ", block_overview_latest_json)

            # Get the Meta Info
            block_overview_meta_json = get_meta_info(_socket=socket)
            print("Block Overview: ", block_overview_meta_json)

            # Save data in the cache
            if settings.DEBUG is False:
                cache.set('block_overview_latest_json', block_overview_latest_json, 1)
                cache.set('block_overview_meta_json', block_overview_meta_json, 1)
        else:
            logger.info("Serving from Cache")
            print("Serving from Cache")

        # Main Table
        # Todo Sort JSON by height
        # table_data = OverviewTable(block_overview_latest_json['result'])
        table_data = OverviewTable(block_overview_latest_list_dict)

        # Meta Table
        pool_hashrate = block_overview_meta_json['result']['pool_hashrate']
        mining_mode = block_overview_meta_json['result']['mining_mode']
        round_duration = block_overview_meta_json['result']['round_duration']
        fee = block_overview_meta_json['result']['fee']

        return render(request, template_name, {'table': table_data,
                                               'pool_hashrate': pool_hashrate,
                                               'mining_mode': mining_mode,
                                               'round_duration': round_duration,
                                               'fee': fee,
                                               })

    except Exception as ex:
        print(ex)
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
    # Todo get no result from backend!?

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
        account_json = get_account(_socket=socket, _account=wallet_id)
        print("account_json json: ", account_json)

        if None in account_json['result']:
            print("Received no Result for Wallet Request")
            logger.error(f"Received no Result for Wallet Request with ID: {wallet_id}")
            raise Exception(f"Received no Result for Wallet Request with ID: {wallet_id}")

        # last_day_recv = account_header_json['result']['last_day_recv']
        # unpaid_balance = account_header_json['result']['unpaid_balance']
        account = account_json['result']['account']
        created_at = account_json['result']['created_at']
        last_active = account_json['result']['last_active']
        shares = account_json['result']['shares']
        hashrate = account_json['result']['hashrate']

        last_active = last_active[:19]

        # Get the Account Payouts List
        get_account_payouts_json = get_account_payouts(_socket=socket, _account=wallet_id)

        # Extract the necessary data as list of dicts
        get_account_payouts_list = get_account_payouts_json['result']

        # Add the new Hash column to show only a part of the hash
        for item in get_account_payouts_list:
            item.update({"txhash": item['txhash'][:10] + '...' + item['txhash'][-10:]})

        # Sort the Block Data
        get_account_payouts_list = sorted(get_account_payouts_list,
                                          key=lambda i: i['time'], reverse=True)

        get_account_payouts_table = AccountPayoutsTable(get_account_payouts_list)

        print("get_account_payouts_json: ", get_account_payouts_json)

        socket_disconnect(_socket=socket)

        return render(request, template_name, {'wallet_id': wallet_id,
                                               # 'last_day_recv': last_day_recv,
                                               # 'unpaid_balance': unpaid_balance,
                                               'account': account,
                                               'created_at': created_at,
                                               'last_active': last_active,
                                               'shares': shares,
                                               'hashrate': hashrate,
                                               # 'table_account_works': account_works_table,
                                               'table_account_payouts': get_account_payouts_table
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
