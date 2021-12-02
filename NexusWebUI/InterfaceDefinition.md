# Interface Definiton

## Pool API's
### get_meta_info


## get_latest_blocks
   ### JSON Request
{"jsonrpc": "2.0", "method": "get_latest_blocks", "id": 1}
### Result
      #### Result Fields:
            - block_reward
            - hash
            - height
            - network diff
            - payment_method
            - time
      #### JSON Result:
    {'id': 1, 'jsonrpc': '2.0', 'result': [{'block_reward': 0.06, 'hash': '012f77d1fe0f58f0b814c80d98979085', 'height': 56, 'network_diff': 62.45, 'payment_method': 'test', 'time': '2021-09-22 22:42:24.000000'}]




## Wallet Detail API's
- ###get_account_header
    - **total_revenue**
      - *Total Amount of Nexus mined*

- ###get_account_works
    - **status**
    - **hslast10**
    - **hslast1d**
    - **lastshare**
    - **rejectratio**
    
- ###get_account_payouts
    - **time**
      - *Time of Payout*
    - **amount**
      - *Amount of Payout
    - **state**
      - *State of Payout*
    - **txhash**
      - *Hash of Transaction



