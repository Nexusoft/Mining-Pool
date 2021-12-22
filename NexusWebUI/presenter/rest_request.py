import logging
import requests
from django.conf import settings

logger = logging.getLogger('NexusWebUI')


def normalize_endpoint_url(method):
    host = getattr(settings, "POOL_HOST", None)
    port = getattr(settings, "POOL_PORT", None)

    if host[-1] == '/':
        host = host[:-1]
        endpoint = f"{host}:{port}/{method}"
    else:
        endpoint = f"{host}:{port}/{method}"

    return endpoint


def rest_request(method, parameters=None):

    try:
        print("Sending request")
        if parameters:
            response = requests.get(normalize_endpoint_url(method), params=parameters)
        else:
            response = requests.get(normalize_endpoint_url(method))

        print(f"Response: {response}")

        # if response.status_code != 200:
        #     print(f"Status Code: {response.status_code}")
        #     logger.error("Did not receive a proper Response from Backend!")
        #     logger.error(f"Response Code: {response.status_code}")
        #     return None
        # else:
        #     print("Returning Valid Response")
        #     return response

        return response

    except Exception as ex:
        print(f"Exception when sending rest request: {ex}")
        logger.info("Exception when sending rest request: ")
        logger.info(ex)
        return None



