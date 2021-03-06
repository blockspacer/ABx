--------------------------------------------------------------------------------
-- General Settings ------------------------------------------------------------
--------------------------------------------------------------------------------
-- ID for this server
server_id = "230a6dd3-907e-4193-87e5-a25789d68016"
location = "AT"
server_name = "AT1"

require("config/data_server")

data_dir = EXE_PATH .. "/data"
recordings_dir = EXE_PATH .. "/recordings"
record_games = false
watch_assets = true

base_port = 2749          -- 0xABE

require("config/server")
require("config/login")
require("config/mechanics")
require("config/msg_server")
