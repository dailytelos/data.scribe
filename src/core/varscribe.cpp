
//**************************** PUBLIC ACTIONS

ACTION datascribe::regvar(name signor, name scope, name varname, name vardgt, string type, uint64_t tlimit, uint8_t vlimit) {
  require_auth(signor);

  check(signor.value == scope.value, "Account signor must match account scope. (E|data.scribe|update|var.scribe.cpp:7|)");

  _regvar( signor,  scope,  varname,  vardgt,  type,  tlimit,  vlimit);
};

ACTION datascribe::delvar(name signor, name scope, name varname) {
  require_auth(signor);

  check(signor.value == scope.value, "Account signor must match account scope. (E|data.scribe|update|var.scribe.cpp:7|)");

  varreg_index _varreg(get_self(), scope.value);
  auto itr_varreg = _varreg.find(varname.value);

  check(itr_varreg != _varreg.end(), "varname does not exist, you must register a varname first with regvar. (E|" + get_self().to_string() + "|update|var.scribe.cpp:7|)");
  check(itr_varreg->tcount == 0, "variables currently exist inside vars table, you must clear those first. (E|" + get_self().to_string() + "|update|var.scribe.cpp:7|");

  _varreg.erase(itr_varreg);
};

ACTION datascribe::update(name signor, name scope, name varname, string operation, uint8_t index, vector<uint128_t> uval, vector<string> sval, vector<int128_t> nval) {
  require_auth(signor);

  check(signor.value == scope.value, "Account signor must match account scope. (E|data.scribe|update|var.scribe.cpp:7|)");

  _update(signor, scope, varname, operation, index, uval, sval, nval);
}

ACTION datascribe::clearbytime(name signor, name scope, name varname, time_point_sec time) {
  require_auth(signor);

  check(signor.value == scope.value, "Account signor must match account scope. (E|data.scribe|update|var.scribe.cpp:7|)");

  varreg_index _varreg(get_self(), scope.value);

  auto itr_varreg = _varreg.find(varname.value);
  check(itr_varreg != _varreg.end(), "varname does not exist, you must register a varname first with regvar. (E|" + get_self().to_string() + "|update|var.scribe.cpp:7|)");
  check(itr_varreg->tcount > 0, "No data stored in this variable, according to tcount. (E|" + get_self().to_string() + "|clearlast|var.scribe.cpp:136|)");

  //handle date type variables
  uint8_t nType = itr_varreg->t;
  string sDATE = getdatestr(nType, time_point::from_iso_string(time.to_string()));

  //build varid from varreg row
  string sType = itr_varreg->char_type();
  string sVarID = sType + itr_varreg->vardgt.to_string() + sDATE;

  vars_index _vars(get_self(), scope.value);
  auto itr_var = _vars.find(name(sVarID).value);
  check(itr_var != _vars.end(), "varname does not exist or time is incorrect. (E|" + get_self().to_string() + "|update|var.scribe.cpp:33|)");
  
  //erase var
  _vars.erase(itr_var);
  //decrement count
  _varreg.modify( itr_varreg, same_payer, [&]( auto& vrow ) {
      if(vrow.tcount == 1) { vrow.tcount = 0; }
      else { vrow.tcount--; }
  });
}


ACTION datascribe::clearlast(name signor, name scope, name varname, uint8_t qty) {
  require_auth(signor);

  check(signor.value == scope.value, "Account signor must match account scope. (E|data.scribe|update|var.scribe.cpp:7|)");

  varreg_index _varreg(get_self(), scope.value);

  auto itr_varreg = _varreg.find(varname.value);
  check(itr_varreg != _varreg.end(), "varname does not exist, you must register a varname first with regvar. (E|" + get_self().to_string() + "|clearlast|var.scribe.cpp:135|)");
  check(itr_varreg->tcount > 0, "No data stored in this variable, according to tcount. (E|" + get_self().to_string() + "|clearlast|var.scribe.cpp:136|)");

  vars_index _vars(get_self(), scope.value);

  uint8_t nIndex = 0;
  for ( auto itr_var = _vars.begin(); ((itr_var != _vars.end()) && (nIndex < qty)); nIndex++ ) {
    itr_var = _vars.erase(itr_var);
  }

  //decrement count
  _varreg.modify( itr_varreg, same_payer, [&]( auto& vrow ) {
      if((vrow.tcount - nIndex) <= 0) { vrow.tcount = 0; }
      else { vrow.tcount = vrow.tcount - nIndex; }
  });
}

//**************************** PRIVATE FUNCTIONS

void datascribe::_regvar(name signor, name scope, name varname, name vardgt, string type, uint64_t tlimit, uint8_t vlimit) {

  check(type.size() == 1, "Length of type must equal 1. (E|" + get_self().to_string() + "|update|var.scribe.cpp:12|)");

  //require auth for reserved varname registration
  _authvarname(varname);

  varreg_index _varreg(get_self(), scope.value);
  auto itr_varreg = _varreg.find(varname.value);

  check(itr_varreg == _varreg.end(), "varname already exists, you must delete and re-create it. (E|" + get_self().to_string() + "|update|var.scribe.cpp:7|)");

  //check by secondary index
  auto idx = _varreg.get_index<name("vardgt")>();
  auto itr = idx.find(vardgt.value);
  check(itr == idx.end(), "vardgt already exists, the digits cannot be redundant. (E|" + get_self().to_string() + "|update|var.scribe.cpp:7|)");

  _varreg.emplace( signor, [&]( auto& regrow ) {
    regrow.varname = varname;
    regrow.vardgt = vardgt;
    regrow.t = (uint8_t) type[0];
    regrow.tcount = 0;
    regrow.tlimit = (type == "x") ? 1 : tlimit;
    regrow.vlimit = vlimit;
  });

}

void datascribe::_update(name signor, name scope, name varname, string operation, uint8_t index, vector<uint128_t> uval, vector<string> sval, vector<int128_t> nval) {

  varreg_index _varreg(get_self(), scope.value);

  auto itr_varreg = _varreg.find(varname.value);
  //check(itr_varreg != _varreg.end(), "varname does not exist, you must register a varname first with regvar. (E|" + get_self().to_string() + "|update|var.scribe.cpp:7|)");
  //create new var if none exists ----- START
    if(itr_varreg == _varreg.end()) {
      _regvar( signor,  scope,  varname,  name(varname.to_string().substr(0, 3)),  "x",  1,  1);
      itr_varreg = _varreg.find(varname.value);
    }
  //----------------------------------- END
  
  //handle date type variables
  uint8_t nType = itr_varreg->t;
  string sDATE = getdatestr(nType, current_time_point());

  //build varid from varreg row
  string sType = itr_varreg->char_type();
  string sVarID = sType + itr_varreg->vardgt.to_string() + sDATE;

  vars_index _vars(get_self(), scope.value);
  auto itr_var = _vars.find(name(sVarID).value);

  if(itr_var == _vars.end()) {

    if(itr_varreg->tcount >= itr_varreg->tlimit) {
      //delete on FIFO basis
	    _vars.erase(_vars.begin());
    } else {
      //increase counter
      _varreg.modify( itr_varreg, same_payer, [&]( auto& vrow ) {
        vrow.tcount++;
        check(vrow.tcount <= 18446744073709551615, "You're crazy. (E|" + get_self().to_string() + "|update|var.scribe.cpp:74|)");
      });
    }

    _vars.emplace( signor, [&]( auto& vrow ) {
      vrow.varid = name(sVarID);
      vrow.data = varstrct(name(sVarID), itr_varreg->vlimit);
      vrow.data.update(operation, index, uval, sval, nval);
    });
  } else {
    _vars.modify( itr_var, same_payer, [&]( auto& vrow ) {
      vrow.data.update(operation, index, uval, sval, nval);
    });
  }
}


datascribe::varstrct datascribe::_getvar(name scope, name varname) {

  varreg_index _varreg(get_self(), scope.value);

  auto itr_varreg = _varreg.find(varname.value);
  //check(itr_varreg != _varreg.end(), 
  //return null varstrct if no var exists ----- START
    if(itr_varreg == _varreg.end()) {

      return varstrct();
    }
  //----------------------------------- END
  
  //handle date type variables
  uint8_t nType = itr_varreg->t;
  string sDATE = getdatestr(nType, current_time_point());

  //build varid from varreg row
  string sType = itr_varreg->char_type();
  string sVarID = sType + itr_varreg->vardgt.to_string() + sDATE;

  vars_index _vars(get_self(), scope.value);
  auto itr_var = _vars.find(name(sVarID).value);

  if(itr_var == _vars.end()) {
    return varstrct();
  } 
  
  return itr_var->data;
}

uint8_t datascribe::getvartype(name varid) {
  return varstrct(varid, 1).type();
}

string datascribe::getdatestr(uint8_t nType, time_point tTime) {

  string sRet = "";

  if(nType <= VTYPE_YY) { //date type format
      time_point tpDate = tTime; 
      string sDate = tpDate.to_string();
      vector<string> aStrDateTime = split(sDate,"T");
      vector<string> aStrDate = split(aStrDateTime[0],"-");
      string sYear = ndig(aStrDate[0].substr(2, 2));
      string sMonth = ndig(aStrDate[1]);
      string sDay = ndig(aStrDate[2]);
      vector<string> aStrTime = split(aStrDateTime[1],":");
      string sHour = ndig(aStrTime[0]);
      
      if(nType == VTYPE_YY) { sRet = sYear; }
      else if(nType == VTYPE_YYMM) { sRet = sYear + sMonth; }
      else if(nType == VTYPE_YYMMDD) { sRet += sYear + sMonth + sDay; }
      else if(nType == VTYPE_YYMMDDHH) { sRet += sYear + sMonth + sDay + sHour; }
      else { check(false, "Contract var type error. (E|" + get_self().to_string() + "|update|var.scribe.cpp:33|)"); }
  }

  return sRet;
}



void datascribe::_authvarname(name varname) {
     //restrict varname registration by signor, to reserve following names to get_self() contract only
    // will still be scoped under the user name
    // global.xxxx
    // stat.xxxx
    // data.xxxx
    if( (varname.to_string().substr(0,7) == "global.") ||
        (varname.to_string().substr(0,7) == "global.") ||
        (varname.to_string().substr(0,7) == "global.") ) {
        require_auth(get_self());
    }
}
