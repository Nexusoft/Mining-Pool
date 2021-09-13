CREATE TABLE IF NOT EXISTS round (
  round_number INTEGER PRIMARY KEY AUTOINCREMENT,
  total_shares REAL,
  total_reward REAL,
  blocks INTEGER,
  connection_count INTEGER,
  start_date_time TEXT NOT NULL,
  end_date_time TEXT,
  is_active INTEGER NOT NULL,
  is_paid INTEGER NOT NULL
);

CREATE TABLE IF NOT EXISTS block (
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  hash TEXT NOT NULL,
  height INTEGER NOT NULL,
  type TEXT NOT NULL,
  shares REAL NOT NULL,
  difficulty REAL NOT NULL,
  orphan INTEGER NOT NULL,
  block_finder TEXT NOT NULL,
  round INTEGER NOT NULL,
  block_found_time TEXT NOT NULL,
  accepted_by_mainnet INTEGER NOT NULL,
  mainnet_reward REAL NOT NULL,
  FOREIGN KEY(round) REFERENCES round(round_number),
  FOREIGN KEY(block_finder) REFERENCES account(name)
);

CREATE TABLE IF NOT EXISTS account (
  name TEXT PRIMARY KEY,
  created_at TEXT NOT NULL,
  last_active TEXT,
  connection_count INTEGER,
  shares REAL,
  hashrate REAL
);

CREATE TABLE IF NOT EXISTS payment (
  id INTEGER PRIMARY KEY AUTOINCREMENT, 
  name TEXT NOT NULL,
  amount REAL,
  shares REAL,
  payment_date_time TEXT,
  round INTEGER NOT NULL,
  FOREIGN KEY(round) REFERENCES round(round_number),
  FOREIGN KEY(name) REFERENCES account(name)
);

CREATE TABLE IF NOT EXISTS banned_connections_api (
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  ip TEXT NOT NULL
);

CREATE TABLE IF NOT EXISTS banned_users_connections (
  user TEXT,
  ip TEXT,
  PRIMARY KEY (user, ip)
);

CREATE TABLE IF NOT EXISTS config (
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  version TEXT NOT NULL,
  difficulty_divider INTEGER NOT NULL,
  fee INTEGER NOT NULL,
  mining_mode TEXT NOT NULL,
  round_duration_hours INTEGER NOT NULL
);