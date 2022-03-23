
struct varstrct {
    name varid;

    vector<uint128_t>  uval;
    vector<string>     sval;
    vector<int128_t>   nval;
    vector<asset>      aval;
    uint8_t vlimit; //max vector length

    varstrct() {
        varid = name(NAME_NULL);
        vlimit = 1;
    };

    varstrct(name i_varid, uint8_t i_vlimit) {
        varid = i_varid;
        vlimit = i_vlimit;
    };

    varstrct(name i_varid, uint8_t i_vlimit, vector<uint128_t> i_uval, vector<string> i_sval, vector<int128_t> i_nval, vector<asset> i_aval) {
        varid = i_varid;
        uval = i_uval;
        sval = i_sval;
        nval = i_nval;
        aval = i_aval;
        vlimit = i_vlimit;
        check((uval.size() <= vlimit) && (sval.size() <= vlimit) && (nval.size() <= vlimit) && (aval.size() <= vlimit), "vlimit exceeded for variable. (E|varstrct|var_struct.hpp:26)");
    };

    uint8_t const type() {
      string s = varid.to_string();
      
      if(s.size() == 0) { return VTYPE_NONE; }
      s = s.substr(0, 1);

      if(s == "x") { return VTYPE_NORMAL; }
      if(s == "h") { return VTYPE_YYMMDDHH; }
      if(s == "d") { return VTYPE_YYMMDD; }
      if(s == "m") { return VTYPE_YYMM; }
      if(s == "n") { return VTYPE_YY; }
      return VTYPE_NONE;
    };

    void update(string operation, uint8_t index, vector<uint128_t> i_uval, vector<string> i_sval, vector<int128_t> i_nval, vector<asset> i_aval) {
        //update uval 
        for(uint8_t i = 0; i < i_uval.size(); i++) {
            if((i+index) < uval.size()) { uval[i+index] = oper(operation, uval[i+index], i_uval[i]); }
            else { uval.push_back(oper(operation, (operation == "min") ? i_uval[i] : (uint128_t) 0, i_uval[i])); }
        }
        check(uval.size() <= vlimit, "vlimit exceeded for variable uval:" + to_string(uval.size()) + " (E|varstrct|var_struct.hpp:49)");
        //update sval 
        for(uint8_t i = 0; i < i_sval.size(); i++) {
            if((i+index) < sval.size()) { sval[i+index] = oper(operation, sval[i+index], i_sval[i]); }
            else { sval.push_back(oper(operation, "", i_sval[i])); }
        }
        check(sval.size() <= vlimit, "vlimit exceeded for variable sval:" + to_string(sval.size()) + "  (E|varstrct|var_struct.hpp:55)");
        //update nval 
        for(uint8_t i = 0; i < i_nval.size(); i++) {
            if((i+index) < nval.size()) { nval[i+index] = oper(operation, nval[i+index], i_nval[i]); }
            else { nval.push_back(oper(operation, (operation == "min") ? i_nval[i] : (int128_t) 0, i_nval[i])); }
        }
        check(nval.size() <= vlimit, "vlimit exceeded for variable nval:" + to_string(nval.size()) + "  (E|varstrct|var_struct.hpp:61)");
        //update aval 
        for(uint8_t i = 0; i < i_aval.size(); i++) {
            if((i+index) < aval.size()) { aval[i+index] = oper(operation, aval[i+index], i_aval[i]); }
            else { aval.push_back(oper(operation, (operation == "min") ? i_aval[i] : asset(0, i_aval[i].symbol), i_aval[i])); }
        }
        check(aval.size() <= vlimit, "vlimit exceeded for variable aval:" + to_string(aval.size()) + "  (E|varstrct|var_struct.hpp:69)");
    };

    void push_back(vector<uint128_t> i_uval, vector<string> i_sval, vector<int128_t> i_nval, vector<asset> i_aval) {
        uval.insert( uval.end(), i_uval.begin(), i_uval.end() );
        sval.insert( sval.end(), i_sval.begin(), i_sval.end() );
        nval.insert( nval.end(), i_nval.begin(), i_nval.end() );
        aval.insert( aval.end(), i_aval.begin(), i_aval.end() );
        check((uval.size() <= vlimit) && (sval.size() <= vlimit) && (nval.size() <= vlimit) && (aval.size() <= vlimit), "vlimit exceeded for variable. (E|varstrct|var_struct.hpp:77)");
    };

    uint128_t const oper(string operation, uint128_t uval_old, uint128_t uval_new) {

        if(operation == "set") { return uval_new; }
        if(operation == "+") { return (uval_old + uval_new); }
        if(operation == "-") { return (uval_old - uval_new); }
        if(operation == "*") { return (uval_old * uval_new); }
        if(operation == "/") { if(uval_new == 0) { return 0; } return (uval_old / uval_new); }
        if(operation == "%") { if(uval_new == 0) { return 0; } return (uval_old % uval_new); }
        if(operation == "min") { if(uval_old <= uval_new) { return uval_old; } return uval_new; }
        if(operation == "max") { if(uval_old >= uval_new) { return uval_old; } return uval_new; }

        check(false, "oper(string, uint128_t, uint128_t) invalid operation given. (E|varstrct|var_struct.hpp:82)");
    }

    string const oper(string operation, string sval_old, string sval_new) {

        if(operation == "set") { return sval_new; }
        if(operation == "+") { return (sval_old + sval_new); }

        check(false, "oper(string, string, string) invalid operation given. (E|varstrct|var_struct.hpp:90)");
    }

    int128_t const oper(string operation, int128_t nval_old, int128_t nval_new) {

        if(operation == "set") { return nval_new; }
        if(operation == "+") { return (nval_old + nval_new); }
        if(operation == "-") { return (nval_old - nval_new); }
        if(operation == "*") { return (nval_old * nval_new); }
        if(operation == "/") { if(nval_new == 0) { return 0; } return (nval_old / nval_new); }
        if(operation == "%") { if(nval_new == 0) { return 0; } return (nval_old % nval_new); }
        if(operation == "min") { if(nval_old <= nval_new) { return nval_old; } return nval_new; }
        if(operation == "max") { if(nval_old >= nval_new) { return nval_old; } return nval_new; }

        check(false, "oper(string, int128_t, int128_t) invalid operation given. (E|varstrct|var_struct.hpp:104)");
    }

    asset const oper(string operation, asset aval_old, asset aval_new) {
        if(operation == "set") { return aval_new; }

        check(aval_old.symbol.code().to_string() == aval_new.symbol.code().to_string(), "oper(string, asset, asset) asset codes do not match. (E|varstrct|var_struct.hpp:118)"); 
        check(aval_old.symbol.precision() == aval_new.symbol.precision(), "oper(string, asset, asset) asset precisions do not match. (E|varstrct|var_struct.hpp:119)"); 

        if(operation == "+") { return asset((aval_old.amount + aval_new.amount), aval_old.symbol); }
        if(operation == "-") { return asset((aval_old.amount - aval_new.amount), aval_old.symbol); }
        if(operation == "*") { return asset((aval_old.amount * aval_new.amount) / pow(10, aval_new.symbol.precision()), aval_old.symbol); }
        if(operation == "/") { if(aval_new.amount == 0) { return asset(0, aval_old.symbol); } return asset((((aval_old.amount * pow(10, aval_new.symbol.precision())) / aval_new.amount)), aval_old.symbol); }
        if(operation == "%") { if(aval_new.amount == 0) { return asset(0, aval_old.symbol); } return asset((aval_old.amount % aval_new.amount), aval_old.symbol); }
        if(operation == "min") { if(aval_old.amount <= aval_new.amount) { return aval_old; } return aval_new; }
        if(operation == "max") { if(aval_old.amount >= aval_new.amount) { return aval_old; } return aval_new; }

        check(false, "oper(string, asset, asset) invalid operation given. (E|varstrct|var_struct.hpp:104)");
    }

    EOSLIB_SERIALIZE(varstrct, (varid)(uval)(sval)(nval)(aval)(vlimit));
};

