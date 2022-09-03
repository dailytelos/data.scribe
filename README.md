# data.scribe -- Not reccomended to use this version.

Open source contract written in EOSIO C++ for blockchain messaging and RAM logging.  This contract is available on
the **Telos Network Blockchain** under `data.scribe` account (testnet and mainnet).

This contract is written to handle:
- Blockchain Messaging
- Oracle data posting / Oracle RAM data updates
- Stats logging in RAM, organized by hour / day / month / year (your choice)
- Front-End dApp User Settings stored in RAM (users can later delete their settings to free RAM)
- Smart contract stats logging / tracking in RAM, just have your contract call `ACTION update`

**WARNING:** *This project is insufficiently tested / audited to secure financial data. Daily Telos reserves the right to reset the public contract for any reason including the discovery a bug.*

### Features
- Can organize variables by year / month / day / hour, (ie. daily totals, daily average)
- Variety of valid operations: `set` `+` `-` `*` `/` `%` `min` `max`
- Stores RAM data in following formats: `uint128_t` `int128_t` `std::string` `asset`
- Single RAM row can contain multiple values
- Messagings can multiple variable updates using `|!|...|!|` formatting

-----------
## ACTION message

Send string data for logging to the blockchain.  It will filter for `|!|` to find RAM variable updates.

* `sender` (name) - EOSIO account to pay for all RAM, the one authorized to send the message.
* `message` (std::string) - String of the message that may or may not contain RAM updates.

### Variable Formatting

* `|!|var_name|vtype|value|!|` -- Will perform `set` operation for var_name at **index = 0**, setting to **value**
* `|!|var_name|vtype|value|operation|index|!|` -- Will perform specified **operation** for var_name at **index** using **value*.
* `|!|var_name|vtype|value|operation|index|vardgt(regvar)|type(regvar)|tlimit|vlimit|!|` -- Includes additional variable registration details.

* `vtype` - set to `s` for string, `u` for uint128_t, `a` for asset, `n` for int128_t

### Operators

* `set`: Sets variable at vector index, valid for uint64_t, int64_t, and std::string
* `+` : Adds value to variable at vector index, for std::string this will **append** on the end of the string
* `-` : Subtracts value to variable at vector index, invalid for std::string
* `*` : Multiplies at index, invalid for std::string
* `/` : Divides at index, invalid for std::string
* `%` : Modulus at index, invalid for std::string
* `min` : Checks new value against old value at index, keeps the lesser, invalid for std::string
* `max` : Checks new value against old value at index, keeps the higher, invalid for std::string

## ACTION msgid

Called by the contract function itself `get_self()`, as a by-product of calling `ACTION message` it registers a message identifier under `global.msgid` inside `TABLE vregister` using a `uval`.

## ACTION regvar

Registers a new variable in `TABLE vregister` using `varname` under the `scope`.  When specifying a `type` other than `x`, the signor can create 
RAM variable logging / mathematical operations by hour, day, month, year.  This could be used to perform daily totals, monthly averages,
daily lows, daily highs, etc.  `vardgt` must be the unique 3 digit identifier for this variable.

* `signor` (name) - EOSIO account to pay for all RAM, the authorized signor to update the variable.
* `scope` (name) - EOSIO account (default contract setup must equal `signor`) to which the variable is scoped
* `varname` (name) - EOSIO name used to register the variable in `regvar` ACTION
* `vardgt` (name) - EOSIO name that is exactly 3 digits long when converted to string format.  Inside `TABLE vartable` these digits will follow the type character in the row id: `tXXXyymmddhh` (the XXX position).  As such, the three digits must be unique.
* `type` (std::string) - `x` normal,  `h` hourly, `d` daily, `m` monthly, `n` yearly.  This value will comprise the first character of the id in `TABLE vartable` .
* `tlimit` (uint64_t) - Max number of rows assigned to this variable (pertains to `type` = `h` `d` `m` `n`)
* `vlimit` (uint8_t) - Inside each row, this is the maximum vector size for `uval` `sval` and `nval`

## ACTION update

This action controls a direct update to a RAM variable, this is great for oracles to use, or more advanced users.  It is specifically to handle
updates to variables.  It provides for finer controls of uploading multiple operations at once and having vectors of uint128_t / int128_t / std::string / asset
uploaded to the blockchain in one action.

* `signor` (name) - EOSIO account to pay for all RAM, the authorized signor to update the variable.
* `scope` (name) - EOSIO account (default contract setup must equal `signor`) to which the variable is scoped
* `varname` (name) - EOSIO name used to register the variable in `regvar` ACTION
* `header` (vector<std::string>) - Provides a column header description for the data in uval, sval, nval, aval, currently does nothing in the contract
* `operation` (vector<std::string>) - Valid operations to perform on all data: `set` `+` `-` `*` `/` `%` `min` `max`
* `index` (uint8_t) - Index of where to apply **value** and **operation** into `uval` `sval` and `nval`
* `uval` (vector<uint128_t>) - Vector of uint128_t
* `sval` (vector<std::string>) - Vector of std::string
* `nval` (vector<int128_t>) - Vector of int128_t
* `aval` (vector<eosio::asset>) - Vector of asset

## ACTION delvar

Deletes a registered variable from `TABLE vregister`, so long as all the rows were deleted.

* `signor` (name) - EOSIO account to pay for all RAM, the authorized signor to update the variable.
* `scope` (name) - EOSIO account (default contract setup must equal `signor`) to which the variable is scoped
* `varname` (name) - EOSIO name used to register the variable in `regvar` ACTION

## ACTION clearbytime

Deletes item from `TABLE vartable` by `time` specified

* `signor` (name) - EOSIO account to pay for all RAM, the authorized signor to update the variable.
* `scope` (name) - EOSIO account (default contract setup must equal `signor`) to which the variable is scoped
* `varname` (name) - EOSIO name used to register the variable in `regvar` ACTION
* `time` (time_point_sec) - Specify the time of variable you want to deleted

## ACTION clearlast

Deletes item from `TABLE vartable` by last items in that table.  Good for quickly clearing out RAM.

* `signor` (name) - EOSIO account to pay for all RAM, the authorized signor to update the variable.
* `scope` (name) - EOSIO account (default contract setup must equal `signor`) to which the variable is scoped
* `qty` (uint8_t) - Quantity of variables to delete
