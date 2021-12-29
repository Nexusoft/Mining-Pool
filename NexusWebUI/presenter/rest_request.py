import logging
import requests
from django.conf import settings
from requests.auth import HTTPBasicAuth

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
        user = getattr(settings, "POOL_USER", None)
        password = getattr(settings, "POOL_PWD", None)

        if user and password:
            print("Sending authenticated request")

            if parameters:
                response = requests.get(normalize_endpoint_url(method), params=parameters,
                                        auth=HTTPBasicAuth(user, password))
            else:
                response = requests.get(normalize_endpoint_url(method), auth=HTTPBasicAuth(user, password))

        else:
            print("Sending non-authenticated request")

            if parameters:
                response = requests.get(normalize_endpoint_url(method), params=parameters)
            else:
                response = requests.get(normalize_endpoint_url(method))

        print(f"Response: {response}")

        return response

    except Exception as ex:
        print(f"Exception when sending rest request: {ex}")
        logger.info("Exception when sending rest request: ")
        logger.info(ex)
        return None



