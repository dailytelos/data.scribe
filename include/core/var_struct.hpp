
struct varstrct {
    name varid;

    vector<uint128_t>  uval;
    vector<string>     sval;
    vector<int128_t>   nval;
    uint8_t vlimit; //max vector length

    varstrct() {
        varid = name(NAME_NULL);
        vlimit = 1;
    };

    varstrct(name i_varid, uint8_t i_vlimit) {
        varid = i_varid;
        vlimit = i_vlimit;
    };

    varstrct(name i_varid, uint8_t i_vlimit, vector<uint128_t> i_uval, vector<string> i_sval, vector<int128_t> i_nval) {
        varid = i_varid;
        uval = i_uval;
        sval = i_sval;
        nval = i_nval;
        vlimit = i_vlimit;
        check((uval.size() <= vlimit) && (sval.size() <= vlimit) && (nval.size() <= vlimit), "vlimit exceeded for variable. (E|varstrct|var_struct.hpp:26)");
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

    void update(string operation, uint8_t index, vector<uint128_t> i_uval, vector<string> i_sval, vector<int128_t> i_nval) {
        //update uval 
        for(uint8_t i = 0; i < i_uval.size(); i++) {
            if((i+index) < uval.size()) { uval[i+index] = oper(operation, uval[i+index], i_uval[i]); }
            else { uval.push_back(oper(operation, (uint128_t) 0, i_uval[i])); }
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
            else { nval.push_back(oper(operation, (int128_t) 0, i_nval[i])); }
        }
        check(nval.size() <= vlimit, "vlimit exceeded for variable nval:" + to_string(nval.size()) + "  (E|varstrct|var_struct.hpp:61)");
    };

    void push_back(vector<uint128_t> i_uval, vector<string> i_sval, vector<int128_t> i_nval) {
        uval.insert( uval.end(), i_uval.begin(), i_uval.end() );
        sval.insert( sval.end(), i_sval.begin(), i_sval.end() );
        nval.insert( nval.end(), i_nval.begin(), i_nval.end() );
        check((uval.size() <= vlimit) && (sval.size() <= vlimit) && (nval.size() <= vlimit), "vlimit exceeded for variable. (E|varstrct|var_struct.hpp:68)");
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

    EOSLIB_SERIALIZE(varstrct, (varid)(uval)(sval)(nval)(vlimit));
};

