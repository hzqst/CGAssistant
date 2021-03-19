cd boost

if exist b2.exe goto bjam

bootstrap.bat

:bjam

b2 -toolset=msvc-14.1 --with-date_time --with-thread --with-container --with-regex --with-system --with-locale --with-serialization --stagedir="stage" link=static stage
b2 -toolset=msvc-14.1 --with-date_time --with-thread --with-container --with-regex --with-system --with-locale --with-serialization --stagedir="stage" runtime-link=static link=static stage