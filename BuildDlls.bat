msbuild xinputReader.sln /property:Configuration=Release /property:Platform=x64
msbuild xinputReader.sln /property:Configuration=Release /property:Platform=x86
copy x64\Release\xinputReader.dll xinputReader_64.dll
copy Release\xinputReader.dll xinputReader_32.dll
