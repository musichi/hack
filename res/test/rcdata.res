        ��  ��                  @      ��
 ��*     0 	        42 RCDATA "test/rcdata.rc"
43 RcdatA BEGIN "test/rcdata.rc" END
44 rCDAta { "test/rcdata.rc" }
FOO RCDATA { "asdf" }
FOO.CSS RCDATA { "asdf" }
// FIXME: rc ignores `/**/` in the middle of an identifier
// FIXME: rc allows , in the middle of identifiers
F\tOO-0+1/2*3(4)~5|6&7>>8<<9%0^1?2:3=4"a\ts""df".dll RCDATA { "as/**/df" }
//44 RCDATA {
  //"asdf1"
  // FIXME: tab escapes, " escaping, numbers, floats, arithmetic
/*
  "c:\as\0d\nf\\d\t"
  "lol""lol"  // has the whacko cmd.exe " rules, but not the \ rules.
  4  5 56.8f 56.8 -2 8 + 9  // FIXME
*/
//END
       ��
 ��+     0 	        test/rcdata.rc         ��
 ��,     0 	        test/rcdata.rc     $   ��
 F O O       0 	        asdf   ,   ��
 F O O . C S S       0 	        asdf   �   ��
 F \ T O O - 0 + 1 / 2 * 3 ( 4 ) ~ 5 | 6 & 7 > > 8 < < 9 % 0 ^ 1 ? 2 : 3 = 4 " A \ T S " " D F " . D L L         0 	        as/**/df