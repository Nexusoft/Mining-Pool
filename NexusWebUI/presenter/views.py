import datetime
import json
import logging
import math

from django.http import JsonResponse
from django.http.response import HttpResponse
from django.shortcuts import render, redirect
from django.views.decorators.csrf import csrf_exempt
from django.views.generic import TemplateView
from .tables import OverviewTable, AccountPayoutsTable, Top5FindersTable, LongestChainTable
from .forms import WalletSearchForm, CalcForm
from django.conf import settings
from django.core.cache import cache
from .helpers import get_exchange_rate_nxs2usd
from .rest_request import rest_request
from .models import MiningCalcHardware

logger = logging.getLogger('NexusWebUI')


def block_overview_list(request):
    template_name = 'presenter/overview_list.html'
    table_data = None

    try:
        # Try to retrieve from cache
        block_overview_latest_json = cache.get('block_overview_latest_json')
        block_overview_meta_json = cache.get('block_overview_meta_json')

        if not block_overview_latest_json or not block_overview_meta_json:
            logger.info("Rebuilding Cache")
            print("Rebuilding Cache")

            block_overview_meta = rest_request(method='metainfo')
            block_overview_meta_json = block_overview_meta.json()
            if not block_overview_meta_json:
                raise Exception("No Meta Data received")

            block_overview_latest = rest_request(method='latestblocks')
            block_overview_latest_json = block_overview_latest.json()

            # Check if Blocks were returned from the Pool
            if not block_overview_latest_json['blocks']:
                logger.info("Received no Blocks from Pool!")
            else:
                logger.info(f"Received {len(block_overview_latest_json['blocks'])} Blocks from Pool")
                block_overview_latest_dict = sorted(block_overview_latest_json['blocks'],
                                                    key=lambda i: i['height'],
                                                    reverse=True)
                table_data = OverviewTable(block_overview_latest_dict)

            # Save data in the cache
            if settings.DEBUG is False and block_overview_meta_json:
                cache.set('block_overview_meta_json', block_overview_meta_json, 1)
                if block_overview_latest_json['blocks']:
                    cache.set('block_overview_latest_json', block_overview_latest_json, 1)

        else:
            logger.info("Serving from Cache")
            print("Serving from Cache")

        print(f"Meta: {block_overview_meta_json}")

        # Meta Table
        pool_hashrate = round((float(block_overview_meta_json['pool_hashrate'])), 2)
        mining_mode = block_overview_meta_json['mining_mode']
        round_duration = block_overview_meta_json['round_duration']
        fee = block_overview_meta_json['fee']
        active_miners = block_overview_meta_json['active_miners']
        wallet_version = block_overview_meta_json['wallet_version']
        pool_version = block_overview_meta_json['pool_version']
        payout_time = block_overview_meta_json['payout_time']

        return render(request, template_name, {'table': table_data,
                                               'pool_hashrate': pool_hashrate,
                                               'mining_mode': mining_mode,
                                               'round_duration': round_duration,
                                               'active_miners': active_miners,
                                               'fee': fee,
                                               'wallet_version': wallet_version,
                                               'pool_version': pool_version,
                                               'payout_time': payout_time
                                               })

    except Exception as ex:
        print(ex)
        logger.error(ex)
        return redirect('presenter:error_pool')


def statistic_list(request):
    template_name = 'presenter/statistic_list.html'

    try:
        # Try to retrieve from cache
        block_overview_meta_json = cache.get('block_overview_meta_json')
        top_5_finders_json = cache.get('top_5_finders_json')
        longest_chain_json = cache.get('longest_chain_json')

        if not block_overview_meta_json:
            block_overview_meta = rest_request(method='metainfo')
            block_overview_meta_json = block_overview_meta.json()
            if not block_overview_meta_json:
                raise Exception("No Meta Data received")
            else:
                print(block_overview_meta_json)

            # Save data in the cache
            if settings.DEBUG is False and block_overview_meta_json:
                cache.set('block_overview_meta_json', block_overview_meta_json, 1)

        if not top_5_finders_json:
            top_5_finders = rest_request(method="/statistics/blockfinders", parameters={'num': 5})
            top_5_finders_json = top_5_finders.json()
            if not top_5_finders_json:
                raise Exception("Top 5 Finders Data not received")
            else:
                print(top_5_finders_json)

            # Save data in the cache
            if settings.DEBUG is False and top_5_finders_json:
                cache.set('top_5_finders_json', top_5_finders_json, 1)

        mining_mode = block_overview_meta_json['mining_mode']

        if mining_mode == 'PRIME':
            if not longest_chain_json:
                longest_chain = rest_request(method="/statistics/longestchain")
                longest_chain_json = longest_chain.json()
                if not longest_chain_json:
                    raise Exception("Longest Chain Data not received")
                else:
                    print(longest_chain_json)

                # Save data in the cache
                if settings.DEBUG is False and longest_chain_json:
                    cache.set('longest_chain_json', longest_chain_json, 1)
        else:
            longest_chain_json = None
            longest_chain_table = None

        top_5_finders_list = top_5_finders_json['block_finders']
        top_5_table = Top5FindersTable(top_5_finders_list)

        if longest_chain_json:
            longest_chain_list = [longest_chain_json,]
            longest_chain_table = LongestChainTable(longest_chain_list)

        return render(request, template_name, {'top_5_table': top_5_table,
                                               'longest_chain_table': longest_chain_table,
                                               'mining_mode': mining_mode,
                                               })

    except Exception as ex:
        print(ex)
        logger.error(ex)
        return redirect('presenter:error_pool')


class ErrorView(TemplateView):
    template_name = 'presenter/error.html'


class ErrorPoolView(TemplateView):
    template_name = 'presenter/errors/error_pool_not_available.html'


class ErrorInvalidWalletIdView(TemplateView):
    template_name = 'presenter/errors/error_invalid_wallet_id.html'


class ErrorUnknownWallet(TemplateView):
    template_name = 'presenter/errors/error_unknown_wallet.html'


@csrf_exempt
def wallet_detail(request):
    # Todo implement Cache
    # Todo Print Logs to Console also

    template_name = 'presenter/wallet_detail.html'
    get_account_payouts_table = None

    try:
        form = WalletSearchForm(request.POST)

        if not form.is_valid():
            print(f"Form is invalid")
            errors_json = json.loads(form.errors.as_json())
            wallet_error = errors_json['__all__'][0]['message']

            if wallet_error == 'pool_error':
                return redirect('presenter:error_pool')
            elif wallet_error == 400:
                return redirect('presenter:error_invalid_wallet')
            elif wallet_error == 404:
                return redirect('presenter:error_unknown_wallet')
            else:
                return redirect('presenter:error_invalid_wallet')

        else:
            print("Form is Valid!")

        wallet_id = request.POST.get('wallet_id')

        account_info = rest_request(method="account/detail", parameters={'account': wallet_id})

        if account_info is None:
            print("Connectivity Issue when fetching Wallet Account Infos")
            logger.error(f"Connectivity Issue when fetching Wallet Account Infos: {wallet_id}")
            raise Exception(f"Connectivity Issue when fetching Wallet Account Infos: {wallet_id}")
        else:
            account_info_json = account_info.json()

            print(account_info_json)

            account = account_info_json['account']
            created_at = account_info_json['created_at']
            last_active = account_info_json['last_active']
            shares = account_info_json['shares']
            hashrate = round((float(account_info_json['hashrate'])), 2)
            display_name = account_info_json['display_name']

            last_active = last_active[:19]

            # Get the Account Payouts List
            get_account_payouts = rest_request(method='/account/payout', parameters={'account': wallet_id})

            if get_account_payouts:
                get_account_payouts_json = get_account_payouts.json()

                # Extract the necessary data as list of dicts
                get_account_payouts_list = get_account_payouts_json['payouts']

                print(get_account_payouts_list)

                # Rename the txhash column to re-use the hash logic from the overview page
                for item in get_account_payouts_list:
                    item.update({"hash": item['txhash']})
                    del item['txhash']

                # Sort the Block Data
                get_account_payouts_list = sorted(get_account_payouts_list,
                                                  key=lambda i: i['time'], reverse=True)

                # Convert to Table Object
                get_account_payouts_table = AccountPayoutsTable(get_account_payouts_list)

            return render(request, template_name, {'wallet_id': wallet_id,
                                                   'account': account,
                                                   'created_at': created_at,
                                                   'last_active': last_active,
                                                   'shares': shares,
                                                   'hashrate': hashrate,
                                                   'display_name': display_name,
                                                   'table_account_payouts': get_account_payouts_table
                                                   })

    except Exception as ex:
        print("Exception when trying to fetch Wallet Info: ", ex)
        logger.error(ex)
        return redirect('presenter:error_pool')


def block_detail(request, hash):
    return redirect(f'https://explorer.nexus.io/search/{hash}')


@csrf_exempt
def mining_calc(request):
    template_name = 'presenter/mining_calc.html'

    try:
        if request.GET & CalcForm.base_fields.keys():
            form = CalcForm(request.GET)

        elif request.POST:
            form = CalcForm(request.POST)

            if form.is_valid():

                search_rate = form.cleaned_data['search_rate']
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
                print("Form not valid!")
                return render(request, template_name, {'form': form})

        else:
            # Try to retrieve from cache
            reward_data_json = cache.get('reward_data_json')
            hardware_data_json = cache.get('hardware_data_json')

            if not reward_data_json or not hardware_data_json:
                logger.info("Rebuilding Cache for Calulator")
                print("Rebuilding Cache for Calulator")

                # Get Initial Values from Pool
                # Reward Data => NetworkDifficulty / BlockReward
                reward_data = rest_request(method='/reward_data')
                reward_data_json = reward_data.json()

                print(f"reward_data_json: {reward_data_json}")

                # Hardware Data =>
                hardware_data = rest_request(method='/hardware_data')
                hardware_data_json = hardware_data.json()

                print(f"hardware_data_json: {hardware_data_json}")

                if not reward_data_json or not hardware_data_json:
                    raise Exception("No Data for Calulcator received from Pool")

                # Check if any Devices have not been added to the DB and create if necessary
                devices = hardware_data_json['devices']
                for device in devices:
                    if not MiningCalcHardware.objects.filter(model_name=device['model']).exists():
                        MiningCalcHardware.objects.create(
                            model_name=device['model'],
                            hashrate=device['hashrate'],
                            power_consumption=device['power_consumption']
                        )

                # Todo TEST --> Deactivated to see if updates
                # Save data in the cache
                # if settings.DEBUG is False and hardware_data_json and reward_data_json:
                #     cache.set('reward_data_json', reward_data_json, 720)
                #     cache.set('hardware_data_json', hardware_data_json, 720)

            else:
                logger.info("Serving from Cache")
                print("Serving from Cache")

                # Check if any Devices have not been added to the DB and create if necessary
                devices = hardware_data_json['devices']
                for device in devices:
                    if not MiningCalcHardware.objects.filter(model_name=device['model']).exists():
                        MiningCalcHardware.objects.create(
                            model_name=device['model'],
                            hashrate=device['hashrate'],
                            power_consumption=device['power_consumption']
                        )

            form = CalcForm(initial={'network_difficulty': reward_data_json['network_diff'],
                                     'exchange_rate': get_exchange_rate_nxs2usd(),
                                     'block_reward': reward_data_json['block_reward'],
                                     'electricity_cost': 1
                                     }
                            )

        return render(request, template_name, {'form': form})

    except Exception as ex:
        print("Exception when trying to calculate: ", ex)
        logger.error(ex)
        return redirect('presenter:error_pool')


def load_hardware_detail(request):
    # Get the info from the request object
    hardware_model = request.GET.get('HardwareModelID')

    # Get the electricity_cost for the given hardware model from the DB
    power_consumption = MiningCalcHardware.objects.filter(model_name=hardware_model).values_list('power_consumption',
                                                                                                 flat=True)
    search_rate = MiningCalcHardware.objects.filter(model_name=hardware_model).values_list('hashrate',
                                                                                           flat=True)

    # Convert the Queryset to the desire result
    power_consumption = round((power_consumption[0]), 2)
    search_rate = round((search_rate[0]), 2)

    return JsonResponse({'power_consumption': power_consumption,
                         'search_rate': search_rate})


def load_payout_timer(request, payout_time):
    """
    Returns the difference to the given payout time in the form of a string hh:mm:ss
    """

    # Format to an iso datetime
    payout_time = payout_time[:19]
    # Convert to a proper DateTime Object
    payout_time = datetime.datetime.fromisoformat(payout_time)

    # Calculate the time difference
    current_date_time = datetime.datetime.now()
    time_difference = payout_time - current_date_time

    diff = str(datetime.timedelta(seconds=time_difference.total_seconds()))
    diff = diff.split(".", 1)[0]

    return HttpResponse(f'{diff}')
    # return HttpResponse(f'test')