cd boost

if exist b2.exe goto bjam

call bootstrap.bat

:bjam

b2 --toolset=msvc-14.1 --with-date_time --with-thread --with-container --with-system --with-locale --with-serialization --with-regex --stagedir="stage" link=static stage
b2 --toolset=msvc-14.1 --with-date_time --with-thread --with-container --with-system --with-locale --with-serialization --with-regex --stagedir="stage" runtime-link=static link=static stage