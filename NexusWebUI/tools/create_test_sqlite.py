import os
import sqlite3
import sys
from sqlite3 import Error


def clean_db_file(_file):
    if os.path.exists(_file):
        os.remove(_file)
        print("Cleaned Old Version of DB")


def create_db_and_connect(full_path):
    """ create a database connection to a SQLite database """

    conn = sqlite3.connect(full_path)
    print(sqlite3.version)

    return conn

def create_table_structure(connection):

    sql_round = f"""
    CREATE TABLE IF NOT EXISTS round ( 
      id INTEGER PRIMARY KEY AUTOINCREMENT, 
      round_number INTEGER NOT NULL, 
      total_shares REAL, 
      total_reward REAL, 
      blocks INTEGER, 
      connection_count INTEGER 
    ); """

    sql_block = f"""
     CREATE TABLE IF NOT EXISTS block ( 
      id INTEGER PRIMARY KEY AUTOINCREMENT, 
      hash TEXT NOT NULL, 
      height INTEGER NOT NULL, 
      type TEXT NOT NULL, 
      reward INTEGER NOT NULL, 
      difficulty REAL NOT NULL, 
      orphan INTEGER NOT NULL, 
      block_finder TEXT NOT NULL, 
      round INTEGER NOT NULL, 
      block_found_time TEXT NOT NULL, 
      FOREIGN KEY(round) REFERENCES round(id), 
      FOREIGN KEY(block_finder) REFERENCES account(name) 
    ); 
     """

    sql_account = f"""
    CREATE TABLE IF NOT EXISTS account ( 
      name TEXT PRIMARY KEY, 
      created_at TEXT NOT NULL, 
      last_active TEXT, 
      connection_count INTEGER, 
      shares REAL, 
      reward REAL, 
      hashrate REAL 
    ); 
    """

    banned_connections_api = f"""
     CREATE TABLE IF NOT EXISTS banned_connections_api ( 
      id INTEGER PRIMARY KEY AUTOINCREMENT, 
      ip TEXT NOT NULL 
     ); 
     """

    sql_banned_users_connections = f"""
     CREATE TABLE IF NOT EXISTS banned_users_connections ( 
      user TEXT, 
      ip TEXT, 
      PRIMARY KEY (user, ip) 
    );
     """

    connection.execute(sql_round)
    connection.execute(sql_block)
    connection.execute(sql_account)
    connection.execute(banned_connections_api)
    connection.execute(sql_banned_users_connections)


###################################
con = None
db_name = 'test.sqlite3'

if __name__ == '__main__':
    try:
        clean_db_file(_file=db_name)

        print("Creating new DB")
        script_dir = os.path.abspath(os.path.dirname(sys.argv[0]))
        db_con = create_db_and_connect(os.path.join(script_dir, db_name))
        print("DB created")

        create_table_structure(connection=db_con)

    except Exception as ex:
        print(ex)
        exit(-1)
    finally:
        if con:
            con.close()
