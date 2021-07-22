CREATE TABLE IF NOT EXISTS round (
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  round_number INTEGER NOT NULL,
  total_shares REAL,
  total_reward REAL,
  blocks INTEGER,
  connection_count INTEGER
);

CREATE TABLE IF NOT EXISTS block (
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  hash TEXT NOT NULL,
  height INTEGER NOT NULL,
  type TEXT NOT NULL
  reward INTEGER NOT NULL,
  difficulty REAL NOT NULL,
  orphan INTEGER NOT NULL,
  block_finder TEXT NOT NULL,
  round INTEGER NOT NULL,
  block_found_time TEXT NOT NULL,
  FOREIGN KEY(round) REFERENCES round(id),
  FOREIGN KEY(block_finder) REFERENCES account(name)
);

CREATE TABLE IF NOT EXISTS account (
  name TEXT PRIMARY KEY,
  created_at TEXT NOT NULL,
  last_active TEXT,
  connection_count INTEGER,
  shares REAL,
  reward REAL,
  hashrate REAL
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