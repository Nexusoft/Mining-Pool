import collections
import json
import logging
import math

import requests
from collections import OrderedDict
from django.shortcuts import render, redirect
from django.contrib import messages
from django.views.generic import TemplateView
from .tables import OverviewTable, AccountPayoutsTable
from .forms import WalletSearchForm, CalcForm
from .rpc_requests import get_latest_blocks, get_meta_info, socket_connect, socket_disconnect, get_account, \
    get_account_works, get_account_payouts, get_account_header
from django.conf import settings
from django.core.cache import cache
from .helpers import get_exchange_rate_nxs2usd

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

            print("block_overview_latest_json: ", block_overview_latest_json)

            # Extract the necessary data as list of dicts & Sort the Block Data

            block_overview_latest_list = block_overview_latest_json['result']
            block_overview_latest_list_dict = sorted(block_overview_latest_list,
                                                     key=lambda i: i['height'], reverse=True)

            # print("Blocks Received: ", block_overview_latest_json)

            # Get the Meta Info
            block_overview_meta_json = get_meta_info(_socket=socket)
            print("Block Overview: ", block_overview_meta_json)

            # Save data in the cache
            if settings.DEBUG is False and block_overview_latest_json and block_overview_meta_json:
                cache.set('block_overview_latest_json', block_overview_latest_json, 1)
                cache.set('block_overview_meta_json', block_overview_meta_json, 1)
        else:
            logger.info("Serving from Cache")
            print("Serving from Cache")

        # Main Table
        # table_data = OverviewTable(block_overview_latest_json['result'])
        table_data = OverviewTable(block_overview_latest_list_dict)

        # Meta Table
        pool_hashrate = round((float(block_overview_meta_json['result']['pool_hashrate']) / 1000000000), 2)
        mining_mode = block_overview_meta_json['result']['mining_mode']
        round_duration = block_overview_meta_json['result']['round_duration']
        fee = block_overview_meta_json['result']['fee']
        active_miners = block_overview_meta_json['result']['active_miners']

        return render(request, template_name, {'table': table_data,
                                               'pool_hashrate': pool_hashrate,
                                               'mining_mode': mining_mode,
                                               'round_duration': round_duration,
                                               'active_miners': active_miners,
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

        account = account_json['result']['account']
        created_at = account_json['result']['created_at']
        last_active = account_json['result']['last_active']
        shares = account_json['result']['shares']
        hashrate = round((float(account_json['result']['hashrate']) / 1000000000), 2)

        last_active = last_active[:19]

        # Get the Account Payouts List
        get_account_payouts_json = get_account_payouts(_socket=socket, _account=wallet_id)

        # Extract the necessary data as list of dicts
        get_account_payouts_list = get_account_payouts_json['result']

        # Rename the txhash column to re-use the hash logic from the overview page
        for item in get_account_payouts_list:
            item.update({"hash": item['txhash']})
            del item['txhash']

        # Sort the Block Data
        get_account_payouts_list = sorted(get_account_payouts_list,
                                          key=lambda i: i['time'], reverse=True)

        get_account_payouts_table = AccountPayoutsTable(get_account_payouts_list)

        print("get_account_payouts_json: ", get_account_payouts_json)

        socket_disconnect(_socket=socket)

        return render(request, template_name, {'wallet_id': wallet_id,
                                               'account': account,
                                               'created_at': created_at,
                                               'last_active': last_active,
                                               'shares': shares,
                                               'hashrate': hashrate,
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


def mining_calc(request):
    template_name = 'presenter/mining_calc.html'

    try:

        if request.GET & CalcForm.base_fields.keys():
            form = CalcForm(request.GET)

        elif request.POST:

            form = CalcForm(request.POST)

            if form.is_valid():
                search_rate = int(form.cleaned_data['hardware'])
                network_difficulty = form.cleaned_data['network_difficulty']
                block_reward = form.cleaned_data['block_reward']
                power_consumption = form.cleaned_data['power_consumption']
                electricity_cost = form.cleaned_data['electricity_cost']
                exchange_rate = form.cleaned_data['exchange_rate']

                blocks_per_day = 24 * 3600 / (
                            (7.47101117 * math.exp(4.31557609 * network_difficulty)) / 1000000000 / search_rate)
                nexus_per_day = blocks_per_day * block_reward
                cost_per_day = power_consumption / 1000 * electricity_cost * 24
                profit_per_day = nexus_per_day * exchange_rate - cost_per_day

                return render(request, template_name, {'form': form,
                                                       'blocks_per_day': blocks_per_day,
                                                       'nexus_per_day': nexus_per_day,
                                                       'cost_per_day': cost_per_day,
                                                       'profit_per_day': profit_per_day,
                                                       }
                              )

        else:

            form = CalcForm(initial={'network_difficulty': 8,
                                     'exchange_rate': get_exchange_rate_nxs2usd()})

        return render(request, template_name, {'form': form})

    except Exception as ex:
        print("Exception when trying to calculate: ", ex)
        logger.error(ex)
        return redirect('presenter:error_pool')
