cd boost_1_63_0

if exist bjam.exe goto bjam
bootstrap.bat

:bjam
bjam -toolset=msvc-14.1 --with-date_time --with-thread --with-container --with-regex --with-system --with-locale --with-serialization --stagedir="stage" runtime-link=static link=static stage