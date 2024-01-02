cxx.std = experimental
cxx.features.modules = true

using cxx

./ : liba{success} 	: hxx{someheader} hxx{yyy} cxx{source_ok}
./ : liba{failure} : hxx{someheader}mxx{xxx} cxx{source_fail}






