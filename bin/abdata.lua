--------------------------------------------------------------------------------
-- abdata Settings -------------------------------------------------------------
--------------------------------------------------------------------------------

server_id = "cd00b30c-fc7f-416d-be9e-cec9fb34fb79"
location = "AT"
server_name = "abdata"

-- If true it doesn't write to the DB
read_only = false
-- Set to false to skip database version check
db_version_check = true

data_port = 2770
-- IP it listens on
data_ip = "0.0.0.0"
data_host = ""

allowed_ips = "127.0.0.1;192.168.1.51"

-- Max memory size, 1GB
max_size = 1024 * 1024 * 1024
-- Flush cache every minute
flush_interval = 1000 * 60
-- Clean cache every 10min
clean_interval = 1000 * 60 * 10

require("config/db")
