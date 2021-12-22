import requests
import logging
from django.core.cache import cache

logger = logging.getLogger('NexusWebUI')


def get_exchange_rate_nxs2usd():
    # Try to retrieve from cache
    exchange_rate = cache.get('exchange_rate_nxs2usd')

    if exchange_rate:
        return exchange_rate
    else:
        try:
            nxs2btc = requests.get(url="https://api.binance.com/api/v3/ticker/price",
                                   params={'symbol': 'NXSBTC'}).json()['price']

            btc2usd = requests.get(url="https://api.binance.com/api/v3/ticker/price",
                                   params={'symbol': 'BTCUSDT'}).json()['price']

            exchange_rate = float(nxs2btc) * float(btc2usd)

            cache.set('exchange_rate_nxs2usd', exchange_rate, 1)

        except Exception as ex:
            logger.error("Could not fetch Exchange Rate Info:")
            logger.error(ex)

            return 0

    return exchange_rate
