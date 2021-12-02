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


def create_table_structure_from_schema(connection):
    schema_file_path = None
    # Todo move to config
    schema_file_name = 'dbschema_sqlite.sql'

    if os.path.isfile(schema_file_name):
        schema_file_path = schema_file_name
        print(f"Trying to open Schema File in local Dir: {schema_file_path}")
    else:
        schema_file_path = os.path.join('../..', schema_file_name)
        print(f"Trying to open Schema File: {schema_file_path}")

    try:
        with open(schema_file_path) as fp:
            connection.executescript(fp.read())
    except Exception as ex:
        print("Error when trying to read Schema File")
        print(ex)

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

        # create_table_structure(connection=db_con)
        create_table_structure_from_schema(connection=db_con)

        print("Created the DB Structure successfully")

    except Exception as ex:
        print(ex)
        exit(-1)
    finally:
        if con:
            con.close()
