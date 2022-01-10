# NexusPool (BETA)

Pool for Nexus Hash and Prime channel


## Wallet Setup

Ensure you are on latest wallet daemon release 5.0.5 or greater. Ensure wallet has been unlocked for mining.

```
    -llpallowip=<ip-port>   ex: -llpallowip=192.168.0.1:9325 
                            note: this is not needed if you mine to localhost (127.0.0.1). This is primarily used for a local-area-network setup

    -mining               Ensure mining LLP servers are initialized.
    -indexheight          needed to query the wallet for blocks with height param
```

## COMMAND LINE OPTION ARGUMENTS

```
    -p --pool <pool_config_file> Default=pool.conf
    -a --api <api_config_file> Default=<empty> (no API enabled)
    -c --check          run config file check before miner startup
    -v --version        Show NexusPool version
```

  ./NexusPool -p ../../myownpool.conf -a ../api.conf -c

  ## Pool.conf

  Some important config options in pool.conf

  ```
    "wallet_ip"             // the ip the NXS wallet listens to
    "wallet_port"           // port of the NXS wallet
    "local_ip"              // the ip of the POOL. If the wallet runs on the same machine (127.0.0.1) then also this ip must be set to 127.0.0.1
    "local_port" :          // port of the POOL for connecting to NXS wallet. (0 for ephemeral port)
    "public_ip" :            // public ip of the POOL which miners connect to.
    "miner_listen_port" : 0, // port of the POOL for listening to incoming miner connections. Potential miners will use public_ip:miner_listen_port to connect to the POOL.
    "mining_mode" : "HASH",  // mining mode the POOL is started with. Options are 'HASH' or 'PRIME'. Changes to this config option will only take effect after round end.
    "connection_retry_interval" // retry time in seconds trying to connect to NXS wallet if a connection attempt failed.
    "get_height_interval"       // time in seconds polling for current BLOCK height of NXS wallet.
    "session_expiry_time"       // time in seconds after which a miner session expires.
    "log_level"             // Optional, default=2 (info), sets the verbosity of log messages ranges from 0 (trace) - 5 (critical)
    "update_block_hashes_interval"  // Optional, default=600, time in seconds when the found blocks should update their hashes in storage. Automatically happens during round end.
    "get_hashrate_interval"         // Optional, default=300, time in seconds requesting the current hashrate from the connected miners
    "api"                // Option group regarding API for frontend of the POOL. If not present then no API server will be started
        "listen_port" : 0   // port of the POOL for listening to incoming API calls (from the web frontend for example).
        "auth_user"         // Optional. If this config value is given then the API calls requires BasicAuth REST.
        "auth_pw"           // Optional. If the auth_user config option is given then this field is mandatory.
        "reward_calc_update_interval"   // Optional, default=300, time in seconds updating the mining_info for the API

    "persistance"       // Option group regarding used storage for the POOL
        "type"          // which storage type the POOL uses. Currently only 'sqlite' is supported.
        "file"          // filename of the storage.

    "pool"              // Option group regarding POOL mining.
        "account"               // NXS account name used for transfer NXS rewards to the miners.
        "pin"                   // PIN for the used NXS account to transfer NXS rewards to the miners.
        "fee"                   // POOL fee in %
        "difficulty_divider"        // reduced the NXS mainnet difficulty for miners.
        "round_duration_hours"      // time in hours for the duration of a mining round. Payouts to miners can only happen when a round is finished. Config changes to POOL difficulty, fee, mining mode can only happen after round end.
        "nxs_api_user"  // NXS API user name. Must be the same user as in nexus.conf or nxs wallet startup arguments
        "nxs_api_pw"  // NXS API password. Must be the same password as in nexus.conf or nxs wallet startup arguments
```

 ## API

 The pool supports a REST API to accessinformation about the pool.   
 The API server will automatically be started when there is an api config file given    
 The API server supports REST BasicAuth which will be enabled if the "auth_user" option is set in the api.conf    
   
 Some important config options in api.conf
    
  ```
    "mining_mode"           // mining mode the POOL is started with. Options are 'HASH' or 'PRIME'. This has to be the same as for the POOL otherwise unexpected behaviour
    "public_ip"             // public ip of the API which a frontend connect to.
    "listen_port"           // port of the API for listening to incoming API calls (from the web frontend for example).
    "wallet_ip"             // the ip the NXS wallet listens to. Needed for API calls
    "auth_user"         // Optional. If this config value is given then the API calls requires BasicAuth REST.
    "auth_pw"           // Optional. If the auth_user config option is given then this field is mandatory.
    "reward_calc_update_interval"   // Optional, default=300, time in seconds updating the mining_info for the API

    "devices"              // Array where GPU models can be added with hashrate and power_consumption. This is the data for the reward_calculation site if the frontend.
```


## BUILDING

Optional cmake build options are  
* WITH_TESTS          to also build unit tests

### Windows

* SQLite:   build sqlite as static lib (VS Studio x64 command prompt, cl /c /EHsc sqlite3.c && lib sqlite3.obj)  
            copy sqlite3.lib to /libs folder  
* OpenSSL: 

### Ubuntu/Debian

* SQLite:                           sudo apt-get install sqlite3 libsqlite3-dev  
* OpenSSL:                          sudo apt-get install libssl-dev  
