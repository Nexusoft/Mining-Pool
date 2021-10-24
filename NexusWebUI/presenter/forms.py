import logging
from django import forms
from django.conf import settings
from django.shortcuts import redirect

from .models import ValidAccounts
from .rpc_requests import get_account, socket_connect

logger = logging.getLogger('NexusWebUI')


class WalletSearchForm(forms.Form):
    wallet_id = forms.CharField(max_length=100)

    def clean(self):
        cleaned_data = super(WalletSearchForm, self).clean()

        if not cleaned_data:
            logger.info("Invalid Wallet ID")
            raise forms.ValidationError(-10)

        wallet_id = cleaned_data['wallet_id']

        if not ValidAccounts.objects.filter(account=wallet_id).exists():
            logger.info("Given Wallet ID is unknown, checking with Backend")

            socket = socket_connect(_ip=getattr(settings, "POOL_IP", None),
                                    _port=getattr(settings, "POOL_PORT", None))
            get_account_json = get_account(_socket=socket, _account=wallet_id)

            if 'error' in get_account_json:
                logger.info(f"Error when validating Wallet ID: {get_account_json}")
                if get_account_json['error']['code'] == -10:
                    logger.info("Invalid Wallet ID")
                    raise forms.ValidationError(-10)
                if get_account_json['error']['code'] == -11:
                    logger.info("Unknown Wallet ID")
                    raise forms.ValidationError(-11)
            else:
                logger.info("Inserting validated ID into Table")
                ValidAccounts.objects.create(account=wallet_id)
        else:
            logger.info(f"Found Wallet ID in DB: {wallet_id}")
