import os
import random

import sqlalchemy
import pandas as pd
from faker import Faker


def connect_to_db(_file):
    if not os.path.exists(_file):
        raise AssertionError("DB File not found")

    engine = sqlalchemy.create_engine(f'sqlite:///{_file}')
    sqlalchemy_connection = engine.connect()
    print("DB Connection established")

    return sqlalchemy_connection


def clean_db(connection):
    tables = ['round', 'block', 'account', 'banned_connections_api', 'banned_users_connections']

    for table in tables:
        connection.execute(f'DELETE FROM {table}')


def create_round_test_data(connection):
    # Todo get Config
    rounds = 10

    # Create an Empty Dataframe to hold the Data
    dataframe_round = pd.DataFrame(columns=['round_number',
                                            'total_shares',
                                            'total_reward',
                                            'blocks',
                                            'connection_count']
                                   )

    for x in range(rounds):
        total_share = round(random.uniform(1, 10), 2)
        total_reward = round(random.uniform(1, 10), 2)
        blocks = random.randint(1, 10)
        connection_count = random.randint(1, 100)

        data_dict = {
            'round_number': x,
            'total_shares': total_share,
            'total_reward': total_reward,
            'blocks': blocks,
            'connection_count': connection_count,
        }

        dataframe_round = dataframe_round.append(data_dict, ignore_index=True)

    dataframe_round.to_sql('round', con=connection, index=False, if_exists='append')
    print("Inserted Test Data for 'Round'")


def create_account_test_data(connection):
    # Todo get Config
    accounts = 100

    # Create an Empty Dataframe to hold the Data
    dataframe_account = pd.DataFrame(columns=[
        'name',
        'created_at',
        'last_active',
        'connection_count',
        'shares',
        'reward',
        'hashrate'
    ]
    )

    # This prevents getting the same name twice as a set is distinct
    names = set()
    while len(names) < accounts:
        names.add(fake.user_name())

    for x in range(accounts):
        name = list(names)[x]
        created_at = fake.date()
        last_active = fake.date()
        connection_count = random.randint(1, 100)
        shares = round(random.uniform(1, 100), 2)
        reward = round(random.uniform(1, 10), 2)
        hashrate = round(random.uniform(1, 10), 2)

        data_dict = {
            'name': name,
            'created_at': created_at,
            'last_active': last_active,
            'connection_count': connection_count,
            'shares': shares,
            'reward': reward,
            'hashrate': hashrate,
        }

        dataframe_account = dataframe_account.append(data_dict, ignore_index=True)

    dataframe_account.to_sql('account', con=connection, index=False, if_exists='append')
    print("Inserted Test Data for 'Account'")


def create_block_test_data(connection):
    # Todo get Config
    blocks = 1000

    # Create an Empty Dataframe to hold the Data
    dataframe_block = pd.DataFrame(columns=[
        'hash',
        'height',
        'type',
        'reward',
        'difficulty',
        'orphan',
        'block_finder',
        'round',
        'block_found_time',
    ]
    )

    df_tmp = pd.read_sql(sql='SELECT id from round', con=connection)
    round_list = list(df_tmp['id'])

    df_tmp = pd.read_sql(sql='SELECT name from account', con=connection)
    account_list = list(df_tmp['name'])

    for x in range(blocks):
        hash = fake.md5(raw_output=False)
        height = random.randint(1, 100)
        type = 'Test'
        reward = random.randint(1, 100)
        difficulty = round(random.uniform(1, 100), 2)
        orphan = random.randint(1, 100)
        block_finder = str(random.choice(account_list))
        round_value = random.choice(round_list)
        block_found_time = fake.past_datetime()

        data_dict = {
            'hash': hash,
            'height': height,
            'type': type,
            'reward': reward,
            'difficulty': difficulty,
            'orphan': orphan,
            'block_finder': block_finder,
            'round': round_value,
            'block_found_time': block_found_time,
        }

        dataframe_block = dataframe_block.append(data_dict, ignore_index=True)

    dataframe_block.to_sql('block', con=connection, index=False, if_exists='append')
    print("Inserted Test Data for 'Block'")


def create_banned_connections_api_test_data(connection):
    # Todo get Config
    banned_connections_api = 100

    # Create an Empty Dataframe to hold the Data
    dataframe_banned = pd.DataFrame(columns=[
        'ip',
    ]
    )

    for x in range(banned_connections_api):
        ip = fake.ipv4()

        data_dict = {
            'ip': ip,
        }

        dataframe_banned = dataframe_banned.append(data_dict, ignore_index=True)

    dataframe_banned.to_sql('banned_connections_api', con=connection, index=False, if_exists='append')
    print("Inserted Test Data for 'banned_connections_api'")


def create_banned_users_connections_test_data(connection):
    # Todo get Config
    banned_users_connections = 10

    # Create an Empty Dataframe to hold the Data
    dataframe_banned_users = pd.DataFrame(columns=[
        'user',
        'ip',
    ]
    )

    df_tmp = pd.read_sql(sql='SELECT name from account', con=connection)
    account_list = list(df_tmp['name'])

    for x in range(banned_users_connections):
        user = str(random.choice(account_list))
        ip = fake.ipv4()

        data_dict = {
            'user': user,
            'ip': ip,
        }

        dataframe_banned_users = dataframe_banned_users.append(data_dict, ignore_index=True)

    dataframe_banned_users.to_sql('banned_users_connections', con=connection, index=False, if_exists='append')
    print("Inserted Test Data for 'banned_users_connections'")


###################################
con = None
db_name = 'test.sqlite3'

if __name__ == '__main__':
    try:
        fake = Faker()
        db_con = connect_to_db(_file=db_name)

        clean_db(connection=db_con)

        create_round_test_data(connection=db_con)
        create_account_test_data(connection=db_con)
        create_block_test_data(connection=db_con)
        create_banned_connections_api_test_data(connection=db_con)
        create_banned_users_connections_test_data(connection=db_con)

    except Exception as ex:
        print(ex)
        exit(-1)
    finally:
        if con:
            con.close()
