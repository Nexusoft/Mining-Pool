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
    sql_account = f"""
    CREATE TABLE account (
	name TEXT PRIMARY KEY,
	created_at TEXT NOT NULL,
	last_active TEXT NOT NULL,
	connection_count REAL NOT NULL UNIQUE,
	round REAL NOT NULL UNIQUE,
	shares REAL NOT NULL UNIQUE,
	reward REAL NOT NULL UNIQUE,
	hashrate REAL NOT NULL UNIQUE
    );
    """
    connection.execute

###################################
con = None
db_name = 'test.sqlite3'

if __name__ == '__main__':
    try:
        clean_db_file(_file=db_name)

        print("Creating new DB")
        script_dir = os.path.abspath(os.path.dirname(sys.argv[0]))
        create_db_and_connect(os.path.join(script_dir, db_name))
        print("DB created")

    except Exception as ex:
        print(Exception)
        exit(-1)
    finally:
        if con:
            con.close()
