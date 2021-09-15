# NexusPool

Pool for Nexus Hash and Prime channel


## Wallet Setup

Ensure you are on latest wallet daemon release 5.0.5 or greater. Ensure wallet has been unlocked for mining.

```
    -llpallowip=<ip-port>   ex: -llpallowip=192.168.0.1:9325 
                            note: this is not needed if you mine to localhost (127.0.0.1). This is primarily used for a local-area-network setup

    -mining                 Ensure mining LLP servers are initialized.
```



## COMMAND LINE OPTION ARGUMENTS

```
    <pool_config_file> Default=pool.conf
    -c --check          run config file check before miner startup
    -v --version        Show NexusPool version
```

  ./NexusPool ../../myownpool.conf -c

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
