# NexusPool (BETA)

Pool for Nexus Hash and Prime channel


## Wallet Setup

Ensure you are on latest wallet daemon release 5.0.5 or greater. Ensure wallet has been unlocked for mining.

```
    -llpallowip=<ip-port>   ex: -llpallowip=192.168.0.1:9325 
                            note: this is not needed if you mine to localhost (127.0.0.1). This is primarily used for a local-area-network setup

    -mining               Ensure mining LLP servers are initialized.
    -apiauth=0            API calls to the wallet need no authorization (will be changed in future releases)
    -indexheight          needed to query the wallet for blocks with height param
```

## COMMAND LINE OPTION ARGUMENTS

```
    <pool_config_file> Default=pool.conf
    -c --check          run config file check before miner startup
    -v --version        Show NexusPool version
```

  ./NexusPool ../../myownpool.conf -c

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
    "log_level"             // optional, default=2 (info), sets the verbosity of log messages ranges from 0 (trace) - 5 (critical)
    "api"                // Option group regarding API for frontend of the POOL. If not present then no API server will be started
        "listen_port" : 0   // port of the POOL for listening to incoming API calls (from the web frontend for example).
        "auth_user"         // Optional. If this config value is given then the API calls requires BasicAuth REST.
        "auth_pw"           // Optional. If the auth_user config option is given then this field is mandatory.

    "persistance"       // Option group regarding used storage for the POOL
        "type"          // which storage type the POOL uses. Currently only 'sqlite' is supported.
        "file"          // filename of the storage.

    "pool"              // Option group regarding POOL mining.
        "account"               // NXS account name used for transfer NXS rewards to the miners.
        "pin"                   // PIN for the used NXS account to transfer NXS rewards to the miners.
        "fee"                   // POOL fee in %
        "difficulty_divider"        // reduced the NXS mainnet difficulty for miners.
        "round_duration_hours"      // time in hours for the duration of a mining round. Payouts to miners can only happen when a round is finished. Config changes to POOL difficulty, fee, mining mode can only happen after round end.
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
