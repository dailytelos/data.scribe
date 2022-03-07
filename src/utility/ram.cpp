


double datascribe::get_ram_price() {
    //contract: eosio
    //table:  rammarket
    //formula:  ((quote.balance) / (base.balance * quote.weight)) * (base.weight) ==> Price in Telos per Byte
    //example:  
    rammarket _ram(name("eosio"), name("eosio").value);

    auto itr_ram = _ram.begin();

    if(itr_ram != _ram.end()) {
        return (itr_ram->quote.balance.amount / (itr_ram->base.balance.amount * itr_ram->quote.weight)) * itr_ram->base.weight;
    } else { return 0; }
}



asset datascribe::get_ram_tlos_cost(uint64_t bytes) {

    double dCost = ((double) (bytes) * get_ram_price());
    int64_t nCost = (int64_t) (dCost * 10000);
    
    return asset(nCost, symbol("TLOS", 4));
}
