
public:

[[eosio::on_notify("*::transfer")]] 
void on_transfer(name from, name to, asset quant, std::string memo) {

    name contract = get_first_receiver();
    int64_t nAmt = quant.amount;
    symbol_code cUnit = quant.symbol.code();
    string sUnit = cUnit.to_string();
    uint8_t nPrec = quant.symbol.precision();

    check(nAmt > 0, "Insufficient funds sent. ");

    if( ((memo.substr(0,10) == "!deposit!-")||(memo.substr(0,11) == "!purchase!-")) &&
        (to.value == get_self().value) ) {
        
    }

    //*** BUYRAM / SELLRAM *******************************
    /*if( ((memo.substr(0,6) == "buyram")||(memo.substr(0,7) == "sellram")) &&
        (to.value == name("msg.scribe").value) ) 
    {
        check((to.value == name("msg.scribe").value), "msg.scribe only operates by sending funds directly to it. ");
        vector<string> split_memo = split(memo, ","); //buyram,<name>  //  sellram,<name>

        check(split_memo.size() == 2, "msg.scribe improper memo format for buyram / sellram:  buyram,<name> or sellram,<name> ");
        check((split_memo[0] == "buyram") || (split_memo[0] == "sellram"), "msg.scribe improper memo format for buyram / sellram:  buyram,<name> or sellram,<name> ");

        name ram_for = name(split_memo[1]);
        check( is_account( ram_for ), "ram_for account does not exist");

        if(cUnit.raw() == symbol_code("TLOS").raw())
        {
            check(contract == name("eosio.token"), "TLOS must be sent to the contract from eosio.token. ");
            check(nPrec == 4, "Incorrect precision, wrong token properties. ");

            if(memo.substr(0,6) == "buyram") {
                //increment local balance
                //buyram on eosio
            } else {
                require_auth(ram_for);
                //decrement local balance
                //sellram on eosio
            }
        } else {
            check(false, "Wrong type of token. ");
        }   
    } else if(to.value == name("msg.scribe").value) {
        //message processing
        msg_proc(from, memo);
    }*/
}

private:
