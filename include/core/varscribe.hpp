
public:

#include "./var_struct.hpp"

//---------------PUBLIC ACTIONS
ACTION regvar(name signor, name scope, name varname, name vardgt, string type, uint64_t tlimit, uint8_t vlimit);
ACTION delvar(name signor, name scope, name varname);
ACTION update(name signor, name scope, name varname, string operation, uint8_t index, vector<uint128_t> uval, vector<string> sval, vector<int128_t> nval, vector<asset> aval);
ACTION clearbytime(name signor, name scope, name varname, time_point_sec time);
ACTION clearlast(name signor, name scope, name varname, uint8_t qty);

private:

//---------------PRIVATE FUNCTIONS
void _regvar(name signor, name scope, name varname, name vardgt, string type, uint64_t tlimit, uint8_t vlimit);
void _update(name signor, name scope, name varname, string operation, uint8_t index, vector<uint128_t> uval, vector<string> sval, vector<int128_t> nval, vector<asset> aval);
varstrct _getvar(name scope, name varname);
uint8_t getvartype(name varid);
string getdatestr(uint8_t nType, time_point tTime);
string datestrtoeosio(string datestr);
void _authvarname(name varname);


//*********** TABLE - varreg *******************

    //variable registry table - looks up EOSIO name
    TABLE varreg {
      name varname; //full eosio name lookup
      name vardgt; //3 digit name
      uint8_t t; //var type
      uint64_t tcount;
      uint64_t tlimit;
      uint8_t vlimit;
     
      uint8_t type() const { return t; }
      string char_type() const {
        if(t == VTYPE_NORMAL)   { return "x"; }
        if(t == VTYPE_YYMMDDHH) { return "h"; }
        if(t == VTYPE_YYMMDD)   { return "d"; }
        if(t == VTYPE_YYMM)     { return "m"; }
        if(t == VTYPE_YY)       { return "n"; }
        check(false, "uint8_t t is invalid within varreg TABLE. (E|data.scribe|varreg|varscribe.hpp:43|)");
      }

      uint64_t primary_key() const { return varname.value; }
      uint64_t secondary_key() const { return vardgt.value; }
    };

    typedef eosio::multi_index<
      name("vregister"),
      varreg,
      eosio::indexed_by<
        name("vardgt"),
        eosio::const_mem_fun<
          varreg,
          uint64_t,
          &varreg::secondary_key
        >
      >
    >  varreg_index;


//*********** TABLE - vars *******************

    //variable data table
    TABLE vars {
      name            varid;
      varstrct        data;

      uint8_t type() { return data.type(); }
      char char_type() { return (char) data.type(); }

      auto primary_key() const { return varid.value; }
    };

    typedef multi_index<name("vartable"), vars> vars_index;
