import os
import random
import string
import sqlalchemy
import pandas as pd
from faker import Faker


def get_account_list():

    # This prevents getting the same name twice as a set is distinct
    names = ['8CKSnD9zR65fzBWHpmuBp5p78WBp9FA2YhfY2HsLFaF2VVyCQ6J',
             '8CTwVsrWYYteVy94D1DwVPngYTsk6WQsAy2uPKebRHUQKCxYiEw',
             '8C4wnam86shSdf3zJqpY1tCWLTXMZuR9eZ8GAgz1wH8jktzSvcV',
             '8Bhp3QQTB6kfZE3BFgd4RMy88npm7dsVcifjU9ys226CbH5qvFi',
             '8BiY6fk3vvsjBonZkHnbcjyDhQwz78UYfVnG4rTq82MkkKNrVyv',
             '8BHUuTmNi98grhBgvWmXMqnhGsyKJiUBgkMmhMSCtJMncwYEGcj',
             '8CoryKa25XBFfhptubrkrmTEYuTRw72NgCU6QjDPF7ZMdXTAXy3',
             '8CPWmcRt5LwtmXU7MzQkDsNrTbysNnAmLxJgeqajrzy4AZbnLhB',
             '8BhAC3m9uTi3c3T58Zd8hdp39fLCbSfC6urJF7KE17d1cWMegob',
             '8Bzz4cyDssJTqkNRTufjfDhnRUqzYZ9EqoZt9QRyQhzCeRcsHGS',
             '8C7j5AdtcaScubx13tPjcZdg1APTeYXnWp8XiFZeXkCKK6qeHdk',
             '8Cmpqf95rKyyW2GWuR2FGpG6vTeBvJv4cLNNbufZL64q3e9YxXu',
             '8Cp9RdNXaqJarLKLEhNyy1Zrg5VAyEN2iSFkxWan4eZyGtb7W1k',
             '8Cj3B4bvRLY86vTH6XcFmhochPZQNwNoMpKGA7BWuEMZSsw7sGP',
             '8CWHN4ZdV3JekWHJZCXgsWUtKdYLUk59aZ5k29wRj6gHCYwYYVM',
             '8CTFsoTP6kNh7HWron3HbY8HvtCVzDeCtW4uLhrDnBzhMWBQh2o',
             '8CCCuKBtVXaijVWbNrHnPSmespPd4okcizxaqfiipm1MRidmCUa',
             '8Cm4bWkEMRH8MTBxMveuUxe4tngBrQqTRp8uu1LVXGs18pBi29t',
             '8BM2hifu4q6mLwE575LFiaSpgKo7fuSXdyzBDw7hK7J8GdLTRfV',
             '8BesYQnToDqs8nWWp1cSrvhX8QSNy986exv3cTe1SUa6wxNFgHt',
             '8Ctk4LxqP46SytePF7D9QLPLFY4yFdejRJ9h8RwMxEdF8UWKmU5',
             '8CRTS7CVqazRxMQzitYZc92dDBwUL5pzhRMWXPfYVwH9H1qrLJ5',
             '8BRTWUAJHoHHGAo1JRAfUQEKzexF2jqgHT9RAGkCC2M7qngtN36',
             '8CJgxoM1mWDZXA75azb1hQsnvhF8oLkGrJ1mQjhFMbZ9yizaSWg',
             '8BR87ZgfnF5nESUQHMQB89m2R2ghxfxXY6hmTQ2PYJ6mUmviCRa',
             '8BJdKw7nDhX3pcZQsXBL7R8FCHjwbyi8tDiaetS15ioQ93VpUHW',
             '8BqAjDB2V7ewAYv5RMJXdRtsrYyFmYXNTf41a9gVNExs8aBeafG',
             '8CmFWSkrcCpNcXWXb7YsWeRAfhRew9zgYiqnoGQADvmNjV6UYF6',
             '8CgXtDXoGEjK78RCJrYa1LV8sJZpC9bStGazzAqjtL64aGZXsFX',
             '8C7rTGDKtugNdVyHi5fuYaA7uV26p7Uyn2v3ErB6XrJ59HDxTkk',
             '8C9pUPmWTGFgA8yPi1jPxcM4Xbt1GQp5hpc1VLcURVX28rGipoJ',
             '8CmjZxUTZXgBzLtkD1Q8o3J9gNCxnVmU8ef733DN8fx8qo22hUp',
             '8BNLRzpWzCrSTwLyGC3mYCnwKaSDWLn3wSieMTjFd75Pxgfnn9i',
             '8B5s8xjssn2HxoH7ZS9s7fzpUrtPdtSzAz5xkwHomMVbK3ddS29',
             '8BmiGvA96on5MLki2YScAMjJXWe9qr5CH2etQUxeHKf72eZaHmy',
             '8ChusBPxxTj8HJ1GoG23Hpmedr1YDJqrZeEYYT8teXq94mqrBEH',
             '8B4k4fNpib3yG6CqZX9P5hWdztr6D47G5CRh5FBWJCTPG4dV4tx',
             '8BZF397yZeAxd2VBuQax6VLH6QvcT1CMhX1k3qGoexW2GaQUGKq',
             '8BBaWXo5k6cnwWwEvoWuy3m1nEDyTrZ2JbfFMSNJzA4rwC5onKC',
             '8BAEssaX6FaLy1XM2c5qoY2YbgCpb1Sx452EGxUFZwdGkisRfBw',
             '8BjbGEzoYsVPBRRnr9bdsLN9ifsyiHKBLGeAiAYXqFvzfoBMLns',
             '8CsFXhyPwzguAQABxf6GXaEgmTvqyX1X5s2vo5UM6Byt3mmw869',
             '8CTsQwJmbjPunUruEbuk9AdbvWu8imKcUHcPZYAKfigsr3qWCgX',
             '8CeeTSptjiWXdmKtAGtKHPN2t7CrF7RhbBMgkgofPED7bUuvNCE',
             '8BQ8XDB635krFHEqa2ABTpDg3Ys6dh3Yu6VgHGmCKj7tgtBRyxK',
             '8CEM6FY6wrymk2RhFEdphFZDanyGUoFcN2yEg1S4bDBq8ehCXZP',
             '8CFrBxGXUY1WjyCXmy4VUTzEJSscjzCD3pVo1S3BrEePWZrTQNb',
             '8BiBFmLpadLDCAxHqWnux2GM7jgMXMc1CZgUquZ8BSDoHVBxnAv',
             '8Cq7pSrRGk8RNx4aW8utPdehYDbwwNSVrnRiPgVDh9Jw97zeV3d',
             '8BMjPuGN6GtFDdHKgpCAhrM5g7SSjGxUF5UN5xWvdZ46ZXm8Ut7',
             '8BQYaTzno1Y5uf5ieo7bNoJjkQjQiNtxLyWbBQDUMKMqpv7PwFN',
             '8BK924aqmKWVACuhpR3RuxJ6d47M77saRvHso3nei9X5cRT4RN7',
             '8Ciea9g8GLFWWCL5DMomeiyAngaw45yGeYcnYACVBTJtZCTWKan',
             '8B7ADReDumjbmG5FXnUtQCaQprtTzyDBFr9NRaxRoNi2vMoG9mf',
             '8Ce7JQ7J9L1grb3dRjLSoNiE6n17xeC5ravajd34dVYJtwLJ7yq',
             '8Ct8BWUY2g7XUhjhxQe6trLffjdoAXJNbuCsQaBxPv62iqyjh2x',
             '8CsExqqMPcPGrRwftQzqd7rX5ct153sxYf6FVzCpFdsU7MMEB1K',
             '8CTMRLu9YMRGDHn1nQ4Tap2jm1WqLW1iMdnj1hf9d6SbLvQRdXd',
             '8Cvr4uTigCjK2QgFqqShm6AkiNYZcuWGpBdSuZwsQJYQcBybh8c',
             '8C8hefCEdFYJSyLEzYECAy7gq4hSMHpUgx3xpvqPuSo8Dvv3PHj',
             '8BWqTGmMsr4TKDBYnm9CUkcegMVxpbtUwuFfH7SbEEtLmzZRJnR',
             '8CX5PtVkBx9Nb3ZmsxYmQDz4vPXgTG3XrpuLye4YNebAZme7Y7y',
             '8BPXUoanSqiUvgcxtcwhHzR2YHsrbjpKw85Z2T3SDqekgXBjnKN',
             '8CPXwzBXtKP1GzE7jiidCSgQ1zsDV8n6wmsUDCm8pjPPMymKBzb',
             '8CQhzwprLxFBcTbytZeUuM5odChBeet1DFU6kPXU2BXwjh999K5',
             '8CJHvBWSDr2qDu15QP79mBsjjZ4U5x5k6VWVhu8bNSeWE1D7Cmm',
             '8BCg6QChiyJjChK2ZNKhqE1H5WQKs8q5GwGfyQSXh1cov9qQ6e9',
             '8CRVqp1wSPR2P9RCFEfoUaozGpZALfcdU2xUaUCYNJcdydtp8JA',
             '8BBxtCi8WNkN7sZgQcEHr3XTbWGpxE5zhKsPqut6yozvrUoeuth',
             '8CTfKi3u2vHtBdue5dteu85UaYhdTgXwyhY1XFDv3LhmcszerZH',
             '8BZekjR4AVGhfZHPWKX9ndhDWLiX3Ct9qbtMDuTBXscwE7e7Tos',
             '8CBDQzCRcLZXaGRo57ZMgMKZyMvJgZo79Aqnk2K4cMzRgwanQRq',
             '8CdLqeo5suB3WXJVw9n9AP2FMe45DFQmUCL8QxXjLfH6kr7zUFj',
             '8BZvYY4tVL2wV1GosfbYnv1ksbKFKUY59gxjdMyfURLN9DEy87d',
             '8BzSzR9nvWcrtHPZNyAyt1iEKMo8skjx4P3YLaUZFqsWiiU8LDN',
             '8CGpximcfNAWkLPtoAWX1SifT1xZdFP6G9v8sTLh2yrDowQg1xf',
             '8Chto99dXc2Suoi6hxJWpyY56uFR8AP2B6jcexQciGBRAGGuh5r',
             '8Bwedb6kYrfHAaqhtZK9jCbn63tY7GEpHiZ3zrjE6CbrGCcMxvr',
             '8Cnhxitj3eGXEFpkYboHEqUs5AE1GQUjPNg38pNvim6ZzkUez5E',
             '8CP1XUYQ9ykRf6WZ2EVSDKgAAaLi1dNXag1ucpFtvVQmyTKSFHV',
             '8CMh85NEgD3kGGDVqpmKvrhzZkTYEBNRMPAuuPcnKQw4C8Bbc2b',
             '8BqskufT59C3mW6PHmahhdwmE62Hq7VWh2oW4rbBm8fRsXxdBHb',
             '8Cs7GG1RkVfSdoHE7rxswsZbLaYRQ6vvD2iGHsD6jzwZergNm4E',
             '8C2F5c49ui8ynbFhgV5djTR17wBkFiD1SgG5RSSUKyGf218cFt3',
             '8BGXLjceZmfco8SH5F6FZPUnE5X11N3LD2ot7mLRy4Z7TCVfNzZ',
             '8CJdb1CQxTGDXe2LKfLExr9uKjJyWajaRmjEJCbPiUYheFzaNog',
             '8Bay4RcMATvqBw4KALwUd9iD4VpzWWwuxrfsFeZDqqwabHFFbe5',
             '8Cfq3Xtvzsgy8viCMF9nNDwLtABjL6GvF45eTdKe2YdWrVA5ETz',
             '8C2XtRwMHYL8teWchCsrWdcK4sxvW7dnx2r5zPHW17p5XL7CKAt',
             '8CLCK3j7SxnaXENCedvLhJoZtakvr7HzBoyKk3MVpU7K6tQLNgN',
             '8BmYoKq2WmzPp17cMdpfWqFz3scfgf1GXmGCTQQjA15j7zhQExp',
             '8CCintVwSC6a2kw9rCnYcXgn8yBmYCcNpJtzXXbdqqNpfFbiRke',
             '8CptBktPwLY38Ktuv6p81bmvxwPGMDWbfHqnJwFMRPJ8awioask',
             '8CU3PBEDEuk5AYzQhDiWQF9i2xeZPrsAQtHLggr5JfbuGVJ5oG7',
             '8CYDCUNh6XcNnFnYa1VTT5xPUBboXfH8psp3TGYXQPFZ87Bx4Kf',
             '8CuBrDBj5ARApUjoxNzPAbQt88EvPtVG97qLSWQce1kq4vhFqTF',
             '8Cqn4pknZ8XkAjYuesVu4WH1Ev6bbMofLiRqviwomoAJ445ED36',
             '8BDEy6RrnaVZ1cKYV6N2ANZYvYP4vEXtVHjDhC2o4w7aMRXg1Jo',
             '8CZtQm4zgvj6i2YhrRcpAQpdWkMpmdyY3aphPpscPUkokFgcYrM',
             '8BhpkJmja3r5ECrwZEWhu8FuazwT7XXvtkh8eGUtxo3uvrmaLyY',
             ]

    return names


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
                                            'start_date_time',
                                            'end_date_time',
                                            'is_active',
                                            'is_paid'
                                            ]
                                   )

    for x in range(rounds):
        total_share = round(random.uniform(1, 10), 2)
        total_reward = round(random.uniform(1, 10), 2)
        blocks = random.randint(1, 10)
        start_date_time = fake.past_datetime()
        end_date_time = fake.past_datetime()
        is_active = random.randint(0, 1)
        is_paid = random.randint(0, 1)

        data_dict = {
            'total_shares': total_share,
            'total_reward': total_reward,
            'blocks': blocks,
            'start_date_time': start_date_time,
            'end_date_time': end_date_time,
            'is_active': is_active,
            'is_paid': is_paid,
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
        'hashrate',
        'display_name'
    ]
    )

    names = get_account_list()

    for x in range(accounts):
        name = names[x]
        created_at = fake.past_datetime()
        last_active = fake.past_datetime()
        connection_count = random.randint(1, 100)
        shares = round(random.uniform(1, 100), 2)
        hashrate = round(random.uniform(1, 10), 2)
        letters = string.ascii_lowercase
        display_name = ''.join(random.choice(letters) for i in range(10))        

        data_dict = {
            'name': name,
            'created_at': created_at,
            'last_active': last_active,
            'connection_count': connection_count,
            'shares': shares,
            'hashrate': hashrate,
            'display_name' : display_name,
        }

        dataframe_account = dataframe_account.append(data_dict, ignore_index=True)

    dataframe_account.to_sql('account', con=connection, index=False, if_exists='append')
    print("Inserted Test Data for 'Account'")


def create_payment_test_data(connection):
    accounts = 100

    # Create an Empty Dataframe to hold the Data
    dataframe_payment = pd.DataFrame(columns=[
        'name',
        'amount',
        'shares',
        'payment_date_time',
        'round',
        'tx_id',
    ]
    )

    names = get_account_list()

    df_tmp = pd.read_sql(sql='SELECT round_number from round', con=connection)
    round_list = list(df_tmp['round_number'])

    for x in range(accounts):
        name = names[x]
        amount = round(random.uniform(1, 10), 2)
        shares = round(random.uniform(1, 100), 2)
        payment_date_time = fake.past_datetime()
        round_value = random.choice(round_list)
        tx_id_value = fake.md5(raw_output=False)

        data_dict = {
            'name': name,
            'amount': amount,
            'shares': shares,
            'payment_date_time': payment_date_time,
            'round': round_value,
            'tx_id': tx_id_value,
        }

        dataframe_payment = dataframe_payment.append(data_dict, ignore_index=True)

    dataframe_payment.to_sql('payment', con=connection, index=False, if_exists='append')
    print("Inserted Test Data for 'Payment'")


def create_block_test_data(connection):
    # Todo get Config
    blocks = 1000

    # Create an Empty Dataframe to hold the Data
    dataframe_block = pd.DataFrame(columns=[
        'hash',
        'height',
        'type',
        'difficulty',
        'orphan',
        'block_finder',
        'round',
        'block_found_time',
        'mainnet_reward',
    ]
    )

    df_tmp = pd.read_sql(sql='SELECT round_number from round', con=connection)
    round_list = list(df_tmp['round_number'])

    df_tmp = pd.read_sql(sql='SELECT name from account', con=connection)
    account_list = list(df_tmp['name'])

    for x in range(blocks):
        hash = fake.md5(raw_output=False)
        height = random.randint(1, 100)
        type = 'Test'
        difficulty = round(random.uniform(1, 100), 2)
        orphan = random.randint(1, 100)
        block_finder = str(random.choice(account_list))
        round_value = random.choice(round_list)
        block_found_time = fake.past_datetime()
        mainnet_reward = round(random.uniform(0, 1), 2)

        data_dict = {
            'hash': hash,
            'height': height,
            'type': type,
            'difficulty': difficulty,
            'orphan': orphan,
            'block_finder': block_finder,
            'round': round_value,
            'block_found_time': block_found_time,
            'mainnet_reward': mainnet_reward,
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

# Todo Create proper log

if __name__ == '__main__':
    try:
        fake = Faker()
        db_con = connect_to_db(_file=db_name)

        clean_db(connection=db_con)
        create_round_test_data(connection=db_con)
        create_account_test_data(connection=db_con)
        create_payment_test_data(connection=db_con)
        create_block_test_data(connection=db_con)
        create_banned_connections_api_test_data(connection=db_con)
        create_banned_users_connections_test_data(connection=db_con)

    except Exception as ex:
        print(ex)
        exit(-1)
    finally:
        if con:
            con.close()
