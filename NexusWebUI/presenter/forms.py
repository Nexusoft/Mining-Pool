import logging
from django import forms
from .models import ValidAccounts
from .rpc_requests import get_account, socket_connect

logger = logging.getLogger('NexusWebUI')


class WalletSearchForm(forms.Form):
    wallet_id = forms.CharField(max_length=100)

    def clean(self):
        cleaned_data = super(WalletSearchForm, self).clean()

        wallet_id = cleaned_data['wallet_id']

        if not ValidAccounts.objects.filter(account=wallet_id).exists():
            logger.info("Given Wallet ID is unknown, checking with Backend")

            socket = socket_connect(_ip='127.0.0.1', _port=5000)
            get_account_json = get_account(_socket=socket, _account=wallet_id)

            if 'error' in get_account_json:
                logger.info(f"Error when validating Wallet ID: {get_account_json}")
                if get_account_json['error']['code'] == -10:
                    raise forms.ValidationError(f'The Wallet ID you entered is not valid!')
                if get_account_json['error']['code'] == -11:
                    raise forms.ValidationError(f'The Wallet ID you entered does not exist')
            else:
                logger.info("Inserting validated ID into Table")
                ValidAccounts.objects.create(account=wallet_id)
        else:
            logger.info(f"Found Wallet ID in DB: {wallet_id}")

