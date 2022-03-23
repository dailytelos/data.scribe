
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
        (to.value == get_self().value) )  {
        
    } else if(to.value == get_self().value) {

    }*/
}

private:
