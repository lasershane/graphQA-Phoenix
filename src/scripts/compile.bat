v:\users\pcc\Phoenix\Scripts\mk_nets.perl < %1.gra > nets
v:/users/pcc/phoenix/bin/compile -SymBufSize 200000 -f %1 > log
grep WARN log

v:/users/pcc/phoenix/bin/concept_leaf -grammar %1.net
